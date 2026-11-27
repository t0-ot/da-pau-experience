/**
 * @file frogger_logic.c
 * @brief Implements endless vertical scrolling, Pinoy traffic spawning, and collisions.
 */

#include <stdlib.h>
#include <math.h>
#include "engine/animation/animation.h"
#include "engine/input/movement.h"
#include "games/frogger/frogger_logic.h"
#include "games/frogger/frogger_internal.h"
#include "engine/storage/highscore.h"

static StreetRow *get_mutable_row(frogger_runtime *runtime, int world_row) {
    int idx = world_row % FROGGER_BUFFER_ROWS;
    if (idx < 0)
        idx += FROGGER_BUFFER_ROWS;
    return &runtime->rows[idx];
}

const StreetRow *frogger_get_row(const frogger_runtime *runtime, int world_row) {
    int idx = world_row % FROGGER_BUFFER_ROWS;
    if (idx < 0)
        idx += FROGGER_BUFFER_ROWS;
    return &runtime->rows[idx];
}

static void generate_row(frogger_runtime *runtime, int world_row) {
    StreetRow *row = get_mutable_row(runtime, world_row);
    row->world_row = world_row;
    row->object_count = 0;
    row->speed = 0.0f;

    for (int c = 0; c < FROGGER_COLS; c++) {
        row->blocked_cols[c] = false;
    }

    if (world_row <= 2) {
        row->type = LANE_SIDEWALK;
        return;
    }

    float difficulty = 1.0f + (float)world_row * 0.015f;
    if (difficulty > 2.6f)
        difficulty = 2.6f;

    const StreetRow *prev1 = frogger_get_row(runtime, world_row - 1);
    const StreetRow *prev2 = frogger_get_row(runtime, world_row - 2);
    const StreetRow *prev3 = frogger_get_row(runtime, world_row - 3);
    const StreetRow *prev4 = frogger_get_row(runtime, world_row - 4);

    bool force_sidewalk = (prev1->type != LANE_SIDEWALK &&
                           prev2->type != LANE_SIDEWALK &&
                           prev3->type != LANE_SIDEWALK &&
                           prev4->type != LANE_SIDEWALK);

    int roll = rand() % 100;

    if (force_sidewalk || roll < 26) {
        row->type = LANE_SIDEWALK;

        int posts = 0;
        for (int c = 0; c < FROGGER_COLS; c++) {
            if (world_row == 3 && c == FROGGER_COLS / 2)
                continue;

            if (posts < 5 && (rand() % 100) < 22) {
                row->blocked_cols[c] = true;
                posts++;
            }
        }
    } else if (roll < 82) {
        row->type = LANE_KALSADA;

        int dir = (rand() % 2 == 0) ? 1 : -1;
        if (prev1->type == LANE_KALSADA && (prev1->speed * (float)dir) > 0.0f) {
            dir = -dir;
        }

        int vehicle_roll = rand() % 100;
        ObjectKind kind;
        float width;
        float base_speed;
        int count;

        if (vehicle_roll < 25) {
            kind = OBJ_MOTOR;
            width = 1.0f;
            base_speed = 4.6f;
            count = 3;
        } else if (vehicle_roll < 52) {
            kind = OBJ_TRICYCLE;
            width = 1.4f;
            base_speed = 2.6f;
            count = 3;
        } else if (vehicle_roll < 85) {
            kind = OBJ_JEEPNEY;
            width = 2.2f;
            base_speed = 3.2f;
            count = 2 + (rand() % 2);
        } else {
            kind = OBJ_BUS;
            width = 3.2f;
            base_speed = 2.2f;
            count = 2;
        }

        row->speed = (float)dir * base_speed * difficulty;
        row->object_count = count;

        float spacing = (float)FROGGER_COLS / (float)count;
        float start_offset = (float)(rand() % 5);

        for (int i = 0; i < count; i++) {
            row->objects[i] = (LaneObject){
                .x = fmodf(start_offset + (float)i * spacing, (float)FROGGER_COLS),
                .width = width,
                .kind = kind,
                .color_idx = rand() % 5
            };
        }
    } else {
        row->type = LANE_BAHA;

        int dir = (prev1->type == LANE_BAHA && prev1->speed > 0.0f) ? -1 : 1;
        row->speed = (float)dir * (2.0f + (float)(rand() % 10) * 0.12f) * difficulty;
        row->object_count = 3;

        float spacing = (float)FROGGER_COLS / 3.0f;
        float start_offset = (float)(rand() % 4);

        for (int i = 0; i < 3; i++) {
            row->objects[i] = (LaneObject){
                .x = fmodf(start_offset + (float)i * spacing, (float)FROGGER_COLS),
                .width = 2.8f,
                .kind = OBJ_PLANK,
                .color_idx = 0
            };
        }
    }
}

static void save_if_high_score(frogger_runtime *runtime) {
    if (runtime->score > runtime->current_hi_score) {
        runtime->current_hi_score = runtime->score;
        highscore_save("frogger", runtime->current_hi_score);
    }
}

static void trigger_game_over(frogger_runtime *runtime, DeathReason reason) {
    runtime->game_over = true;
    runtime->death_reason = reason;
    animation_player_pause(&runtime->hop_anim);
    save_if_high_score(runtime);
}

