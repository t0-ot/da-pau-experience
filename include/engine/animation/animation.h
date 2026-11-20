#ifndef ANIMATION_H
#define ANIMATION_H

#include <stdbool.h>
#include "raylib.h"

typedef enum {
    ANIM_INVALID_ARGUMENTS,
    ANIM_INVALID_CLIP,
    ANIM_CLIP_SUCCESSFUL_INIT,
    ANIM_PLAYER_SUCCESSFUL_INIT,
    ANIM_SUCCESSFUL_UPDATE,
    ANIM_SUCCESSFUL_SOURCE
} AnimationResult;

typedef enum {
    ANIMATION_PLAY_ONCE,
    ANIMATION_LOOP
} animation_playback_mode;

typedef struct {
    int start_x;
    int start_y;

    int frame_count;

    int sprite_columns;
    int sprite_rows;

    int frame_width;
    int frame_height;

    float frame_duration;

    animation_playback_mode playback_mode;
} animation_clip;

typedef struct {
    const animation_clip *clip;
    float elapsed_time;
    int current_frame;
    bool is_playing;
    bool is_finished;
} animation_player;

AnimationResult animation_clip_init(
    animation_clip *clip,
    int start_x,
    int start_y,
    int frame_count,
    int sprite_columns,
    int sprite_rows,
    int frame_width,
    int frame_height,
    float frame_duration,
    animation_playback_mode playback_mode
);

AnimationResult animation_player_init(animation_player *player, const animation_clip *clip);

AnimationResult animation_player_get_source(const animation_player *player, Rectangle *source_coordinates);

void animation_player_play(animation_player *player);
void animation_player_pause(animation_player *player);
void animation_player_reset(animation_player *player);

AnimationResult animation_player_update(animation_player *player, float delta_time);

#endif // ANIMATION_H
