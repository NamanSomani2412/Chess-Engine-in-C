// --------------------------------------------------------------------------------------
// config.h
// --- Contains global constants and enums for UI layout, rendering, and game modes.
// --- Used to configure visual settings and game behavior across the project.
// --------------------------------------------------------------------------------------

#ifndef CONFIG_H
#define CONFIG_H

// --- Display Settings ---
constexpr float FPS = 60.0f;                  // --- Frames per second for game loop ---
constexpr float SQUARE_SIZE = 120.0f;         // --- Size of each chessboard square in pixels ---
constexpr int MAX_VISIBLE_MOVES = 22;         // --- Max number of moves shown in history panel ---

// --- UI Layout ---
// --- Coordinates and dimensions for the "Hint" button in learning/puzzle modes ---
constexpr int btn_x = 1020;
constexpr int btn_y = 810;
constexpr int btn_w = 240;
constexpr int btn_h = 40;

// --- Mode Selection enum ---
// --- Defines various game modes available in the UI ---
enum GameMode
{
    VS_HUMAN,         // --- Player vs Player mode ---
    VS_ENGINE,        // --- Player vs Computer mode ---
    PUZZLE_MODE,      // --- Single puzzle-solving mode ---
    PUZZLE_RUSH,      // --- Timed puzzle rush challenge ---
    PUZZLE_ENDGAME,   // --- Endgame scenario training mode ---
    LEARNING_MODE     // --- Guided learning with hints and explanations ---
};

#endif // CONFIG_H
