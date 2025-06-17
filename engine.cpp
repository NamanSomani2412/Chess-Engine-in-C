// --------------------------------------------------------------------------------------
// engine.cpp
// --- Implements the Engine class which handles AI logic and move generation.
// --- Uses Minimax with alpha-beta pruning to determine best moves for both sides.
// --- Evaluates positions using Evaluation class and tracks nodes explored.
// --------------------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include "engine.h"
#include <string>

using namespace std;

int paths = 0; // --- Global counter for nodes explored in the current search ---

// --- Constructor for the Engine class. Initializes evaluation tables. ---
Engine::Engine()
{
    Evaluation::initialize_piece_square_tables();
}

// --- Returns and apply the best move for Black using minimax search ---
EngineMove Engine::make_black_move(board_state &position)
{
    Board board;
    board.get_position() = position;
    int from_i = -1, from_j = -1, to_i = -1, to_j = -1;
    int best_score = INT_MAX;
    board_state possible_position;
    board_state best_position;
    string computer_move_notation;

    Board::reset_en_passant(position, BLACK);

    for (int m = 0; m < 8; m++)
    {
        for (int n = 0; n < 8; n++)
        {
            if (position.board[m][n] < 0)
            {
                if (position.board[m][n] == BLACK_PAWN)
                {
                    // one move forward by pawn
                    if (position.board[m + 1][n] == 0)
                    {
                        // if not moving to last rank
                        if (m + 1 != 7)
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m + 1][n] = BLACK_PAWN;
                            possible_position.board[m][n] = 0;

                            if (!Board::king_is_in_check(possible_position.board, BLACK))
                            {
                                int alpha = INT_MIN, beta = INT_MAX;
                                int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                                if (score < best_score)
                                {
                                    best_position = Board::copy_position(possible_position);
                                    best_score = score;
                                    from_i = m;
                                    from_j = n;
                                    to_i = m + 1;
                                    to_j = n;
                                }
                            }
                        }
                        // if moves last rank, then cycle through promotion of all pieces
                        else
                        {
                            // variable i denotes possible pieces from black knight to queen
                            for (char i = -5; i <= -2; i++)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m + 1][n] = i;
                                possible_position.board[m][n] = 0;

                                if (!Board::king_is_in_check(possible_position.board, BLACK))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                                    if (score < best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = m + 1;
                                        to_j = n;
                                    }
                                }
                            }
                        }
                    }
                    // two moves from starting position by pawn
                    if (m == 1 && position.board[m + 1][n] == 0 && position.board[m + 2][n] == 0)
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[m + 2][n] = BLACK_PAWN;
                        possible_position.board[m][n] = 0;

                        possible_position.pawn_two_squares_black[n] = true;

                        if (!Board::king_is_in_check(possible_position.board, BLACK))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                            if (score < best_score)
                            {
                                best_position = Board::copy_position(possible_position);
                                best_score = score;
                                from_i = m;
                                from_j = n;
                                to_i = m + 2;
                                to_j = n;
                            }
                        }
                    }
                    // capture to the left
                    if (n != 0 && position.board[m + 1][n - 1] > 0)
                    {
                        if (m + 1 != 7)
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m + 1][n - 1] = BLACK_PAWN;
                            possible_position.board[m][n] = 0;
                            if (!Board::king_is_in_check(possible_position.board, BLACK))
                            {
                                int alpha = INT_MIN, beta = INT_MAX;
                                int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                                if (score < best_score)
                                {
                                    best_position = Board::copy_position(possible_position);
                                    best_score = score;
                                    from_i = m;
                                    from_j = n;
                                    to_i = m + 1;
                                    to_j = n - 1;
                                }
                            }
                        }
                        // if moves last rank, then cycle through promotion of all pieces
                        else
                        {
                            // variable i is possible pieces from black knight to queen
                            for (char i = -5; i <= -2; i++)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m + 1][n - 1] = i;
                                possible_position.board[m][n] = 0;

                                if (!Board::king_is_in_check(possible_position.board, BLACK))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                                    if (score < best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = m + 1;
                                        to_j = n - 1;
                                    }
                                }
                            }
                        }
                    }
                    // capture to the right
                    if (n != 7 && position.board[m + 1][n + 1] > 0)
                    {
                        if (m + 1 != 7)
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m + 1][n + 1] = BLACK_PAWN;
                            possible_position.board[m][n] = 0;
                            if (!Board::king_is_in_check(possible_position.board, BLACK))
                            {
                                int alpha = INT_MIN, beta = INT_MAX;
                                int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                                if (score < best_score)
                                {
                                    best_position = Board::copy_position(possible_position);
                                    best_score = score;
                                    from_i = m;
                                    from_j = n;
                                    to_i = m + 1;
                                    to_j = n + 1;
                                }
                            }
                        }
                        // if moves last rank, then cycle through promotion of all pieces
                        else
                        {
                            // variable i is possible pieces from black knight to queen
                            for (char i = -5; i <= -2; i++)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m + 1][n + 1] = i;
                                possible_position.board[m][n] = 0;

                                if (!Board::king_is_in_check(possible_position.board, BLACK))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                                    if (score < best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = m + 1;
                                        to_j = n + 1;
                                    }
                                }
                            }
                        }
                    }
                    // en passant to the right
                    if (n != 7 && position.board[m][n + 1] == 1 && position.pawn_two_squares_white[n + 1])
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[m + 1][n + 1] = BLACK_PAWN;
                        possible_position.board[m][n] = 0;
                        possible_position.board[m][n + 1] = 0;
                        if (!Board::king_is_in_check(possible_position.board, BLACK))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                            if (score < best_score)
                            {
                                best_position = Board::copy_position(possible_position);
                                best_score = score;
                                from_i = m;
                                from_j = n;
                                to_i = m + 1;
                                to_j = n + 1;
                            }
                        }
                    }
                    // en passant to the left
                    if (n != 0 && position.board[m][n - 1] == 1 && position.pawn_two_squares_white[n - 1])
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[m + 1][n - 1] = BLACK_PAWN;
                        possible_position.board[m][n] = 0;
                        possible_position.board[m][n - 1] = 0;
                        if (!Board::king_is_in_check(possible_position.board, BLACK))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                            if (score < best_score)
                            {
                                best_position = Board::copy_position(possible_position);
                                best_score = score;
                                from_i = m;
                                from_j = n;
                                to_i = m + 1;
                                to_j = n - 1;
                            }
                        }
                    }
                }

                if (position.board[m][n] == BLACK_KNIGHT)
                {
                    for (int k = 0; k < 8; k++)
                    {
                        int k_i = m + knight_move[k][0];
                        int k_j = n + knight_move[k][1];
                        if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8)
                        {
                            if (Board::under_knight_control(m, n, k_i, k_j) && position.board[k_i][k_j] >= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[k_i][k_j] = BLACK_KNIGHT;
                                possible_position.board[m][n] = 0;
                                if (!Board::king_is_in_check(possible_position.board, BLACK))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                                    if (score < best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = k_i;
                                        to_j = k_j;
                                    }
                                }
                            }
                        }
                    }
                }

                if (position.board[m][n] == BLACK_BISHOP)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        int path_i = m + bishop_direction[k][0];
                        int path_j = n + bishop_direction[k][1];
                        while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                        {
                            if (Board::under_bishop_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] >= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[path_i][path_j] = BLACK_BISHOP;
                                possible_position.board[m][n] = 0;
                                if (!Board::king_is_in_check(possible_position.board, BLACK))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                                    if (score < best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = path_i;
                                        to_j = path_j;
                                    }
                                }
                            }
                            // if next square is not empty, we stop
                            if (position.board[path_i][path_j] != 0)
                            {
                                break;
                            }
                            path_i += bishop_direction[k][0];
                            path_j += bishop_direction[k][1];
                        }
                    }
                }

                if (position.board[m][n] == BLACK_ROOK)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        int path_i = m + rook_direction[k][0];
                        int path_j = n + rook_direction[k][1];
                        while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                        {
                            if (Board::under_rook_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] >= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[path_i][path_j] = BLACK_ROOK;
                                possible_position.board[m][n] = 0;
                                if (m == 0 && n == 0)
                                {
                                    possible_position.can_castle_black[0] = false;
                                }
                                if (m == 0 && n == 7)
                                {
                                    possible_position.can_castle_black[1] = false;
                                }
                                if (!Board::king_is_in_check(possible_position.board, BLACK))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                                    if (score < best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = path_i;
                                        to_j = path_j;
                                    }
                                }
                            }
                            // if next square is not empty, we stop
                            if (position.board[path_i][path_j] != 0)
                            {
                                break;
                            }
                            path_i += rook_direction[k][0];
                            path_j += rook_direction[k][1];
                        }
                    }
                }

                if (position.board[m][n] == BLACK_QUEEN)
                {
                    for (int k = 0; k < 8; k++)
                    {
                        int path_i = m + every_direction[k][0];
                        int path_j = n + every_direction[k][1];
                        while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                        {
                            if (Board::under_queen_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] >= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[path_i][path_j] = BLACK_QUEEN;
                                possible_position.board[m][n] = 0;
                                if (!Board::king_is_in_check(possible_position.board, BLACK))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                                    if (score < best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = path_i;
                                        to_j = path_j;
                                    }
                                }
                            }
                            // if next square is not empty, we stop
                            if (position.board[path_i][path_j] != 0)
                            {
                                break;
                            }
                            path_i += every_direction[k][0];
                            path_j += every_direction[k][1];
                        }
                    }
                }

                if (position.board[m][n] == BLACK_KING)
                {
                    for (int k = 0; k < 8; k++)
                    {
                        int k_i = m + every_direction[k][0];
                        int k_j = n + every_direction[k][1];
                        if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8)
                        {
                            if (Board::under_king_control(m, n, k_i, k_j) && position.board[k_i][k_j] >= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[k_i][k_j] = BLACK_KING;
                                possible_position.board[m][n] = 0;
                                possible_position.can_castle_black[0] = false;
                                possible_position.can_castle_black[1] = false;
                                if (!Board::king_is_in_check(possible_position.board, BLACK))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                                    if (score < best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = k_i;
                                        to_j = k_j;
                                    }
                                }
                            }
                        }
                    }
                    // castling kingside
                    if (m == 0 && n == 4 && position.board[0][5] == 0 && position.board[0][6] == 0 && position.board[0][7] == BLACK_ROOK && !Board::under_control(position.board, 0, 4, WHITE) && !Board::under_control(position.board, 0, 5, WHITE) && !Board::under_control(position.board, 0, 6, WHITE) && position.can_castle_black[1])
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[0][6] = BLACK_KING;
                        possible_position.board[0][4] = 0;
                        possible_position.board[0][7] = 0;
                        possible_position.board[0][5] = BLACK_ROOK;
                        possible_position.can_castle_black[0] = false;
                        possible_position.can_castle_black[1] = false;
                        if (!Board::king_is_in_check(possible_position.board, BLACK))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                            if (score < best_score)
                            {
                                best_position = Board::copy_position(possible_position);
                                best_score = score;
                                from_i = 0;
                                from_j = 4;
                                to_i = 0;
                                to_j = 6;
                            }
                        }
                    }
                    // castling queenside
                    if (m == 0 && n == 4 && position.board[0][1] == 0 && position.board[0][2] == 0 && position.board[0][3] == 0 && position.board[0][0] == BLACK_ROOK && !Board::under_control(position.board, 0, 2, WHITE) && !Board::under_control(position.board, 0, 3, WHITE) && !Board::under_control(position.board, 0, 4, WHITE) && position.can_castle_black[0])
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[0][2] = BLACK_KING;
                        possible_position.board[0][4] = 0;
                        possible_position.board[0][0] = 0;
                        possible_position.board[0][3] = BLACK_ROOK;
                        possible_position.can_castle_black[0] = false;
                        possible_position.can_castle_black[1] = false;
                        if (!Board::king_is_in_check(possible_position.board, BLACK))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, true, alpha, beta);
                            if (score < best_score)
                            {
                                best_position = Board::copy_position(possible_position);
                                best_score = score;
                                from_i = 0;
                                from_j = 4;
                                to_i = 0;
                                to_j = 2;
                            }
                        }
                    }
                }
            }
        }
    }

    // --- Stores Information in result data structure ---
    computer_move_notation = board.generate_move_notation(from_i, from_j, to_i, to_j, BLACK);
    EngineMove result;
    result.notation = computer_move_notation;
    result.from_i = from_i;
    result.from_j = from_j;
    result.to_i = to_i;
    result.to_j = to_j;
    result.eval = (float)best_score / 100;
    result.nodes = paths;

    // --- resets the total no of nodes visited ---
    paths = 0;

    // --- Apply best move to actual game ---
    position = Board::copy_position(best_position);

    return result;
}

