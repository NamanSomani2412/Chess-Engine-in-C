// --------------------------------------------------------------------------------------
// board.cpp
// --- Implements the functions declared in board.h for board logic and piece movement.
// --- Handles move execution, legality checks, square control checks, en passant logic,
// --- check and checkmate evaluation, and move notation creation.
// --- Contains the core functionality that governs how the chess pieces behave and how
// --- the board state changes in response to player or engine actions.
// --------------------------------------------------------------------------------------

#include "board.h"
#include <string>
#include <iostream>

using namespace std;

// --- Counts how many moves have been played in the game. ---
int move_number = 1;

// --- String used for printing out move information to move display. ---
string move_notation = "";

// --- Returns true if the square at (i, j) is occupied by a white piece. ---
// --- White pieces are represented with positive integers. ---
bool Board::square_occupied_by_white(int i, int j)
{
    if (position.board[i][j] > 0)
    {
        return true;
    }
    return false;
}

// --- Returns true if the square at (i, j) is occupied by a black piece. ---
// --- Black pieces are represented with negative integers. ---
bool Board::square_occupied_by_black(int i, int j)
{
    if (position.board[i][j] < 0)
    {
        return true;
    }
    return false;
}

// --- Returns true if the square at (i, j) is occupied by any piece. ---
// --- A non-zero value indicates that a piece is present. ---
bool Board::square_occupied(int i, int j)
{
    if (position.board[i][j] != 0)
    {
        return true;
    }
    return false;
}

// --- Returns true if the square at (i, j) is occupied by a piece belonging to the opponent of side s. ---
// --- Positive values represent White pieces, and negative values represent Black pieces. ---
bool Board::square_occupied_by_opponent(char board[8][8], int i, int j, side s)
{
    if (s == WHITE)
    {
        if (board[i][j] < 0) // Opponent is Black
        {
            return true;
        }
    }
    else
    {
        if (board[i][j] > 0) // Opponent is White
        {
            return true;
        }
    }
    return false;
}

