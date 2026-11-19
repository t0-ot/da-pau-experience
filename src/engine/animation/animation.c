#include "engine/animation/animation.h"
#include <stddef.h>
#include <stdbool.h>

animation_clip animation_clip_cinit(
    int frame_count,
    int sprite_column,
    int sprite_row,
    int frame_width,
    int frame_height,
    float frame_duration,
    bool is_looping) {


}

bool animation_player_init(animation_player *data, const animation_clip *clip) {
    if (clip == NULL || data == NULL)
        return false;
    if (clip->frame_count < 0 || clip->frame_duration < 0)
        return false;
    if (clip->sprite_columns < 0 || clip->sprite_rows < 0)
        return false;
    if (clip->frame_width < 0 || clip->frame_height < 0)
        return false;

    *data = (animation_player){
        .clip = clip,
        .elapsed_time = 0,
        .current_frame = 0,
        .is_playing = false,
        .is_finished = false
    };
    return true;
}

void animation_player_play(animation_player *player) {
}
void animation_player_pause(animation_player *player) {
}
void animation_player_reset(animation_player *player) {
}

void animation_player_update(animation_player *player, const float delta_time) {
    if (player == NULL || player->clip == NULL)
        return;

    if (!player->is_playing || player->is_finished)
        return;

    if (player->clip->frame_count <= 0 ||
        player->clip->frame_duration <= 0.0f ||
        delta_time <= 0.0f)
        return;

    player->elapsed_time += delta_time;

    while (player->elapsed_time >= player->clip->frame_duration) {
        player->elapsed_time -= player->clip->frame_duration;

        if (player->current_frame + 1 < player->clip->frame_count) {
            player->current_frame++;
        } else if (player->clip->is_looping) {
            player->current_frame = 0;
        } else {
            player->is_finished = true;
            player->is_playing = false;
            player->elapsed_time = 0.0f;
            break;
        }
    }
}
