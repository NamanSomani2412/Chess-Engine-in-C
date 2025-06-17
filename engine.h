// --------------------------------------------------------------------------------------
// engine.h
// --- Declares the Engine class that controls the AI logic for chess moves.
// --- Includes move evaluation, search (minimax + alpha-beta), and best move generation.
// --------------------------------------------------------------------------------------

#include "evaluation.h"
#include <string>

// --- Structure to represent a move chosen by the engine ---
struct EngineMove {
    std::string notation;   // --- Algebraic notation of the move (e.g., e2e4 or Nf3) ---
    int from_i, from_j;     // --- Starting square (row, column) ---
    int to_i, to_j;         // --- Destination square (row, column) ---
    float eval = 0;         // --- Evaluation score for the move ---
    int nodes = 0;          // --- Number of nodes evaluated to make this move ---
};

class Engine
{
private:
    const int DEPTH = 4;    // --- Search depth for the minimax algorithm ---

public:
    Engine(); // --- Constructor for the Engine class ---

    // --- Returns the best move for Black using minimax search ---
    EngineMove make_black_move(board_state &position);

    // --- Returns the best move for White using minimax search ---
    EngineMove make_white_move(board_state &position);

    // --- Returns a vector of top 1–3 best White moves sorted by evaluation ---
    std::vector<EngineMove> get_best_white_moves(board_state &position);

    // --- Minimax function with alpha-beta pruning for evaluating board positions ---
    int adv_minimax(board_state &position, int depth, bool maximizingPlayer, int alpha, int beta);

    // --- Checks if the game is over due to checkmate or stalemate ---
    bool game_is_over(board_state &position);
};