// --- Handles movement logic for a White piece from a start square to a target square.
// --- Validates move legality based on piece type and board state
// --- Updates board state only if the move is legal and does not leave the king in check.
// --- Returns standard algebraic move notation string if successful; empty string otherwise.
std::string Board::handle_white_move(int start_i, int start_j, int target_i, int target_j)
{
    // --- Create a copy of the current position to simulate the move before confirming legality ---
    std::string move_string = "";
    board_state possible_position;
    copy_board(position.board, possible_position.board);

    // --- Copy castling and en passant flags ---
    possible_position.can_castle_black[0] = position.can_castle_black[0];
    possible_position.can_castle_black[1] = position.can_castle_black[1];
    possible_position.can_castle_white[0] = position.can_castle_white[0];
    possible_position.can_castle_white[1] = position.can_castle_white[1];

    for (int n = 0; n < 8; n++)
    {
        possible_position.pawn_two_squares_black[n] = position.pawn_two_squares_black[n];
        possible_position.pawn_two_squares_white[n] = position.pawn_two_squares_white[n];
    }

    bool move_is_legal = false;
    promote_pawn = false;
    en_passant_index = -1;

    // --- If target square contains a white piece, the move is illegal ---
    if (position.board[target_i][target_j] > 0)
    {
        return "";
    }

    // --- Validate the move depending on the type of piece being moved ---
    switch (position.board[start_i][start_j])
    {
    case WHITE_PAWN:
        if (pawn_move(start_i, start_j, target_i, target_j))
        {
            move_piece(possible_position, start_i, start_j, target_i, target_j);

            // --- Handle en passant capture ---
            if (start_i == 3 && target_i == 2 &&
                abs(target_j - start_j) == 1 &&
                position.board[start_i][target_j] == BLACK_PAWN &&
                position.pawn_two_squares_black[target_j])
            {
                possible_position.board[start_i][target_j] = 0;
            }
            move_is_legal = true;
        }
        break;
    case WHITE_KNIGHT:
        if (under_knight_control(start_i, start_j, target_i, target_j))
        {
            move_piece(possible_position, start_i, start_j, target_i, target_j);
            move_is_legal = true;
        }
        break;
    case WHITE_BISHOP:
        if (under_bishop_control(position.board, start_i, start_j, target_i, target_j))
        {
            move_piece(possible_position, start_i, start_j, target_i, target_j);
            move_is_legal = true;
        }
        break;
    case WHITE_ROOK:
        if (under_rook_control(position.board, start_i, start_j, target_i, target_j))
        {
            move_piece(possible_position, start_i, start_j, target_i, target_j);

            // --- Update castling rights if rook moves from its original square ---
            if (start_i == 7 && start_j == 0)
            {
                possible_position.can_castle_white[0] = false;
            }
            if (start_i == 7 && start_j == 7)
            {
                possible_position.can_castle_white[1] = false;
            }

            move_is_legal = true;
        }
        break;
    case WHITE_QUEEN:
        if (under_queen_control(position.board, start_i, start_j, target_i, target_j))
        {
            move_piece(possible_position, start_i, start_j, target_i, target_j);
            move_is_legal = true;
        }
        break;
    case WHITE_KING:
        if (under_king_control(start_i, start_j, target_i, target_j))
        {
            move_piece(possible_position, start_i, start_j, target_i, target_j);

            // --- King moved, so castling becomes invalid ---
            possible_position.can_castle_white[0] = false;
            possible_position.can_castle_white[1] = false;
            move_is_legal = true;
        }

        // --- Castling kingside ---
        if (start_i == 7 && start_j == 4 && target_i == 7 && target_j == 6 &&
            position.board[7][5] == 0 && position.board[7][6] == 0 &&
            !under_control(position.board, 7, 4, BLACK) &&
            !under_control(position.board, 7, 5, BLACK) &&
            !under_control(position.board, 7, 6, BLACK) &&
            position.can_castle_white[1])
        {
            possible_position.board[target_i][target_j] = possible_position.board[start_i][start_j];
            possible_position.board[start_i][start_j] = 0;
            possible_position.board[7][7] = 0;
            possible_position.board[7][5] = WHITE_ROOK;
            possible_position.can_castle_white[0] = false;
            possible_position.can_castle_white[1] = false;
            move_is_legal = true;
        }

        // --- Castling queenside ---
        if (start_i == 7 && start_j == 4 && target_i == 7 && target_j == 2 &&
            position.board[7][1] == 0 && position.board[7][2] == 0 && position.board[7][3] == 0 &&
            !under_control(position.board, 7, 2, BLACK) &&
            !under_control(position.board, 7, 3, BLACK) &&
            !under_control(position.board, 7, 4, BLACK) &&
            position.can_castle_white[0])
        {
            possible_position.board[target_i][target_j] = possible_position.board[start_i][start_j];
            possible_position.board[start_i][start_j] = 0;
            possible_position.board[7][0] = 0;
            possible_position.board[7][3] = WHITE_ROOK;
            possible_position.can_castle_white[0] = false;
            possible_position.can_castle_white[1] = false;
            move_is_legal = true;
        }
        break;
    default:
        break;
    }

    // --- Final legality check: king must not be in check after move ---
    if (move_is_legal && !king_is_in_check(possible_position.board, WHITE))
    {
        move_string = generate_move_notation(start_i, start_j, target_i, target_j, WHITE);
        copy_board(possible_position.board, position.board);

        // --- Copy updated castling and en passant info to main board ---
        position.can_castle_white[0] = possible_position.can_castle_white[0];
        position.can_castle_white[1] = possible_position.can_castle_white[1];
        position.can_castle_black[0] = possible_position.can_castle_black[0];
        position.can_castle_black[1] = possible_position.can_castle_black[1];

        for (int i = 0; i < 8; i++)
        {
            position.pawn_two_squares_white[i] = false;
            position.pawn_two_squares_black[i] = possible_position.pawn_two_squares_black[i];
        }

        if (en_passant_index != -1)
        {
            position.pawn_two_squares_white[en_passant_index] = true;
        }

        // --- Handle pawn promotion ---
        if (promote_pawn)
        {
            char user_input = gui_promotion_callback(); // Get piece type from GUI

            switch (user_input)
            {
            case 'k':
                position.board[target_i][target_j] = WHITE_KNIGHT;
                break;
            case 'b':
                position.board[target_i][target_j] = WHITE_BISHOP;
                break;
            case 'r':
                position.board[target_i][target_j] = WHITE_ROOK;
                break;
            case 'q':
            default:
                position.board[target_i][target_j] = WHITE_QUEEN;
                break;
            }
        }

        return move_string;
    }

    // --- Move is illegal or would place king in check ---
    return "";
}

