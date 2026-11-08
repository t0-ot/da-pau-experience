#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "engine/terminal.h"
#include "engine/entity.h"
#include "engine/utils.h"
#include "games/tron/tron.h"
#include "games/tron/tron_internal.h"
#include "games/tron/tron_logic.h"

#define GAME_NAME " -= Da Pau Tron Game=- "

static int level_intro_finished(const struct timespec *start) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    double elapsed =
        (now.tv_sec - start->tv_sec) +
        (now.tv_nsec - start->tv_nsec) / 1000000000.0;

    return elapsed >= 3.0;
}

void tron_init(void *runtime_data) {
    tron_runtime *runtime = (tron_runtime *)runtime_data;

    *runtime = (tron_runtime){
        .title_text = {
            .text = GAME_NAME,
            .startX = get_centered_x(0, tron_module.game_width, 23),
            .startY = 0,
            .color = {
                {.r = 255, .g = 105, .b = 180},
                {.r = 255, .g = 215, .b = 0}
            }
        },
        .player = {
            .entity_visual = {
                [0 ... 1].color = {
                    .r = 89,
                    .g = 89,
                    .b = 255
                },
                [0].sym = 'T',
                [1].sym = 'O',
            },
            .segments.amt = 1,
            .alive = 1
        },
        .enemy = {
            [0] = {
                .entity_visual = {
                    [0 ... 1].color = {
                        .r = 255,
                        .g = 215,
                        .b = 0
                    },
                    [0].sym = 'Y',
                    [1].sym = 'O',
                },
                .segments.amt = 1,
                .alive = 1
            },
            [1] = {
                .entity_visual = {
                    [0 ... 1].color = {
                        .r = 245,
                        .g = 190,
                        .b = 20
                    },
                    [0].sym = 'Y',
                    [1].sym = 'O',
                },
                .segments.amt = 1,
                .alive = 1
            },
            [2] = {
                .entity_visual = {
                    [0 ... 1].color = {
                        .r = 255,
                        .g = 235,
                        .b = 80
                    },
                    [0].sym = 'Y',
                    [1].sym = 'O',
                },
                .segments.amt = 1,
                .alive = 1
            }
        },
        .view_enemies = {
            .enemy_amt = 1
        },
        .level_text = {
            .text = (char *)get_level_name(1),
            .startX = get_centered_x(0, tron_module.game_width, strlen(get_level_name(1))),
            .startY = get_centered_y(0, tron_module.game_height, 1),
            .color = {
                [0] = {.r = 255, .g = 255, .b = 255}
            }
        },
        .last_dir_input = {{0, 0}},
        .toggle_turbo = 0,
        .level = 1,
        .intro_level = 1,
        .enemy_distracted = 75,
        .enemy_countdown = 3,
        .enemy_update_interval = 3,
        .level_starting = 1
    };
    clock_gettime(CLOCK_MONOTONIC, &runtime->level_start_time);
    runtime->player.segments.points = malloc(runtime->player.segments.amt * sizeof(Point));
    runtime->player.segments.points[0].posX = tron_module.game_width / 2;
    runtime->player.segments.points[0].posY = 15;


    for (int i = 0; i < 3; i++) {

        runtime->enemy[i].segments.points = malloc(runtime->enemy[i].segments.amt * sizeof(Point));
        runtime->enemy[i].segments.points[0].posX = enemy_start_x[i];
        runtime->enemy[i].segments.points[0].posY = enemy_start_y[i];
        runtime->enemy_obj[i].data = &runtime->enemy[i];
        runtime->enemy_obj[i].stamp = entity_stamper;
    }

    runtime->level_text_obj.data = &runtime->level_text;
    runtime->level_text_obj.stamp = text_stamper;
    runtime->title_text_obj.data = &runtime->title_text;
    runtime->player_obj.data = &runtime->player;

    runtime->title_text_obj.stamp = gradient_text_stamper;
    runtime->player_obj.stamp = entity_stamper;

    runtime->view_enemies.enemy = runtime->enemy;


    runtime->screen_render_list[0] = &runtime->title_text_obj;
    runtime->screen_render_list[1] = &runtime->player_obj;

    for (int i = 0; i < runtime->view_enemies.enemy_amt; i++)
        runtime->screen_render_list[i + 2] = &runtime->enemy_obj[i];

    runtime->screen_render_list[5] = &runtime->level_text_obj;

    runtime->total_render_list = sizeof(runtime->screen_render_list) / sizeof(runtime->screen_render_list[0]);

    set_raw_mode(1);
}

module_action tron_upd(void *runtime_data) {
    tron_runtime *runtime = (tron_runtime *)runtime_data;

    char input = inputHandler();

    if (input == 'q')
        return MODULE_FINISHED;

    // Handle level introduction
    if (runtime->level_starting) {
        if (level_intro_finished(&runtime->level_start_time)) {
            runtime->level_starting = 0;
        } else {
            return MODULE_CONTINUE;
        }
    }

    if (input == 'e')
        runtime->toggle_turbo = (runtime->toggle_turbo == 0) ? 1 : 0;

    update_tron_player(runtime, input, tron_module.game_width, tron_module.game_height);

    if (--runtime->enemy_countdown == 0) {
        enemy_lightcycle_ai(&runtime->view_enemies, &runtime->player.segments, runtime->enemy_distracted, tron_module.game_width - 1, tron_module.game_height - 1);
        runtime->enemy_countdown = runtime->enemy_update_interval;
    }

    if (all_enemies_dead(runtime)) {
        advance_tron_level(runtime, tron_module.game_width);
        runtime->level_text.text = (char *)get_level_name(runtime->level);
        runtime->level_text.startX = get_centered_x(0, tron_module.game_width, strlen(runtime->level_text.text));
    }

    update_enemy_renderlist(runtime);

    return MODULE_CONTINUE;
}

void tron_render(void *runtime_data, Canvas_buffer *canvas) {
    tron_runtime *runtime = (tron_runtime *)runtime_data;
    runtime->screen_render_list[5] = runtime->level_starting ? &runtime->level_text_obj : NULL;
    render_frame(canvas, runtime->screen_render_list, runtime->total_render_list, 0);

    printf("W - UP, S - DOWN, A - LEFT, D - RIGHT, E - TOGGLE TURBO\n");
    printf("Level: %d", runtime->level);

    fflush(stdout);
    usleep(100000);
}

void tron_cleanup(void *runtime_data) {
    tron_runtime *runtime = (tron_runtime *)runtime_data;

    free(runtime->player.segments.points);

    for (int i = 0; i < 3; i++) {
        free(runtime->enemy[i].segments.points);
    }

    free(runtime);

    set_raw_mode(0);
}

const game_module tron_module = {
    .game_width = 80,
    .game_height = 20,
    .runtime_size = sizeof(tron_runtime),
    .init = tron_init,
    .upd = tron_upd,
    .render = tron_render,
    .clean_up = tron_cleanup
};
