// --------------------------------------------------------------------------------------
// evaluation.cpp
// --- Contains definitions and initialization for evaluation logic and piece-square tables.
// --- These tables provide positional values for pieces to aid the evaluation function. ---
// --------------------------------------------------------------------------------------

#include "evaluation.h"
#include <iostream>

// --- Piece-square tables for White pieces (used to evaluate position bonuses) ---
int Evaluation::PAWN_TABLE_WHITE[8][8];
int Evaluation::KNIGHT_TABLE_WHITE[8][8];
int Evaluation::BISHOP_TABLE_WHITE[8][8];
int Evaluation::ROOK_TABLE_WHITE[8][8];
int Evaluation::QUEEN_TABLE_WHITE[8][8];
int Evaluation::KING_MIDDLE_TABLE_WHITE[8][8];
int Evaluation::KING_END_TABLE_WHITE[8][8];

// --- Piece-square tables for Black pieces (will be mirrored from White's tables) ---
int Evaluation::PAWN_TABLE_BLACK[8][8];
int Evaluation::KNIGHT_TABLE_BLACK[8][8];
int Evaluation::BISHOP_TABLE_BLACK[8][8];
int Evaluation::ROOK_TABLE_BLACK[8][8];
int Evaluation::QUEEN_TABLE_BLACK[8][8];
int Evaluation::KING_MIDDLE_TABLE_BLACK[8][8];
int Evaluation::KING_END_TABLE_BLACK[8][8];

// --- Copies a piece-square table from source to destination ---
void Evaluation::copy_table(int source[8][8], int destination[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            destination[i][j] = source[i][j];
        }
    }
}

