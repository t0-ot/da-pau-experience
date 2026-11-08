
#include "engine/entity.h"
#include "engine/utils.h"

void shift_entity_segments(Segmented_entity *entity) {
    for (int i = entity->segments.amt - 1; i > 0; i--)
        entity->segments.points[i] = entity->segments.points[i - 1];
}

int move_entity_segments(Point *entity, Point *last_dir, int play_width, int play_height) {
    int next_x = entity[0].posX + last_dir[0].posX;
    int next_y = entity[0].posY + last_dir[0].posY;

    if (next_x >= 0 && next_x < play_width &&
        next_y >= 0 && next_y < play_height) {

        entity[0].posX = next_x;
        entity[0].posY = next_y;
        return 1;
    }

    return 0;
}

int check_seg_entity_life(Point_list *entity, Point *last_dir, int play_width, int play_height) {
    if (last_dir->posX == 0 && last_dir->posY == 0)
        return 1;

    if (entity->points[0].posX < 0 ||
        entity->points[0].posY < 0 ||
        entity->points[0].posX >= play_width ||
        entity->points[0].posY >= play_height)
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
    entity->alive = true;
}