void frogger_init_state(frogger_runtime *runtime) {
    runtime->player = (Point){ FROGGER_COLS / 2, 1 };
    runtime->player_float_x = (float)runtime->player.posX;

    runtime->hop_ease_timer = 0.0f;
    runtime->hop_ease_duration = 0.12f;

    animation_player_reset(&runtime->hop_anim);
    animation_player_play(&runtime->hop_anim);

    runtime->camera_row = 0.0f;
    runtime->scroll_speed = .65f;

    runtime->score = 0;
    runtime->current_hi_score = highscore_load("frogger");
    runtime->game_over = false;
    runtime->death_reason = DEATH_NONE;

    runtime->highest_generated_row = -1;
    for (int r = 0; r < FROGGER_BUFFER_ROWS; r++) {
        generate_row(runtime, r);
        runtime->highest_generated_row = r;
    }
}

void frogger_update_logic(
    frogger_runtime *runtime,
    const GameInput *input,
    float delta_time
) {
    if (runtime->game_over) {
        if (input->confirm) {
            frogger_init_state(runtime);
        }
        return;
    }

    animation_player_update(&runtime->hop_anim, delta_time);

    if (runtime->hop_ease_timer > 0.0f) {
        runtime->hop_ease_timer -= delta_time;
        if (runtime->hop_ease_timer < 0.0f)
            runtime->hop_ease_timer = 0.0f;
    }

    /* 1. Capture directional input */
    grid_movement_handler(runtime->input_buffer, input, &runtime->buffered_amt);

    /* 2. Consume buffered input when ready */
    if (runtime->hop_ease_timer <= 0.0f) {
        Point direction;

        if (consume_buffer_input(
                &direction,
                runtime->input_buffer,
                &runtime->buffered_amt)) {

            int target_col = runtime->player.posX + direction.posX;
            int target_row = runtime->player.posY - direction.posY;

            /* 3. Check destination boundaries */
            if (target_col >= 0 && target_col < FROGGER_COLS &&
                target_row >= 0) {

                /* 4. Generate rows ahead */
                while (runtime->highest_generated_row <
                       target_row + FROGGER_VISIBLE_ROWS) {
                    runtime->highest_generated_row++;
                    generate_row(runtime, runtime->highest_generated_row);
                }

                /* 5. Check collision */
                const StreetRow *dest = frogger_get_row(runtime, target_row);

                bool blocked =
                    dest->type == LANE_SIDEWALK &&
                    dest->blocked_cols[target_col];

                if (!blocked) {
                    runtime->player.posX = target_col;
                    runtime->player.posY = target_row;

                    runtime->player_float_x = (float)target_col;
                    runtime->hop_ease_timer = runtime->hop_ease_duration;

                    /* 6. Update score */
                    int progress = runtime->player.posY - 1;

                    if (progress > runtime->score) {
                        runtime->score = progress;
                        save_if_high_score(runtime);
                    }
                }
            }
        }
    }

    /* 2. Continuous Upward Camera Scroll */
    float dynamic_scroll = 1.15f + (float)runtime->score * 0.012f;
    if (dynamic_scroll > 3.7f)
        dynamic_scroll = 3.7f;
    runtime->scroll_speed = dynamic_scroll;

    runtime->camera_row += runtime->scroll_speed * delta_time;

    float min_camera_for_player = (float)runtime->player.posY - ((float)FROGGER_VISIBLE_ROWS * 0.55f);
    if (min_camera_for_player > runtime->camera_row) {
        runtime->camera_row += (min_camera_for_player - runtime->camera_row) * 8.0f * delta_time;
    }

    int top_needed = (int)runtime->camera_row + FROGGER_VISIBLE_ROWS + 4;
    while (runtime->highest_generated_row < top_needed) {
        runtime->highest_generated_row++;
        generate_row(runtime, runtime->highest_generated_row);
    }

    if ((float)runtime->player.posY < runtime->camera_row - 0.35f) {
        trigger_game_over(runtime, DEATH_NADAPA);
        return;
    }

    /* 3. Update All Active Vehicles & Floating Planks */
    int first_active = (int)runtime->camera_row - 1;
    if (first_active < 0)
        first_active = 0;

    for (int r = first_active; r <= runtime->highest_generated_row; r++) {
        StreetRow *row = get_mutable_row(runtime, r);
        for (int i = 0; i < row->object_count; i++) {
            row->objects[i].x += row->speed * delta_time;

            if (row->speed > 0.0f && row->objects[i].x > (float)FROGGER_COLS) {
                row->objects[i].x = -row->objects[i].width;
            } else if (row->speed < 0.0f &&
                       row->objects[i].x + row->objects[i].width < 0.0f) {
                row->objects[i].x = (float)FROGGER_COLS;
            }
        }
    }

    /* 4. Check Current Row Hazards */
    const StreetRow *curr = frogger_get_row(runtime, runtime->player.posY);

    if (curr->type == LANE_KALSADA) {
        float px = runtime->player_float_x;
        for (int i = 0; i < curr->object_count; i++) {
            LaneObject veh = curr->objects[i];
            if (px + 0.78f > veh.x && px + 0.22f < veh.x + veh.width) {
                trigger_game_over(runtime, DEATH_NASAGASAAN);
                return;
            }
        }
    } else if (curr->type == LANE_BAHA) {
        bool on_plank = false;
        float px = runtime->player_float_x;

        for (int i = 0; i < curr->object_count; i++) {
            LaneObject plank = curr->objects[i];
            if (px + 0.55f >= plank.x && px + 0.45f <= plank.x + plank.width) {
                on_plank = true;
                break;
            }
        }

        if (!on_plank) {
            trigger_game_over(runtime, DEATH_NALUNOD);
            return;
        }

        runtime->player_float_x += curr->speed * delta_time;
        runtime->player.posX = (int)roundf(runtime->player_float_x);
        if (runtime->player_float_x < -0.4f ||
        runtime->player_float_x > (float)FROGGER_COLS - 0.6f) {
        trigger_game_over(runtime, DEATH_NALUNOD);
            return;
        }
    }
}
