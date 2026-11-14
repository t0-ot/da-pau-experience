
/**
 * @file entity.h
 * @brief Gameplay entity structures and movement operations.
 *
 * Provides reusable operations for segmented entities without
 * depending on a particular rendering system.
 */

#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include <stdbool.h>
#include "engine/core/game_types.h"

/**
 * @brief Represents an entity composed of multiple connected segments.
 */
typedef struct {
    Point_list segments; /**< Positions and count of the entity's segments. */
    int head;            /**< Index of the newest segment (head) */
    int tail;            /**< Index of the oldest segment (tail) */
    int capacity;        /**< Total allocated slots in segments.points (e.g. width * height) */
    bool alive;          /**< Whether the entity is currently alive */
} Segmented_entity;

/**
 * @brief Shifts each segment into the previous segment's position.
 * @param entity Entity whose segments will be shifted.
 */
void advance_entity_tail(Segmented_entity *entity);

void grow_entity(Segmented_entity *entity);

/**
 * @brief Moves an entity's head in its current direction.
 * @param entity Array of entity points.
 * @param last_dir Current movement direction.
 * @param play_width Width of the playable area.
 * @param play_height Height of the playable area.
 * @return 1 if movement succeeds, otherwise 0.
 */
int move_entity_segments(Segmented_entity *entity, Point *last_dir, int play_width, int play_height);

/**
 * @brief Checks whether an entity is still alive.
 * @param entity Entity segment list.
 * @param last_dir Current movement direction.
 * @param play_width Width of the playable area.
 * @param play_height Height of the playable area.
 * @return 1 if alive, otherwise 0.
 */
int check_seg_entity_life(Segmented_entity *entity, Point *last_dir, int play_width, int play_height);

/**
 * @brief Resets an entity to a starting position and length.
 * @param entity Entity to reset.
 * @param seg_len Number of segments.
 * @param x Starting horizontal coordinate.
 * @param y Starting vertical coordinate.
 */
void reset_entity(Segmented_entity *entity, int seg_len, int x, int y);

int segmented_self_collision_check(Segmented_entity *entity);

int segmented_collision_with_pointlist(Segmented_entity *entity, Point_list *objView2, int checkIndexObj2);

#endif // ENTITY_H_INCLUDED
