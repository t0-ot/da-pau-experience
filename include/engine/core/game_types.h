#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Represents a two-dimensional position on the game grid.
 */
typedef struct {
    int8_t posX; /**< Horizontal coordinate. */
    int8_t posY; /**< Vertical coordinate. */
} Point;

/**
 * @brief Stores a dynamically allocated collection of points.
 */
typedef struct {
    Point *points; /**< Array containing the stored points. */
    int16_t amt;   /**< Number of points currently stored. */
} Point_list;

#endif // GAME_TYPES_H
