#include <stddef.h>
#include "engine/entity.h"
#include "engine/utils.h"

void advance_entity_tail(Segmented_entity *entity) {
    entity->tail = (entity->tail + 1) % entity->capacity;
}

void grow_entity(Segmented_entity *entity) {
    if (entity == NULL)
        return;

    if (entity->segments.points == NULL || entity->capacity <= 0)
        return;

    if (entity->segments.amt < 0 || entity->segments.amt >= entity->capacity)
        return;

    entity->segments.amt++;
}

int move_entity_segments(Segmented_entity *entity, Point *last_dir, int play_width, int play_height) {
    // 1. Read the current nose (before moving)
    int next_x = entity->segments.points[entity->head].posX + last_dir->posX;
    int next_y = entity->segments.points[entity->head].posY + last_dir->posY;

    if (next_x >= 0 && next_x < play_width &&
        next_y >= 0 && next_y < play_height) {
        entity->head = (entity->head + 1) % entity->capacity;

        // 3. Write the new nose coordinates into that brand-new seat
        entity->segments.points[entity->head].posX = next_x;
        entity->segments.points[entity->head].posY = next_y;
        return 1;
    }

    return 0;
}

int check_seg_entity_life(Segmented_entity *entity, Point *last_dir, int play_width, int play_height) {
    if (last_dir->posX == 0 && last_dir->posY == 0)
        return 1;

    if (entity->segments.points[entity->head].posX < 0 ||
        entity->segments.points[entity->head].posY < 0 ||
        entity->segments.points[entity->head].posX >= play_width ||
        entity->segments.points[entity->head].posY >= play_height)
        return 0;

    if (segmented_self_collision_check(entity))
        return 0;

    return 1;
}

void reset_entity(Segmented_entity *entity, int seg_len, int x, int y) {
    if (entity == NULL || seg_len < 0 || seg_len > entity->capacity || entity->segments.points == NULL)
        return;

    entity->segments.amt = seg_len;

    entity->head = seg_len - 1;
    entity->tail = 0;

    for (int i = 0; i < seg_len; i++) {
        // Snake spawns extending horizontally to the left
        entity->segments.points[i].posX = x - (seg_len - 1 - i);
        entity->segments.points[i].posY = y;
    }
    entity->alive = true;
}

int segmented_self_collision_check(Segmented_entity *entity) {
    for (int i = 0; i < entity->segments.amt-1; i++) {
        int index = (entity->tail + i) % entity->capacity;
        if (entity->segments.points[entity->head].posX == entity->segments.points[index].posX &&
            entity->segments.points[entity->head].posY == entity->segments.points[index].posY) {

            return 1;
        }
    }
    return 0;
}

int segmented_collision_with_pointlist(Segmented_entity *entity, Point_list *objView2, int checkIndexObj2) {
    for (int i = 0; i < entity->segments.amt; i++) {
        int index = (entity->tail + i) % entity->capacity;
        if (entity->segments.points[index].posX == objView2->points[checkIndexObj2].posX && entity->segments.points[index].posY == objView2->points[checkIndexObj2].posY)
            return i+1;
    }
    return 0;
}