// --- Initializes piece-square tables for all white and black pieces.
// --- These tables are used for positional evaluation in the engine.
void Evaluation::initialize_piece_square_tables()
{
    // --- White Pawn Position Table ---
    int white_pawn[8][8] =
        {
            {0, 0, 0, 0, 0, 0, 0, 0},
            {50, 50, 50, 50, 50, 50, 50, 50},
            {10, 10, 20, 30, 30, 20, 10, 10},
            {5, 5, 10, 25, 25, 10, 5, 5},
            {0, 0, 0, 20, 20, 0, 0, 0},
            {5, -5, -10, 0, 0, -10, -5, 5},
            {5, 10, 10, -20, -20, 10, 10, 5},
            {0, 0, 0, 0, 0, 0, 0, 0}};

    // --- White Knight Position Table ---
    int white_knight[8][8] =
        {
            {-50, -40, -30, -30, -30, -30, -40, -50},
            {-40, -20, 0, 0, 0, 0, -20, -40},
            {-30, 0, 10, 15, 15, 10, 0, -30},
            {-30, 5, 15, 20, 20, 15, 5, -30},
            {-30, 0, 15, 20, 20, 15, 0, -30},
            {-30, 5, 10, 15, 15, 10, 5, -30},
            {-40, -20, 0, 5, 5, 0, -20, -40},
            {-50, -40, -30, -30, -30, -30, -40, -50}};

    // --- White Bishop Position Table ---
    int white_bishop[8][8] = {
        {-20, -10, -10, -10, -10, -10, -10, -20},
        {-10, 0, 0, 0, 0, 0, 0, -10},
        {-10, 0, 5, 10, 10, 5, 0, -10},
        {-10, 5, 5, 10, 10, 5, 5, -10},
        {-10, 0, 10, 10, 10, 10, 0, -10},
        {-10, 10, 10, 10, 10, 10, 10, -10},
        {-10, 5, 0, 0, 0, 0, 5, -10},
        {-20, -10, -10, -10, -10, -10, -10, -20}};

    // --- White Rook Position Table ---
    int white_rook[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {5, 10, 10, 10, 10, 10, 10, 5},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {0, 0, 0, 5, 5, 3, 0, 0}};

    // --- White Queen Position Table ---
    int white_queen[8][8] = {
        {-20, -10, -10, -5, -5, -10, -10, -20},
        {-10, 0, 0, 0, 0, 0, 0, -10},
        {-10, 0, 5, 5, 5, 5, 0, -10},
        {-5, 0, 5, 5, 5, 5, 0, -5},
        {0, 0, 5, 5, 5, 5, 0, -5},
        {-10, 5, 5, 5, 5, 5, 0, -10},
        {-10, 0, 5, 0, 0, 0, 0, -10},
        {-20, -10, -10, -5, -5, -10, -10, -20}};

    // --- White King Middle Game Position Table ---
    int white_king_middle[8][8] = {
        {-30, -40, -40, -50, -50, -40, -40, -30},
        {-30, -40, -40, -50, -50, -40, -40, -30},
        {-30, -40, -40, -50, -50, -40, -40, -30},
        {-30, -40, -40, -50, -50, -40, -40, -30},
        {-20, -30, -30, -40, -40, -30, -30, -20},
        {-10, -20, -20, -20, -20, -20, -20, -10},
        {20, 20, 0, 0, 0, 0, 20, 20},
        {20, 30, 10, 0, 0, 10, 30, 20}};

    // --- White King End Game Position Table ---
    int white_king_end[8][8] = {
        {-50, -40, -30, -20, -20, -30, -40, -50},
        {-30, -20, -10, 0, 0, -10, -20, -30},
        {-30, -10, 20, 30, 30, 20, -10, -30},
        {-30, -10, 30, 40, 40, 30, -10, -30},
        {-30, -10, 30, 40, 40, 30, -10, -30},
        {-30, -10, 20, 30, 30, 20, -10, -30},
        {-30, -30, 0, 0, 0, 0, -30, -30},
        {-50, -30, -30, -30, -30, -30, -30, -50}};

    // --- Black Pawn Position Table ---
    int black_pawn[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {5, 10, 10, -20, -20, 10, 10, 5},
        {5, -5, -10, 0, 0, -10, -5, 5},
        {0, 0, 0, 20, 20, 0, 0, 0},
        {5, 5, 10, 25, 25, 10, 5, 5},
        {10, 10, 20, 30, 30, 20, 10, 10},
        {50, 50, 50, 50, 50, 50, 50, 50},
        {0, 0, 0, 0, 0, 0, 0, 0}};

    // --- Black Knight Position Table ---
    int black_knight[8][8] = {
        {-50, -40, -30, -30, -30, -30, -40, -50},
        {-40, -20, 0, 5, 5, 0, -20, -40},
        {-30, 5, 10, 15, 15, 10, 5, -30},
        {-30, 0, 15, 20, 20, 15, 0, -30},
        {-30, 5, 15, 20, 20, 15, 5, -30},
        {-30, 0, 10, 15, 15, 10, 0, -30},
        {-40, -20, 0, 0, 0, 0, -20, -40},
        {-50, -40, -30, -30, -30, -30, -40, -50}};

    // --- Black Bishop Position Table ---
    int black_bishop[8][8] = {
        {-20, -10, -10, -10, -10, -10, -10, -20},
        {-10, 5, 0, 0, 0, 0, 5, -10},
        {-10, 10, 10, 10, 10, 10, 10, -10},
        {-10, 0, 10, 10, 10, 10, 0, -10},
        {-10, 5, 5, 10, 10, 5, 5, -10},
        {-10, 0, 5, 10, 10, 5, 0, -10},
        {-10, 0, 0, 0, 0, 0, 0, -10},
        {-20, -10, -10, -10, -10, -10, -10, -20}};

    // --- Black Rook Position Table ---
    int black_rook[8][8] = {
        {0, 0, 0, 5, 5, 3, 0, 0},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {5, 10, 10, 10, 10, 10, 10, 5},
        {0, 0, 0, 0, 0, 0, 0, 0}};

    // --- Black Queen Position Table ---
    int black_queen[8][8] = {
        {-20, -10, -10, -5, -5, -10, -10, -20},
        {-10, 0, 5, 0, 0, 0, 0, -10},
        {-10, 5, 5, 5, 5, 5, 0, -10},
        {0, 0, 5, 5, 5, 5, 0, -5},
        {-5, 0, 5, 5, 5, 5, 0, -5},
        {-10, 0, 5, 5, 5, 5, 0, -10},
        {-10, 0, 0, 0, 0, 0, 0, -10},
        {-20, -10, -10, -5, -5, -10, -10, -20}};

    // --- Black King Middle Game Table ---
    int black_king_middle[8][8] = {
        {20, 30, 10, 0, 0, 10, 30, 20},
        {20, 20, 0, 0, 0, 0, 20, 20},
        {-10, -20, -20, -20, -20, -20, -20, -10},
        {-20, -30, -30, -40, -40, -30, -30, -20},
        {-30, -40, -40, -50, -50, -40, -40, -30},
        {-30, -40, -40, -50, -50, -40, -40, -30},
        {-30, -40, -40, -50, -50, -40, -40, -30},
        {-30, -40, -40, -50, -50, -40, -40, -30}};

    // --- Black King End Game Table ---
    int black_king_end[8][8] = {
        {-50, -30, -30, -30, -30, -30, -30, -50},
        {-30, -30, 0, 0, 0, 0, -30, -30},
        {-30, -10, 20, 30, 30, 20, -10, -30},
        {-30, -10, 30, 40, 40, 30, -10, -30},
        {-30, -10, 30, 40, 40, 30, -10, -30},
        {-30, -10, 20, 30, 30, 20, -10, -30},
        {-30, -20, -10, 0, 0, -10, -20, -30},
        {-50, -40, -30, -20, -20, -30, -40, -50}};

    // --- Copy to static member tables using helper function ---
    copy_table(white_pawn, PAWN_TABLE_WHITE);
    copy_table(white_knight, KNIGHT_TABLE_WHITE);
    copy_table(white_bishop, BISHOP_TABLE_WHITE);
    copy_table(white_rook, ROOK_TABLE_WHITE);
    copy_table(white_queen, QUEEN_TABLE_WHITE);
    copy_table(white_king_middle, KING_MIDDLE_TABLE_WHITE);
    copy_table(white_king_end, KING_END_TABLE_WHITE);

    copy_table(black_pawn, PAWN_TABLE_BLACK);
    copy_table(black_knight, KNIGHT_TABLE_BLACK);
    copy_table(black_bishop, BISHOP_TABLE_BLACK);
    copy_table(black_rook, ROOK_TABLE_BLACK);
    copy_table(black_queen, QUEEN_TABLE_BLACK);
    copy_table(black_king_middle, KING_MIDDLE_TABLE_BLACK);
    copy_table(black_king_end, KING_END_TABLE_BLACK);
}

