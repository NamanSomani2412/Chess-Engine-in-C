// --------------------------------------------------------------------------------------
// main.cpp
// --- Entry point of the chess application. This file handles initialization, setup,
// --- GUI interaction, input events, and controls the overall game loop.
// --- It supports multiple game modes like VS Engine, VS Human, Puzzle, Learning, etc.
// --- Relies on Allegro5 for graphics, image rendering, font handling, and UI components.
// --------------------------------------------------------------------------------------

// --- Standard C++ libraries used for I/O, data structures, and file handling ---
#include <iostream>
#include <time.h>
#include <vector>
#include <string>
#include <queue>
#include <fstream>
#include <sstream>

// --- Allegro5 graphics library headers for rendering, input, fonts, and primitives ---
#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include "allegro5/allegro_font.h"
#include "allegro5/allegro_ttf.h"
#include <allegro5/allegro_primitives.h>

// --- Project-specific headers for configuration and game logic ---
#include "config.h" // Contains global declarations and shared constants
#include "engine.h" // Handles chess engine logic (AI, evaluation, move generation)
#include "human.h"  // Handles human player input and interaction
#include "puzzle.h" // Handles puzzle setup and puzzle-related game modes

using namespace std;

// --- Game core objects ---
Board board;
Engine engine;
Human human;

// --- GUI and game flow function declarations ---
void load_images();
void display_start_menu();
void handle_mouse_events();
char select_player();
void perform_engine_move();
void handle_vs_engine_moves();
void handle_vs_human_moves();
void handle_puzzle_mode_moves();
void handle_puzzle_rush();
void handle_endgame_puzzle();
void handle_Learning_mode();
void select_puzzle_mode();
void setup_puzzle_on_board();
void draw_screen();
void draw_piece(int i, int j);
char show_promotion_menu(bool is_white);
void draw_move_history(ALLEGRO_EVENT ev);
void draw_evaluation_bar();
void draw_details();
void pop_message(const string &title, const string &message, int type = 0);
void al_draw_text_button(int x, int y, int w, int h, const char *label, ALLEGRO_FONT *font);
string coords_to_string(int from_i, int from_j, int to_i, int to_j);
bool left_mouse_clicked();
bool right_mouse_clicked();

// --- Timer used to measure how long the engine takes to make a move ---
clock_t start_time;

// --- Allegro library core components ---
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_EVENT ev;

// --- Images for pieces and UI ---
ALLEGRO_BITMAP *background_img = NULL;
ALLEGRO_BITMAP *icon_img = NULL;
ALLEGRO_BITMAP *white_pawn_img = NULL;
ALLEGRO_BITMAP *white_knight_img = NULL;
ALLEGRO_BITMAP *white_bishop_img = NULL;
ALLEGRO_BITMAP *white_rook_img = NULL;
ALLEGRO_BITMAP *white_queen_img = NULL;
ALLEGRO_BITMAP *white_king_img = NULL;
ALLEGRO_BITMAP *black_pawn_img = NULL;
ALLEGRO_BITMAP *black_knight_img = NULL;
ALLEGRO_BITMAP *black_bishop_img = NULL;
ALLEGRO_BITMAP *black_rook_img = NULL;
ALLEGRO_BITMAP *black_queen_img = NULL;
ALLEGRO_BITMAP *black_king_img = NULL;
ALLEGRO_BITMAP *random_img = NULL;

// --- Input and display state tracking ---
bool suppress_mouse_input = false;
bool piece_selected = false;
bool redraw_screen = true;

// --- Board square selection tracking ---
int i;
int j;
int selected_square_i = -1;
int selected_square_j = -1;

// --- Game state control ---
side turn = WHITE;
char team = 'W';
bool game_over = false;

// --- Game mode enum and selector ---
GameMode game_mode = VS_ENGINE;

// --- Engine evaluation feedback ---
float evaluation = 0;
int nodes = 0;
float time_used = 0;

// --- Puzzle Rush mode state ---
struct PuzzleRushState
{
    clock_t start_time = 0;
    int lives = 3;
    int score = 0;
    bool active = false;
    float time_elapsed = 0.0f;
};
PuzzleRushState puzzleRush;

// --- Move history and scroll control ---
vector<string> move_history;
int move_history_offset = 0;
bool user_scrolled = false;

// --- Stores top move suggestions from engine (for hinting) ---
std::vector<EngineMove> top_white_moves;

// --- Main function ---
int main()
{
    _chdir(".."); // --- Change working directory to parent (used for accessing assets) ---
    // std::setvbuf(stdout, nullptr, _IONBF, 0);

    // --- Initialize Allegro core systems ---
    al_init();             // Initialize Allegro core
    al_install_mouse();    // Enable mouse input
    al_init_image_addon(); // Enable image loading support

    // --- Create game window and event queue ---
    display = al_create_display(960, 960); // Set display size
    event_queue = al_create_event_queue(); // Create event queue for input and window events

    load_images(); // --- Load all piece and UI images into memory ---

    // --- Set display title and window icon ---
    al_set_window_title(display, "Chess");
    al_set_display_icon(display, icon_img);

    // --- Register input sources for mouse events ---
    al_register_event_source(event_queue, al_get_mouse_event_source());

    // --- Setup promotion callbacks for GUI prompts ---
    board.gui_promotion_callback = []()
    {
        return show_promotion_menu(true); // --- Handle white pawn promotion ---
    };

    human.gui1_promotion_callback = []()
    {
        return show_promotion_menu(false); // --- Handle black pawn promotion ---
    };

    // --- Show main menu and let player select game mode ---
    display_start_menu();

    // --- Register additional event sources ---
    al_register_event_source(event_queue, al_get_display_event_source(display));

    // --- Main event/game loop ---
    while (true)
    {
        al_wait_for_event(event_queue, &ev); // --- Wait for next event in the queue ---

        // --- Handle window close ---
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            break; // Exit game loop
        }
        // --- Handle mouse input events ---
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            handle_mouse_events(); // Process mouse release events
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            redraw_screen = true; // Mark screen for redraw on mouse press
        }

        // --- Redraw screen if needed and no pending events ---
        if (redraw_screen && al_is_event_queue_empty(event_queue))
        {
            redraw_screen = false;
            al_clear_to_color(al_map_rgb(0, 0, 0)); // Clear background
            draw_screen();                          // Draw updated game state
            al_flip_display();                      // Show buffer on screen
        }
    }

    // --- Cleanup resources before program exits ---
    al_destroy_display(display);
    al_destroy_bitmap(background_img);
    al_destroy_event_queue(event_queue);

    return 0;
}

// --- Loads image files into Allegro bitmap pointers used throughout the UI and game ---
void load_images()
{
    // --- Load board background and window icon ---
    background_img = al_load_bitmap("pictures/board.png");
    icon_img = al_load_bitmap("pictures/icon.png");

    // --- Load white piece images ---
    white_pawn_img = al_load_bitmap("pictures/white_pawn.png");
    white_knight_img = al_load_bitmap("pictures/white_knight.png");
    white_bishop_img = al_load_bitmap("pictures/white_bishop.png");
    white_rook_img = al_load_bitmap("pictures/white_rook.png");
    white_queen_img = al_load_bitmap("pictures/white_queen.png");
    white_king_img = al_load_bitmap("pictures/white_king.png");

    // --- Load black piece images ---
    black_pawn_img = al_load_bitmap("pictures/black_pawn.png");
    black_knight_img = al_load_bitmap("pictures/black_knight.png");
    black_bishop_img = al_load_bitmap("pictures/black_bishop.png");
    black_rook_img = al_load_bitmap("pictures/black_rook.png");
    black_queen_img = al_load_bitmap("pictures/black_queen.png");
    black_king_img = al_load_bitmap("pictures/black_king.png");

    // --- Load extra/random asset used in the UI ---
    random_img = al_load_bitmap("pictures/random.png");
}

