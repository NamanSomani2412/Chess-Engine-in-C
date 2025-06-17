// --------------------------------------------------------------------------------------
// puzzle.cpp
// --- Implements puzzle loading, initialization, and FEN parsing for puzzle mode.
// --------------------------------------------------------------------------------------

#include "puzzle.h"
#include "board.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <random>

// --- External board instance shared across modules ---
extern Board board;

// --- Global instance to hold the currently active puzzle ---
Puzzle currentPuzzle;

// --- Loads a puzzle from a CSV file based on given difficulty level.
// --- Returns true if puzzle is successfully loaded; false otherwise.
bool load_puzzle_by_difficulty(int difficulty)
{
    std::string file;

    // --- Select file based on difficulty ---
    switch (difficulty)
    {
    case 1:
        file = "puzzles/puzzles_easy.csv";
        break;
    case 2:
        file = "puzzles/puzzles_medium.csv";
        break;
    case 3:
        file = "puzzles/puzzles_hard.csv";
        break;
    case 4:
        file = "puzzles/endgame_puzzles.csv";
        break;
    default:
        return false;
    }

    // --- Open the file ---
    std::ifstream in(file);
    if (!in.is_open())
    {
        std::cerr << "Failed to open " << file << "\n";
        return false;
    }

    // --- Skip CSV header line ---
    std::string header;
    std::getline(in, header);

    std::vector<std::string> lines;
    std::string line;

    // --- Read all non-empty lines into vector ---
    while (std::getline(in, line))
    {
        if (!line.empty())
            lines.push_back(line);
    }

    if (lines.empty())
        return false;

    // --- Pick a random puzzle line ---
    std::mt19937 rng(static_cast<unsigned int>(time(nullptr)));
    std::uniform_int_distribution<> dist(0, lines.size() - 1);
    std::string chosen = lines[dist(rng)];

    // --- Extract fields from CSV line ---
    std::stringstream ss(chosen);
    std::string id, fen, moves, rating, popularity, themes, difficulty;

    std::getline(ss, id, ',');
    std::getline(ss, fen, ',');
    std::getline(ss, moves, ',');
    std::getline(ss, rating, ',');
    std::getline(ss, popularity, ',');
    std::getline(ss, themes, ',');
    std::getline(ss, difficulty, ',');

    // --- Store puzzle metadata ---
    currentPuzzle.id = id;
    currentPuzzle.fen = fen;
    currentPuzzle.rating = rating;
    currentPuzzle.themes = difficulty;

    // --- Clear move history ---
    currentPuzzle.bestMoves.clear();
    currentPuzzle.playerMoves.clear();

    // --- Generate internal board representation from FEN ---
    set_board_from_fen(fen, currentPuzzle.puzzle_board_state);

    // --- Parse best move sequence into vector ---
    std::stringstream moveStream(moves);
    std::string move;
    while (moveStream >> move)
    {
        currentPuzzle.bestMoves.push_back(move);
    }

    return true;
}

// --- Initializes puzzle rush mode by randomly selecting a difficulty and loading a puzzle ---
void start_puzzle()
{
    // --- Randomly select a difficulty between 1 and 3 ---
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 3);
    int difficulty = dist(gen);

    // --- Load the puzzle ---
    load_puzzle_by_difficulty(difficulty);

    return;
}

// --- Converts a single character from FEN notation into internal piece value.
int piece_char_to_int(char c)
{
    switch (c)
    {
    case 'P':
        return 1;
    case 'N':
        return 2;
    case 'B':
        return 3;
    case 'R':
        return 4;
    case 'Q':
        return 5;
    case 'K':
        return 6;
    case 'p':
        return -1;
    case 'n':
        return -2;
    case 'b':
        return -3;
    case 'r':
        return -4;
    case 'q':
        return -5;
    case 'k':
        return -6;
    default:
        return 0;
    }
}

