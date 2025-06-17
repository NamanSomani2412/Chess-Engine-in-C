// --------------------------------------------------------------------------------------
// evaluation.h
// --- Header file for the Evaluation class responsible for scoring a board position. ---
// --- Includes piece-square tables for both colors and evaluation logic.             ---
// --------------------------------------------------------------------------------------

#ifndef EVALUATION_H
#define EVALUATION_H

#include "board.h"

// --- Evaluation class handles static evaluation of a given board state ---
class Evaluation
{
private:
    // --- Piece-square tables for White ---
    static int PAWN_TABLE_WHITE[8][8];
    static int KNIGHT_TABLE_WHITE[8][8];
    static int BISHOP_TABLE_WHITE[8][8];
    static int ROOK_TABLE_WHITE[8][8];
    static int QUEEN_TABLE_WHITE[8][8];
    static int KING_MIDDLE_TABLE_WHITE[8][8];
    static int KING_END_TABLE_WHITE[8][8];

    // --- Piece-square tables for Black (mirrored from White) ---
    static int PAWN_TABLE_BLACK[8][8];
    static int KNIGHT_TABLE_BLACK[8][8];
    static int BISHOP_TABLE_BLACK[8][8];
    static int ROOK_TABLE_BLACK[8][8];
    static int QUEEN_TABLE_BLACK[8][8];
    static int KING_MIDDLE_TABLE_BLACK[8][8];
    static int KING_END_TABLE_BLACK[8][8];

    // --- Utility function to copy one table to another ---
    static void copy_table(int source[8][8], int destination[8][8]);

public:
    // --- Initializes all piece-square tables (called once before evaluations) ---
    static void initialize_piece_square_tables();

    // --- Evaluates the current board state and returns an integer score ---
    static int evaluate(board_state &position, int d);

    // --- Keeps track of move count used for evaluations/debugging ---
    static int moves;
};

#endif