/**
 * @file input.h
 * @brief Declares the application's input management functions.
 *
 * Provides functions for initializing input handling and polling
 * keyboard input during application execution.
 */

#ifndef INPUT_H
#define INPUT_H

#include "game_input.h"

/**
 * @brief Initializes the input management system.
 *
 * Configures the input backend for use by the application.
 */
void input_init(void);

/**
 * @brief Polls the current keyboard input state.
 *
 * Collects keyboard input and returns it as a GameInput structure
 * for use by the active game module.
 *
 * @return A GameInput structure containing the current input state.
 */
GameInput input_poll(void);

#endif // INPUT_H