// --- Returns the best move for White using minimax search ---
EngineMove Engine::make_white_move(board_state &position)
{
    Board board;
    board.get_position() = position;
    int from_i = -1, from_j = -1, to_i = -1, to_j = -1;
    int best_score = INT_MIN;
    board_state possible_position;
    board_state best_position;
    string computer_move_notation;

    Board::reset_en_passant(position, WHITE);

    for (int m = 0; m < 8; m++)
    {
        for (int n = 0; n < 8; n++)
        {
            if (position.board[m][n] > 0)
            {
                if (position.board[m][n] == WHITE_PAWN)
                {
                    // one move forward by pawn
                    if (position.board[m - 1][n] == 0)
                    {
                        // if not moving to last rank
                        if (m - 1 != 0)
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m - 1][n] = WHITE_PAWN;
                            possible_position.board[m][n] = 0;

                            if (!Board::king_is_in_check(possible_position.board, WHITE))
                            {
                                int alpha = INT_MIN, beta = INT_MAX;
                                int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                                if (score > best_score)
                                {
                                    best_position = Board::copy_position(possible_position);
                                    best_score = score;
                                    from_i = m;
                                    from_j = n;
                                    to_i = m - 1;
                                    to_j = n;
                                }
                            }
                        }
                        // if moves last rank, then cycle through promotion of all pieces
                        else
                        {
                            // variable i denotes possible pieces from white knight to queen
                            for (char i = 5; i >= 2; i--)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m - 1][n] = i;
                                possible_position.board[m][n] = 0;

                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                                    if (score > best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = m - 1;
                                        to_j = n;
                                    }
                                }
                            }
                        }
                    }
                    // two moves from starting position by pawn
                    if (m == 6 && position.board[m - 1][n] == 0 && position.board[m - 2][n] == 0)
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[m - 2][n] = WHITE_PAWN;
                        possible_position.board[m][n] = 0;

                        possible_position.pawn_two_squares_white[n] = true;

                        if (!Board::king_is_in_check(possible_position.board, WHITE))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                            if (score > best_score)
                            {
                                best_position = Board::copy_position(possible_position);
                                best_score = score;
                                from_i = m;
                                from_j = n;
                                to_i = m - 2;
                                to_j = n;
                            }
                        }
                    }
                    // capture to the left
                    if (n != 7 && position.board[m - 1][n + 1] < 0)
                    {
                        if (m - 1 != 0)
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m - 1][n + 1] = WHITE_PAWN;
                            possible_position.board[m][n] = 0;
                            if (!Board::king_is_in_check(possible_position.board, WHITE))
                            {
                                int alpha = INT_MIN, beta = INT_MAX;
                                int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                                if (score > best_score)
                                {
                                    best_position = Board::copy_position(possible_position);
                                    best_score = score;
                                    from_i = m;
                                    from_j = n;
                                    to_i = m - 1;
                                    to_j = n + 1;
                                }
                            }
                        }
                        // if moves last rank, then cycle through promotion of all pieces
                        else
                        {
                            // variable i is possible pieces from knight to queen
                            for (char i = 5; i >= 2; i--)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m - 1][n + 1] = i;
                                possible_position.board[m][n] = 0;

                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                                    if (score > best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = m - 1;
                                        to_j = n + 1;
                                    }
                                }
                            }
                        }
                    }
                    // capture to the right
                    if (n != 0 && position.board[m - 1][n - 1] < 0)
                    {
                        if (m - 1 != 0)
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m - 1][n - 1] = WHITE_PAWN;
                            possible_position.board[m][n] = 0;
                            if (!Board::king_is_in_check(possible_position.board, WHITE))
                            {
                                int alpha = INT_MIN, beta = INT_MAX;
                                int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                                if (score > best_score)
                                {
                                    best_position = Board::copy_position(possible_position);
                                    best_score = score;
                                    from_i = m;
                                    from_j = n;
                                    to_i = m - 1;
                                    to_j = n - 1;
                                }
                            }
                        }
                        // if moves last rank, then cycle through promotion of all pieces
                        else
                        {
                            // variable i is possible pieces from white knight to queen
                            for (char i = 5; i >= 2; i--)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m - 1][n - 1] = i;
                                possible_position.board[m][n] = 0;

                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                                    if (score > best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = m - 1;
                                        to_j = n - 1;
                                    }
                                }
                            }
                        }
                    }
                    // en passant to the left
                    if (n != 7 && position.board[m][n + 1] == -1 && position.pawn_two_squares_black[n + 1])
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[m - 1][n + 1] = WHITE_PAWN;
                        possible_position.board[m][n] = 0;
                        possible_position.board[m][n + 1] = 0;
                        if (!Board::king_is_in_check(possible_position.board, WHITE))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                            if (score > best_score)
                            {
                                best_position = Board::copy_position(possible_position);
                                best_score = score;
                                from_i = m;
                                from_j = n;
                                to_i = m - 1;
                                to_j = n + 1;
                            }
                        }
                    }
                    // en passant to the right
                    if (n != 0 && position.board[m][n - 1] == -1 && position.pawn_two_squares_black[n - 1])
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[m - 1][n - 1] = WHITE_PAWN;
                        possible_position.board[m][n] = 0;
                        possible_position.board[m][n - 1] = 0;
                        if (!Board::king_is_in_check(possible_position.board, WHITE))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                            if (score > best_score)
                            {
                                best_position = Board::copy_position(possible_position);
                                best_score = score;
                                from_i = m;
                                from_j = n;
                                to_i = m - 1;
                                to_j = n - 1;
                            }
                        }
                    }
                }

                if (position.board[m][n] == WHITE_KNIGHT)
                {
                    for (int k = 0; k < 8; k++)
                    {
                        int k_i = m + knight_move[k][0];
                        int k_j = n + knight_move[k][1];
                        if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8)
                        {
                            if (Board::under_knight_control(m, n, k_i, k_j) && position.board[k_i][k_j] <= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[k_i][k_j] = WHITE_KNIGHT;
                                possible_position.board[m][n] = 0;
                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                                    if (score > best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = k_i;
                                        to_j = k_j;
                                    }
                                }
                            }
                        }
                    }
                }

                if (position.board[m][n] == WHITE_BISHOP)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        int path_i = m + bishop_direction[k][0];
                        int path_j = n + bishop_direction[k][1];
                        while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                        {
                            if (Board::under_bishop_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] <= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[path_i][path_j] = WHITE_BISHOP;
                                possible_position.board[m][n] = 0;
                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                                    if (score > best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = path_i;
                                        to_j = path_j;
                                    }
                                }
                            }
                            // if next square is not empty, we stop
                            if (position.board[path_i][path_j] != 0)
                            {
                                break;
                            }
                            path_i += bishop_direction[k][0];
                            path_j += bishop_direction[k][1];
                        }
                    }
                }

                if (position.board[m][n] == WHITE_ROOK)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        int path_i = m + rook_direction[k][0];
                        int path_j = n + rook_direction[k][1];
                        while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                        {
                            if (Board::under_rook_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] <= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[path_i][path_j] = WHITE_ROOK;
                                possible_position.board[m][n] = 0; // recheck logic
                                if (m == 7 && n == 0)
                                {
                                    possible_position.can_castle_white[0] = false;
                                }
                                if (m == 7 && n == 7)
                                {
                                    possible_position.can_castle_white[1] = false;
                                }
                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                                    if (score > best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = path_i;
                                        to_j = path_j;
                                    }
                                }
                            }
                            // if next square is not empty, we stop
                            if (position.board[path_i][path_j] != 0)
                            {
                                break;
                            }
                            path_i += rook_direction[k][0];
                            path_j += rook_direction[k][1];
                        }
                    }
                }

                if (position.board[m][n] == WHITE_QUEEN)
                {
                    for (int k = 0; k < 8; k++)
                    {
                        int path_i = m + every_direction[k][0];
                        int path_j = n + every_direction[k][1];
                        while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                        {
                            if (Board::under_queen_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] <= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[path_i][path_j] = WHITE_QUEEN;
                                possible_position.board[m][n] = 0;
                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                                    if (score > best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = path_i;
                                        to_j = path_j;
                                    }
                                }
                            }
                            // if next square is not empty, we stop
                            if (position.board[path_i][path_j] != 0)
                            {
                                break;
                            }
                            path_i += every_direction[k][0];
                            path_j += every_direction[k][1];
                        }
                    }
                }

                if (position.board[m][n] == WHITE_KING)
                {
                    for (int k = 0; k < 8; k++)
                    {
                        int k_i = m + every_direction[k][0];
                        int k_j = n + every_direction[k][1];
                        if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8)
                        {
                            if (Board::under_king_control(m, n, k_i, k_j) && position.board[k_i][k_j] <= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[k_i][k_j] = WHITE_KING;
                                possible_position.board[m][n] = 0;
                                possible_position.can_castle_white[0] = false;
                                possible_position.can_castle_white[1] = false;
                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                                    if (score > best_score)
                                    {
                                        best_position = Board::copy_position(possible_position);
                                        best_score = score;
                                        from_i = m;
                                        from_j = n;
                                        to_i = k_i;
                                        to_j = k_j;
                                    }
                                }
                            }
                        }
                    }

                    // castling kingside
                    if (m == 7 && n == 4 && position.board[7][5] == 0 && position.board[7][6] == 0 && position.board[7][7] == WHITE_ROOK && !Board::under_control(position.board, 7, 4, BLACK) && !Board::under_control(position.board, 7, 5, BLACK) && !Board::under_control(position.board, 7, 6, BLACK) && position.can_castle_white[1])
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[7][6] = WHITE_KING;
                        possible_position.board[7][4] = 0;
                        possible_position.board[7][7] = 0;
                        possible_position.board[7][5] = WHITE_ROOK;
                        possible_position.can_castle_white[0] = false;
                        possible_position.can_castle_white[1] = false;
                        if (!Board::king_is_in_check(possible_position.board, WHITE))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                            if (score > best_score)
                            {
                                best_position = Board::copy_position(possible_position);
                                best_score = score;
                                from_i = 7;
                                from_j = 4;
                                to_i = 7;
                                to_j = 6;
                            }
                        }
                    }
                    // castling queenside
                    if (m == 7 && n == 4 && position.board[7][1] == 0 && position.board[7][2] == 0 && position.board[7][3] == 0 && position.board[7][0] == WHITE_ROOK && !Board::under_control(position.board, 7, 2, BLACK) && !Board::under_control(position.board, 7, 3, BLACK) && !Board::under_control(position.board, 7, 4, BLACK) && position.can_castle_white[0])
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[7][2] = WHITE_KING;
                        possible_position.board[7][4] = 0;
                        possible_position.board[7][0] = 0;
                        possible_position.board[7][3] = WHITE_ROOK;
                        possible_position.can_castle_white[0] = false;
                        possible_position.can_castle_white[1] = false;
                        if (!Board::king_is_in_check(possible_position.board, WHITE))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);
                            if (score > best_score)
                            {
                                best_position = Board::copy_position(possible_position);
                                best_score = score;
                                from_i = 7;
                                from_j = 4;
                                to_i = 7;
                                to_j = 2;
                            }
                        }
                    }
                }
            }
        }
    }

    // --- Stores Information in result data structure ---
    computer_move_notation = board.generate_move_notation(from_i, from_j, to_i, to_j, WHITE);
    EngineMove result;
    result.notation = computer_move_notation;
    result.from_i = from_i;
    result.from_j = from_j;
    result.to_i = to_i;
    result.to_j = to_j;
    result.eval = (float)best_score / 100;
    result.nodes = paths;

    // --- resets the total no of nodes visited ---
    paths = 0;

    // --- Apply best move to actual game ---
    position = Board::copy_position(best_position);

     return result;
}

