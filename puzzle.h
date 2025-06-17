// --------------------------------------------------------------------------------------
// puzzle.h
// --- Declares the Puzzle structure and related functions for handling puzzle mode.
// --- Supports loading puzzles from FEN, applying moves, and tracking user inputs.
// --------------------------------------------------------------------------------------

#pragma once

#include <vector>
#include <string>
#include "board.h"  // --- Required for board_state struct ---

// --- Represents a single chess puzzle ---
struct Puzzle
{
    std::string id;                      // --- Unique ID of the puzzle ---
    std::string fen;                     // --- FEN string representing puzzle position ---
    board_state puzzle_board_state;      // --- Internal board representation of the FEN ---

    std::string rating;                  // --- Difficulty rating (from CSV source) ---
    std::string themes;                  // --- Themes like "Easy", "Hard", "Opening", etc. ---

    std::vector<std::string> bestMoves;  // --- List of correct move(s) to solve the puzzle ---
    std::vector<std::string> playerMoves; // --- Moves played by the user so far ---
};

// --- Global variable to hold the currently active puzzle ---
extern Puzzle currentPuzzle;

// --- Puzzle Lifecycle Functions ---
// --- Loads a puzzle based on difficulty level (e.g., 0 = Easy, 1 = Medium, 2 = Hard) ---
bool load_puzzle_by_difficulty(int difficulty);

// --- Initializes the current puzzle (resets board state, move counters, etc.) ---
void start_puzzle();

// --- Move Handling Functions ---
// --- Executes a move in the current puzzle given algebraic move notation ---
void play_move(const std::string &move_str);

// --- FEN Parsing & Board Setup Functions ---
// --- Converts FEN character to internal piece integer representation ---
int piece_char_to_int(char c);

// --- Parses a FEN string and updates a board_state object accordingly ---
void set_board_from_fen(const std::string& fen, board_state& bs);

// --- Resets the board_state object to an empty/default state ---
void reset_board_state(board_state &bs);
