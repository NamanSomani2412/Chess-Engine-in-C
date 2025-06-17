// --------------------------------------------------------------------------------------
// board.h
// --- Declares the Board class and essential data structures used to represent and 
// --- manipulate the chessboard state.
// --- Contains enums, constants for piece types, direction arrays for movement logic, 
// --- and the `board_state` structure for tracking full board status including castling 
// --- and en passant conditions.
// --- Provides declarations for move legality checks, control detection, move execution, 
// --- checkmate/stalemate detection, and notation generation.
// --------------------------------------------------------------------------------------

#ifndef BOARD_H
#define BOARD_H

// --- Include necessary headers ---
#include <sstream>
#include <string>
#include <functional>

// --- Define sides ---
enum side
{
    WHITE,
    BLACK
};

// --- Piece constants ---
const char EMPTY = 0;
const char WHITE_PAWN = 1;
const char WHITE_KNIGHT = 2;
const char WHITE_BISHOP = 3;
const char WHITE_ROOK = 4;
const char WHITE_QUEEN = 5;
const char WHITE_KING = 6;
const char BLACK_PAWN = -1;
const char BLACK_KNIGHT = -2;
const char BLACK_BISHOP = -3;
const char BLACK_ROOK = -4;
const char BLACK_QUEEN = -5;
const char BLACK_KING = -6;

// --- Pawn starting rows ---
const int WHITE_PAWN_STARTING_ROWN = 6;
const int BLACK_PAWN_STARTING_ROWN = 1;

// --- Movement patterns for pieces ---
const int knight_move[8][2] = {{-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {-2, -1}, {-2, 1}, {2, -1}, {2, 1}};
const int bishop_direction[4][2] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};
const int rook_direction[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
const int every_direction[8][2] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}, {0, 1}, {1, 0}, {-1, 0}, {0, -1}};

// --- Holds the state of the board including castling and en passant statuses ---
struct board_state
{
    char board[8][8]; // Current board layout
    bool pawn_two_squares_black[8]; // En passant tracker for black
    bool pawn_two_squares_white[8]; // En passant tracker for white
    bool can_castle_white[2]; // [0]: kingside, [1]: queenside
    bool can_castle_black[2];
};

// --- Main Board class that handles game state and move logic ---
class Board
{
private:
    // --- Current board position (initialized to default setup) ---
    board_state position = {
        {{-4, -2, -3, -5, -6, -3, -2, -4},
         {-1, -1, -1, -1, -1, -1, -1, -1},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {1, 1, 1, 1, 1, 1, 1, 1},
         {4, 2, 3, 5, 6, 3, 2, 4}},

        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1},
        {1, 1}};

    int en_passant_index; // For pawn en passant tracking
    bool promote_pawn; // Promotion flag

public:
    // --- Optional promotion selector from GUI ---
    std::function<char()> gui_promotion_callback;

    // --- Utility functions to check board occupancy ---
    bool square_occupied_by_white(int i, int j);
    bool square_occupied_by_black(int i, int j);
    bool square_occupied(int i, int j);
    static bool square_occupied_by_opponent(char board[8][8], int i, int j, side s);

    // --- Player move handlers ---
    std::string handle_white_move(int start_i, int start_j, int target_i, int target_j);
    bool is_black_move_legal(board_state &position, int start_i, int start_j, int target_i, int target_j, board_state &possible_position);
    bool pawn_move(int start_i, int start_j, int target_i, int target_j); // Special pawn handling

    // --- Board control functions (attack detection) ---
    static bool under_pawn_control(const char board[8][8], int start_i, int start_j, int target_i, int target_j);
    static bool under_knight_control(int start_i, int start_j, int target_i, int target_j);
    static bool under_king_control(int start_i, int start_j, int target_i, int target_j);
    static bool under_bishop_control(const char board[8][8], int start_i, int start_j, int target_i, int target_j);
    static bool under_rook_control(const char board[8][8], int start_i, int start_j, int target_i, int target_j);
    static bool under_queen_control(const char board[8][8], int start_i, int start_j, int target_i, int target_j);
    static bool under_control(const char board[8][8], int i, int j, side);

    // --- Check / Checkmate / Stalemate detection ---
    static bool king_is_in_check(char board[8][8], side s);
    bool king_is_in_check(side s);
    static bool is_checkmate(board_state p, side s);
    bool is_checkmate(side s);
    static bool is_stalemate(board_state &p, side s);

    // --- Move and state utilities ---
    char get_piece(int i, int j); // Get piece at position
    void move_piece(board_state &pos, int start_i, int start_j, int destination_i, int destination_j);
    static void copy_board(const char source[8][8], char destination[8][8]);
    static void reset_en_passant(board_state &pos, side s);
    void reset_en_passant(side s);

    // --- Board state access and manipulation ---
    board_state &get_position();
    static board_state copy_position(board_state position);

    // --- Move notation generation ---
    std::string generate_move_notation(int from_i, int from_j, int to_i, int to_j, side player);

    // --- Load board from external state ---
    void load_position(const board_state &state);
};

#endif