// --- Displays the main start menu and handles user selection of game mode ---
void display_start_menu()
{
    bool selection_made = false;

    // --- Initialize Allegro font and drawing addons ---
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    // --- Load fonts for title and options ---
    ALLEGRO_FONT *title_font = al_load_ttf_font("files/gamefont2.ttf", 140, 0); // Large title
    ALLEGRO_FONT *option_font = al_load_ttf_font("files/gamefont2.ttf", 60, 0); // Option labels
    ALLEGRO_FONT *sign_font = al_load_ttf_font("files/gamefont3.ttf", 30, 0);   // Optional signature

    // --- Draw background and translucent overlay ---
    al_draw_bitmap(background_img, 0, 0, 0);
    al_draw_filled_rectangle(0, 0, 960, 960, al_map_rgba(0, 0, 0, 100));

    // --- Draw title with shadow ---
    al_draw_text(title_font, al_map_rgb(50, 50, 50), 483, 243, ALLEGRO_ALIGN_CENTRE, "CHESS GAME");
    al_draw_text(title_font, al_map_rgb(255, 255, 255), 480, 240, ALLEGRO_ALIGN_CENTRE, "CHESS GAME");

    // --- Draw menu options with shadows ---
    al_draw_text(option_font, al_map_rgb(50, 50, 50), 483, 403, ALLEGRO_ALIGN_CENTRE, "1. Play vs Engine");
    al_draw_text(option_font, al_map_rgb(255, 255, 255), 480, 400, ALLEGRO_ALIGN_CENTRE, "1. Play vs Engine");

    al_draw_text(option_font, al_map_rgb(50, 50, 50), 483, 513, ALLEGRO_ALIGN_CENTRE, "2. Play vs Human");
    al_draw_text(option_font, al_map_rgb(255, 255, 255), 480, 510, ALLEGRO_ALIGN_CENTRE, "2. Play vs Human");

    al_draw_text(option_font, al_map_rgb(50, 50, 50), 483, 623, ALLEGRO_ALIGN_CENTRE, "3. Puzzle");
    al_draw_text(option_font, al_map_rgb(255, 255, 255), 480, 620, ALLEGRO_ALIGN_CENTRE, "3. Puzzle");

    al_draw_text(option_font, al_map_rgb(50, 50, 50), 483, 733, ALLEGRO_ALIGN_CENTRE, "4. Assisted");
    al_draw_text(option_font, al_map_rgb(255, 255, 255), 480, 730, ALLEGRO_ALIGN_CENTRE, "4. Assisted");

    // Optional footer
    al_draw_text(sign_font, al_map_rgb(50, 50, 50), 953, 848, ALLEGRO_ALIGN_RIGHT, "By - Naman Somani");
    al_draw_text(sign_font, al_map_rgb(255, 255, 255), 950, 845, ALLEGRO_ALIGN_RIGHT, "By - Naman Somani");

    // --- Update the display ---
    al_flip_display();

    // --- Temporary event queue for mouse and display events ---
    ALLEGRO_EVENT_QUEUE *tempQueue = al_create_event_queue();
    al_register_event_source(tempQueue, al_get_mouse_event_source());
    al_register_event_source(tempQueue, al_get_display_event_source(display));

    while (!selection_made)
    {

        // Wait for event
        al_wait_for_event(tempQueue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            // Cleanup on window close
            al_destroy_font(title_font);
            al_destroy_font(option_font);
            al_destroy_font(sign_font);
            al_destroy_event_queue(tempQueue);
            al_destroy_display(display);
            exit(0);
        }

        if (suppress_mouse_input)
        {
            al_rest(0.55);
            suppress_mouse_input = false;
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            int y = ev.mouse.y;

            if (y >= 385 && y <= 465)
            {
                game_mode = VS_ENGINE;
                selection_made = true;
            }
            else if (y >= 495 && y <= 575)
            {
                game_mode = VS_HUMAN;
                selection_made = true;
            }
            else if (y >= 605 && y <= 685)
            {
                game_mode = PUZZLE_MODE;
                selection_made = true;
            }
            else if (y >= 715 && y <= 795)
            {
                game_mode = LEARNING_MODE;
                selection_made = true;
            }
        }
    }

    // --- Recreate display and initialize selected mode ---
    if (game_mode == VS_ENGINE)
    {
        al_destroy_display(display);
        display = al_create_display(1320, 1000);
        al_set_window_title(display, "Chess - VS Engine");
        al_set_display_icon(display, icon_img);

        // --- Choose engine team (W/B/R for random) ---
        team = select_player();
        if (team == 'R')
        {
            static bool seeded = false;
            if (!seeded)
            {
                srand(static_cast<unsigned int>(time(0)));
                seeded = true;
            }
            team = (rand() % 2 == 0) ? 'W' : 'B';
        }
    }
    else if (game_mode == VS_HUMAN)
    {
        al_destroy_display(display);
        display = al_create_display(1320, 960);
        al_set_window_title(display, "Chess - VS Human");
        al_set_display_icon(display, icon_img);
    }
    else if (game_mode == PUZZLE_MODE)
    {
        al_destroy_display(display);
        display = al_create_display(1320, 960);
        al_set_display_icon(display, icon_img);
        al_set_window_title(display, "Chess - Puzzle Mode");
        al_set_display_icon(display, icon_img);
        select_puzzle_mode();
    }
    else if (game_mode == LEARNING_MODE)
    {
        al_destroy_display(display);
        display = al_create_display(1320, 1000);
        al_set_window_title(display, "Chess - Assisted");
        al_set_display_icon(display, icon_img);
    }

    // --- Cleanup: destroy temporary resources ---
    al_destroy_font(title_font);
    al_destroy_font(option_font);
    al_destroy_font(sign_font);
    al_destroy_event_queue(tempQueue);

    return;
}

// --- Deals with user mouse input ---
void handle_mouse_events()
{
    // --- Debounce logic: Skip input briefly after certain actions ---
    if (suppress_mouse_input)
    {
        al_rest(0.25);                // Pause input for 0.25s
        suppress_mouse_input = false; // Reset flag
        piece_selected = false;       // Clear any selected piece
        return;                       // Skip this frame's input
    }

    // --- Handle Puzzle Mode: "Show Solution" button click ---
    if (game_mode == PUZZLE_MODE && ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && ev.mouse.x > 960)
    {
        int mx = ev.mouse.x;
        int my = ev.mouse.y;

        // --- Check if click was inside the button area ---
        if (mx >= btn_x && mx <= btn_x + btn_w &&
            my >= btn_y && my <= btn_y + btn_h)
        {
            // --- Format and display solution moves ---
            std::string moves;
            for (size_t i = 0; i < currentPuzzle.bestMoves.size(); ++i)
            {
                if (i % 2 == 0)
                    moves += std::to_string(i / 2 + 1) + ". ";
                moves += currentPuzzle.bestMoves[i] + " ";
                if (i % 2 == 1)
                    moves += "\n";
            }

            pop_message("Solution", moves, 2); // Show solution pop-up
            return;                            // Skip regular move logic
        }
    }

    // --- Handle Assisted Mode: "Get Best Move(s)" button click ---
    if (game_mode == LEARNING_MODE && ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && ev.mouse.x > 960)
    {
        int mx = ev.mouse.x;
        int my = ev.mouse.y;

        // --- Check if click was inside the button area ---
        if (mx >= btn_x && mx <= btn_x + btn_w &&
            my >= btn_y && my <= btn_y + btn_h)
        {
            top_white_moves = engine.get_best_white_moves(board.get_position());
            draw_screen();
            al_clear_to_color(al_map_rgb(0, 0, 0));  
            draw_screen();
            al_flip_display();
            // Clear after drawing to show once
            top_white_moves.clear();
            return;
        }
    }

    // --- Convert mouse coordinates to board grid indices ---
    i = ev.mouse.y / 120;
    j = ev.mouse.x / 120;

    // --- Handle Left Mouse Click (select/move piece) ---
    if (left_mouse_clicked() && !game_over)
    {
        switch (game_mode)
        {
        case VS_HUMAN:
            handle_vs_human_moves();
            break;

        case VS_ENGINE:
            handle_vs_engine_moves();
            break;

        case PUZZLE_MODE:
        case PUZZLE_RUSH:
            handle_puzzle_mode_moves();
            break;

        case PUZZLE_ENDGAME:
            handle_vs_engine_moves();
            break;

        case LEARNING_MODE:
            handle_Learning_mode();
            break;

        default:
            break;
        }
    }

    // --- Handle Right Mouse Click (cancel selection) ---
    if (right_mouse_clicked())
    {
        piece_selected = false; // Deselect any selected piece
    }
}

