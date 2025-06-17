// --------------------------------------------------------------------------------------
// human.h
// --- Declares the Human class for handling human player interactions in the game.
// --- Provides methods to process a human move and check for game termination.
// --- Includes a callback mechanism for handling promotion selection via GUI.
// --------------------------------------------------------------------------------------

#ifndef HUMAN_H
#define HUMAN_H

#include <functional>
#include "board.h"  // <-- required for board_state

class Human
{
public:
    // --- Function pointer to handle promotion piece selection via GUI input ---
    std::function<char()> gui1_promotion_callback;

    // --- Handles a black move made by the human player, updates board state, returns move notation ---
    std::string handle_black_move(board_state &position, int from_x, int from_y, int to_x, int to_y);

    // --- Checks if the game is over (e.g., checkmate, stalemate, insufficient material) ---
    bool game_is_over(board_state &position);
};

#endif
