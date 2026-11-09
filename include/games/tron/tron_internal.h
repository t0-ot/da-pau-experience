/**
 * @file tron_internal.h
 * @brief Private runtime data for the Tron game.
 */

#ifndef TRON_INTERNAL_H
#define TRON_INTERNAL_H

#include "engine/entity.h"
#include <stdbool.h>

/** @brief Width of the Tron play area in grid cells. */
#define TRON_WIDTH 30

/** @brief Height of the Tron play area in grid cells. */
#define TRON_HEIGHT 30

/**
 * @brief Holds references to the Tron enemies.
 */
typedef struct Enemy_list {
    Segmented_entity *enemy; /**< Array of enemy entities. */
    int enemy_amt;           /**< Number of enemies in the array. */
} Enemy_list;

/**
 * @brief Stores the runtime state of a Tron game session.
 */
typedef struct tron_runtime {
    Segmented_entity player; /**< Player's lightcycle and trail. */
    Segmented_entity enemy[3]; /**< Enemy lightcycles and their trails. */

    Enemy_list view_enemies; /**< Enemy collection used by game logic. */

    Point last_dir_input; /**< Player's most recent movement direction. */

    int toggle_turbo; /**< Whether turbo movement is enabled. */
    int level; /**< Current level number. */
    int enemy_distracted; /**< Remaining duration of enemy distraction. */
    float enemy_move_timer; /**< Accumulated time since previous enemy move. */
    float enemy_move_interval; /**< Time required between enemy movements. */

    float move_timer; /**< Accumulated time since the previous movement. */
    float move_interval; /**< Time required between player movements. */
    float intro_timer; /**< Remaining duration of the level introduction. */

    Point buffered_dir; /**< Direction queued for the next player movement. */
    bool has_buffered_dir; /**< Whether a direction is currently queued. */

    int level_starting; /**< State indicating whether a level introduction is active. */
} tron_runtime;

/** @brief Initial horizontal positions of the three enemies. */
static const int enemy_start_x[3] = {
    TRON_WIDTH / 4,
    TRON_WIDTH / 2,
    (TRON_WIDTH * 3) / 4
};

/** @brief Initial vertical positions of the three enemies. */
static const int enemy_start_y[3] = {5, 5, 5};

/** @brief Display names associated with each Tron level. */
static const char *level_names[] = {
    "CMSC-10",
    "CMSC-11",
    "CMSC-56",
    "KAS-1",
    "MATH-18",
    "PE-1",
    "SCI-10"
    "NSTP-1"
};

/** @brief Number of levels defined in the level_names array. */
#define LEVEL_COUNT (sizeof(level_names) / sizeof(level_names[0]))

#endif // TRON_INTERNAL_H