// --- Prompts the user to select a team (White, Random, Black) before engine match ---
char select_player()
{
    // --- Clear screen and draw dimmed overlay ---
    al_clear_to_color(al_map_rgb(0, 0, 0));
    draw_screen();
    al_draw_filled_rectangle(0, 0, 960, 960, al_map_rgba(0, 0, 0, 140));

    // --- Temporary event queue for mouse and display events ---
    ALLEGRO_EVENT_QUEUE *tempQueue = al_create_event_queue();
    al_register_event_source(tempQueue, al_get_mouse_event_source());
    al_register_event_source(tempQueue, al_get_display_event_source(display));

    // --- Layout variables for selection boxes ---
    int box_size = 120;
    int padding = 50;
    int start_x = (960 - (3 * box_size + 2 * padding)) / 2;
    int box_y = (960 - box_size) / 2;

    // --- Load font for title text ---
    ALLEGRO_FONT *title_font = al_load_ttf_font("files/gamefont2.ttf", 32, 0);

    // --- Define selection types and their corresponding images ---
    const char side_codes[3] = {'W', 'R', 'B'}; // White, Random, Black
    ALLEGRO_BITMAP *piece_images[3] = {
        white_king_img,
        random_img,
        black_king_img};

    // --- Initialize selection logic ---
    bool waiting = true;
    char selected = 'W';

    // --- Draw instruction text ---
    al_draw_text(title_font, al_map_rgb(255, 255, 255), 960 / 2, box_y - 60, ALLEGRO_ALIGN_CENTER, "Choose a side to play:");

    // --- Draw 3 selection boxes with icons ---
    for (int i = 0; i < 3; i++)
    {
        int x = start_x + i * (box_size + padding);

        al_draw_filled_rounded_rectangle(x, box_y, x + box_size, box_y + box_size, 12, 12, al_map_rgb(40, 40, 40));
        al_draw_rounded_rectangle(x, box_y, x + box_size, box_y + box_size, 12, 12, al_map_rgb(255, 255, 255), 3);

        ALLEGRO_BITMAP *img = piece_images[i];
        if (img)
        {
            int img_w = al_get_bitmap_width(img);
            int img_h = al_get_bitmap_height(img);
            float scale = std::min(box_size / (float)img_w, box_size / (float)img_h) * 0.7f;

            float cx = x + box_size / 2;
            float cy = box_y + box_size / 2;

            al_draw_scaled_bitmap(img, 0, 0, img_w, img_h,
                                  cx - img_w * scale / 2, cy - img_h * scale / 2,
                                  img_w * scale, img_h * scale, 0);
        }
    }

    // --- Display everything ---
    al_flip_display();

    // --- Wait for user to click a box ---
    while (waiting)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(tempQueue, &ev);

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            int mx = ev.mouse.x;
            int my = ev.mouse.y;

            for (int i = 0; i < 3; i++)
            {
                int x = start_x + i * (box_size + padding);
                if (mx >= x && mx <= x + box_size &&
                    my >= box_y && my <= box_y + box_size)
                {
                    selected = side_codes[i];
                    waiting = false;
                    break;
                }
            }
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            selected = 'W';
            break;
        }
    }

    // --- Cleanup: destroy temporary resources ---
    al_destroy_font(title_font);
    al_destroy_event_queue(tempQueue);

    return selected;
}

// --- Executes engine's move based on current turn (White or Black) ---
void perform_engine_move()
{
    start_time = clock();

    // --- Generate the best move from engine for the current turn ---
    EngineMove move = (turn == WHITE)
                          ? engine.make_white_move(board.get_position())
                          : engine.make_black_move(board.get_position());

    // --- Save evaluation info for UI/analysis ---
    evaluation = move.eval;
    nodes = move.nodes;

    // --- Store move notation and update internal state ---
    move_history.push_back(move.notation);
    selected_square_i = move.from_i;
    selected_square_j = move.from_j;
    i = move.to_i;
    j = move.to_j;
    piece_selected = true;

    // --- Adjust move history view if user hasn't manually scrolled ---
    if (!user_scrolled)
        move_history_offset = std::max(0, ((int)move_history.size() + 1) / 2 - MAX_VISIBLE_MOVES);
    user_scrolled = false;

    // --- Track engine time used ---
    time_used = (float)((int)(float(clock() - start_time))) / 1000;

    // --- Redraw board ---
    al_clear_to_color(al_map_rgb(0, 0, 0));
    draw_screen();
    al_flip_display();

    // --- Check if move results in checkmate ---
    if (board.king_is_in_check(BLACK) && board.is_checkmate(BLACK))
    {
        pop_message("Game Over", "White Wins!", 1);
        game_over = true;
        return;
    }
    if (board.king_is_in_check(WHITE) && board.is_checkmate(WHITE))
    {
        pop_message("Game Over", "Black Wins!", 1);
        game_over = true;
        return;
    }

    // --- Finalize engine move and switch turn ---
    board.reset_en_passant(turn);
    turn = (turn == WHITE ? BLACK : WHITE);
    piece_selected = false;
}

// --- Handles player interaction and alternating engine response in vs-engine mode ---
void handle_vs_engine_moves()
{
    // --- Engine plays first if player chose Black team ---
    if (move_history.empty() && team == 'B' && turn == WHITE && !game_over)
    {
        perform_engine_move();
        return;
    }

    // --- Cancel selection if same square clicked again ---
    if (i == selected_square_i && j == selected_square_j)
    {
        piece_selected = false;
        selected_square_i = -1;
        selected_square_j = -1;
    }
    // --- Select a piece if it belongs to the player and no piece is currently selected ---
    else if (!piece_selected && ((turn == WHITE && board.square_occupied_by_white(i, j)) ||
                                 (turn == BLACK && board.square_occupied_by_black(i, j))))
    {
        piece_selected = true;
        selected_square_i = i;
        selected_square_j = j;
    }
    // --- Attempt move if a piece is selected ---
    else if (piece_selected)
    {
        // --- Check if it's the human player's turn ---
        if ((team == 'W' && turn == WHITE) || (team == 'B' && turn == BLACK))
        {
            // --- Attempt the move ---
            string move_str = (turn == WHITE)
                                  ? board.handle_white_move(selected_square_i, selected_square_j, i, j)
                                  : human.handle_black_move(board.get_position(), selected_square_i, selected_square_j, i, j);

            if (move_str != "")
            {
                // --- Valid move: update move history and scroll position ---
                move_history.push_back(move_str);
                if (!user_scrolled)
                    move_history_offset = std::max(0, ((int)move_history.size() + 1) / 2 - MAX_VISIBLE_MOVES);
                user_scrolled = false;

                // --- Redraw board ---
                al_clear_to_color(al_map_rgb(0, 0, 0));
                draw_screen();
                al_flip_display();

                // --- Check for checkmate after player's move ---
                if (board.king_is_in_check(BLACK) && board.is_checkmate(BLACK))
                {
                    pop_message("Game Over", "White Wins!", 1);
                    game_over = true;
                    return;
                }
                if (board.king_is_in_check(WHITE) && board.is_checkmate(WHITE))
                {
                    pop_message("Game Over", "Black Wins!", 1);
                    game_over = true;
                    return;
                }

                // --- Finalize human move ---
                board.reset_en_passant(turn);
                turn = (turn == WHITE ? BLACK : WHITE);
                piece_selected = false;

                // --- Let engine respond if game isn't over ---
                if (!game_over)
                    perform_engine_move();
            }
            else
            {
                // --- Invalid move: clear selection and redraw ---
                piece_selected = false;
                selected_square_i = -1;
                selected_square_j = -1;
                al_clear_to_color(al_map_rgb(0, 0, 0));
                draw_screen();
                al_flip_display();
            }
        }
    }
    // --- Deselect if clicked on an empty/invalid square ---
    else if (piece_selected && ((turn == WHITE && !board.square_occupied_by_white(i, j)) ||
                                (turn == BLACK && !board.square_occupied_by_black(i, j))))
    {
        piece_selected = false;
    }
}

// --- Handles user input and move logic for Human vs Human mode ---
void handle_vs_human_moves()
{
    // --- Deselect if same square clicked again ---
    if (i == selected_square_i && j == selected_square_j)
    {
        piece_selected = false;
        selected_square_i = -1;
        selected_square_j = -1;
    }
    // --- Select a piece if none is selected and the clicked piece belongs to the current player ---
    else if (!piece_selected &&
             ((turn == WHITE && board.square_occupied_by_white(i, j)) ||
              (turn == BLACK && board.square_occupied_by_black(i, j))))
    {
        piece_selected = true;
        selected_square_i = i;
        selected_square_j = j;
    }
    // --- Attempt move if a piece is already selected ---
    else if (piece_selected)
    {
        std::string move_str = "";

        // --- Try to perform the move for the current player ---
        if (turn == WHITE)
        {
            move_str = board.handle_white_move(selected_square_i, selected_square_j, i, j);
        }
        else if (turn == BLACK)
        {
            move_str = human.handle_black_move(board.get_position(), selected_square_i, selected_square_j, i, j);
        }

        // --- If the move is legal ---
        if (move_str != "")
        {
            move_history.push_back(move_str);

            // --- Update move history display ---
            if (!user_scrolled)
                move_history_offset = std::max(0, ((int)move_history.size() + 1) / 2 - MAX_VISIBLE_MOVES);
            user_scrolled = false;

            // --- Redraw screen after move ---
            al_clear_to_color(al_map_rgb(0, 0, 0));
            draw_screen();
            al_flip_display();

            // --- Check for checkmate ---
            if (board.king_is_in_check(BLACK) && board.is_checkmate(BLACK))
            {
                pop_message("Game Over", "White Wins!", 1);
                game_over = true;
            }
            else if (board.king_is_in_check(WHITE) && board.is_checkmate(WHITE))
            {
                pop_message("Game Over", "Black Wins!", 1);
                game_over = true;
            }

            // --- Finalize move ---
            turn = (turn == WHITE) ? BLACK : WHITE;
            piece_selected = false;
        }
        // --- If move is invalid, deselect and redraw ---
        else
        {
            piece_selected = false;
            selected_square_i = -1;
            selected_square_j = -1;

            al_clear_to_color(al_map_rgb(0, 0, 0));
            draw_screen();
            al_flip_display();
        }
    }
    // --- If user clicked an invalid square after selecting, cancel selection ---
    else if (piece_selected &&
             ((turn == WHITE && !board.square_occupied_by_white(i, j)) ||
              (turn == BLACK && !board.square_occupied_by_black(i, j))))
    {
        piece_selected = false;
    }
}

