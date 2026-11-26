/**
 * @file game_types.h
 * @brief Defines shared data types used throughout the game modules.
 *
 * Contains common structures and type definitions used to represent
 * game entities and other shared gameplay data.
 */

#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Represents a two-dimensional position on the game grid.
 */
typedef struct {
    int posX; /**< Horizontal coordinate. */
    int posY; /**< Vertical coordinate. */
} Point;

/**
 * @brief Stores a dynamically allocated collection of points.
 */
typedef struct {
    Point *points; /**< Array containing the stored points. */
    int16_t amt;   /**< Number of points currently stored. */
} Point_list;

#endif // GAME_TYPES_H
