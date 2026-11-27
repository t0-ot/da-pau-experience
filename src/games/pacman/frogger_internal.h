#ifndef FROGGER_INTERNAL_H_INCLUDED
#define FROGGER_INTERNAL_H_INCLUDED

#include <stdbool.h>
#include "engine/core/game_types.h"

#define FROGGER_COLS 15
#define FROGGER_VISIBLE_ROWS 14
#define FROGGER_BUFFER_ROWS 24
#define FROGGER_MAX_LANE_OBJECTS 4

typedef enum{
    Lane_sidewalk,
    Lane_kalsada,
    Lane_baha
} Lane_type;

typedef enum{
    Obj_motor,
    Obj_tricycle,
    Obj_jeepney,
    Obj_plank
}Object_kind;

typedef struct{
    float x;
    float width;
    Object_kind;
    int color_idx;
}Lane_object;


typedef struct{
    int world_row;
    Lane_type type;
    float speed;
    bool blocked_cols[FROGGER_COLS];
    Lane_object objects[FROGGER_MAX_LANE_OBJECTS];
    int object_count;
} Street_row;

typedef struct frogger_runtime{
    int player_col;
    int player_row;
    float player_float_x;


    float camera_row;
    float scroll_speed;

    int score;
    int current_hi_score;
    bool game_over;

    int highest_generated_row;
    Street_riw rows[FROGGER_BUFFER_ROWS]
    }frogger_runtime;




#endif // FROGGER_INTERNAL_H_INCLUDED
