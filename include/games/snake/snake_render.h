
/**
 * @file snake_render.h
 * @brief Raylib rendering interface for Snake.
 */

#ifndef SNAKE_RENDER_H
#define SNAKE_RENDER_H

#include "engine/entity.h"
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
void snake_render_game(const Segmented_entity *snake, const fruitData *fruit,int score, int high_score, int play_width, int play_height);

#endif // SNAKE_RENDER_H
