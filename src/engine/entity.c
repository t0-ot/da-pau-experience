#include "engine/entity.h"
#include "engine/utils.h"

void shift_entity_segments(Segmented_entity *entity) {
    for (int i = entity->segments.amt - 1; i > 0; i--)
        entity->segments.points[i] = entity->segments.points[i-1];
}

int move_entity_segments(Point *entity, Point *last_dir, int play_width, int play_height) {
    if ((entity[0].posX + last_dir[0].posX > 0 && entity[0].posX + last_dir[0].posX < play_width) &&
        (entity[0].posY + last_dir[0].posY > 0 && entity[0].posY + last_dir[0].posY < play_height)) {

        entity[0].posX += last_dir[0].posX;
        entity[0].posY += last_dir[0].posY;
        return 1;
    } else
        return 0;
}

int movement_input_handler(Point prevDirInput[], char input) {
    int8_t dx = 0, dy = 0;

    if (input == '\0') return 0;

    switch (input) {
        case 'w': dx =  0; dy = -1; break;
        case 's': dx =  0; dy =  1; break;
        case 'a': dx = -1; dy =  0; break;
        case 'd': dx =  1; dy =  0; break;
        default:  return 1;
    }

    if (prevDirInput[0].posX * -1 == dx && prevDirInput[0].posY * -1 == dy)
        return 1;

    prevDirInput[0].posX = dx;
    prevDirInput[0].posY = dy;
    return 0;
}

void entity_stamper(void *data, Canvas_buffer *canvas) {
    Segmented_entity *p = (Segmented_entity *)data;
    for (int i = p->segments.amt - 1; i >= 0; i--) {
        int x = p->segments.points[i].posX;
        int y = p->segments.points[i].posY;
        char sym = (i == 0) ? p->entity_visual[0].sym : p->entity_visual[1].sym;
        int r = (i == 0) ? p->entity_visual[0].color.r : p->entity_visual[1].color.r;
        int g = (i == 0) ? p->entity_visual[0].color.g : p->entity_visual[1].color.g;
        int b = (i == 0) ? p->entity_visual[0].color.b : p->entity_visual[1].color.b;

        // Use cellColorAssignment helper with the session's canvas
        cell_color_assign(canvas, x, y, sym, r, g, b);
    }
}

int check_seg_entity_life(Point_list *entity, Point *last_dir, int play_width, int play_height) {
    if (last_dir->posX == 0 && last_dir->posY == 0)
        return 1;
    if (entity->points[0].posX == 0 || entity->points[0].posY == 0 || entity->points[0].posX == play_width || entity->points[0].posY == play_height)
        return 0;
    if (collisionCheckOne(entity, entity, 1, 0))
        return 0;
    return 1;
}

void reset_entity(Segmented_entity *entity, int seg_len, int x, int y) {
    entity->segments.amt = seg_len;
    objectPointResize(&entity->segments);

    entity->segments.points[0].posX = x;
    entity->segments.points[0].posY = y;
    entity->alive = 1;
}
