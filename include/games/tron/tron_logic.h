/**
 * @file tron_logic.h
 * @brief Gameplay logic and enemy AI for Tron.
 */

#ifndef TRON_LOGIC_H
#define TRON_LOGIC_H

#include "engine/entity.h"

typedef struct tron_runtime tron_runtime;
typedef struct Enemy_list Enemy_list;

/**
 * @brief Updates enemy lightcycle AI.
 *
 * Evaluates available movement directions and selects a move
 * based on accessible territory and distance from the player.
 *
 * @param view_enemies Collection of enemy lightcycles to update.
 * @param player Player's current trail and position.
 * @param distraction_modifier Chance of an enemy becoming distracted (0–100).
 * @param play_width Width of the playable area in grid cells.
 * @param play_height Height of the playable area in grid cells.
 */
void enemy_lightcycle_ai(
    Enemy_list *view_enemies,
    Point_list *player,
    int distraction_modifier,
    int play_width,
    int play_height
);

/**
 * @brief Resets the player and enemy state after player death.
 * @param runtime Pointer to the Tron runtime state.
 * @param play_width Width of the playable area in grid cells.
 */
void handle_player_death(tron_runtime *runtime, int play_width);

/**
 * @brief Updates the player's trail and movement.
 * @param runtime Pointer to the Tron runtime state.
 * @param dx Horizontal movement direction.
 * @param dy Vertical movement direction.
 * @param play_width Width of the playable area in grid cells.
 * @param play_height Height of the playable area in grid cells.
 */
void update_tron_player(
    tron_runtime *runtime,
    int dx,
    int dy,
    int play_width,
    int play_height
);

/**
 * @brief Advances the game to the next level.
 * @param runtime Pointer to the Tron runtime state.
 * @param play_width Width of the playable area in grid cells.
 */
void advance_tron_level(tron_runtime *runtime, int play_width);

/**
 * @brief Checks whether all active enemies have been defeated.
 * @param runtime Pointer to the Tron runtime state.
 * @return 1 if all enemies are defeated; otherwise, 0.
 */
int all_enemies_dead(tron_runtime *runtime);

/**
 * @brief Retrieves the display name for a level.
 * @param level Level number, starting at 1.
 * @return Name of the level, or "UNKNOWN" if the level is invalid.
 */
const char *get_level_name(int level);

#endif // TRON_LOGIC_H