// --- Parses a full FEN string and updates the given board_state object accordingly.
// --- Handles board setup, turn, castling rights, en passant flags.
void set_board_from_fen(const std::string &fen, board_state &bs)
{
    std::istringstream iss(fen);
    std::string boardPart, turn, castling, enpassant;
    int halfmove, fullmove;

    // --- Split FEN string into parts ---
    iss >> boardPart >> turn >> castling >> enpassant >> halfmove >> fullmove;

    // --- Reset state ---
    std::fill(std::begin(bs.pawn_two_squares_white), std::end(bs.pawn_two_squares_white), false);
    std::fill(std::begin(bs.pawn_two_squares_black), std::end(bs.pawn_two_squares_black), false);
    bs.can_castle_white[0] = bs.can_castle_white[1] = false;
    bs.can_castle_black[0] = bs.can_castle_black[1] = false;

    // --- Parse board layout ---
    int row = 0, col = 0;
    for (char c : boardPart)
    {
        if (c == '/')
        {
            row++;
            col = 0;
        }
        else if (isdigit(c))
        {
            col += c - '0'; // Skip empty squares
        }
        else
        {
            bs.board[row][col++] = piece_char_to_int(c);
        }
    }

    // --- Parse castling rights ---
    for (char c : castling)
    {
        if (c == 'K')
            bs.can_castle_white[1] = true; // White kingside
        if (c == 'Q')
            bs.can_castle_white[0] = true; // White queenside
        if (c == 'k')
            bs.can_castle_black[1] = true; // Black kingside
        if (c == 'q')
            bs.can_castle_black[0] = true; // Black queenside
    }

    // --- Parse en passant square ---
    if (enpassant != "-")
    {
        int file = enpassant[0] - 'a';
        int rank = enpassant[1] - '1';
        if (rank == 2)
            bs.pawn_two_squares_black[file] = true;
        else if (rank == 5)
            bs.pawn_two_squares_white[file] = true;
    }
}

// --- Resets the given board_state to an empty/default state.
// --- Clears board, castling rights, and en passant flags.
void reset_board_state(board_state &bs)
{
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            bs.board[r][c] = 0;

    std::fill(std::begin(bs.pawn_two_squares_white), std::end(bs.pawn_two_squares_white), false);
    std::fill(std::begin(bs.pawn_two_squares_black), std::end(bs.pawn_two_squares_black), false);

    bs.can_castle_white[0] = bs.can_castle_white[1] = false;
    bs.can_castle_black[0] = bs.can_castle_black[1] = false;
}

// --- Executes a move string (UCI format) on the internal board state.
void play_move(const std::string &move_str)
{
    // --- Basic validation ---
    if (move_str.length() < 4)
        return;

    // --- Decode move notation (e.g., "e2e4" or "e7e8q") ---
    int from_file = move_str[0] - 'a';
    int from_rank = '8' - move_str[1];
    int to_file = move_str[2] - 'a';
    int to_rank = '8' - move_str[3];

    board_state &bs = board.get_position();
    int piece = bs.board[from_rank][from_file];

    if (piece == 0)
        return; // --- No piece to move ---

    // --- Handle promotion (5th character exists and piece is a pawn) ---
    if (move_str.length() == 5 && abs(piece) == 1)
    {
        char promo = move_str[4];
        int promoted_piece = 0;

        switch (promo)
        {
        case 'q':
            promoted_piece = (piece > 0) ? 5 : -5;
            break; // Queen
        case 'r':
            promoted_piece = (piece > 0) ? 4 : -4;
            break; // Rook
        case 'b':
            promoted_piece = (piece > 0) ? 3 : -3;
            break; // Bishop
        case 'n':
            promoted_piece = (piece > 0) ? 2 : -2;
            break; // Knight
        default:
            promoted_piece = (piece > 0) ? 5 : -5;
            break; // Default to Queen
        }

        bs.board[to_rank][to_file] = promoted_piece;
    }
    else
    {
        // --- Normal move ---
        bs.board[to_rank][to_file] = piece;
    }

    // --- Clear source square ---
    bs.board[from_rank][from_file] = 0;

    // --- Reset en passant flags for both sides ---
    std::fill(std::begin(bs.pawn_two_squares_white), std::end(bs.pawn_two_squares_white), false);
    std::fill(std::begin(bs.pawn_two_squares_black), std::end(bs.pawn_two_squares_black), false);

    // --- Update board ---
    board.load_position(bs);
}