// --- Evaluates the board state and returns a score based on material, position,
//     mobility, and pawn structure. Accounts for endgame/midgame differences and
//     special conditions like checkmate/stalemate ---
int Evaluation::evaluate(board_state &position, int depth)
{
    // --- Stalemate check: Drawn position ---
    if (Board::is_stalemate(position, BLACK) || Board::is_stalemate(position, WHITE))
    {
        return 0;
    }

    // --- Checkmate conditions: More favorable if quicker (depth matters) ---
    if (Board::is_checkmate(position, WHITE))
    {
        return -100000 - depth;
    }
    if (Board::is_checkmate(position, BLACK))
    {
        return 100000 + depth;
    }

    // --- Initialize evaluation parameters ---
    int score = 0;
    int legal_moves_white = 0;
    int legal_moves_black = 0;

    int wk_i = 0, wk_j = 0;
    int bk_i = 0, bk_j = 0;

    bool is_end_game = false;
    bool white_queen_on_board = false;
    bool black_queen_on_board = false;
    int num_of_white_minor_pieces = 0;
    int num_of_black_minor_pieces = 0;

    int white_pawns_in_file[8] = {0};
    int black_pawns_in_file[8] = {0};

    // --- Traverse the board and evaluate all pieces ---
    for (int m = 0; m < 8; m++)
    {
        for (int n = 0; n < 8; n++)
        {
            // --- WHITE PAWN Evaluation ---
            if (position.board[m][n] == WHITE_PAWN)
            {
                score += 100;
                score += PAWN_TABLE_WHITE[m][n];
                white_pawns_in_file[n]++;

                // --- White Pawn mobility ---
                if (position.board[m - 1][n] == 0)
                    legal_moves_white++;
                if (m == 6 && position.board[m - 1][n] == 0 && position.board[m - 2][n] == 0)
                    legal_moves_white++;
                if (n != 0 && position.board[m - 1][n - 1] < 0)
                    legal_moves_white++;
                if (n != 7 && position.board[m - 1][n + 1] < 0)
                    legal_moves_white++;
                if (n != 7 && position.board[m][n + 1] == -1 && position.pawn_two_squares_black[n + 1])
                    legal_moves_white++;
                if (n != 0 && position.board[m][n - 1] == -1 && position.pawn_two_squares_black[n - 1])
                    legal_moves_white++;
            }

            // --- WHITE KNIGHT Evaluation ---
            else if (position.board[m][n] == WHITE_KNIGHT)
            {
                score += 320;
                score += KNIGHT_TABLE_WHITE[m][n];
                num_of_white_minor_pieces++;

                for (int k = 0; k < 8; k++)
                {
                    int k_i = m + knight_move[k][0];
                    int k_j = n + knight_move[k][1];
                    if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8 && position.board[k_i][k_j] <= 0)
                        legal_moves_white++;
                }
            }

            // --- WHITE BISHOP Evaluation ---
            else if (position.board[m][n] == WHITE_BISHOP)
            {
                score += 330;
                score += BISHOP_TABLE_WHITE[m][n];
                num_of_white_minor_pieces++;

                for (int k = 0; k < 4; k++)
                {
                    int path_i = m + bishop_direction[k][0];
                    int path_j = n + bishop_direction[k][1];
                    while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                    {
                        if (position.board[path_i][path_j] <= 0)
                            legal_moves_white++;
                        if (position.board[path_i][path_j] != 0)
                            break;
                        path_i += bishop_direction[k][0];
                        path_j += bishop_direction[k][1];
                    }
                }
            }

            // --- WHITE ROOK Evaluation ---
            else if (position.board[m][n] == WHITE_ROOK)
            {
                score += 500;
                score += ROOK_TABLE_WHITE[m][n];
                num_of_white_minor_pieces++;

                for (int k = 0; k < 4; k++)
                {
                    int path_i = m + rook_direction[k][0];
                    int path_j = n + rook_direction[k][1];
                    while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                    {
                        if (position.board[path_i][path_j] <= 0)
                            legal_moves_white++;
                        if (position.board[path_i][path_j] != 0)
                            break;
                        path_i += rook_direction[k][0];
                        path_j += rook_direction[k][1];
                    }
                }
            }

            // --- WHITE QUEEN Evaluation ---
            else if (position.board[m][n] == WHITE_QUEEN)
            {
                score += 900;
                score += QUEEN_TABLE_WHITE[m][n];
                white_queen_on_board = true;
            }

            // --- WHITE KING Evaluation ---
            else if (position.board[m][n] == WHITE_KING)
            {
                wk_i = m;
                wk_j = n;
                for (int k = 0; k < 8; k++)
                {
                    int k_i = m + every_direction[k][0];
                    int k_j = n + every_direction[k][1];
                    if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8 &&
                        position.board[k_i][k_j] <= 0 &&
                        !Board::under_control(position.board, k_i, k_j, BLACK))
                    {
                        legal_moves_white++;
                    }
                }
            }

            // --- BLACK PIECES (same logic, mirrored signs) ---
            // --- BLACK PAWN Evaluation ---
            else if (position.board[m][n] == BLACK_PAWN)
            {
                score -= 100;
                score -= PAWN_TABLE_BLACK[m][n];
                black_pawns_in_file[n]++;

                if (position.board[m + 1][n] == 0)
                    legal_moves_black++;
                if (m == 1 && position.board[m + 1][n] == 0 && position.board[m + 2][n] == 0)
                    legal_moves_black++;
                if (n != 0 && position.board[m + 1][n - 1] > 0)
                    legal_moves_black++;
                if (n != 7 && position.board[m + 1][n + 1] > 0)
                    legal_moves_black++;
                if (n != 7 && position.board[m][n + 1] == 1 && position.pawn_two_squares_white[n + 1])
                    legal_moves_black++;
                if (n != 0 && position.board[m][n - 1] == 1 && position.pawn_two_squares_white[n - 1])
                    legal_moves_black++;
            }

            // --- BLACK KNIGHT ---
            else if (position.board[m][n] == BLACK_KNIGHT)
            {
                score -= 320;
                score -= KNIGHT_TABLE_BLACK[m][n];
                num_of_black_minor_pieces++;

                for (int k = 0; k < 8; k++)
                {
                    int k_i = m + knight_move[k][0];
                    int k_j = n + knight_move[k][1];
                    if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8 && position.board[k_i][k_j] >= 0)
                        legal_moves_black++;
                }
            }

            // --- BLACK BISHOP ---
            else if (position.board[m][n] == BLACK_BISHOP)
            {
                score -= 330;
                score -= BISHOP_TABLE_BLACK[m][n];
                num_of_black_minor_pieces++;

                for (int k = 0; k < 4; k++)
                {
                    int path_i = m + bishop_direction[k][0];
                    int path_j = n + bishop_direction[k][1];
                    while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                    {
                        if (position.board[path_i][path_j] >= 0)
                            legal_moves_black++;
                        if (position.board[path_i][path_j] != 0)
                            break;
                        path_i += bishop_direction[k][0];
                        path_j += bishop_direction[k][1];
                    }
                }
            }

            // --- BLACK ROOK ---
            else if (position.board[m][n] == BLACK_ROOK)
            {
                score -= 500;
                score -= ROOK_TABLE_BLACK[m][n];
                num_of_black_minor_pieces++;

                for (int k = 0; k < 4; k++)
                {
                    int path_i = m + rook_direction[k][0];
                    int path_j = n + rook_direction[k][1];
                    while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                    {
                        if (position.board[path_i][path_j] >= 0)
                            legal_moves_black++;
                        if (position.board[path_i][path_j] != 0)
                            break;
                        path_i += rook_direction[k][0];
                        path_j += rook_direction[k][1];
                    }
                }
            }

            // --- BLACK QUEEN ---
            else if (position.board[m][n] == BLACK_QUEEN)
            {
                score -= 900;
                score -= QUEEN_TABLE_BLACK[m][n];
                black_queen_on_board = true;
            }

            // --- BLACK KING ---
            else if (position.board[m][n] == BLACK_KING)
            {
                bk_i = m;
                bk_j = n;
                for (int k = 0; k < 8; k++)
                {
                    int k_i = m + every_direction[k][0];
                    int k_j = n + every_direction[k][1];
                    if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8 &&
                        position.board[k_i][k_j] >= 0 &&
                        !Board::under_control(position.board, k_i, k_j, WHITE))
                    {
                        legal_moves_black++;
                    }
                }
            }
        }
    }

    // --- Add mobility bonus ---
    score += 10 * (legal_moves_white - legal_moves_black);

    // --- Determine if it's endgame based on queen + minor pieces ---
    if ((!white_queen_on_board && !black_queen_on_board) ||
        (white_queen_on_board && num_of_white_minor_pieces <= 1) ||
        (black_queen_on_board && num_of_black_minor_pieces <= 1))
    {
        is_end_game = true;
    }

    // --- Add king positional bonuses based on game phase ---
    score += is_end_game ? KING_END_TABLE_WHITE[wk_i][wk_j] : KING_MIDDLE_TABLE_WHITE[wk_i][wk_j];
    score -= is_end_game ? KING_END_TABLE_BLACK[bk_i][bk_j] : KING_MIDDLE_TABLE_BLACK[bk_i][bk_j];

    // --- Evaluate pawn structure: doubled pawns, isolated pawns, and pawn islands ---
    int white_pawn_islands = 0;
    int black_pawn_islands = 0;
    bool white_pawn_island_flag = false;
    bool black_pawn_island_flag = false;

    for (int i = 0; i < 8; i++)
    {
        if (white_pawns_in_file[i] > 1)
            score -= white_pawns_in_file[i] * 15;
        if (black_pawns_in_file[i] > 1)
            score += black_pawns_in_file[i] * 15;

        if (white_pawns_in_file[i] > 0 && !white_pawn_island_flag)
        {
            white_pawn_islands++;
            white_pawn_island_flag = true;
        }
        if (white_pawns_in_file[i] == 0)
            white_pawn_island_flag = false;

        if (black_pawns_in_file[i] > 0 && !black_pawn_island_flag)
        {
            black_pawn_islands++;
            black_pawn_island_flag = true;
        }
        if (black_pawns_in_file[i] == 0)
            black_pawn_island_flag = false;

        if (i != 0 && i != 7)
        {
            if (white_pawns_in_file[i] > 0 && white_pawns_in_file[i - 1] == 0 && white_pawns_in_file[i + 1] == 0)
                score -= 30;
            if (black_pawns_in_file[i] > 0 && black_pawns_in_file[i - 1] == 0 && black_pawns_in_file[i + 1] == 0)
                score += 30;
        }
    }

    // --- Apply pawn island penalties ---
    score -= 10 * (white_pawn_islands - black_pawn_islands);

    // --- Return final evaluation score ---
    return score;
}