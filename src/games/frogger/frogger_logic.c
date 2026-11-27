/**
 * @file frogger_logic.c
 * @brief Implements endless vertical scrolling, Pinoy traffic spawning, buffs, and collisions.
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
    row->pickup_type = PICKUP_NONE;
    row->pickup_col = -1;

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

         int dir = (rand() % 2 == 0) ? 1 : -1;
        if (prev1->type == LANE_BAHA && (prev1->speed * (float)dir) > 0.0f) {
            dir = -dir;
        }
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

static void spawn_guaranteed_buff(frogger_runtime *runtime) {
    int base_row = runtime->player.posY + 3 + (rand() % 4);
    if (base_row <= (int)runtime->camera_row + 2) {
        base_row = (int)runtime->camera_row + 3;
    }

    while (runtime->highest_generated_row < base_row + 6) {
        runtime->highest_generated_row++;
        generate_row(runtime, runtime->highest_generated_row);
    }

    int target_row = base_row;
    for (int offset = 0; offset < 5; offset++) {
        const StreetRow *candidate = frogger_get_row(runtime, base_row + offset);
        if (candidate->type != LANE_BAHA) {
            target_row = base_row + offset;
            break;
        }
    }

    const StreetRow *row = frogger_get_row(runtime, target_row);

    int col = 2 + (rand() % (FROGGER_COLS - 4));
    for (int tries = 0; tries < FROGGER_COLS; tries++) {
        int test_col = (col + tries) % FROGGER_COLS;
        if (row->type != LANE_SIDEWALK || !row->blocked_cols[test_col]) {
            col = test_col;
            break;
        }
    }

    runtime->active_pickup_type = (rand() % 2 == 0) ? PICKUP_KWEKKWEK : PICKUP_FISHBALL;
    runtime->active_pickup_row = target_row;
    runtime->active_pickup_col = col;
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
    runtime->buffered_amt = 0;

    runtime->hop_ease_timer = 0.0f;
    runtime->hop_ease_duration = 0.12f;

    animation_player_reset(&runtime->hop_anim);
    animation_player_play(&runtime->hop_anim);

    runtime->camera_row = 0.0f;
    runtime->scroll_speed = 0.65f;


    runtime->kwekkwek_timer = 0.0f;
    runtime->fishball_timer = 0.0f;
    runtime->buff_cooldown_timer = 0.0f;
    runtime->buff_window_timer = 0.0f;
    runtime->buff_spawn_trigger = 5.0f;
    runtime->buff_window_active = true;

    runtime->active_pickup_type = PICKUP_NONE;
    runtime->active_pickup_row = -1;
    runtime->active_pickup_col = -1;

    runtime->popup_timer = 0.0f;
    runtime->popup_text = NULL;

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

    if (runtime->kwekkwek_timer > 0.0f) {
        runtime->kwekkwek_timer -= delta_time;
        if (runtime->kwekkwek_timer < 0.0f)
            runtime->kwekkwek_timer = 0.0f;
    }
    if (runtime->fishball_timer > 0.0f) {
        runtime->fishball_timer -= delta_time;
        if (runtime->fishball_timer < 0.0f)
            runtime->fishball_timer = 0.0f;
    }
    if (runtime->popup_timer > 0.0f) {
        runtime->popup_timer -= delta_time;
        if (runtime->popup_timer <= 0.0f) {
            runtime->popup_timer = 0.0f;
            runtime->popup_text = NULL;
        }
    }

    /* Buff Spawner: 20s gap -> guaranteed spawn within next 20s window */
    if (!runtime->buff_window_active) {
        runtime->buff_cooldown_timer -= delta_time;
        if (runtime->buff_cooldown_timer <= 0.0f) {
            runtime->buff_window_active = true;
            runtime->buff_window_timer = 0.0f;
            runtime->buff_spawn_trigger = (float)(rand() % 200) / 10.0f;
        }
    } else {
        runtime->buff_window_timer += delta_time;
        if (runtime->buff_window_timer >= runtime->buff_spawn_trigger) {
            spawn_guaranteed_buff(runtime);
            runtime->buff_window_active = false;
            runtime->buff_cooldown_timer = 20.0f;
        }
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

            if (target_col >= 0 && target_col < FROGGER_COLS &&
                target_row >= 0) {

                while (runtime->highest_generated_row <
                       target_row + FROGGER_VISIBLE_ROWS) {
                        runtime->highest_generated_row++;
                    generate_row(runtime, runtime->highest_generated_row);
                }

                const StreetRow *dest = frogger_get_row(runtime, target_row);

                bool blocked =
                    dest->type == LANE_SIDEWALK &&
                    dest->blocked_cols[target_col];

                if (!blocked) {
                    runtime->player.posX = target_col;
                    runtime->player.posY = target_row;

                    runtime->player_float_x = (float)target_col;
                    runtime->hop_ease_timer = runtime->hop_ease_duration;

                    int progress = runtime->player.posY - 1;

                    if (progress > runtime->score) {
                        runtime->score = progress;
                        save_if_high_score(runtime);
                    }
                }
            }
        }
    }

    /* 7. Continuous Upward Camera Scroll (Frozen while Kwek-Kwek is active!) */
    float dynamic_scroll = 0.65f + (float)runtime->score * 0.008f;
    if (dynamic_scroll > 2.2f)
        dynamic_scroll = 2.2f;

    if (runtime->kwekkwek_timer > 0.0f) {
        runtime->scroll_speed = 0.0f;
    } else {
        runtime->scroll_speed = dynamic_scroll;
        runtime->camera_row += runtime->scroll_speed * delta_time;
    }

    float min_camera_for_player = (float)runtime->player.posY - ((float)FROGGER_VISIBLE_ROWS * 0.55f);
    if (min_camera_for_player > runtime->camera_row) {
        runtime->camera_row += (min_camera_for_player - runtime->camera_row) * 8.0f * delta_time;
    }

    int top_needed = (int)runtime->camera_row + FROGGER_VISIBLE_ROWS + 4;
    while (runtime->highest_generated_row < top_needed) {
        runtime->highest_generated_row++;
        generate_row(runtime, runtime->highest_generated_row);
    }

    if (runtime->kwekkwek_timer <= 0.0f &&
        (float)runtime->player.posY < runtime->camera_row - 0.35f) {
        trigger_game_over(runtime, DEATH_NADAPA);
        return;
    }

    /* 8. Update All Active Vehicles & Floating Planks */
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

    /* 9. Check Street-Food Pickup on Player's Row */
    if (runtime->active_pickup_type != PICKUP_NONE) {
        if (runtime->active_pickup_row < (int)runtime->camera_row - 1) {
            runtime->active_pickup_type = PICKUP_NONE;
            runtime->active_pickup_row = -1;
            runtime->active_pickup_col = -1;
        } else if (runtime->active_pickup_row == runtime->player.posY &&
                   runtime->active_pickup_col == runtime->player.posX) {
            if (runtime->active_pickup_type == PICKUP_KWEKKWEK) {
                runtime->kwekkwek_timer = 10.0f;
                runtime->popup_text = "KWEK-KWEK! TIME FREEZE (10s)";
                runtime->popup_timer = 2.5f;
            } else if (runtime->active_pickup_type == PICKUP_FISHBALL) {
                runtime->fishball_timer = 10.0f;
                runtime->popup_text = "FISHBALL! RIDE VEHICLES (10s)";
                runtime->popup_timer = 2.5f;
            }
            runtime->active_pickup_type = PICKUP_NONE;
            runtime->active_pickup_row = -1;
            runtime->active_pickup_col = -1;
        }
    }

    /* 10. Check Current Row Hazards */
    const StreetRow *curr = frogger_get_row(runtime, runtime->player.posY);

    if (curr->type == LANE_KALSADA) {
        float px = runtime->player_float_x;

        if (runtime->fishball_timer > 0.0f) {
            for (int i = 0; i < curr->object_count; i++) {
                LaneObject veh = curr->objects[i];
                if (px + 0.55f >= veh.x && px + 0.45f <= veh.x + veh.width) {
                    runtime->player_float_x += curr->speed * delta_time;
                    runtime->player.posX = (int)roundf(runtime->player_float_x);
                    if (runtime->player_float_x < -0.4f ||
                        runtime->player_float_x > (float)FROGGER_COLS - 0.6f) {
                        trigger_game_over(runtime, DEATH_NASAGASAAN);
                        return;
                    }
                    break;
                }
            }
        } else {
            for (int i = 0; i < curr->object_count; i++) {
                LaneObject veh = curr->objects[i];
                if (px + 0.78f > veh.x && px + 0.22f < veh.x + veh.width) {
                    trigger_game_over(runtime, DEATH_NASAGASAAN);
                    return;
                }
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
