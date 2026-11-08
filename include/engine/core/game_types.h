#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int8_t posX;
    int8_t posY;
} Point;

typedef struct {
    Point *points;
    int16_t amt;
} Point_list;

#endif // GAME_TYPES_H
