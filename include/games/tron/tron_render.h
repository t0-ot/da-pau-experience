/**
 * @file tron_render.h
 * @brief Raylib rendering interface for Tron.
 */

#ifndef TRON_RENDER_H
#define TRON_RENDER_H

#include "engine/entity.h"

/**
 * @brief Draws the Tron game scene.
 *
 * Renders the player, enemy lightcycles, and current level information.
 *
 * @param player Player's lightcycle and trail.
 * @param enemies Array of enemy lightcycles.
 * @param enemy_count Number of enemies to render.
 * @param level Current level number.
 * @param level_starting Whether the level introduction is active.
 * @param turbo Whether the player is using turbo movement.
 */
void tron_render_game(
    const Segmented_entity *player,
    const Segmented_entity enemies[],
    int enemy_count,
    int level,
    int level_starting,
    int turbo
);

#endif // TRON_RENDER_H