// --- Validates black move legality based on piece type and board state
bool Board::is_black_move_legal(board_state &position, int start_i, int start_j, int target_i, int target_j, board_state &possible_position)
{
    // --- Copy current board state to possible_position for simulation ---
    copy_board(position.board, possible_position.board);
    possible_position.can_castle_black[0] = position.can_castle_black[0];
    possible_position.can_castle_black[1] = position.can_castle_black[1];
    possible_position.can_castle_white[0] = position.can_castle_white[0];
    possible_position.can_castle_white[1] = position.can_castle_white[1];

    for (int n = 0; n < 8; n++)
    {
        possible_position.pawn_two_squares_black[n] = position.pawn_two_squares_black[n];
        possible_position.pawn_two_squares_white[n] = position.pawn_two_squares_white[n];
    }

    // --- Initialize legality flag and en passant index tracker ---
    bool move_is_legal = false;
    int en_passant_index = -1;

    // --- Check move legality based on piece type ---
    switch (position.board[start_i][start_j])
    {
    case BLACK_PAWN:
    {
        // --- One square forward ---
        if (target_i == start_i + 1 && target_j == start_j)
        {
            // --- Check if square is empty ---
            if (position.board[target_i][target_j] == 0)
            {
                move_piece(possible_position, start_i, start_j, target_i, target_j);
                move_is_legal = true;
            }
        }
        // --- Two squares forward from starting position ---
        else if (start_i == 1 && target_i == start_i + 2 && start_j == target_j)
        {
            if (position.board[target_i - 1][target_j] == 0 && position.board[target_i][target_j] == 0)
            {
                // --- Reset en passant flags ---
                for (int i = 0; i < 8; i++)
                    possible_position.pawn_two_squares_black[i] = false;

                // --- Mark en passant eligible column ---
                en_passant_index = target_j;
                move_piece(possible_position, start_i, start_j, target_i, target_j);
                possible_position.pawn_two_squares_black[target_j] = true;
                move_is_legal = true;
            }
        }
        // --- En passant capture ---
        else if (start_i == 4 && target_i == 5 && (target_j == (start_j - 1) || target_j == (start_j + 1)) &&
                 position.pawn_two_squares_white[target_j])
        {
            move_piece(possible_position, start_i, start_j, target_i, target_j);
            possible_position.board[4][target_j] = 0; // --- Remove captured pawn ---
            move_is_legal = true;
        }
        // --- Diagonal capture ---
        else if ((target_i == start_i + 1) && (target_j == start_j - 1 || target_j == start_j + 1) && position.board[target_i][target_j] > 0)
        {
            move_piece(possible_position, start_i, start_j, target_i, target_j);
            move_is_legal = true;
        }

        break;
    }

    case BLACK_KNIGHT:
        // --- Knight's L-shaped move ---
        if (under_knight_control(start_i, start_j, target_i, target_j))
        {
            move_piece(possible_position, start_i, start_j, target_i, target_j);
            move_is_legal = true;
        }
        break;

    case BLACK_BISHOP:
        // --- Bishop diagonal movement ---
        if (under_bishop_control(position.board, start_i, start_j, target_i, target_j))
        {
            move_piece(possible_position, start_i, start_j, target_i, target_j);
            move_is_legal = true;
        }
        break;

    case BLACK_ROOK:
        // --- Rook horizontal/vertical movement ---
        if (under_rook_control(position.board, start_i, start_j, target_i, target_j))
        {
            move_piece(possible_position, start_i, start_j, target_i, target_j);

            // --- Disable castling rights if rook moves from original square ---
            if (start_i == 0 && start_j == 0)
            {
                possible_position.can_castle_black[0] = false;
            }
            if (start_i == 0 && start_j == 7)
            {
                possible_position.can_castle_black[1] = false;
            }
            move_is_legal = true;
        }
        break;

    case BLACK_QUEEN:
        // --- Queen diagonal + straight movement ---
        if (under_queen_control(position.board, start_i, start_j, target_i, target_j))
        {
            move_piece(possible_position, start_i, start_j, target_i, target_j);
            move_is_legal = true;
        }
        break;

    case BLACK_KING:
        // --- King standard 1-square movement ---
        if (under_king_control(start_i, start_j, target_i, target_j))
        {
            move_piece(possible_position, start_i, start_j, target_i, target_j);
            possible_position.can_castle_black[0] = false;
            possible_position.can_castle_black[1] = false;
            move_is_legal = true;
        }

        // --- Castling kingside ---
        if (start_i == 0 && start_j == 4 && target_i == 0 && target_j == 6 &&
            position.board[0][5] == 0 && position.board[0][6] == 0 &&
            !under_control(position.board, 0, 4, WHITE) &&
            !under_control(position.board, 0, 5, WHITE) &&
            !under_control(position.board, 0, 6, WHITE) &&
            position.can_castle_black[1])
        {
            possible_position.board[target_i][target_j] = possible_position.board[start_i][start_j];
            possible_position.board[start_i][start_j] = 0;
            possible_position.board[0][7] = 0;
            possible_position.board[0][5] = BLACK_ROOK;

            possible_position.can_castle_black[0] = false;
            possible_position.can_castle_black[1] = false;
            move_is_legal = true;
        }

        // --- Castling queenside ---
        else if (start_i == 0 && start_j == 4 && target_i == 0 && target_j == 2 &&
                 position.board[0][1] == 0 && position.board[0][2] == 0 && position.board[0][3] == 0 &&
                 !under_control(position.board, 0, 2, WHITE) &&
                 !under_control(position.board, 0, 3, WHITE) &&
                 !under_control(position.board, 0, 4, WHITE) &&
                 position.can_castle_black[0])
        {
            possible_position.board[target_i][target_j] = possible_position.board[start_i][start_j];
            possible_position.board[start_i][start_j] = 0;
            possible_position.board[0][0] = 0;
            possible_position.board[0][3] = BLACK_ROOK;

            possible_position.can_castle_black[0] = false;
            possible_position.can_castle_black[1] = false;
            move_is_legal = true;
        }
        break;

    default:
        break;
    }

    // --- Final legality check: move must not leave king in check ---
    if (move_is_legal && !king_is_in_check(possible_position.board, BLACK))
    {
        // --- Commit move to actual position ---
        copy_board(possible_position.board, position.board);
        position.can_castle_white[0] = possible_position.can_castle_white[0];
        position.can_castle_white[1] = possible_position.can_castle_white[1];
        position.can_castle_black[0] = possible_position.can_castle_black[0];
        position.can_castle_black[1] = possible_position.can_castle_black[1];

        for (int i = 0; i < 8; i++)
        {
            position.pawn_two_squares_white[i] = possible_position.pawn_two_squares_white[i];
            position.pawn_two_squares_black[i] = possible_position.pawn_two_squares_black[i];
        }

        // --- If en passant capture occurred, mark it ---
        if (en_passant_index != -1)
        {
            position.pawn_two_squares_white[en_passant_index] = true;
        }

        return true;
    }

    // --- Move was illegal or left king in check ---
    return false;
}

