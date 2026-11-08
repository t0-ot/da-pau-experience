#ifndef TRON_INTERNAL_H
#define TRON_INTERNAL_H

#include <time.h>
#include "engine/canvas.h"
#include "engine/entity.h"

typedef struct Enemy_list{
    Segmented_entity *enemy;
    int enemy_amt;
} Enemy_list;

typedef struct tron_runtime {
    Text_layout title_text;
    Segmented_entity player;
    Segmented_entity enemy[3];
    Text_layout level_text;
    RenderObj level_text_obj;
    RenderObj player_obj;
    RenderObj enemy_obj[3];
    RenderObj title_text_obj;
    RenderObj* screen_render_list[6];
    struct Enemy_list view_enemies;
    Point last_dir_input[1];
    int total_render_list;
    int toggle_turbo;
    int level;
    int enemy_distracted;
    int enemy_countdown;
    int enemy_update_interval;
    int level_starting;
    int intro_level;
    struct timespec level_start_time;
} tron_runtime;

static const char *level_names[] = {
    "CMSC-10",
    "CMSC-11",
    "CMSC-56",
    "KAS-1",
    "MATH-18",
    "NSTP-1",
    "PE-1",
    "SCI-10"
};

#define LEVEL_COUNT (sizeof(level_names) / sizeof(level_names[0]))

#endif // TRON_INTERNAL_H