// --- Returns a vector of top 1–3 best White moves sorted by evaluation ---
std::vector<EngineMove> Engine::get_best_white_moves(board_state &position)
{
    Board board;
    board.get_position() = position;
    Board::reset_en_passant(position, WHITE);
    board_state possible_position;

    std::vector<EngineMove> move_list;

    for (int m = 0; m < 8; m++)
    {
        for (int n = 0; n < 8; n++)
        {
            if (position.board[m][n] > 0)
            {
                if (position.board[m][n] == WHITE_PAWN)
                {
                    // one move forward by pawn
                    if (position.board[m - 1][n] == 0)
                    {
                        // if not moving to last rank
                        if (m - 1 != 0)
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m - 1][n] = WHITE_PAWN;
                            possible_position.board[m][n] = 0;

                            if (!Board::king_is_in_check(possible_position.board, WHITE))
                            {
                                int alpha = INT_MIN, beta = INT_MAX;
                                int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                                EngineMove em;
                                em.from_i = m;
                                em.from_j = n;
                                em.to_i = m - 1;
                                em.to_j = n;
                                em.eval = (float)score / 100;
                                em.notation = board.generate_move_notation(m, n, m - 1, n, WHITE);
                                em.nodes = paths;

                                move_list.push_back(em);
                            }
                        }
                        // if moves last rank, then cycle through promotion of all pieces
                        else
                        {
                            // variable i denotes possible pieces from white knight to queen
                            for (char i = 5; i >= 2; i--)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m - 1][n] = i;
                                possible_position.board[m][n] = 0;

                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                                    EngineMove em;
                                    em.from_i = m;
                                    em.from_j = n;
                                    em.to_i = m - 1;
                                    em.to_j = n;
                                    em.eval = (float)score / 100;
                                    em.notation = board.generate_move_notation(m, n, m - 1, n, WHITE);
                                    em.nodes = paths;

                                    move_list.push_back(em);
                                }
                            }
                        }
                    }
                    // two moves from starting position by pawn
                    if (m == 6 && position.board[m - 1][n] == 0 && position.board[m - 2][n] == 0)
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[m - 2][n] = WHITE_PAWN;
                        possible_position.board[m][n] = 0;

                        possible_position.pawn_two_squares_white[n] = true;

                        if (!Board::king_is_in_check(possible_position.board, WHITE))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                            EngineMove em;
                            em.from_i = m;
                            em.from_j = n;
                            em.to_i = m - 2;
                            em.to_j = n;
                            em.eval = (float)score / 100;
                            em.notation = board.generate_move_notation(m, n, m - 2, n, WHITE);
                            em.nodes = paths;

                            move_list.push_back(em);
                        }
                    }
                    // capture to the left
                    if (n != 7 && position.board[m - 1][n + 1] < 0)
                    {
                        if (m - 1 != 0)
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m - 1][n + 1] = WHITE_PAWN;
                            possible_position.board[m][n] = 0;
                            if (!Board::king_is_in_check(possible_position.board, WHITE))
                            {
                                int alpha = INT_MIN, beta = INT_MAX;
                                int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                                EngineMove em;
                                em.from_i = m;
                                em.from_j = n;
                                em.to_i = m - 1;
                                em.to_j = n + 1;
                                em.eval = (float)score / 100;
                                em.notation = board.generate_move_notation(m, n, m - 1, n + 1, WHITE);
                                em.nodes = paths;

                                move_list.push_back(em);
                            }
                        }
                        // if moves last rank, then cycle through promotion of all pieces
                        else
                        {
                            // variable i is possible pieces from knight to queen
                            for (char i = 5; i >= 2; i--)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m - 1][n + 1] = i;
                                possible_position.board[m][n] = 0;

                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                                    EngineMove em;
                                    em.from_i = m;
                                    em.from_j = n;
                                    em.to_i = m - 1;
                                    em.to_j = n + 1;
                                    em.eval = (float)score / 100;
                                    em.notation = board.generate_move_notation(m, n, m - 1, n + 1, WHITE);
                                    em.nodes = paths;

                                    move_list.push_back(em);
                                }
                            }
                        }
                    }
                    // capture to the right
                    if (n != 0 && position.board[m - 1][n - 1] < 0)
                    {
                        if (m - 1 != 0)
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m - 1][n - 1] = WHITE_PAWN;
                            possible_position.board[m][n] = 0;
                            if (!Board::king_is_in_check(possible_position.board, WHITE))
                            {
                                int alpha = INT_MIN, beta = INT_MAX;
                                int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                                EngineMove em;
                                em.from_i = m;
                                em.from_j = n;
                                em.to_i = m - 1;
                                em.to_j = n - 1;
                                em.eval = (float)score / 100;
                                em.notation = board.generate_move_notation(m, n, m - 1, n - 1, WHITE);
                                em.nodes = paths;

                                move_list.push_back(em);
                            }
                        }
                        // if moves last rank, then cycle through promotion of all pieces
                        else
                        {
                            // variable i is possible pieces from white knight to queen
                            for (char i = 5; i >= 2; i--)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m - 1][n - 1] = i;
                                possible_position.board[m][n] = 0;

                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                                    EngineMove em;
                                    em.from_i = m;
                                    em.from_j = n;
                                    em.to_i = m - 1;
                                    em.to_j = n - 1;
                                    em.eval = (float)score / 100;
                                    em.notation = board.generate_move_notation(m, n, m - 1, n - 1, WHITE);
                                    em.nodes = paths;

                                    move_list.push_back(em);
                                }
                            }
                        }
                    }
                    // en passant to the left
                    if (n != 7 && position.board[m][n + 1] == -1 && position.pawn_two_squares_black[n + 1])
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[m - 1][n + 1] = WHITE_PAWN;
                        possible_position.board[m][n] = 0;
                        possible_position.board[m][n + 1] = 0;
                        if (!Board::king_is_in_check(possible_position.board, WHITE))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                            EngineMove em;
                            em.from_i = m;
                            em.from_j = n;
                            em.to_i = m - 1;
                            em.to_j = n + 1;
                            em.eval = (float)score / 100;
                            em.notation = board.generate_move_notation(m, n, m - 1, n + 1, WHITE);
                            em.nodes = paths;

                            move_list.push_back(em);
                        }
                    }
                    // en passant to the right
                    if (n != 0 && position.board[m][n - 1] == -1 && position.pawn_two_squares_black[n - 1])
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[m - 1][n - 1] = WHITE_PAWN;
                        possible_position.board[m][n] = 0;
                        possible_position.board[m][n - 1] = 0;
                        if (!Board::king_is_in_check(possible_position.board, WHITE))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                            EngineMove em;
                            em.from_i = m;
                            em.from_j = n;
                            em.to_i = m - 1;
                            em.to_j = n - 1;
                            em.eval = (float)score / 100;
                            em.notation = board.generate_move_notation(m, n, m - 1, n - 1, WHITE);
                            em.nodes = paths;

                            move_list.push_back(em);
                        }
                    }
                }

                if (position.board[m][n] == WHITE_KNIGHT)
                {
                    for (int k = 0; k < 8; k++)
                    {
                        int k_i = m + knight_move[k][0];
                        int k_j = n + knight_move[k][1];
                        if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8)
                        {
                            if (Board::under_knight_control(m, n, k_i, k_j) && position.board[k_i][k_j] <= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[k_i][k_j] = WHITE_KNIGHT;
                                possible_position.board[m][n] = 0;
                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                                    EngineMove em;
                                    em.from_i = m;
                                    em.from_j = n;
                                    em.to_i = k_i;
                                    em.to_j = k_j;
                                    em.eval = (float)score / 100;
                                    em.notation = board.generate_move_notation(m, n, k_i, k_j, WHITE);
                                    em.nodes = paths;

                                    move_list.push_back(em);
                                }
                            }
                        }
                    }
                }

                if (position.board[m][n] == WHITE_BISHOP)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        int path_i = m + bishop_direction[k][0];
                        int path_j = n + bishop_direction[k][1];
                        while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                        {
                            if (Board::under_bishop_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] <= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[path_i][path_j] = WHITE_BISHOP;
                                possible_position.board[m][n] = 0;
                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                                    EngineMove em;
                                    em.from_i = m;
                                    em.from_j = n;
                                    em.to_i = path_i;
                                    em.to_j = path_j;
                                    em.eval = (float)score / 100;
                                    em.notation = board.generate_move_notation(m, n, path_i, path_j, WHITE);
                                    em.nodes = paths;

                                    move_list.push_back(em);
                                }
                            }
                            // if next square is not empty, we stop
                            if (position.board[path_i][path_j] != 0)
                            {
                                break;
                            }
                            path_i += bishop_direction[k][0];
                            path_j += bishop_direction[k][1];
                        }
                    }
                }

                if (position.board[m][n] == WHITE_ROOK)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        int path_i = m + rook_direction[k][0];
                        int path_j = n + rook_direction[k][1];
                        while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                        {
                            if (Board::under_rook_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] <= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[path_i][path_j] = WHITE_ROOK;
                                possible_position.board[m][n] = 0; // recheck logic
                                if (m == 7 && n == 0)
                                {
                                    possible_position.can_castle_white[0] = false;
                                }
                                if (m == 7 && n == 7)
                                {
                                    possible_position.can_castle_white[1] = false;
                                }
                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                                    EngineMove em;
                                    em.from_i = m;
                                    em.from_j = n;
                                    em.to_i = path_i;
                                    em.to_j = path_j;
                                    em.eval = (float)score / 100;
                                    em.notation = board.generate_move_notation(m, n, path_i, path_j, WHITE);
                                    em.nodes = paths;

                                    move_list.push_back(em);
                                }
                            }
                            // if next square is not empty, we stop
                            if (position.board[path_i][path_j] != 0)
                            {
                                break;
                            }
                            path_i += rook_direction[k][0];
                            path_j += rook_direction[k][1];
                        }
                    }
                }

                if (position.board[m][n] == WHITE_QUEEN)
                {
                    for (int k = 0; k < 8; k++)
                    {
                        int path_i = m + every_direction[k][0];
                        int path_j = n + every_direction[k][1];
                        while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                        {
                            if (Board::under_queen_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] <= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[path_i][path_j] = WHITE_QUEEN;
                                possible_position.board[m][n] = 0;
                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                                    EngineMove em;
                                    em.from_i = m;
                                    em.from_j = n;
                                    em.to_i = path_i;
                                    em.to_j = path_j;
                                    em.eval = (float)score / 100;
                                    em.notation = board.generate_move_notation(m, n, path_i, path_j, WHITE);
                                    em.nodes = paths;

                                    move_list.push_back(em);
                                }
                            }
                            // if next square is not empty, we stop
                            if (position.board[path_i][path_j] != 0)
                            {
                                break;
                            }
                            path_i += every_direction[k][0];
                            path_j += every_direction[k][1];
                        }
                    }
                }

                if (position.board[m][n] == WHITE_KING)
                {
                    for (int k = 0; k < 8; k++)
                    {
                        int k_i = m + every_direction[k][0];
                        int k_j = n + every_direction[k][1];
                        if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8)
                        {
                            if (Board::under_king_control(m, n, k_i, k_j) && position.board[k_i][k_j] <= 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[k_i][k_j] = WHITE_KING;
                                possible_position.board[m][n] = 0;
                                possible_position.can_castle_white[0] = false;
                                possible_position.can_castle_white[1] = false;
                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    int alpha = INT_MIN, beta = INT_MAX;
                                    int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                                    EngineMove em;
                                    em.from_i = m;
                                    em.from_j = n;
                                    em.to_i = k_i;
                                    em.to_j = k_j;
                                    em.eval = (float)score / 100;
                                    em.notation = board.generate_move_notation(m, n, k_i, k_j, WHITE);
                                    em.nodes = paths;

                                    move_list.push_back(em);
                                }
                            }
                        }
                    }

                    // castling kingside
                    if (m == 7 && n == 4 && position.board[7][5] == 0 && position.board[7][6] == 0 && position.board[7][7] == WHITE_ROOK && !Board::under_control(position.board, 7, 4, BLACK) && !Board::under_control(position.board, 7, 5, BLACK) && !Board::under_control(position.board, 7, 6, BLACK) && position.can_castle_white[1])
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[7][6] = WHITE_KING;
                        possible_position.board[7][4] = 0;
                        possible_position.board[7][7] = 0;
                        possible_position.board[7][5] = WHITE_ROOK;
                        possible_position.can_castle_white[0] = false;
                        possible_position.can_castle_white[1] = false;
                        if (!Board::king_is_in_check(possible_position.board, WHITE))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                            EngineMove em;
                            em.from_i = 7;
                            em.from_j = 4;
                            em.to_i = 7;
                            em.to_j = 6;
                            em.eval = (float)score / 100;
                            em.notation = board.generate_move_notation(7, 4, 7, 6, WHITE);
                            em.nodes = paths;

                            move_list.push_back(em);
                        }
                    }
                    // castling queenside
                    if (m == 7 && n == 4 && position.board[7][1] == 0 && position.board[7][2] == 0 && position.board[7][3] == 0 && position.board[7][0] == WHITE_ROOK && !Board::under_control(position.board, 7, 2, BLACK) && !Board::under_control(position.board, 7, 3, BLACK) && !Board::under_control(position.board, 7, 4, BLACK) && position.can_castle_white[0])
                    {
                        possible_position = Board::copy_position(position);
                        possible_position.board[7][2] = WHITE_KING;
                        possible_position.board[7][4] = 0;
                        possible_position.board[7][0] = 0;
                        possible_position.board[7][3] = WHITE_ROOK;
                        possible_position.can_castle_white[0] = false;
                        possible_position.can_castle_white[1] = false;
                        if (!Board::king_is_in_check(possible_position.board, WHITE))
                        {
                            int alpha = INT_MIN, beta = INT_MAX;
                            int score = adv_minimax(possible_position, DEPTH - 1, false, alpha, beta);

                            EngineMove em;
                            em.from_i = 7;
                            em.from_j = 4;
                            em.to_i = 7;
                            em.to_j = 2;
                            em.eval = (float)score / 100;
                            em.notation = board.generate_move_notation(7, 4, 7, 2, WHITE);
                            em.nodes = paths;

                            move_list.push_back(em);
                        }
                    }
                }
            }
        }
    }

    // --- Sort the output vector by eval descending (best move first) ---
    std::sort(move_list.begin(), move_list.end(), [](const EngineMove &a, const EngineMove &b)
              { return a.eval > b.eval; });

    // --- Filter top 3 or less moves with reasonable evals ---
    std::vector<EngineMove> top_moves;
    for (const auto &move : move_list)
    {
        if (move.eval >= -0.5f) // Filter out very bad moves
        {
            top_moves.push_back(move);
            if (top_moves.size() == 3)
                break;
        }
    }

    // --- Fallback if not enough good moves ---
    if (top_moves.empty() && !move_list.empty())
        top_moves.push_back(move_list.front()); // at least return best move

    paths = 0;

    return top_moves;
}