// --- Handles player interactions during puzzle mode (both Classic and Puzzle Rush) ---
void handle_puzzle_mode_moves()
{
    // --- Cancel selection if same square is clicked again ---
    if (i == selected_square_i && j == selected_square_j)
    {
        piece_selected = false;
        selected_square_i = -1;
        selected_square_j = -1;
    }
    // --- Select a white piece ---
    else if (!piece_selected && board.square_occupied_by_white(i, j))
    {
        piece_selected = true;
        selected_square_i = i;
        selected_square_j = j;
    }
    // --- Attempt to make a move if a piece is already selected ---
    else if (piece_selected)
    {
        std::string move_str = board.handle_white_move(selected_square_i, selected_square_j, i, j);
        std::string move_coord = coords_to_string(selected_square_i, selected_square_j, i, j);

        if (move_str != "")
        {
            size_t current_index = currentPuzzle.playerMoves.size();

            // --- Invalid extra move beyond expected best moves ---
            if (current_index >= currentPuzzle.bestMoves.size())
            {
                pop_message("Puzzle Error", "Too many moves.", 1);
                return;
            }

            std::string expected = currentPuzzle.bestMoves[current_index];

            // --- Correct player move ---
            if (move_coord == expected.substr(0, 4))
            {
                currentPuzzle.playerMoves.push_back(move_coord);
                move_history.push_back(move_coord);

                // --- Switch turn to engine (black) ---
                board.reset_en_passant(BLACK);
                turn = BLACK;
                piece_selected = false;

                al_clear_to_color(al_map_rgb(0, 0, 0));
                draw_screen();
                al_flip_display();

                // --- Play engine's reply if available ---
                current_index++;
                if (current_index < currentPuzzle.bestMoves.size())
                {
                    std::string engine_move = currentPuzzle.bestMoves[current_index];

                    play_move(engine_move);
                    move_history.push_back(engine_move);
                    currentPuzzle.playerMoves.push_back(engine_move);

                    // --- Animate engine move ---
                    int from_file = engine_move[0] - 'a';
                    int from_rank = '8' - engine_move[1];
                    int to_file = engine_move[2] - 'a';
                    int to_rank = '8' - engine_move[3];

                    selected_square_i = from_rank;
                    selected_square_j = from_file;
                    i = to_rank;
                    j = to_file;
                    piece_selected = true;

                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    draw_screen();
                    al_flip_display();

                    board.reset_en_passant(WHITE);
                    turn = WHITE;

                    piece_selected = false;
                    selected_square_i = selected_square_j = -1;

                    // --- Puzzle completed after engine move ---
                    if (currentPuzzle.playerMoves.size() == currentPuzzle.bestMoves.size())
                    {
                        game_over = true;

                        // --- Handle Puzzle Rush success ---
                        if (game_mode == PUZZLE_RUSH)
                        {
                            if (currentPuzzle.themes == "Easy")
                                puzzleRush.score += 10;
                            else if (currentPuzzle.themes == "Medium")
                                puzzleRush.score += 20;
                            else if (currentPuzzle.themes == "Hard")
                                puzzleRush.score += 30;

                            reset_board_state(currentPuzzle.puzzle_board_state);
                            currentPuzzle.playerMoves.clear();
                            move_history.clear();
                            start_puzzle();
                            setup_puzzle_on_board();
                        }
                        else
                        {
                            pop_message("Success", "Well done! Puzzle completed.", 1);
                        }
                    }
                }
                // --- Puzzle completed if no more engine move ---
                else
                {
                    game_over = true;

                    if (game_mode == PUZZLE_RUSH)
                    {
                        if (currentPuzzle.themes == "Easy")
                            puzzleRush.score += 10;
                        else if (currentPuzzle.themes == "Medium")
                            puzzleRush.score += 20;
                        else if (currentPuzzle.themes == "Hard")
                            puzzleRush.score += 30;

                        reset_board_state(currentPuzzle.puzzle_board_state);
                        currentPuzzle.playerMoves.clear();
                        move_history.clear();
                        start_puzzle();
                        setup_puzzle_on_board();
                    }
                    else
                    {
                        pop_message("Success", "Well done! Puzzle completed.", 1);
                    }
                }
            }
            // --- Incorrect player move ---
            else
            {
                piece_selected = false;
                selected_square_i = -1;
                selected_square_j = -1;

                if (game_mode == PUZZLE_RUSH)
                {
                    puzzleRush.lives--;

                    // --- Game over in Puzzle Rush ---
                    if (puzzleRush.lives <= 0)
                    {
                        puzzleRush.time_elapsed = (float)(clock() - puzzleRush.start_time) / CLOCKS_PER_SEC;
                        puzzleRush.active = false;

                        // --- Load high score ---
                        int saved_score = 0;
                        float saved_time = 0;
                        std::ifstream infile("puzzles/puzzle_rush_score.txt");
                        if (infile.is_open())
                        {
                            std::string line;
                            std::getline(infile, line);
                            std::stringstream ss(line);
                            ss >> saved_score >> saved_time;
                            infile.close();
                        }

                        // --- Check and save new high score ---
                        bool is_new_highscore = false;
                        if (puzzleRush.score > saved_score)
                            is_new_highscore = true;
                        else if (puzzleRush.score == saved_score && puzzleRush.time_elapsed < saved_time)
                            is_new_highscore = true;

                        if (is_new_highscore)
                        {
                            std::ofstream outfile("puzzles/puzzle_rush_score.txt");
                            if (outfile.is_open())
                            {
                                outfile << puzzleRush.score << " " << puzzleRush.time_elapsed << std::endl;
                                outfile.close();
                            }
                        }

                        // --- Show result message ---
                        std::string msg = "Score: " + std::to_string(puzzleRush.score) +
                                          "\nTime: " + std::to_string((int)puzzleRush.time_elapsed) + " sec\n";

                        if (is_new_highscore)
                            msg += "\nNew High Score!";
                        else
                            msg += "\nBest: " + std::to_string(saved_score) + " in " + std::to_string((int)saved_time) + " sec";

                        // --- Reset puzzle rush ---
                        puzzleRush.score = 0;
                        puzzleRush.time_elapsed = 0.0f;
                        puzzleRush.start_time = 0.0f;

                        pop_message("Puzzle Rush Over", msg, 1);
                    }
                    else
                    {
                        // --- Try next puzzle with remaining lives ---
                        currentPuzzle.playerMoves.clear();
                        move_history.clear();
                        setup_puzzle_on_board();
                    }
                }
                else
                {
                    pop_message("Incorrect", "That's not the right move.", 2);
                }
            }
        }
        // --- Move was invalid ---
        else
        {
            piece_selected = false;
            selected_square_i = -1;
            selected_square_j = -1;
            draw_screen();
            al_flip_display();
        }
    }
}

// --- Initializes and starts a new Puzzle Rush session ---
void handle_puzzle_rush()
{
    // --- Start the puzzle rush timer ---
    puzzleRush.start_time = clock();

    // --- Initialize lives and activate puzzle rush mode ---
    puzzleRush.lives = 3;
    puzzleRush.active = true;

    // --- Load and setup the first puzzle on the board ---
    start_puzzle();
    setup_puzzle_on_board();
}

// --- Handles setting up an endgame-themed puzzle for the user ---
void handle_endgame_puzzle()
{
    // --- Load a puzzle from the 'Endgame' difficulty category ---
    load_puzzle_by_difficulty(4);

    // --- Set up the loaded puzzle on the board ---
    setup_puzzle_on_board();
}

