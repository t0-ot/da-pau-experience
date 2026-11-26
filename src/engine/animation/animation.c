#include "engine/animation/animation.h"
#include <stddef.h>
#include <math.h>

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
) {

    // INPUT VALIDITY CHECK
    if (clip == NULL)
        return ANIM_INVALID_ARGUMENTS;
    if (start_x < 0 || start_y < 0)
        return ANIM_INVALID_ARGUMENTS;
    if (frame_width <= 0 || frame_height <= 0)
        return ANIM_INVALID_ARGUMENTS;
    if (playback_mode != ANIMATION_PLAY_ONCE && playback_mode != ANIMATION_LOOP)
        return ANIM_INVALID_ARGUMENTS;
    if (frame_count <= 0)
        return ANIM_INVALID_ARGUMENTS;
    if (sprite_columns <= 0 || sprite_rows <= 0)
        return ANIM_INVALID_ARGUMENTS;
    if (frame_duration <= 0.0f)
        return ANIM_INVALID_ARGUMENTS;
    if ((size_t)frame_count > (size_t)sprite_columns * (size_t)sprite_rows)
        return ANIM_INVALID_ARGUMENTS;

    *clip = (animation_clip){
        .start_x = start_x,
        .start_y = start_y,
        .frame_count = frame_count,
        .sprite_columns = sprite_columns,
        .sprite_rows = sprite_rows,
        .frame_width = frame_width,
        .frame_height = frame_height,
        .frame_duration = frame_duration,
        .playback_mode = playback_mode
    };

    return ANIM_CLIP_SUCCESSFUL_INIT;
}

AnimationResult animation_player_init(animation_player *data, const animation_clip *clip) {
    if (clip == NULL || data == NULL)
        return ANIM_INVALID_ARGUMENTS;

    *data = (animation_player){
        .clip = clip,
        .elapsed_time = 0,
        .current_frame = 0,
        .is_playing = false,
        .is_finished = false
    };
    return ANIM_PLAYER_SUCCESSFUL_INIT;
}

AnimationResult animation_player_get_source(const animation_player *player, Rectangle *source_coordinates) {
    if (player == NULL || player->clip == NULL || source_coordinates == NULL)
        return ANIM_INVALID_ARGUMENTS;

    if (player->clip->frame_count <= 0 ||
        player->clip->sprite_columns <= 0 ||
        player->clip->sprite_rows <= 0 ||
        player->clip->frame_width <= 0 ||
        player->clip->frame_height <= 0)
        return ANIM_INVALID_CLIP;

    if (player->current_frame < 0 ||
        player->current_frame >= player->clip->frame_count)
        return ANIM_INVALID_ARGUMENTS;


    int column = player->current_frame % player->clip->sprite_columns;
    int row = player->current_frame / player->clip->sprite_columns;

    int source_x = player->clip->start_x + column * player->clip->frame_width;
    int source_y = player->clip->start_y + row * player->clip->frame_height;

    *source_coordinates = (Rectangle){
        .x = source_x,
        .y = source_y,
        .width = player->clip->frame_width,
        .height = player->clip->frame_height
    };

    return ANIM_SUCCESSFUL_SOURCE;
}

void animation_player_play(animation_player *player) {
    if (player == NULL || player->is_finished)
        return;
    player->is_playing = true;
}

void animation_player_pause(animation_player *player) {
    if (player == NULL)
        return;
    player->is_playing = 0;
}

void animation_player_reset(animation_player *player) {
    if (player == NULL)
        return;
    player->is_finished = 0;
    player->current_frame = 0;
    player->is_playing = 0;
    player->elapsed_time = 0.0f;
}

AnimationResult animation_player_update(animation_player *player, const float delta_time) {
    if (player == NULL || player->clip == NULL)
        return ANIM_INVALID_ARGUMENTS;

    if (!player->is_playing || player->is_finished)
        return ANIM_SUCCESSFUL_UPDATE;

    if (player->clip->frame_count <= 0 ||
        player->clip->frame_duration <= 0.0f)
        return ANIM_INVALID_ARGUMENTS;

    if (!isfinite(delta_time) || delta_time < 0.0f)
        return ANIM_INVALID_ARGUMENTS;

    player->elapsed_time += delta_time;

    while (player->elapsed_time >= player->clip->frame_duration) {
        player->elapsed_time -= player->clip->frame_duration;

        if (player->current_frame + 1 < player->clip->frame_count) {
            player->current_frame++;
        } else if (player->clip->playback_mode == ANIMATION_LOOP) {
            player->current_frame = 0;
        } else {
            player->is_finished = true;
            player->is_playing = false;
            player->elapsed_time = 0.0f;
            break;
        }
    }
    return ANIM_SUCCESSFUL_UPDATE;
}
