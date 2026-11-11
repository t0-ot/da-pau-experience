#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdbool.h>
#include "engine/core/game_types.h"
#include "engine/core/game_input.h"

void latch_movement_handler(Point *target, Point *buffer, const GameInput *input, int *buffered_amt);
void latch_movement_no_reversal(Point *target, Point *buffer, const GameInput *input, int *buffered_amt);
bool consume_buffer_input(Point *target, Point *buffer, int *buffer_amt);

#endif // MOVEMENT_H
