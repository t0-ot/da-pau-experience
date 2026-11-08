/**
 * @file snake.h
 * @brief Public interface for the Snake game module.
 */

#ifndef SNAKE_H
#define SNAKE_H

#include "engine/core/game_module.h"

/**
 * @brief Registered Snake game module.
 *
 * The game session owns the runtime memory.
 */
extern const GameModule snake_module;

#endif // SNAKE_H