// --- Manages gameplay for Learning Mode ---
void handle_Learning_mode()
{
    // --- Deselect piece if same square is clicked again ---
    if (i == selected_square_i && j == selected_square_j)
    {
        piece_selected = false;
        selected_square_i = -1;
        selected_square_j = -1;
    }
    // --- Select piece based on whose turn it is ---
    else if (!piece_selected && ((turn == WHITE && board.square_occupied_by_white(i, j)) ||
                                 (turn == BLACK && board.square_occupied_by_black(i, j))))
    {
        piece_selected = true;
        selected_square_i = i;
        selected_square_j = j;
    }
    // --- A piece was selected previously; handle move logic ---
    else if (piece_selected)
    {
        // --- Handle White's turn ---
        if (turn == WHITE)
        {
            string move_str = board.handle_white_move(selected_square_i, selected_square_j, i, j);
            if (move_str != "")
            {
                // --- Add move to history and handle scrolling ---
                move_history.push_back(move_str);
                if (!user_scrolled)
                    move_history_offset = std::max(0, ((int)move_history.size() + 1) / 2 - MAX_VISIBLE_MOVES);

                user_scrolled = false;

                // --- Refresh screen ---
                al_clear_to_color(al_map_rgb(0, 0, 0));
                draw_screen();
                al_flip_display();

                // --- Check for checkmate after White move ---
                if (board.king_is_in_check(BLACK) && board.is_checkmate(BLACK))
                {
                    pop_message("Game Over", "White Wins!", 1);
                    game_over = true;
                    return;
                }

                // --- Switch to Black's turn ---
                board.reset_en_passant(BLACK);
                turn = BLACK;
                piece_selected = false;

                // --- Let the engine play as Black ---
                if (!game_over)
                {
                    // --- Start engine move timer ---
                    start_time = clock();

                    // --- Engine makes Black move ---
                    EngineMove move = engine.make_black_move(board.get_position());
                    evaluation = move.eval;
                    nodes = move.nodes;
                    move_history.push_back(move.notation);

                    // --- Prepare for animation/selection of engine move ---
                    selected_square_i = move.from_i;
                    selected_square_j = move.from_j;
                    i = move.to_i;
                    j = move.to_j;
                    piece_selected = true;

                    if (!user_scrolled)
                        move_history_offset = std::max(0, ((int)move_history.size() + 1) / 2 - MAX_VISIBLE_MOVES);
                    user_scrolled = false;

                    // --- Track engine move time ---
                    time_used = (float)((int)(float(clock() - start_time))) / 1000;

                    // --- Refresh screen ---
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    draw_screen();
                    al_flip_display();

                    // --- Check for checkmate after Black move ---
                    if (board.king_is_in_check(WHITE) && board.is_checkmate(WHITE))
                    {
                        pop_message("Game Over", "Black Wins!", 1);
                        game_over = true;
                    }

                    // --- Switch back to White's turn ---
                    board.reset_en_passant(WHITE);
                    turn = WHITE;
                    piece_selected = false;
                }
            }
            else
            {
                // --- Handle invalid move ---
                piece_selected = false;
                selected_square_i = -1;
                selected_square_j = -1;

                al_clear_to_color(al_map_rgb(0, 0, 0));
                draw_screen();
                al_flip_display();
            }
        }
    }
    // --- Clicked on wrong-color piece or empty square after selecting ---
    else if (piece_selected && ((turn == WHITE && !board.square_occupied_by_white(i, j)) ||
                                (turn == BLACK && !board.square_occupied_by_black(i, j))))
    {
        piece_selected = false;
    }
}

// --- Displays puzzle mode and difficulty selection UI ---
void select_puzzle_mode()
{
    al_clear_to_color(al_map_rgb(0, 0, 0));
    draw_screen();
    al_draw_filled_rectangle(0, 0, 960, 960, al_map_rgba(0, 0, 0, 140));

    ALLEGRO_EVENT_QUEUE *tempQueue = al_create_event_queue();
    al_register_event_source(tempQueue, al_get_mouse_event_source());
    al_register_event_source(tempQueue, al_get_display_event_source(display));

    bool selection_made = false;
    int difficulty = 0;

    const int box_size = 120;
    const int padding = 50;

    // --- Difficulty section ---
    const char *difficulties[] = {"Easy", "Medium", "Hard"};
    const int num_difficulties = 3;
    int diff_start_x = (960 - (num_difficulties * box_size + (num_difficulties - 1) * padding)) / 2;
    int diff_y = 300;

    // --- Game mode section ---
    const char *modes[] = {"Rush", "WildStart"};
    const int num_modes = 2;
    int mode_start_x = (960 - (num_modes * box_size + (num_modes - 1) * padding)) / 2;
    int mode_y = diff_y + 240;

    ALLEGRO_FONT *title_font = al_load_ttf_font("files/gamefont2.ttf", 32, 0);
    ALLEGRO_FONT *font = al_load_ttf_font("files/gamefont3.ttf", 30, 0);

    // --- Draw difficulty title and buttons ---
    al_draw_text(title_font, al_map_rgb(255, 255, 255), 960 / 2, diff_y - 60, ALLEGRO_ALIGN_CENTER, "Select Difficulty Level:");
    for (int i = 0; i < num_difficulties; ++i)
    {
        int x = diff_start_x + i * (box_size + padding);
        al_draw_filled_rounded_rectangle(x, diff_y, x + box_size, diff_y + box_size, 12, 12, al_map_rgb(40, 40, 40));
        al_draw_rounded_rectangle(x, diff_y, x + box_size, diff_y + box_size, 12, 12, al_map_rgb(255, 255, 255), 3);
        al_draw_text(font, al_map_rgb(255, 255, 255), x + box_size / 2, diff_y + 35, ALLEGRO_ALIGN_CENTER, difficulties[i]);
    }

    // --- Draw mode title and buttons ---
    al_draw_text(title_font, al_map_rgb(255, 255, 255), 960 / 2, mode_y - 60, ALLEGRO_ALIGN_CENTER, "Puzzle Modes:");
    for (int i = 0; i < num_modes; ++i)
    {
        int x = mode_start_x + i * (box_size + padding);
        al_draw_filled_rounded_rectangle(x, mode_y, x + box_size, mode_y + box_size, 12, 12, al_map_rgb(40, 40, 40));
        al_draw_rounded_rectangle(x, mode_y, x + box_size, mode_y + box_size, 12, 12, al_map_rgb(255, 255, 255), 3);
        al_draw_text(font, al_map_rgb(255, 255, 255), x + box_size / 2, mode_y + 35, ALLEGRO_ALIGN_CENTER, modes[i]);
    }

    al_flip_display();

    while (!selection_made)
    {
        suppress_mouse_input = true;

        ALLEGRO_EVENT ev;
        al_wait_for_event(tempQueue, &ev);

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            int mx = ev.mouse.x;
            int my = ev.mouse.y;

            // --- Difficulty click detection ---
            for (int i = 0; i < num_difficulties; ++i)
            {
                int x = diff_start_x + i * (box_size + padding);
                if (mx >= x && mx <= x + box_size && my >= diff_y && my <= diff_y + box_size)
                {
                    difficulty = i + 1;
                    selection_made = true;
                    break;
                }
            }

            // --- Mode click detection ---
            for (int i = 0; i < num_modes && !selection_made; ++i)
            {
                int x = mode_start_x + i * (box_size + padding);
                if (mx >= x && mx <= x + box_size && my >= mode_y && my <= mode_y + box_size)
                {
                    if (strcmp(modes[i], "Rush") == 0)
                    {
                        game_mode = PUZZLE_RUSH;
                        al_destroy_font(title_font);
                        al_destroy_font(font);
                        al_destroy_event_queue(tempQueue);
                        handle_puzzle_rush();
                        return;
                    }
                    else if (strcmp(modes[i], "WildStart") == 0)
                    {
                        game_mode = PUZZLE_ENDGAME;
                        al_destroy_font(title_font);
                        al_destroy_font(font);
                        al_destroy_event_queue(tempQueue);
                        handle_endgame_puzzle();
                        return;
                    }
                }
            }
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            exit(0);
        }
    }

    // --- Cleanup: destroy temporary resources ---
    al_destroy_font(title_font);
    al_destroy_font(font);
    al_destroy_event_queue(tempQueue);

    if (!load_puzzle_by_difficulty(difficulty))
    {
        pop_message("Error", "Failed to load puzzle.");
        return;
    }

    setup_puzzle_on_board();
}

// --- Sets up the current puzzle on the board ---
void setup_puzzle_on_board()
{
    game_over = false;
    board.load_position(currentPuzzle.puzzle_board_state);

    // --- Play the last move from where the puzzle starts ---
    if (!currentPuzzle.bestMoves.empty())
    {
        std::string move = currentPuzzle.bestMoves[0];
        play_move(move);
        currentPuzzle.playerMoves.push_back(move);
        move_history.push_back(move);

        // --- Extract and highlight the move on board ---
        int from_file = move[0] - 'a';
        int from_rank = '8' - move[1];
        int to_file = move[2] - 'a';
        int to_rank = '8' - move[3];

        selected_square_i = from_rank;
        selected_square_j = from_file;
        i = to_rank;
        j = to_file;
        piece_selected = true;

        al_clear_to_color(al_map_rgb(0, 0, 0));
        draw_screen();
        al_flip_display();

        piece_selected = false;

        // --- Set turn to White after move ---
        turn = WHITE;
        board.reset_en_passant(WHITE);
    }
}

