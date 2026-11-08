#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include <stdbool.h>
#include "engine/canvas.h"

typedef struct {
    Point_list segments;
    Canvas entity_visual[2];
    bool alive;
} Segmented_entity;

void shift_entity_segments(Segmented_entity *entity);
void entity_stamper(void *data, Canvas_buffer *canvas);
int movement_input_handler(Point prevDirInput[], char input);
void reset_entity(Segmented_entity *entity, int seg_len, int x, int y);
int move_entity_segments(Point *entity, Point *last_dir, int play_width, int play_height);
int check_seg_entity_life(Point_list *entity, Point *last_dir, int play_width, int play_height);

#endif // ENTITY_H_INCLUDED
