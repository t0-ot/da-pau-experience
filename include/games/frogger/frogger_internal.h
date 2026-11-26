#ifndef FROGGER_INTERNAL_H_INCLUDED
#define FROGGER_INTERNAL_H_INCLUDED

#include <stdbool.h>
#include "engine/core/game_types.h"
#include "engine/assets/asset_manager.h"
#include "engine/assets/texture.h"
#include "engine/animation/animation.h"

#define FROGGER_COLS 15
#define FROGGER_VISIBLE_ROWS 14
#define FROGGER_BUFFER_ROWS 24
#define FROGGER_MAX_LANE_OBJECTS 4

typedef enum {
    LANE_SIDEWALK,
    LANE_KALSADA,
    LANE_BAHA
} LaneType;

typedef enum {
    OBJ_MOTOR,
    OBJ_TRICYCLE,
    OBJ_JEEPNEY,
    OBJ_BUS,
    OBJ_PLANK
} ObjectKind;

typedef struct {
    float x;
    float width;
    ObjectKind kind;
    int color_idx;
} LaneObject;

typedef struct StreetRow {
    int world_row;
    LaneType type;
    float speed;
    bool blocked_cols[FROGGER_COLS];
    LaneObject objects[FROGGER_MAX_LANE_OBJECTS];
    int object_count;
} StreetRow;

typedef enum {
    DEATH_NONE,
    DEATH_NASAGASAAN,
    DEATH_NALUNOD,
    DEATH_NADAPA
} DeathReason;

typedef struct frogger_runtime {
    Point player;
    float player_float_x;

    AssetManager *assets;
    AssetGroupHandle asset_group;
    TextureHandle player_tex;
    TextureHandle vehicle_tex;
    bool has_player_sprite;
    bool has_vehicle_sprite;

    animation_clip hop_clip;
    animation_player hop_anim;
    float hop_ease_timer;
    float hop_ease_duration;

    float camera_row;
    float scroll_speed;

    int score;
    int current_hi_score;
    bool game_over;
    DeathReason death_reason;


    int highest_generated_row;
    StreetRow rows[FROGGER_BUFFER_ROWS];
} frogger_runtime;

#endif /* FROGGER_INTERNAL_H_INCLUDED */