// --- Draws the entire chess screen, including board, pieces, highlights, UI, and overlays ---
void draw_screen()
{
    // --- Draw background image ---
    al_draw_bitmap(background_img, 0, 0, 0);

    // --- Show top 3 recommended moves in Learning Mode ---
    if (game_mode == LEARNING_MODE && !top_white_moves.empty())
    {
        ALLEGRO_COLOR highlight_colors[3] = {
            al_map_rgba(255, 153, 153, 150), // Light Red
            al_map_rgba(153, 255, 153, 150), // Light Green
            al_map_rgba(255, 255, 153, 150)  // Light Yellow
        };

        ALLEGRO_COLOR border_colors[3] = {
            al_map_rgb(255, 102, 102), // Red border
            al_map_rgb(102, 255, 102), // Green border
            al_map_rgb(255, 255, 102)  // Yellow border
        };

        for (size_t idx = 0; idx < top_white_moves.size() && idx < 3; ++idx)
        {
            EngineMove move = top_white_moves[idx];

            // Highlight FROM square
            al_draw_filled_rectangle(
                move.from_j * SQUARE_SIZE, move.from_i * SQUARE_SIZE,
                (move.from_j + 1) * SQUARE_SIZE - 1, (move.from_i + 1) * SQUARE_SIZE - 1,
                highlight_colors[idx]);

            al_draw_rectangle(
                move.from_j * SQUARE_SIZE, move.from_i * SQUARE_SIZE,
                (move.from_j + 1) * SQUARE_SIZE - 1, (move.from_i + 1) * SQUARE_SIZE - 1,
                border_colors[idx],
                3);

            // Highlight TO square
            al_draw_filled_rectangle(
                move.to_j * SQUARE_SIZE, move.to_i * SQUARE_SIZE,
                (move.to_j + 1) * SQUARE_SIZE - 1, (move.to_i + 1) * SQUARE_SIZE - 1,
                highlight_colors[idx]);

            al_draw_rectangle(
                move.to_j * SQUARE_SIZE, move.to_i * SQUARE_SIZE,
                (move.to_j + 1) * SQUARE_SIZE - 1, (move.to_i + 1) * SQUARE_SIZE - 1,
                border_colors[idx],
                3);
        }
    }

    // --- Highlight selected piece and its destination ---
    if (piece_selected)
    {
        // Selected square highlight
        al_draw_filled_rectangle(
            selected_square_j * SQUARE_SIZE, selected_square_i * SQUARE_SIZE,
            (selected_square_j + 1) * SQUARE_SIZE - 1, (selected_square_i + 1) * SQUARE_SIZE - 1,
            al_map_rgba(144, 222, 245, 150));

        al_draw_rectangle(
            selected_square_j * SQUARE_SIZE, selected_square_i * SQUARE_SIZE,
            (selected_square_j + 1) * SQUARE_SIZE - 1, (selected_square_i + 1) * SQUARE_SIZE - 1,
            al_map_rgb(119, 170, 242), 3);

        // Destination square highlight
        al_draw_filled_rectangle(
            j * SQUARE_SIZE, i * SQUARE_SIZE,
            (j + 1) * SQUARE_SIZE - 1, (i + 1) * SQUARE_SIZE - 1,
            al_map_rgba(144, 222, 245, 150));

        al_draw_rectangle(
            j * SQUARE_SIZE, i * SQUARE_SIZE,
            (j + 1) * SQUARE_SIZE - 1, (i + 1) * SQUARE_SIZE - 1,
            al_map_rgb(119, 170, 242), 3);
    }

    // --- Draw all chess pieces based on board state ---
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board.square_occupied(i, j))
            {
                draw_piece(i, j);
            }
        }
    }

    // --- Draw move history and evaluation bar if needed ---
    draw_move_history(ev);

    if (game_mode == VS_ENGINE || game_mode == LEARNING_MODE)
    {
        draw_evaluation_bar();

        // --- Draw 'Hint' button in Learning Mode ---
        if (game_mode == LEARNING_MODE)
        {
            ALLEGRO_FONT *font = al_load_ttf_font("files/gamefont2.ttf", 28, 0);

            al_draw_filled_rounded_rectangle(
                btn_x, btn_y,
                btn_x + btn_w, btn_y + btn_h,
                10, 10, al_map_rgb(70, 70, 70));

            al_draw_text(font, al_map_rgb(255, 255, 255),
                         btn_x + btn_w / 2, btn_y + 8,
                         ALLEGRO_ALIGN_CENTER, "Hint");

            al_destroy_font(font);
        }
    }

    // --- Draw puzzle details if in Puzzle Mode or Puzzle Rush ---
    if (game_mode == PUZZLE_MODE || game_mode == PUZZLE_RUSH)
    {
        draw_details();
    }
}

// --- Draws a chess piece on the board at grid position (i, j) ---
void draw_piece(int i, int j)
{
    ALLEGRO_BITMAP *piece = NULL;

    // --- Get the corresponding bitmap image for the piece type ---
    switch (board.get_piece(i, j))
    {
    case WHITE_PAWN:
        piece = white_pawn_img;
        break;
    case WHITE_KNIGHT:
        piece = white_knight_img;
        break;
    case WHITE_BISHOP:
        piece = white_bishop_img;
        break;
    case WHITE_ROOK:
        piece = white_rook_img;
        break;
    case WHITE_QUEEN:
        piece = white_queen_img;
        break;
    case WHITE_KING:
        piece = white_king_img;
        break;

    case BLACK_PAWN:
        piece = black_pawn_img;
        break;
    case BLACK_KNIGHT:
        piece = black_knight_img;
        break;
    case BLACK_BISHOP:
        piece = black_bishop_img;
        break;
    case BLACK_ROOK:
        piece = black_rook_img;
        break;
    case BLACK_QUEEN:
        piece = black_queen_img;
        break;
    case BLACK_KING:
        piece = black_king_img;
        break;

    default:
        break; // No piece to draw
    }

    // --- Draw the piece bitmap at the correct screen position ---
    al_draw_bitmap(piece, j * SQUARE_SIZE, i * SQUARE_SIZE, 0);
}

// --- Displays the promotion menu and returns selected piece code ('q', 'r', 'b', 'k') ---
char show_promotion_menu(bool is_white)
{
    // --- Layout configuration ---
    const int screen_width = 960;
    const int screen_height = 960;
    const int box_size = 120;
    const int spacing = 20;
    const int total_width = 4 * box_size + 3 * spacing;
    const int start_x = (screen_width - total_width) / 2;
    const int box_y = (screen_height - box_size) / 2;

    // --- Load font for the title text ---
    ALLEGRO_FONT *title_font = al_load_ttf_font("files/gamefont2.ttf", 32, 0);

    // --- Set up mouse and display event queue ---
    ALLEGRO_EVENT_QUEUE *temp_queue = al_create_event_queue();
    al_register_event_source(temp_queue, al_get_mouse_event_source());
    al_register_event_source(temp_queue, al_get_display_event_source(display));

    // --- Setup promotion options ---
    const char piece_codes[4] = {'k', 'b', 'r', 'q'};
    ALLEGRO_BITMAP *piece_images[4] = {
        is_white ? white_knight_img : black_knight_img,
        is_white ? white_bishop_img : black_bishop_img,
        is_white ? white_rook_img : black_rook_img,
        is_white ? white_queen_img : black_queen_img};

    bool waiting = true;
    char selected = 'q';

    // --- Dim background and show prompt ---
    draw_screen();
    al_draw_filled_rectangle(0, 0, screen_width, screen_height, al_map_rgba(0, 0, 0, 140));
    al_draw_text(title_font, al_map_rgb(255, 255, 255), screen_width / 2, box_y - 60, ALLEGRO_ALIGN_CENTER, "Choose a piece to promote to");

    // --- Draw each promotion option ---
    for (int i = 0; i < 4; i++)
    {
        int x = start_x + i * (box_size + spacing);

        // Draw selection box
        al_draw_filled_rounded_rectangle(x, box_y, x + box_size, box_y + box_size, 12, 12, al_map_rgb(40, 40, 40));
        al_draw_rounded_rectangle(x, box_y, x + box_size, box_y + box_size, 12, 12, al_map_rgb(255, 255, 255), 3);

        // Draw scaled piece image inside the box
        ALLEGRO_BITMAP *img = piece_images[i];
        if (img)
        {
            int img_w = al_get_bitmap_width(img);
            int img_h = al_get_bitmap_height(img);
            float scale = std::min(box_size / (float)img_w, box_size / (float)img_h) * 0.7f;

            float cx = x + box_size / 2;
            float cy = box_y + box_size / 2;

            al_draw_scaled_bitmap(img, 0, 0, img_w, img_h,
                                  cx - img_w * scale / 2, cy - img_h * scale / 2,
                                  img_w * scale, img_h * scale, 0);
        }
    }

    al_flip_display();

    // --- Wait for user input ---
    while (waiting)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(temp_queue, &ev);

        // Mouse click: check if a piece was selected
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            int mx = ev.mouse.x;
            int my = ev.mouse.y;

            for (int i = 0; i < 4; i++)
            {
                int x = start_x + i * (box_size + spacing);
                if (mx >= x && mx <= x + box_size &&
                    my >= box_y && my <= box_y + box_size)
                {
                    selected = piece_codes[i];
                    waiting = false;
                    break;
                }
            }
        }
        // Display closed: fallback to queen promotion
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            selected = 'q';
            break;
        }
    }

    // --- Cleanup ---
    al_destroy_font(title_font);
    al_destroy_event_queue(temp_queue);

    return selected;
}

