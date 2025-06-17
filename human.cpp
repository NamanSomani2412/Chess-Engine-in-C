// --------------------------------------------------------------------------------------
// human.cpp
// --- Implements the Human class methods for handling user inputs and move logic.
// --- Includes logic for move validation, promotion handling, and game termination checks.
// --------------------------------------------------------------------------------------

#include "human.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include "board.h"

using namespace std;

// --- Processes a human (black) player's move and returns the move notation string ---
std::string Human::handle_black_move(board_state &position, int from_x, int from_y, int to_x, int to_y)
{
    std::string move_string = "";
    
    // --- Create a Board instance for move handling ---
    Board board;

    // --- Copy the current board state to simulate the move ---
    board_state possible_position = Board::copy_position(position);
    board.get_position() = position;

    // --- Generate notation for the attempted move ---
    move_string = board.generate_move_notation(from_x, from_y, to_x, to_y, BLACK);

    // --- Return empty string if no piece exists at the source position ---
    if (position.board[to_x][to_y] < 0)
    {
        return "";
    }

    // --- Check legality of the move using board logic ---
    if (!board.is_black_move_legal(position, from_x, from_y, to_x, to_y, possible_position))
    {
        return ""; // --- Move is illegal ---
    }

    // --- Ensure the move doesn't leave the king in check ---
    if (Board::king_is_in_check(possible_position.board, BLACK))
    {
        return ""; // --- Move would leave king in check ---
    }

    // --- Handle pawn promotion if reaching the last rank ---
    if (possible_position.board[to_x][to_y] == BLACK_PAWN && to_x == 7)
    {
        char user_input = 'q'; // --- Default to queen if no input provided ---

        // --- Get promotion choice from GUI callback if available ---
        if (gui1_promotion_callback)
        {
            user_input = gui1_promotion_callback();
        }

        // --- Replace pawn with selected promoted piece ---
        switch (user_input)
        {
        case 'k':
            possible_position.board[to_x][to_y] = BLACK_KNIGHT;
            break;
        case 'b':
            possible_position.board[to_x][to_y] = BLACK_BISHOP;
            break;
        case 'r':
            possible_position.board[to_x][to_y] = BLACK_ROOK;
            break;
        case 'q':
        default:
            possible_position.board[to_x][to_y] = BLACK_QUEEN;
            break;
        }
    }

    // --- Apply the move to the main game state ---
    position = possible_position;

    // --- Copy en passant flags for black pawns (used by white engine logic) ---
    for (int i = 0; i < 8; i++)
    {
        position.pawn_two_squares_black[i] = possible_position.pawn_two_squares_black[i];
    }

    // --- Return the valid move notation string ---
    return move_string;
}

// --- Determines if the game is over by checking checkmate and stalemate conditions ---
bool Human::game_is_over(board_state &position)
{
    if (Board::is_checkmate(position, WHITE))
    {
        return true;
    }
    if (Board::is_checkmate(position, BLACK))
    {
        return true;
    }
    if (Board::is_stalemate(position, WHITE))
    {
        return true;
    }
    if (Board::is_stalemate(position, BLACK))
    {
        return true;
    }
    return false;
}