// --- Minimax function with alpha-beta pruning for evaluating board positions ---
int Engine::adv_minimax(board_state &position, int depth, bool maximizingPlayer, int alpha, int beta)
{
    paths++;
    board_state possible_position;

    int score = Evaluation::evaluate(position, depth);
    if (depth == 0 || game_is_over(position))
    {
        return score;
    }

    // --- this is maximizer's move (choose best move for white) ---
    if (maximizingPlayer)
    {
        Board::reset_en_passant(position, WHITE);
        score = INT_MIN;
        for (int m = 0; m < 8; m++)
        {
            for (int n = 0; n < 8; n++)
            {
                if (position.board[m][n] > 0)
                {

                    if (position.board[m][n] == WHITE_PAWN)
                    {
                        // one move forward by pawn
                        if (position.board[m - 1][n] == 0)
                        {
                            // if not move to last rank
                            if (m - 1 != 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m - 1][n] = WHITE_PAWN;
                                possible_position.board[m][n] = 0;

                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                    alpha = max(alpha, score);
                                    if (beta <= alpha)
                                        break;
                                }
                            }
                            // if moves last rank, then promote to all pieces
                            else
                            {
                                // variable i is possible pieces from white knight to queen
                                for (char i = 2; i <= 5; i++)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[m - 1][n] = i;
                                    possible_position.board[m][n] = 0;

                                    if (!Board::king_is_in_check(possible_position.board, WHITE))
                                    {
                                        score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                        alpha = max(alpha, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                            }
                        }
                        // two moves from starting position by pawn
                        if (m == 6 && position.board[m - 1][n] == 0 && position.board[m - 2][n] == 0)
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m - 2][n] = WHITE_PAWN;
                            possible_position.board[m][n] = 0;

                            possible_position.pawn_two_squares_white[n] = true;

                            if (!Board::king_is_in_check(possible_position.board, WHITE))
                            {
                                score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                alpha = max(alpha, score);
                                if (beta <= alpha)
                                    break;
                            }
                        }
                        // capture to the left
                        if (n != 0 && position.board[m - 1][n - 1] < 0)
                        {
                            // not to last rank
                            if (m - 1 != 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m - 1][n - 1] = WHITE_PAWN;
                                possible_position.board[m][n] = 0;
                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                    alpha = max(alpha, score);
                                    if (beta <= alpha)
                                        break;
                                }
                            }
                            else
                            {
                                // variable i is possible pieces from white knight to queen
                                for (char i = 2; i <= 5; i++)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[m - 1][n - 1] = i;
                                    possible_position.board[m][n] = 0;

                                    if (!Board::king_is_in_check(possible_position.board, WHITE))
                                    {
                                        score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                        alpha = max(alpha, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                            }
                        }
                        // capture to the right
                        if (n != 7 && position.board[m - 1][n + 1] < 0)
                        {
                            // not last rank
                            if (m - 1 != 0)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m - 1][n + 1] = WHITE_PAWN;
                                possible_position.board[m][n] = 0;
                                if (!Board::king_is_in_check(possible_position.board, WHITE))
                                {
                                    score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                    alpha = max(alpha, score);
                                    if (beta <= alpha)
                                        break;
                                }
                            }
                            // last rank (promote)
                            else
                            {
                                // variable i is possible pieces from white knight to queen
                                for (char i = 2; i <= 5; i++)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[m - 1][n + 1] = i;
                                    possible_position.board[m][n] = 0;

                                    if (!Board::king_is_in_check(possible_position.board, WHITE))
                                    {
                                        score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                        alpha = max(alpha, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                            }
                        }

                        // en passant to the right
                        if (n != 7 && position.board[m][n + 1] == -1 && position.pawn_two_squares_black[n + 1])
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m - 1][n + 1] = WHITE_PAWN;
                            possible_position.board[m][n] = 0;
                            possible_position.board[m][n + 1] = 0;
                            if (!Board::king_is_in_check(possible_position.board, WHITE))
                            {
                                score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                alpha = max(alpha, score);
                                if (beta <= alpha)
                                    break;
                            }
                        }

                        // en passant to the left
                        if (n != 0 && position.board[m][n - 1] == -1 && position.pawn_two_squares_black[n - 1])
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m - 1][n - 1] = WHITE_PAWN;
                            possible_position.board[m][n] = 0;
                            possible_position.board[m][n - 1] = 0;
                            if (!Board::king_is_in_check(possible_position.board, WHITE))
                            {
                                score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                alpha = max(alpha, score);
                                if (beta <= alpha)
                                    break;
                            }
                        }
                    }

                    if (position.board[m][n] == WHITE_KNIGHT)
                    {
                        for (int k = 0; k < 8; k++)
                        {
                            int k_i = m + knight_move[k][0];
                            int k_j = n + knight_move[k][1];
                            if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8)
                            {
                                if (Board::under_knight_control(m, n, k_i, k_j) && position.board[k_i][k_j] <= 0)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[k_i][k_j] = WHITE_KNIGHT;
                                    possible_position.board[m][n] = 0;
                                    if (!Board::king_is_in_check(possible_position.board, WHITE))
                                    {
                                        score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                        alpha = max(alpha, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                            }
                        }
                    }

                    if (position.board[m][n] == WHITE_BISHOP)
                    {
                        for (int k = 0; k < 4; k++)
                        {
                            int path_i = m + bishop_direction[k][0];
                            int path_j = n + bishop_direction[k][1];
                            while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                            {
                                if (Board::under_bishop_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] <= 0)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[path_i][path_j] = WHITE_BISHOP;
                                    possible_position.board[m][n] = 0;
                                    if (!Board::king_is_in_check(possible_position.board, WHITE))
                                    {
                                        score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                        alpha = max(alpha, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                                // if next square is not empty, we stop
                                if (position.board[path_i][path_j] != 0)
                                {
                                    break;
                                }
                                path_i += bishop_direction[k][0];
                                path_j += bishop_direction[k][1];
                            }
                        }
                    }

                    if (position.board[m][n] == WHITE_ROOK)
                    {
                        for (int k = 0; k < 4; k++)
                        {
                            int path_i = m + rook_direction[k][0];
                            int path_j = n + rook_direction[k][1];
                            while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                            {
                                if (Board::under_rook_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] <= 0)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[path_i][path_j] = WHITE_ROOK;
                                    possible_position.board[m][n] = 0;
                                    if (m == 7 && n == 0)
                                    {
                                        possible_position.can_castle_white[0] = false;
                                    }
                                    if (m == 7 && n == 7)
                                    {
                                        possible_position.can_castle_white[1] = false;
                                    }
                                    if (!Board::king_is_in_check(possible_position.board, WHITE))
                                    {
                                        score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                        alpha = max(alpha, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                                // if next square is not empty, we stop
                                if (position.board[path_i][path_j] != 0)
                                {
                                    break;
                                }
                                path_i += rook_direction[k][0];
                                path_j += rook_direction[k][1];
                            }
                        }
                    }

                    if (position.board[m][n] == WHITE_QUEEN)
                    {
                        for (int k = 0; k < 8; k++)
                        {
                            int path_i = m + every_direction[k][0];
                            int path_j = n + every_direction[k][1];
                            while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                            {
                                if (Board::under_queen_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] <= 0)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[path_i][path_j] = WHITE_QUEEN;
                                    possible_position.board[m][n] = 0;
                                    if (!Board::king_is_in_check(possible_position.board, WHITE))
                                    {
                                        score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                        alpha = max(alpha, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                                // if next square is not empty, we stop
                                if (position.board[path_i][path_j] != 0)
                                {
                                    break;
                                }
                                path_i += every_direction[k][0];
                                path_j += every_direction[k][1];
                            }
                        }
                    }

                    if (position.board[m][n] == WHITE_KING)
                    {
                        for (int k = 0; k < 8; k++)
                        {
                            int k_i = m + every_direction[k][0];
                            int k_j = n + every_direction[k][1];
                            if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8)
                            {
                                if (Board::under_king_control(m, n, k_i, k_j) && position.board[k_i][k_j] <= 0)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[k_i][k_j] = WHITE_KING;
                                    possible_position.board[m][n] = 0;
                                    possible_position.can_castle_white[0] = false;
                                    possible_position.can_castle_white[1] = false;
                                    if (!Board::king_is_in_check(possible_position.board, WHITE))
                                    {
                                        score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                        alpha = max(alpha, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                            }
                        }
                        // castling kingside
                        if (m == 7 && n == 4 && position.board[7][5] == 0 && position.board[7][6] == 0 && position.board[7][7] == WHITE_ROOK && !Board::under_control(position.board, 7, 4, BLACK) && !Board::under_control(position.board, 7, 5, BLACK) && !Board::under_control(position.board, 7, 6, BLACK) && position.can_castle_white[1])
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[7][6] = WHITE_KING;
                            possible_position.board[7][4] = 0;
                            possible_position.board[7][7] = 0;
                            possible_position.board[7][5] = WHITE_ROOK;
                            possible_position.can_castle_white[0] = false;
                            possible_position.can_castle_white[1] = false;
                            if (!Board::king_is_in_check(possible_position.board, WHITE))
                            {
                                score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                alpha = max(alpha, score);
                                if (beta <= alpha)
                                    break;
                            }
                        }
                        // castling queenside
                        if (m == 7 && n == 4 && position.board[7][1] == 0 && position.board[7][2] == 0 && position.board[7][3] == 0 && position.board[7][0] == WHITE_ROOK && !Board::under_control(position.board, 7, 1, BLACK) && !Board::under_control(position.board, 7, 2, BLACK) && !Board::under_control(position.board, 7, 3, BLACK) && position.can_castle_white[0])
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[7][2] = WHITE_KING;
                            possible_position.board[7][4] = 0;
                            possible_position.board[7][0] = 0;
                            possible_position.board[7][3] = WHITE_ROOK;
                            possible_position.can_castle_white[0] = false;
                            possible_position.can_castle_white[1] = false;
                            if (!Board::king_is_in_check(possible_position.board, WHITE))
                            {
                                score = max(score, adv_minimax(possible_position, depth - 1, false, alpha, beta));
                                alpha = max(alpha, score);
                                if (beta <= alpha)
                                    break;
                            }
                        }
                    }
                }
            }
        }
        return score;
    }
    // --- this is the minimizer's move (choose best move for black) ---
    else
    {
        Board::reset_en_passant(position, BLACK);
        score = INT_MAX;
        for (int m = 0; m < 8; m++)
        {
            for (int n = 0; n < 8; n++)
            {
                if (position.board[m][n] < 0)
                {

                    if (position.board[m][n] == BLACK_PAWN)
                    {
                        // one move forward by pawn
                        if (position.board[m + 1][n] == 0)
                        {
                            // if not moving to last rank
                            if (m + 1 != 7)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m + 1][n] = BLACK_PAWN;
                                possible_position.board[m][n] = 0;

                                if (!Board::king_is_in_check(possible_position.board, BLACK))
                                {
                                    score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                    beta = min(beta, score);
                                    if (beta <= alpha)
                                        break;
                                }
                            }
                            // if moves last rank, then promote to all pieces
                            else
                            {
                                // variable i is possible pieces from black knight to queen
                                for (char i = -5; i <= -2; i++)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[m + 1][n] = i;
                                    possible_position.board[m][n] = 0;

                                    if (!Board::king_is_in_check(possible_position.board, BLACK))
                                    {
                                        score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                        beta = min(beta, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                            }
                        }
                        // two moves from starting position by pawn
                        if (m == 1 && position.board[m + 1][n] == 0 && position.board[m + 2][n] == 0)
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m + 2][n] = BLACK_PAWN;
                            possible_position.board[m][n] = 0;

                            possible_position.pawn_two_squares_black[n] = true;

                            if (!Board::king_is_in_check(possible_position.board, BLACK))
                            {
                                score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                beta = min(beta, score);
                                if (beta <= alpha)
                                    break;
                            }
                        }
                        // capture to the left
                        if (n != 0 && position.board[m + 1][n - 1] > 0)
                        {
                            if (m + 1 != 7)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m + 1][n - 1] = BLACK_PAWN;
                                possible_position.board[m][n] = 0;
                                if (!Board::king_is_in_check(possible_position.board, BLACK))
                                {
                                    score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                    beta = min(beta, score);
                                    if (beta <= alpha)
                                        break;
                                }
                            }
                            // if moves last rank, then promote to all pieces
                            else
                            {
                                // variable i is possible pieces from black knight to queen
                                for (char i = -5; i <= -2; i++)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[m + 1][n - 1] = i;
                                    possible_position.board[m][n] = 0;

                                    if (!Board::king_is_in_check(possible_position.board, BLACK))
                                    {
                                        score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                        beta = min(beta, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                            }
                        }
                        // capture to the right
                        if (n != 7 && position.board[m + 1][n + 1] > 0)
                        {
                            if (m + 1 != 7)
                            {
                                possible_position = Board::copy_position(position);
                                possible_position.board[m + 1][n + 1] = BLACK_PAWN;
                                possible_position.board[m][n] = 0;
                                if (!Board::king_is_in_check(possible_position.board, BLACK))
                                {
                                    score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                    beta = min(beta, score);
                                    if (beta <= alpha)
                                        break;
                                }
                            }
                            // if moves last rank, then promote to all pieces
                            else
                            {
                                // variable i is possible pieces from black knight to queen
                                for (char i = -5; i <= -2; i++)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[m + 1][n + 1] = i;
                                    possible_position.board[m][n] = 0;

                                    if (!Board::king_is_in_check(possible_position.board, BLACK))
                                    {
                                        score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                        beta = min(beta, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                            }
                        }

                        // en passant to the right
                        if (n != 7 && position.board[m][n + 1] == 1 && position.pawn_two_squares_white[n + 1])
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m + 1][n + 1] = BLACK_PAWN;
                            possible_position.board[m][n] = 0;
                            possible_position.board[m][n + 1] = 0;
                            if (!Board::king_is_in_check(possible_position.board, BLACK))
                            {
                                score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                beta = min(beta, score);
                                if (beta <= alpha)
                                    break;
                            }
                        }

                        // en passant to the left
                        if (n != 0 && position.board[m][n - 1] == 1 && position.pawn_two_squares_white[n - 1])
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[m + 1][n - 1] = BLACK_PAWN;
                            possible_position.board[m][n] = 0;
                            possible_position.board[m][n - 1] = 0;
                            if (!Board::king_is_in_check(possible_position.board, BLACK))
                            {
                                score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                beta = min(beta, score);
                                if (beta <= alpha)
                                    break;
                            }
                        }
                    }

                    if (position.board[m][n] == BLACK_KNIGHT)
                    {
                        for (int k = 0; k < 8; k++)
                        {
                            int k_i = m + knight_move[k][0];
                            int k_j = n + knight_move[k][1];
                            if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8)
                            {
                                if (Board::under_knight_control(m, n, k_i, k_j) && position.board[k_i][k_j] >= 0)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[k_i][k_j] = BLACK_KNIGHT;
                                    possible_position.board[m][n] = 0;
                                    if (!Board::king_is_in_check(possible_position.board, BLACK))
                                    {
                                        score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                        beta = min(beta, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                            }
                        }
                    }

                    if (position.board[m][n] == BLACK_BISHOP)
                    {
                        for (int k = 0; k < 4; k++)
                        {
                            int path_i = m + bishop_direction[k][0];
                            int path_j = n + bishop_direction[k][1];
                            while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                            {
                                if (Board::under_bishop_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] >= 0)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[path_i][path_j] = BLACK_BISHOP;
                                    possible_position.board[m][n] = 0;
                                    if (!Board::king_is_in_check(possible_position.board, BLACK))
                                    {
                                        score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                        beta = min(beta, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                                // if next square is not empty, we stop
                                if (position.board[path_i][path_j] != 0)
                                {
                                    break;
                                }
                                path_i += bishop_direction[k][0];
                                path_j += bishop_direction[k][1];
                            }
                        }
                    }

                    if (position.board[m][n] == BLACK_ROOK)
                    {
                        for (int k = 0; k < 4; k++)
                        {
                            int path_i = m + rook_direction[k][0];
                            int path_j = n + rook_direction[k][1];
                            while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                            {
                                if (Board::under_rook_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] >= 0)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[path_i][path_j] = BLACK_ROOK;
                                    possible_position.board[m][n] = 0;
                                    if (m == 0 && n == 0)
                                    {
                                        possible_position.can_castle_black[0] = false;
                                    }
                                    if (m == 0 && n == 7)
                                    {
                                        possible_position.can_castle_black[1] = false;
                                    }
                                    if (!Board::king_is_in_check(possible_position.board, BLACK))
                                    {
                                        score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                        beta = min(beta, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                                // if next square is not empty, we stop
                                if (position.board[path_i][path_j] != 0)
                                {
                                    break;
                                }
                                path_i += rook_direction[k][0];
                                path_j += rook_direction[k][1];
                            }
                        }
                    }

                    if (position.board[m][n] == BLACK_QUEEN)
                    {
                        for (int k = 0; k < 8; k++)
                        {
                            int path_i = m + every_direction[k][0];
                            int path_j = n + every_direction[k][1];
                            while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
                            {
                                if (Board::under_queen_control(position.board, m, n, path_i, path_j) && position.board[path_i][path_j] >= 0)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[path_i][path_j] = BLACK_QUEEN;
                                    possible_position.board[m][n] = 0;
                                    if (!Board::king_is_in_check(possible_position.board, BLACK))
                                    {
                                        score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                        beta = min(beta, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                                // if next square is not empty, we stop
                                if (position.board[path_i][path_j] != 0)
                                {
                                    break;
                                }
                                path_i += every_direction[k][0];
                                path_j += every_direction[k][1];
                            }
                        }
                    }

                    if (position.board[m][n] == BLACK_KING)
                    {
                        for (int k = 0; k < 8; k++)
                        {
                            int k_i = m + every_direction[k][0];
                            int k_j = n + every_direction[k][1];
                            if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8)
                            {
                                if (Board::under_king_control(m, n, k_i, k_j) && position.board[k_i][k_j] >= 0)
                                {
                                    possible_position = Board::copy_position(position);
                                    possible_position.board[k_i][k_j] = BLACK_KING;
                                    possible_position.board[m][n] = 0;
                                    possible_position.can_castle_black[0] = false;
                                    possible_position.can_castle_black[1] = false;
                                    if (!Board::king_is_in_check(possible_position.board, BLACK))
                                    {
                                        score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                        beta = min(beta, score);
                                        if (beta <= alpha)
                                            break;
                                    }
                                }
                            }
                        }
                        // castling kingside
                        if (m == 0 && n == 4 && position.board[0][5] == 0 && position.board[0][6] == 0 && position.board[0][7] == BLACK_ROOK && !Board::under_control(position.board, 0, 4, WHITE) && !Board::under_control(position.board, 0, 5, WHITE) && !Board::under_control(position.board, 0, 6, WHITE) && position.can_castle_black[1])
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[0][6] = BLACK_KING;
                            possible_position.board[0][4] = 0;
                            possible_position.board[0][7] = 0;
                            possible_position.board[0][5] = BLACK_ROOK;
                            possible_position.can_castle_black[0] = false;
                            possible_position.can_castle_black[1] = false;
                            if (!Board::king_is_in_check(possible_position.board, BLACK))
                            {
                                score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                beta = min(beta, score);
                                if (beta <= alpha)
                                    break;
                            }
                        }
                        // castling queenside
                        if (m == 0 && n == 4 && position.board[0][1] == 0 && position.board[0][2] == 0 && position.board[0][3] == 0 && position.board[0][0] == BLACK_ROOK && !Board::under_control(position.board, 0, 2, WHITE) && !Board::under_control(position.board, 0, 3, WHITE) && !Board::under_control(position.board, 0, 4, WHITE) && position.can_castle_black[0])
                        {
                            possible_position = Board::copy_position(position);
                            possible_position.board[0][2] = BLACK_KING;
                            possible_position.board[0][4] = 0;
                            possible_position.board[0][0] = 0;
                            possible_position.board[0][3] = BLACK_ROOK;
                            possible_position.can_castle_black[0] = false;
                            possible_position.can_castle_black[1] = false;
                            if (!Board::king_is_in_check(possible_position.board, BLACK))
                            {
                                score = min(score, adv_minimax(possible_position, depth - 1, true, alpha, beta));
                                beta = min(beta, score);
                                if (beta <= alpha)
                                    break;
                            }
                        }
                    }
                }
            }
        }
        return score;
    }
}

// --- Checks if the game is over due to checkmate or stalemate ---
bool Engine::game_is_over(board_state &position)
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