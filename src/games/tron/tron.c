
#include <stdlib.h>

#include "raylib.h"

#include "games/tron/tron.h"
#include "games/tron/tron_internal.h"
#include "games/tron/tron_logic.h"
#include "games/tron/tron_render.h"

#define TRON_MOVE_INTERVAL 0.10f

static void tron_init(void *state) {
    tron_runtime *runtime = state;

    runtime->player.segments.amt = 1;
    runtime->player.segments.points = calloc(1, sizeof(Point));

    runtime->view_enemies.enemy = runtime->enemy;
    runtime->view_enemies.enemy_amt = 1;

    runtime->player.alive = true;

    runtime->player.segments.points[0] = (Point){TRON_WIDTH / 2, 25};

    for (int i = 0; i < 3; i++) {
        runtime->enemy[i].segments.amt = 1;
        runtime->enemy[i].segments.points = calloc(1, sizeof(Point));
        runtime->enemy[i].alive = true;

        runtime->enemy[i].segments.points[0] = (Point){
            enemy_start_x[i], enemy_start_y[i]
        };
    }

    runtime->last_dir_input = (Point){0, 0};

    // Direction buffer initialization
    runtime->buffered_dir = (Point){0, 0};
    runtime->has_buffered_dir = false;

    runtime->toggle_turbo = 0;
    runtime->level = 1;
    runtime->enemy_distracted = 75;
    runtime->enemy_move_timer = 0.0f;
    runtime->enemy_move_interval = 0.20f;

    runtime->move_timer = 0.0f;
    runtime->move_interval = TRON_MOVE_INTERVAL;
    runtime->intro_timer = 3.0f;
    runtime->level_starting = 1;
}

static ModuleAction tron_update(
    void *state,
    const GameInput *input,
    float delta_time
) {
    tron_runtime *runtime = state;

    if (runtime == NULL || input == NULL ||
        runtime->player.segments.points == NULL)
        return MODULE_QUIT;

    if (input->back)
        return MODULE_FINISHED;

    if (input->action)
        runtime->toggle_turbo = !runtime->toggle_turbo;

    if (runtime->level_starting) {
        runtime->intro_timer -= delta_time;

        if (runtime->intro_timer <= 0.0f)
            runtime->level_starting = 0;

        return MODULE_CONTINUE;
    }

    // 1. Capture directional input every frame
    int dx = 0;
    int dy = 0;

    if (input->up)         dy = -1;
    else if (input->down) dy = 1;
    else if (input->left) dx = -1;
    else if (input->right) dx = 1;

    if (dx != 0 || dy != 0) {
        bool reversing =
            dx == -runtime->last_dir_input.posX &&
            dy == -runtime->last_dir_input.posY;

        bool same_direction =
            dx == runtime->last_dir_input.posX &&
            dy == runtime->last_dir_input.posY;

        if (!reversing && !same_direction) {
            runtime->buffered_dir = (Point){dx, dy};
            runtime->has_buffered_dir = true;
        }
    }

    // 2. Wait for the next movement tick
    runtime->move_timer += delta_time;
    runtime->enemy_move_timer += delta_time;

    while (runtime->move_timer >= runtime->move_interval) {
        runtime->move_timer -= runtime->move_interval;

        // Apply buffered direction if one is available.
        if (runtime->has_buffered_dir) {
            runtime->last_dir_input = runtime->buffered_dir;
            runtime->has_buffered_dir = false;
        }

        // Always move using the current direction.
        update_tron_player(
            runtime,
            runtime->last_dir_input.posX,
            runtime->last_dir_input.posY,
            TRON_WIDTH,
            TRON_HEIGHT
        );

        if (all_enemies_dead(runtime)) {
            advance_tron_level(runtime, TRON_WIDTH);
        }
    }
    while (runtime->enemy_move_timer >= runtime->enemy_move_interval) {
        runtime->enemy_move_timer -= runtime->enemy_move_interval;
        enemy_lightcycle_ai(
            &runtime->view_enemies,
            &runtime->player.segments,
            runtime->enemy_distracted,
            TRON_WIDTH,
            TRON_HEIGHT
        );
    }
    return MODULE_CONTINUE;
}

static void tron_draw(void *state) {
    tron_runtime *runtime = state;

    if (runtime == NULL || runtime->player.segments.points == NULL)
        return;

    tron_render_game(
        &runtime->player,
        runtime->enemy,
        runtime->view_enemies.enemy_amt,
        runtime->level,
        runtime->level_starting,
        runtime->toggle_turbo
    );
}

static void tron_cleanup(void *state) {
    tron_runtime *runtime = state;

    if (runtime == NULL)
        return;

    free(runtime->player.segments.points);

    for (int i = 0; i < 3; i++)
        free(runtime->enemy[i].segments.points);
}

const GameModule tron_module = {
    .state_size = sizeof(tron_runtime),
    .init = tron_init,
    .update = tron_update,
    .draw = tron_draw,
    .cleanup = tron_cleanup
};