// --- Draws the move history panel with scroll support via mouse clicks ---
void draw_move_history(ALLEGRO_EVENT ev)
{
    // --- Load fonts for title and move text ---
    ALLEGRO_FONT *title_font = al_load_ttf_font("files/gamefont2.ttf", 32, 0);
    ALLEGRO_FONT *move_font = al_load_ttf_font("files/gamefont2.ttf", 26, 0);

    // --- Layout configuration ---
    int start_x = 980;
    int start_y = 40;
    int area_height = 740;
    int line_spacing = 30;
    int arrow_size = 20;

    int arrow_x_center = start_x + 160;
    int up_arrow_y = start_y + 15;
    int down_arrow_y = start_y + area_height - arrow_size - 10;

    // --- Calculate rows and constrain scroll offset ---
    int total_rows = (move_history.size() + 1) / 2;
    if (move_history_offset < 0)
        move_history_offset = 0;
    if (move_history_offset > total_rows - MAX_VISIBLE_MOVES)
        move_history_offset = std::max(0, total_rows - MAX_VISIBLE_MOVES);

    // --- Handle mouse clicks on scroll arrows ---
    if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1 && ev.mouse.x > 960)
    {
        int mx = ev.mouse.x;
        int my = ev.mouse.y;

        if (mx >= arrow_x_center - arrow_size && mx <= arrow_x_center + arrow_size)
        {
            if (my >= up_arrow_y && my <= up_arrow_y + arrow_size && move_history_offset > 0)
            {
                move_history_offset--;
                user_scrolled = true;
            }
            else if (my >= down_arrow_y && my <= down_arrow_y + arrow_size &&
                     move_history_offset + MAX_VISIBLE_MOVES < total_rows)
            {
                move_history_offset++;
                user_scrolled = true;
            }
        }
    }

    // --- Draw background and title ---
    al_draw_filled_rounded_rectangle(start_x - 10, start_y - 30, 1310, start_y + area_height, 15, 15, al_map_rgb(40, 40, 40));
    al_draw_text(title_font, al_map_rgb(255, 255, 255), arrow_x_center, start_y - 20, ALLEGRO_ALIGN_CENTER, "Move History");

    // --- Draw scroll arrows if needed ---
    if (move_history_offset > 0)
    {
        al_draw_filled_triangle(
            arrow_x_center, up_arrow_y,
            arrow_x_center - arrow_size / 2, up_arrow_y + arrow_size,
            arrow_x_center + arrow_size / 2, up_arrow_y + arrow_size,
            al_map_rgb(255, 255, 255));
    }
    if (move_history_offset + MAX_VISIBLE_MOVES < total_rows)
    {
        al_draw_filled_triangle(
            arrow_x_center, down_arrow_y + arrow_size,
            arrow_x_center - arrow_size / 2, down_arrow_y,
            arrow_x_center + arrow_size / 2, down_arrow_y,
            al_map_rgb(255, 255, 255));
    }

    // --- Draw each move line (e.g., 1. e4 e5) ---
    int move_number = move_history_offset + 1;
    int y = start_y + arrow_size + 25;

    for (size_t i = move_history_offset * 2; i < move_history.size() && (move_number - move_history_offset) <= MAX_VISIBLE_MOVES; i += 2)
    {
        std::string white_move = move_history[i];
        std::string black_move = ((i + 1) < move_history.size()) ? move_history[i + 1] : "";

        std::string move_label = std::to_string(move_number) + ". ";

        al_draw_text(move_font, al_map_rgb(255, 255, 255), start_x + 10, y, 0, move_label.c_str());
        al_draw_text(move_font, al_map_rgb(255, 255, 255), start_x + 65, y, 0, white_move.c_str());
        al_draw_text(move_font, al_map_rgb(255, 255, 255), start_x + 200, y, 0, black_move.c_str());

        move_number++;
        y += line_spacing;
    }

    // --- Free resources ---
    al_destroy_font(title_font);
    al_destroy_font(move_font);
}

// --- Draws a horizontal evaluation bar at the bottom of the screen ---
void draw_evaluation_bar()
{
    // --- Load font for text labels ---
    ALLEGRO_FONT *font = al_load_ttf_font("files/gamefont2.ttf", 24, 0);

    // --- Position and size settings ---
    int start_x = 0;
    int start_y = 960;
    int bar_width = 960;
    int bar_height = 40;

    // --- Clamp evaluation to avoid extreme values ---
    if (evaluation > 15.0f)
        evaluation = 15.0f;
    if (evaluation < -15.0f)
        evaluation = -15.0f;

    // --- Normalize evaluation to range [0, 1] for drawing proportionally ---
    float normalized = (evaluation + 15.0f) / 30.0f;

    int white_width = static_cast<int>(bar_width * normalized);
    int black_width = bar_width - white_width;

    // --- Draw black (lower half) part of bar ---
    al_draw_filled_rectangle(start_x, start_y, start_x + black_width, start_y + bar_height, al_map_rgb(0, 0, 0));

    // --- Draw white (upper half) part of bar ---
    al_draw_filled_rectangle(start_x + black_width, start_y, start_x + bar_width, start_y + bar_height, al_map_rgb(210, 210, 210));

    // --- Draw outer border around the bar ---
    al_draw_rectangle(start_x, start_y, start_x + bar_width, start_y + bar_height, al_map_rgb(200, 200, 200), 5);

    // --- Prepare evaluation, node count, and time strings ---
    char eval_text[30];
    snprintf(eval_text, sizeof(eval_text), "Evaluation: %.2f", evaluation);

    char nodes_text[30];
    snprintf(nodes_text, sizeof(nodes_text), "Nodes : %d", nodes);

    char time_text[30];
    snprintf(time_text, sizeof(time_text), "Time Taken: %.2f seconds", time_used);

    // --- Draw stats text to the right of the bar ---
    al_draw_text(font, al_map_rgb(255, 255, 255), start_x + bar_width + 20, start_y - 70, ALLEGRO_ALIGN_LEFT, nodes_text);
    al_draw_text(font, al_map_rgb(255, 255, 255), start_x + bar_width + 20, start_y - 29, ALLEGRO_ALIGN_LEFT, time_text);
    al_draw_text(font, al_map_rgb(255, 255, 255), start_x + bar_width + 20, start_y + 12, ALLEGRO_ALIGN_LEFT, eval_text);

    // --- Clean up font resources ---
    al_destroy_font(font);
}

// --- Draws puzzle-related details and UI elements in the right panel ---
void draw_details()
{
    // --- Load font for text display ---
    ALLEGRO_FONT *font = al_load_ttf_font("files/gamefont2.ttf", 28, 0);
    if (!font)
        return;

    // --- Position settings ---
    int x = 970;
    int y = 960;

    // --- Construct and draw puzzle ID and rating text ---
    std::string id = "Puzzle ID: " + currentPuzzle.id;
    std::string rating = "Rating: " + currentPuzzle.rating + "   " + currentPuzzle.themes;

    al_draw_text(font, al_map_rgb(255, 255, 255), x, y - 70, ALLEGRO_ALIGN_LEFT, id.c_str());
    al_draw_text(font, al_map_rgb(255, 255, 255), x, y - 29, ALLEGRO_ALIGN_LEFT, rating.c_str());

    // --- Draw "Solution" button if in PUZZLE_MODE ---
    if (game_mode == PUZZLE_MODE)
    {
        al_draw_filled_rounded_rectangle(
            btn_x, btn_y,
            btn_x + btn_w, btn_y + btn_h,
            10, 10, al_map_rgb(70, 70, 70));

        al_draw_text(font, al_map_rgb(255, 255, 255),
                     btn_x + btn_w / 2, btn_y + 8,
                     ALLEGRO_ALIGN_CENTER, "Solution");
    }

    // --- Display Puzzle Rush stats if active ---
    if (game_mode == PUZZLE_RUSH)
    {
        std::string lives = "Lives Remaining: " + std::to_string(puzzleRush.lives);
        std::string score = "Score: " + std::to_string(puzzleRush.score);

        al_draw_text(font, al_map_rgb(255, 255, 255), x + 10, y - 150, 0, lives.c_str());
        al_draw_text(font, al_map_rgb(255, 255, 255), x + 10, y - 110, 0, score.c_str());
    }

    // --- Clean up font resources ---
    al_destroy_font(font);
}

