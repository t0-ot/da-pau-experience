#include "engine/input/movement.h"
#include "engine/core/game_types.h"

void latch_movement_handler(Point *target, Point *buffer, const GameInput *input, int *buffered_amt) {
    int dx = 0, dy = 0;

    if (input->up)    dy = -1;
    if (input->down)  dy = 1;
    if (input->left)  dx = -1;
    if (input->right) dx = 1;

    if (dx == 0 && dy == 0) return;
    if (dx != 0 && dy != 0) dx = 0;

    Point reference_direction;
    if (*buffered_amt > 0)
        reference_direction = buffer[*buffered_amt-1];
    else
        reference_direction = *target;

    bool is_same_dir = (dx == reference_direction.posX && dy == reference_direction.posY);

    if (is_same_dir)
        return;

    if (*buffered_amt < 5) {
        buffer[*buffered_amt] = (Point){dx, dy};
        (*buffered_amt)++;
    }
}

void latch_movement_no_reversal(Point *target, Point *buffer, const GameInput *input, int *buffered_amt) {
    latch_movement_handler(target, buffer, input, buffered_amt);

    bool is_reversing = false;
    if (*buffered_amt > 1)
        is_reversing = (buffer[*buffered_amt - 1].posX == -buffer[*buffered_amt - 2].posX && buffer[*buffered_amt - 1].posY == -buffer[*buffered_amt - 2].posY);
    else if (*buffered_amt == 1)
        is_reversing = (buffer[0].posX == -(target->posX) && buffer[0].posY == -(target->posY));

    if (is_reversing)
        (*buffered_amt)--;
}

bool consume_buffer_input(Point *target, Point *buffer, int *buffer_amt) {
    if (*buffer_amt <= 0)
        return false;

    *target = *buffer;

    for (int i = 0; i < *buffer_amt-1; i++)
        buffer[i] = buffer[i+1];
    (*buffer_amt)--;
}