// --- Returns true if the selected white pawn can legally move to the target square.
bool Board::pawn_move(int start_i, int start_j, int target_i, int target_j)
{
    char starting_row = WHITE_PAWN_STARTING_ROWN;

    // --- One square forward move ---
    if (target_i == start_i - 1 && start_j == target_j)
    {
        // --- Check if the square is empty ---
        if (!square_occupied(target_i, target_j))
        {
            // --- Check for promotion condition ---
            if (target_i == 0)
            {
                promote_pawn = true;
            }
            return true;
        }
    }

    // --- Two squares forward from starting row ---
    if (start_i == starting_row && start_i - 2 == target_i && start_j == target_j)
    {
        // --- Ensure both intermediate and target squares are empty ---
        if (!square_occupied(target_i + 1, target_j) && !square_occupied(target_i, target_j))
        {
            // --- Mark en passant index for potential capture next turn ---
            en_passant_index = target_j;
            return true;
        }
    }

    // --- En passant capture to the left or right ---
    if (start_i == 3 && target_i == 2 && (target_j == start_j - 1 || target_j == start_j + 1) &&
        position.pawn_two_squares_black[target_j])
    {
        return true;
    }

    // --- Normal diagonal captures (both left and right) ---
    int direction = 1;
    for (int n = 0; n < 2; n++)
    {
        if (n == 1)
        {
            direction = -1;
        }

        // --- Check if target square is diagonally in front and contains a black piece ---
        if (start_i - 1 == target_i && target_j + direction == start_j && square_occupied_by_black(target_i, target_j))
        {
            // --- Check for promotion on capture ---
            if (target_i == 0)
            {
                promote_pawn = true;
            }
            return true;
        }
    }

    // --- If no valid move condition matched, return false ---
    return false;
}

// --- Returns true if the pawn at (start_i, start_j) controls the target square (target_i, target_j).
bool Board::under_pawn_control(const char board[8][8], int start_i, int start_j, int target_i, int target_j)
{
    int dir = 1;

    // --- Set direction based on pawn color ---
    if (board[start_i][start_j] < 0) // Black pawn
    {
        dir = -1;
    }

    // --- Diagonal left capture square under control ---
    if (target_i + dir == start_i && start_j == target_j + 1)
    {
        return true;
    }

    // --- Diagonal right capture square under control ---
    if (target_i + dir == start_i && start_j == target_j - 1)
    {
        return true;
    }

    // --- Otherwise, square not under control ---
    return false;
}

// --- Returns true if the knight at (start_i, start_j) can jump to (target_i, target_j).
bool Board::under_knight_control(int start_i, int start_j, int target_i, int target_j)
{
    for (int m = 0; m < 8; m++)
    {
        // --- Check if target matches any valid knight move offset ---
        if (target_i == start_i + knight_move[m][0] && target_j == start_j + knight_move[m][1])
        {
            return true;
        }
    }

    // --- No valid knight move matches the target square ---
    return false;
}

// --- Returns true if the bishop at (start_i, start_j) can move to (target_i, target_j).
bool Board::under_bishop_control(const char board[8][8], int start_i, int start_j, int target_i, int target_j)
{
    for (int f = 0; f < 4; f++)
    {
        int path_i = start_i + bishop_direction[f][0];
        int path_j = start_j + bishop_direction[f][1];

        while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
        {
            // --- Target square is along the bishop's path ---
            if (path_i == target_i && path_j == target_j)
            {
                return true;
            }

            // --- Blocked by a piece ---
            if (board[path_i][path_j] != 0)
            {
                break;
            }

            // --- Continue in the same diagonal direction ---
            path_i += bishop_direction[f][0];
            path_j += bishop_direction[f][1];
        }
    }

    // --- Target square not reachable by bishop ---
    return false;
}

// --- Returns true if the rook at (start_i, start_j) can move to (target_i, target_j).
bool Board::under_rook_control(const char board[8][8], int start_i, int start_j, int target_i, int target_j)
{
    for (int f = 0; f < 4; f++)
    {
        int path_i = start_i + rook_direction[f][0];
        int path_j = start_j + rook_direction[f][1];

        while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
        {
            // --- Target square is along the rook's path ---
            if (path_i == target_i && path_j == target_j)
            {
                return true;
            }

            // --- Blocked by a piece ---
            if (board[path_i][path_j] != 0)
            {
                break;
            }

            // --- Continue in the same straight direction ---
            path_i += rook_direction[f][0];
            path_j += rook_direction[f][1];
        }
    }

    // --- Target square not reachable by rook ---
    return false;
}

// --- Returns true if the queen at (start_i, start_j) can move to (target_i, target_j).
bool Board::under_queen_control(const char board[8][8], int start_i, int start_j, int target_i, int target_j)
{
    for (int f = 0; f < 8; f++)
    {
        int path_i = start_i + every_direction[f][0];
        int path_j = start_j + every_direction[f][1];

        while (path_i >= 0 && path_i < 8 && path_j >= 0 && path_j < 8)
        {
            // --- Target square is along the queen's path ---
            if (path_i == target_i && path_j == target_j)
            {
                return true;
            }

            // --- Blocked by a piece ---
            if (board[path_i][path_j] != 0)
            {
                break;
            }

            // --- Continue in the same direction ---
            path_i += every_direction[f][0];
            path_j += every_direction[f][1];
        }
    }

    // --- Target square not reachable by queen ---
    return false;
}

