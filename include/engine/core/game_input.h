/**
 * @file game_input.h
 * @brief Defines the shared game input structure.
 *
 * Provides an input representation that allows individual game
 * modules to process user input independently of the input backend.
 */

#ifndef GAME_INPUT_H
#define GAME_INPUT_H

#include <stdbool.h>

/**
 * @brief Stores the input state received during a game frame.
 *
 * Provides a platform-independent input snapshot so that game
 * modules do not need to directly access Raylib's keyboard API.
 */
typedef struct {
    bool up_held;      /**< Whether the up direction is being held. */
    bool down_held;    /**< Whether the down direction is being held. */
    bool left_held;    /**< Whether the left direction is being held. */
    bool right_held;   /**< Whether the right direction is being held. */

    bool up_pressed;
    bool down_pressed;
    bool left_pressed;
    bool right_pressed;

    bool confirm; /**< Whether the confirm key was pressed. */
    bool back;    /**< Whether the back key was pressed. */
    bool quit;    /**< Whether the quit key was pressed. */
    bool action;  /**< Whether the action key was pressed. */
} GameInput;

#endif // GAME_INPUT_H
