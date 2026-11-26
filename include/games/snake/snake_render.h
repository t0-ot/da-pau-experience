
/**
 * @file snake_render.h
 * @brief Raylib rendering interface for Snake.
 */

#ifndef SNAKE_RENDER_H
#define SNAKE_RENDER_H

#include "engine/entity.h"
#include "engine/core/engine_context.h"
#include "games/snake/snake_internal.h"
#include "games/snake/snake_logic.h"

/**
 * @brief Draws the Snake game scene.
 *
 * @param snake Current snake entity.
 * @param fruit Current fruit positions.
 * @param score Current score.
 * @param high_score Highest score achieved.
 * @param play_width Width of the playable area.
 * @param play_height Height of the playable area.
 */
void snake_render_game(snake_runtime *snake_data, const EngineContext *engine);

#endif // SNAKE_RENDER_H