// --- Returns true if the king at (start_i, start_j) can move to (target_i, target_j).
bool Board::under_king_control(int start_i, int start_j, int target_i, int target_j)
{
    for (int f = 0; f < 8; f++)
    {
        if (target_i == start_i + every_direction[f][0] &&
            target_j == start_j + every_direction[f][1])
        {
            return true;
        }
    }

    // --- Target not within king's 1-square reach ---
    return false;
}

// --- Returns true if the square (i, j) is under control by any piece of side 's'.
bool Board::under_control(const char board[8][8], int i, int j, side s)
{
    // --- Set piece identifiers based on the side ---
    char pawn = WHITE_PAWN;
    char knight = WHITE_KNIGHT;
    char bishop = WHITE_BISHOP;
    char rook = WHITE_ROOK;
    char queen = WHITE_QUEEN;
    char king = WHITE_KING;

    if (s == BLACK)
    {
        pawn = BLACK_PAWN;
        knight = BLACK_KNIGHT;
        bishop = BLACK_BISHOP;
        rook = BLACK_ROOK;
        queen = BLACK_QUEEN;
        king = BLACK_KING;
    }

    // --- Check if any piece of the given side controls (i, j) ---
    for (int m = 0; m < 8; m++)
    {
        for (int n = 0; n < 8; n++)
        {
            if (board[m][n] == pawn && under_pawn_control(board, m, n, i, j))
            {
                return true;
            }
            if (board[m][n] == knight && under_knight_control(m, n, i, j))
            {
                return true;
            }
            if (board[m][n] == bishop && under_bishop_control(board, m, n, i, j))
            {
                return true;
            }
            if (board[m][n] == rook && under_rook_control(board, m, n, i, j))
            {
                return true;
            }
            if (board[m][n] == queen && under_queen_control(board, m, n, i, j))
            {
                return true;
            }
            if (board[m][n] == king && under_king_control(m, n, i, j))
            {
                return true;
            }
        }
    }

    // --- No piece of side 's' controls the square ---
    return false;
}

