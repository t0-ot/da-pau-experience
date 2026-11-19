#ifndef ANIMATION_H
#define ANIMATION_H

typedef struct {
    int frame_count;

    int sprite_columns;
    int sprite_rows;

    int frame_width;
    int frame_height;

    float frame_duration;

    bool is_looping;
} animation_clip;

typedef struct {
    const animation_clip *clip;
    float elapsed_time;
    int current_frame;
    bool is_playing;
    bool is_finished;
} animation_player;

#endif // ANIMATION_H
