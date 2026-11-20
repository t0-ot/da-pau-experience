#ifndef ANIMATION_H
#define ANIMATION_H

#include <stdbool.h>

typedef enum {
    ANIM_INVALID_ARGUMENTS,
    ANIM_INVALID_CLIP,
    ANIM_CLIP_SUCCESSFUL_INIT,
    ANIM_PLAYER_SUCCESSFUL_INIT,
    ANIM_SUCCESSFUL_UPDATE
} AnimationResult;

typedef enum {
    ANIMATION_PLAY_ONCE,
    ANIMATION_LOOP
} animation_playback_mode;

typedef struct {
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

#endif // ANIMATION_H