// --- Returns true if side s's king is in check on a given board state ---
bool Board::king_is_in_check(char board[8][8], side s)
{
    char king = WHITE_KING;
    side opponent = BLACK;

    if (s == BLACK)
    {
        king = BLACK_KING;
        opponent = WHITE;
    }

    // --- Scan the board for the king and check if it is under attack ---
    for (int m = 0; m < 8; m++)
    {
        for (int n = 0; n < 8; n++)
        {
            if (board[m][n] == king)
            {
                if (under_control(board, m, n, opponent))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

// --- Overloaded version: Checks check status for side s using current board state ---
bool Board::king_is_in_check(side s)
{
    return king_is_in_check(position.board, s);
}

// --- Returns true if the side s is in checkmate ---
// --- A checkmate occurs when the king is in check and no legal moves can prevent it ---
bool Board::is_checkmate(board_state p, side s)
{
    char possible_board[8][8];

    // --- Set pawn direction and attributes depending on side ---
    int dir = -1;
    int startingRank = 6;
    int enPassantRank = 3;
    bool *en_passant = p.pawn_two_squares_black;

    char pawn = WHITE_PAWN;
    char knight = WHITE_KNIGHT;
    char bishop = WHITE_BISHOP;
    char rook = WHITE_ROOK;
    char queen = WHITE_QUEEN;
    char king = WHITE_KING;

    if (s == BLACK)
    {
        dir = 1;
        startingRank = 1;
        enPassantRank = 4;
        en_passant = p.pawn_two_squares_white;

        pawn = BLACK_PAWN;
        knight = BLACK_KNIGHT;
        bishop = BLACK_BISHOP;
        rook = BLACK_ROOK;
        queen = BLACK_QUEEN;
        king = BLACK_KING;
    }

    // --- Try every possible legal move for each piece ---
    for (int m = 0; m < 8; m++)
    {
        for (int n = 0; n < 8; n++)
        {
            // --- Attempt all king moves ---
            if (p.board[m][n] == king)
            {
                for (int f = 0; f < 8; f++)
                {
                    int k_i = m + every_direction[f][0];
                    int k_j = n + every_direction[f][1];

                    if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8)
                    {
                        if (square_occupied_by_opponent(p.board, k_i, k_j, s) || p.board[k_i][k_j] == 0)
                        {
                            copy_board(p.board, possible_board);
                            possible_board[m][n] = 0;
                            possible_board[k_i][k_j] = king;

                            // --- Check if king is safe after move ---
                            if (!king_is_in_check(possible_board, s))
                            {
                                return false;
                            }
                        }
                    }
                }
            }

            // --- Attempt all pawn moves including capture and en passant ---
            if (p.board[m][n] == pawn)
            {
                // one step forward
                if (p.board[m + dir][n] == 0)
                {
                    copy_board(p.board, possible_board);
                    possible_board[m][n] = 0;
                    possible_board[m + dir][n] = pawn;

                    if (!king_is_in_check(possible_board, s))
                        return false;
                }
                // two steps from starting position
                if (startingRank == m && p.board[m + dir][n] == 0 && p.board[m + 2 * dir][n] == 0)
                {
                    copy_board(p.board, possible_board);
                    possible_board[m][n] = 0;
                    possible_board[m + 2 * dir][n] = pawn;

                    if (!king_is_in_check(possible_board, s))
                        return false;
                }
                // capture left
                if (n != 0 && square_occupied_by_opponent(p.board, m + dir, n - 1, s))
                {
                    copy_board(p.board, possible_board);
                    possible_board[m][n] = 0;
                    possible_board[m + dir][n - 1] = pawn;

                    if (!king_is_in_check(possible_board, s))
                        return false;
                }
                // capture right
                if (n != 7 && square_occupied_by_opponent(p.board, m + dir, n + 1, s))
                {
                    copy_board(p.board, possible_board);
                    possible_board[m][n] = 0;
                    possible_board[m + dir][n + 1] = pawn;

                    if (!king_is_in_check(possible_board, s))
                        return false;
                }
                // en passant left
                if (n != 0 && m == enPassantRank && en_passant[n - 1])
                {
                    copy_board(p.board, possible_board);
                    possible_board[m][n] = 0;
                    possible_board[m][n - 1] = 0;
                    possible_board[m + dir][n - 1] = pawn;

                    if (!king_is_in_check(possible_board, s))
                        return false;
                }
                // en passant right
                if (n != 7 && m == enPassantRank && en_passant[n + 1])
                {
                    copy_board(p.board, possible_board);
                    possible_board[m][n] = 0;
                    possible_board[m][n + 1] = 0;
                    possible_board[m + dir][n + 1] = pawn;

                    if (!king_is_in_check(possible_board, s))
                        return false;
                }
            }

            // --- Attempt all knight moves ---
            if (p.board[m][n] == knight)
            {
                for (int f = 0; f < 8; f++)
                {
                    int n_i = m + knight_move[f][0];
                    int n_j = n + knight_move[f][1];

                    if (n_i >= 0 && n_i < 8 && n_j >= 0 && n_j < 8)
                    {
                        if (square_occupied_by_opponent(p.board, n_i, n_j, s) || p.board[n_i][n_j] == 0)
                        {
                            copy_board(p.board, possible_board);
                            possible_board[m][n] = 0;
                            possible_board[n_i][n_j] = knight;

                            if (!king_is_in_check(possible_board, s))
                                return false;
                        }
                    }
                }
            }

            // --- Attempt all rook moves ---
            if (p.board[m][n] == rook)
            {
                for (int f = 0; f < 4; f++)
                {
                    int r_i = m + rook_direction[f][0];
                    int r_j = n + rook_direction[f][1];

                    while (r_i >= 0 && r_i < 8 && r_j >= 0 && r_j < 8)
                    {
                        if (square_occupied_by_opponent(p.board, r_i, r_j, s) || p.board[r_i][r_j] == 0)
                        {
                            copy_board(p.board, possible_board);
                            possible_board[m][n] = 0;
                            possible_board[r_i][r_j] = rook;

                            if (!king_is_in_check(possible_board, s))
                                return false;
                        }

                        if (p.board[r_i][r_j] != 0)
                            break;

                        r_i += rook_direction[f][0];
                        r_j += rook_direction[f][1];
                    }
                }
            }

            // --- Attempt all bishop moves ---
            if (p.board[m][n] == bishop)
            {
                for (int f = 0; f < 4; f++)
                {
                    int b_i = m + bishop_direction[f][0];
                    int b_j = n + bishop_direction[f][1];

                    while (b_i >= 0 && b_i < 8 && b_j >= 0 && b_j < 8)
                    {
                        if (square_occupied_by_opponent(p.board, b_i, b_j, s) || p.board[b_i][b_j] == 0)
                        {
                            copy_board(p.board, possible_board);
                            possible_board[m][n] = 0;
                            possible_board[b_i][b_j] = bishop;

                            if (!king_is_in_check(possible_board, s))
                                return false;
                        }

                        if (p.board[b_i][b_j] != 0)
                            break;

                        b_i += bishop_direction[f][0];
                        b_j += bishop_direction[f][1];
                    }
                }
            }

            // --- Attempt all queen moves (combines rook + bishop logic) ---
            if (p.board[m][n] == queen)
            {
                for (int f = 0; f < 8; f++)
                {
                    int q_i = m + every_direction[f][0];
                    int q_j = n + every_direction[f][1];

                    while (q_i >= 0 && q_i < 8 && q_j >= 0 && q_j < 8)
                    {
                        if (square_occupied_by_opponent(p.board, q_i, q_j, s) || p.board[q_i][q_j] == 0)
                        {
                            copy_board(p.board, possible_board);
                            possible_board[m][n] = 0;
                            possible_board[q_i][q_j] = queen;

                            if (!king_is_in_check(possible_board, s))
                                return false;
                        }

                        if (p.board[q_i][q_j] != 0)
                            break;

                        q_i += every_direction[f][0];
                        q_j += every_direction[f][1];
                    }
                }
            }
        }
    }

    // --- No legal move avoids check: checkmate confirmed ---
    return true;
}

// --- Overloaded version: Checks checkmate status using current board position ---
bool Board::is_checkmate(side s)
{
    return is_checkmate(position, s);
}

// --- Returns true if the given side 's' is stalemated in the current board position ---
bool Board::is_stalemate(board_state &p, side s)
{
    // --- Return false immediately if side is in check (then it's checkmate, not stalemate) ---
    if (king_is_in_check(p.board, s))
    {
        return false;
    }

    // --- Set initial pawn movement direction and details based on side ---
    int dir = -1; // For WHITE
    int enPassantRank = 3;
    bool *en_passant = p.pawn_two_squares_black;
    side opponent = BLACK;

    // --- Assign pieces for WHITE by default ---
    char pawn = WHITE_PAWN;
    char knight = WHITE_KNIGHT;
    char bishop = WHITE_BISHOP;
    char rook = WHITE_ROOK;
    char queen = WHITE_QUEEN;
    char king = WHITE_KING;

    // --- If checking for BLACK, update all values accordingly ---
    if (s == BLACK)
    {
        dir = 1;
        enPassantRank = 4;
        en_passant = p.pawn_two_squares_white;
        opponent = WHITE;

        pawn = BLACK_PAWN;
        knight = BLACK_KNIGHT;
        bishop = BLACK_BISHOP;
        rook = BLACK_ROOK;
        queen = BLACK_QUEEN;
        king = BLACK_KING;
    }

    // --- Traverse the board to find all pieces of side 's' ---
    for (int m = 0; m < 8; m++)
    {
        for (int n = 0; n < 8; n++)
        {
            // --- King Moves ---
            if (p.board[m][n] == king)
            {
                for (int f = 0; f < 8; f++)
                {
                    int k_i = m + every_direction[f][0];
                    int k_j = n + every_direction[f][1];
                    // --- If square is valid and not attacked and can be moved to ---
                    if (k_i >= 0 && k_i < 8 && k_j >= 0 && k_j < 8)
                    {
                        if ((square_occupied_by_opponent(p.board, k_i, k_j, s) || p.board[k_i][k_j] == 0) &&
                            !under_control(p.board, k_i, k_j, opponent))
                        {
                            return false;
                        }
                    }
                }
            }

            // --- Pawn Moves ---
            if (p.board[m][n] == pawn)
            {
                // --- Forward move ---
                if (p.board[m + dir][n] == 0)
                {
                    return false;
                }
                // --- Capture diagonally to the left ---
                if (n != 0 && square_occupied_by_opponent(p.board, m + dir, n - 1, s))
                {
                    return false;
                }
                // --- Capture diagonally to the right ---
                if (n != 7 && square_occupied_by_opponent(p.board, m + dir, n + 1, s))
                {
                    return false;
                }
                // --- En passant left ---
                if (n != 0 && m == enPassantRank && en_passant[n - 1])
                {
                    return false;
                }
                // --- En passant right ---
                if (n != 7 && m == enPassantRank && en_passant[n + 1])
                {
                    return false;
                }
            }

            // --- Knight Moves ---
            if (p.board[m][n] == knight)
            {
                for (int f = 0; f < 8; f++)
                {
                    int n_i = m + knight_move[f][0];
                    int n_j = n + knight_move[f][1];
                    if (n_i >= 0 && n_i < 8 && n_j >= 0 && n_j < 8)
                    {
                        if (square_occupied_by_opponent(p.board, n_i, n_j, s) || p.board[n_i][n_j] == 0)
                        {
                            return false;
                        }
                    }
                }
            }

            // --- Rook Moves ---
            if (p.board[m][n] == rook)
            {
                for (int f = 0; f < 4; f++)
                {
                    int r_i = m + rook_direction[f][0];
                    int r_j = n + rook_direction[f][1];
                    while (r_i >= 0 && r_i < 8 && r_j >= 0 && r_j < 8)
                    {
                        if (square_occupied_by_opponent(p.board, r_i, r_j, s) || p.board[r_i][r_j] == 0)
                        {
                            return false;
                        }
                        if (p.board[r_i][r_j] != 0)
                        {
                            break;
                        }
                        r_i += rook_direction[f][0];
                        r_j += rook_direction[f][1];
                    }
                }
            }

            // --- Bishop Moves ---
            if (p.board[m][n] == bishop)
            {
                for (int f = 0; f < 4; f++)
                {
                    int b_i = m + bishop_direction[f][0];
                    int b_j = n + bishop_direction[f][1];
                    while (b_i >= 0 && b_i < 8 && b_j >= 0 && b_j < 8)
                    {
                        if (square_occupied_by_opponent(p.board, b_i, b_j, s) || p.board[b_i][b_j] == 0)
                        {
                            return false;
                        }
                        if (p.board[b_i][b_j] != 0)
                        {
                            break;
                        }
                        b_i += bishop_direction[f][0];
                        b_j += bishop_direction[f][1];
                    }
                }
            }

            // --- Queen Moves (combination of rook and bishop directions) ---
            if (p.board[m][n] == queen)
            {
                for (int f = 0; f < 8; f++)
                {
                    int q_i = m + every_direction[f][0];
                    int q_j = n + every_direction[f][1];
                    while (q_i >= 0 && q_i < 8 && q_j >= 0 && q_j < 8)
                    {
                        if (square_occupied_by_opponent(p.board, q_i, q_j, s) || p.board[q_i][q_j] == 0)
                        {
                            return false;
                        }
                        if (p.board[q_i][q_j] != 0)
                        {
                            break;
                        }
                        q_i += every_direction[f][0];
                        q_j += every_direction[f][1];
                    }
                }
            }
        }
    }

    // --- No legal moves found and side is not in check -> It's stalemate ---
    return true;
}

// --- Returns the piece located at the specified (i, j) position on the board ---
char Board::get_piece(int i, int j)
{
    return position.board[i][j];
}

// --- Moves a piece from the start square (start_i, start_j) to destination (destination_i, destination_j)
void Board::move_piece(board_state &pos, int start_i, int start_j, int destination_i, int destination_j)
{
    // --- Copy piece to destination square ---
    pos.board[destination_i][destination_j] = pos.board[start_i][start_j];

    // --- Clear the original square ---
    pos.board[start_i][start_j] = 0;
}

// --- Copies all pieces from 'source' 8x8 board array to 'destination' 8x8 board array ---
void Board::copy_board(const char source[8][8], char destination[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            destination[i][j] = source[i][j];
        }
    }
}

// --- Clears en passant flags for side 's' in the provided board state 'pos'
void Board::reset_en_passant(board_state &pos, side s)
{
    if (s == WHITE)
    {
        for (int n = 0; n < 8; n++)
        {
            pos.pawn_two_squares_white[n] = false; // --- Reset en passant possibility for each file ---
        }
    }
    else
    {
        for (int n = 0; n < 8; n++)
        {
            pos.pawn_two_squares_black[n] = false; // --- Reset en passant possibility for each file ---
        }
    }
}

// --- Clears en passant flags for side 's' in the default position object
void Board::reset_en_passant(side s)
{
    if (s == WHITE)
    {
        for (int n = 0; n < 8; n++)
        {
            position.pawn_two_squares_white[n] = 0; // --- Clear en passant flags (as 0 == false) ---
        }
    }
    else
    {
        for (int n = 0; n < 8; n++)
        {
            position.pawn_two_squares_black[n] = 0; // --- Clear en passant flags (as 0 == false) ---
        }
    }
}

// --- Returns a reference to the current board position.
board_state &Board::get_position()
{
    return position;
}

// --- Returns a copy of the board_state passed as a parameter.
board_state Board::copy_position(board_state position)
{
    return position;
}

// --- Generates a move string (e.g., Nf3, exd5, O-O) from given move coordinates.
std::string Board::generate_move_notation(int from_i, int from_j, int to_i, int to_j, side player)
{
    std::stringstream notation;
    char piece_char = '\0';
    char piece = position.board[from_i][from_j];
    piece = abs(piece);
    char target = position.board[to_i][to_j];
    bool is_capture = false;

    if (target != 0)
    {
        is_capture = true; // --- If target square is occupied, it's a capture ---
    }

    // --- Handle castling cases ---
    if (piece == 6)
    {
        if (from_j == 4 && to_j == 6)
            return "O-O"; // --- Kingside castling ---
        else if (from_j == 4 && to_j == 2)
            return "O-O-O"; // --- Queenside castling ---
    }

    // --- Get piece notation character ---
    switch (abs(piece))
    {
    case 1:
        piece_char = '\0'; // pawn
        break;
    case 2:
        piece_char = 'N';
        break;
    case 3:
        piece_char = 'B';
        break;
    case 4:
        piece_char = 'R';
        break;
    case 5:
        piece_char = 'Q';
        break;
    case 6:
        piece_char = 'K';
        break;
    }

    // --- Append piece symbol (if not a pawn) ---
    if (piece_char)
        notation << piece_char;

    // --- For pawn captures, include file letter of origin square ---
    if (piece == 1 && is_capture)
        notation << static_cast<char>('a' + from_j);

    // --- Add 'x' if it's a capture ---
    if (is_capture)
        notation << 'x';

    // --- Add target square (e.g., e4, d5) ---
    notation << static_cast<char>('a' + to_j) << (8 - to_i);

    // --- Simulate the move on a copy of the position ---
    board_state temp = Board::copy_position(position);
    temp.board[to_i][to_j] = temp.board[from_i][from_j];
    temp.board[from_i][from_j] = 0;

    side opponent = (player == WHITE ? BLACK : WHITE);

    bool check = king_is_in_check(temp.board, opponent);
    bool mate = check && is_checkmate(temp, opponent);

    // --- Append check or mate symbol if applicable ---
    if (mate)
        notation << '#';
    else if (check)
        notation << '+';

    return notation.str();
}

// --- Loads a given board_state into the current position.
void Board::load_position(const board_state &state)
{
    // --- Copy each square and en passant arrays ---
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            this->position.board[i][j] = state.board[i][j];
        }
        this->position.pawn_two_squares_white[i] = state.pawn_two_squares_white[i];
        this->position.pawn_two_squares_black[i] = state.pawn_two_squares_black[i];
    }

    // --- Copy castling rights ---
    this->position.can_castle_white[0] = state.can_castle_white[0];
    this->position.can_castle_white[1] = state.can_castle_white[1];
    this->position.can_castle_black[0] = state.can_castle_black[0];
    this->position.can_castle_black[1] = state.can_castle_black[1];
}