// --- Reusable pop-up message with optional action buttons ---
void pop_message(const string &title, const string &message, int type)
{
    // --- Box dimensions and screen center positioning ---
    int box_width = 480;
    int box_height = 350;
    int screen_width = 960;
    int screen_height = 960;

    int box_x = (screen_width - box_width) / 2;
    int box_y = (screen_height - box_height) / 2;

    // --- Load fonts for title and message ---
    ALLEGRO_FONT *title_font = al_load_ttf_font("files/gamefont2.ttf", 42, 0);
    ALLEGRO_FONT *text_font = al_load_ttf_font("files/gamefont2.ttf", 26, 0);

    // --- Prepare multiline message drawing ---
    int line_height = al_get_font_line_height(text_font);
    int y_offset = box_y + 100;

    // --- Setup event queue for mouse and display input ---
    bool waiting = true;
    ALLEGRO_EVENT_QUEUE *temp_queue = al_create_event_queue();
    al_register_event_source(temp_queue, al_get_mouse_event_source());
    al_register_event_source(temp_queue, al_get_display_event_source(display));

    // --- Dim background and draw centered pop-up box ---
    draw_screen();
    al_draw_filled_rectangle(0, 0, screen_width, screen_height, al_map_rgba(0, 0, 0, 120));
    al_draw_filled_rounded_rectangle(box_x, box_y, box_x + box_width, box_y + box_height, 15, 15, al_map_rgb(30, 30, 30));
    al_draw_rounded_rectangle(box_x, box_y, box_x + box_width, box_y + box_height, 15, 15, al_map_rgb(255, 255, 255), 4);

    // --- Title ---
    al_draw_text(title_font, al_map_rgb(255, 255, 255), screen_width / 2, box_y + 30, ALLEGRO_ALIGN_CENTER, title.c_str());

    // --- Message text (supporting multiline) ---
    std::istringstream stream(message);
    std::string line;
    while (std::getline(stream, line))
    {
        al_draw_text(text_font, al_map_rgb(220, 220, 220), screen_width / 2, y_offset, ALLEGRO_ALIGN_CENTER, line.c_str());
        y_offset += line_height + 5;
    }

    // --- Button dimensions and positions ---
    int btn_w = 120, btn_h = 40;
    int btn_y = box_y + box_height - 60;
    int close_x = box_x + box_width / 2 - btn_w / 2;
    int restart_x = box_x + 40;
    int next_x = box_x + box_width - 40 - btn_w;

    // --- Draw main button based on type ---
    if (type == 0)
        al_draw_text_button(close_x, btn_y, btn_w, btn_h, "Close", text_font);
    else if (type == 1)
        al_draw_text_button(close_x, btn_y, btn_w, btn_h, "Home", text_font);
    else if (type == 2)
        al_draw_text_button(close_x, btn_y, btn_w, btn_h, "OK", text_font);

    // --- Draw additional buttons in PUZZLE_MODE ---
    if (game_mode == PUZZLE_MODE)
    {
        al_draw_text_button(restart_x, btn_y, btn_w, btn_h, "Restart", text_font);
        al_draw_text_button(next_x, btn_y, btn_w, btn_h, "Next", text_font);
    }

    al_flip_display();

    // --- Event loop: wait for button click or display close ---
    while (waiting)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(temp_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            int mx = ev.mouse.x;
            int my = ev.mouse.y;

            // --- Handle main button clicks ---
            if (mx >= close_x && mx <= close_x + btn_w &&
                my >= btn_y && my <= btn_y + btn_h)
            {
                if (type == 0)
                    exit(0);        // Exit game
                else if (type == 1) // Go to Home screen
                {
                    reset_board_state(currentPuzzle.puzzle_board_state);
                    currentPuzzle.playerMoves.clear();
                    move_history.clear();

                    al_destroy_display(display);
                    al_destroy_event_queue(event_queue);

                    display = al_create_display(960, 960);
                    event_queue = al_create_event_queue();

                    al_set_window_title(display, "Chess");
                    al_set_display_icon(display, icon_img);

                    al_register_event_source(event_queue, al_get_mouse_event_source());
                    display_start_menu();
                    al_register_event_source(event_queue, al_get_display_event_source(display));
                    suppress_mouse_input = true;
                }
                else if (type == 2) // OK - reload current puzzle
                {
                    currentPuzzle.playerMoves.clear();
                    move_history.clear();
                    suppress_mouse_input = true;
                    setup_puzzle_on_board();
                }

                break;
            }

            // --- Handle Puzzle Mode-specific buttons ---
            if (game_mode == PUZZLE_MODE)
            {
                if (mx >= restart_x && mx <= restart_x + btn_w &&
                    my >= btn_y && my <= btn_y + btn_h)
                {
                    currentPuzzle.playerMoves.clear();
                    move_history.clear();
                    suppress_mouse_input = true;
                    setup_puzzle_on_board();
                    break;
                }

                if (mx >= next_x && mx <= next_x + btn_w &&
                    my >= btn_y && my <= btn_y + btn_h)
                {
                    int diff = (currentPuzzle.themes == "Hard") ? 3 : (currentPuzzle.themes == "Medium") ? 2
                                                                                                         : 1;

                    reset_board_state(currentPuzzle.puzzle_board_state);
                    currentPuzzle.playerMoves.clear();
                    move_history.clear();
                    suppress_mouse_input = true;
                    load_puzzle_by_difficulty(diff);
                    setup_puzzle_on_board();
                    break;
                }
            }
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            waiting = false;
            exit(0);
        }
    }

    // --- Cleanup ---
    al_destroy_font(title_font);
    al_destroy_font(text_font);
    al_destroy_event_queue(temp_queue);
}

// --- Helper to reduce repetition for button drawing ---
void al_draw_text_button(int x, int y, int w, int h, const char *label, ALLEGRO_FONT *font)
{
    al_draw_filled_rounded_rectangle(x, y, x + w, y + h, 10, 10, al_map_rgb(90, 90, 90));
    al_draw_rounded_rectangle(x, y, x + w, y + h, 10, 10, al_map_rgb(255, 255, 255), 2);
    al_draw_text(font, al_map_rgb(255, 255, 255), x + w / 2, y + 8, ALLEGRO_ALIGN_CENTER, label);
}

// --- Converts board coordinates to standard chess notation ---
string coords_to_string(int from_i, int from_j, int to_i, int to_j)
{
    // --- Lambda to convert a single (row, col) pair to chess notation ---
    auto to_coord = [](int row, int col) -> string
    {
        char file = 'a' + col;                    // Convert column index to file (a-h)
        char rank = '8' - row;                    // Convert row index to rank (1-8)
        return string(1, file) + string(1, rank); // Return combined notation
    };

    // --- Return full move string from source to destination ---
    return to_coord(from_i, from_j) + to_coord(to_i, to_j);
}

// --- Returns true if left mouse button is clicked within the board area ---
bool left_mouse_clicked()
{
    // --- Check if left button (bit 1) is pressed and within board bounds (960x960) ---
    if (ev.mouse.button & 1 && ev.mouse.y < 960 && ev.mouse.x < 960)
    {
        int x = ev.mouse.y / 120; // --- Get board row based on y coordinate ---
        int y = ev.mouse.x / 120; // --- Get board column based on x coordinate ---

        // --- Clear screen and redraw with highlight around selected square ---
        al_clear_to_color(al_map_rgb(0, 0, 0));
        draw_screen();
        al_draw_rectangle(
            y * 120 - 1, x * 120 - 1,
            (y + 1) * 120 - 1, (x + 1) * 120 - 1,
            al_map_rgb(119, 170, 242), 3);

        al_flip_display();
        return true;
    }
    return false;
}

// --- Returns true if right mouse button is clicked ---
bool right_mouse_clicked()
{
    if (ev.mouse.button & 2) // --- Check if right mouse button (bit 2) is pressed ---
    {
        return true;
    }
    return false;
}
