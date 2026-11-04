#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "engine/terminal.h"
#include "engine/utils.h"
#include "games/tron/tron.h"
#include "games/snake/snake_logic.h"
#include "games/snake/snake_render.h"

#define GAME_NAME "Da Pau Tron Game"

typedef struct {
    BaseText title_text;
    snakeData player;
    snakeData enemy;
    RenderObj player_obj;
    RenderObj enemy_obj;
    RenderObj title_text_obj;
    RenderObj* screen_render_list[3];
    int total_render_list;
} tron_runtime;

void tron_init(void *runtime_data) {
    tron_runtime *runtime = (tron_runtime *)runtime_data;
    *runtime = (tron_runtime){
        .title_text = {
            .text = GAME_NAME,
            .startX = get_centered_x(tron_module.game_width/2, tron_module.game_width, 17),
            .startY = (tron_module.game_height * 3) / 20,
            .color = {
                {.r = 255, .g = 105, .b = 180},
                {.r = 255, .g = 215, .b = 0}
            }
        },
        .player = {
            .pointList.amt = 1,
            .alive = 1
        },
        .enemy = {
            .pointList.amt = 1,
            .alive = 1
        },

    };
    runtime->player.pointList.points = malloc(runtime->player.pointList.amt * sizeof(Point));
    runtime->player.pointList.points->posX = 25;
    runtime->player.pointList.points->posY = 15;

    runtime->enemy.pointList.points = malloc(runtime->enemy.pointList.amt * sizeof(Point));
    runtime->enemy.pointList.points->posX = 10;
    runtime->enemy.pointList.points->posY = 5;

    runtime->title_text_obj.data = &runtime->title_text;
    runtime->player_obj.data = &runtime->player;
    runtime->enemy_obj.data = &runtime->enemy;
    runtime->title_text_obj.stamp = canvasTextStamperGradient;
    runtime->player_obj.stamp = snakeStamper;
    runtime->enemy_obj.stamp = snakeStamper;

    runtime->screen_render_list[0] = &runtime->title_text_obj;
    runtime->screen_render_list[1] = &runtime->player_obj;
    runtime->screen_render_list[2] = &runtime->enemy_obj;
    runtime->total_render_list = sizeof(runtime->screen_render_list) / sizeof(runtime->screen_render_list[0]);
    set_raw_mode(1);
}

module_action tron_upd(void *runtime_data) {
    tron_runtime *runtime = (tron_runtime *)runtime_data;

    char input = inputHandler();
    if (input == 'q')
        return MODULE_FINISHED;

    return MODULE_CONTINUE;
}

void tron_render(void *runtime_data, CanvasBuffer *canvas) {
    tron_runtime *runtime = (tron_runtime *)runtime_data;
    renderFrame(canvas, runtime->screen_render_list, runtime->total_render_list, (canvas->width / 2));
    printf("W - UP, S - DOWN, A - LEFT, D - RIGHT\n");
    fflush(stdout);
    usleep(100000);
}


/**
 * @brief brief description of what tron_cleanup is doing
 *
 * @param what is the input of tron_cleanup?
 * @return what does it return?
 */
void tron_cleanup(void *runtime_data) {
    tron_runtime *runtime = (tron_runtime *)runtime_data;
}
const game_module tron_module = {
    .game_width = 80,
    .game_height = 20,
    .runtime_size = sizeof(tron_runtime),
    .init = tron_init,
    .upd = tron_upd,
    .render =tron_render,
    .clean_up = tron_cleanup
};
