#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "engine/terminal.h"
#include "engine/utils.h"
#include "games/tron/tron.h"
#include "games/snake/snake_logic.h"
#include "games/snake/snake_render.h"

#define GAME_NAME " -= Da Pau Tron Game=- "

typedef struct {
    BaseText title_text;
    snakeData player;
    snakeData enemy;
    RenderObj player_obj;
    RenderObj enemy_obj;
    RenderObj title_text_obj;
    RenderObj* screen_render_list[3];
    Point last_dir_input[1];
    int total_render_list;
    int toggle_turbo;
} tron_runtime;

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
            .pointList.amt = 1,
            .alive = 1
        },
        .enemy = {
            .pointList.amt = 1,
            .alive = 1
        },
        .last_dir_input = {{0, 0}},
        .toggle_turbo = 0

    };
    runtime->player.pointList.points = malloc(runtime->player.pointList.amt * sizeof(Point));
    runtime->player.pointList.points->posX = (tron_module.game_width / 2);
    runtime->player.pointList.points->posY = 15;

    runtime->enemy.pointList.points = malloc(runtime->enemy.pointList.amt * sizeof(Point));
    runtime->enemy.pointList.points->posX = (tron_module.game_width / 2);
    runtime->enemy.pointList.points->posY = 6;

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
    if (input == 'e')
        runtime->toggle_turbo = (runtime->toggle_turbo == 0) ? 1 : 0;

    /*
        Handle tron player movement by checking the last input made by the user then incrementing the
        trail (segments 'amt') then resize the pointList (x and y coordinates of each segment) by
        the amt using the objectPointResize function. Then we shift the segments by letting the
        furthest back by letting it copy the x and y of the segment infront of it. Then lastly move the bike
        (symbol at the most front) move forward by the value stored in last_dir_input.posX and posY
        we also loop all that so that if ever the user toggles turbo they double the lightcycles speed since
        it runs 2 times instead of 1 (1+runtime->toggle_turbo).
    */
    for (int i = 0; i < 1+runtime->toggle_turbo; i++) {
        movementInputHandler(runtime->last_dir_input, input);
        if (runtime->last_dir_input->posX != runtime->last_dir_input->posY)
            runtime->player.pointList.amt++;
        objectPointResize(&runtime->player.pointList);
        shiftSnakeSegments(&runtime->player);
        snakeDirectionHandler(runtime->player.pointList.points, runtime->last_dir_input, tron_module.game_width-1, tron_module.game_height-1);
        runtime->player.alive = checkLife(runtime->player.pointList, tron_module.game_width, tron_module.game_height);
        if (!(runtime->player.alive))
            break;
    }
    if (runtime->player.alive == 0)
        return MODULE_FINISHED;
    return MODULE_CONTINUE;
}

void tron_render(void *runtime_data, CanvasBuffer *canvas) {
    tron_runtime *runtime = (tron_runtime *)runtime_data;
    renderFrame(canvas, runtime->screen_render_list, runtime->total_render_list, 0);
    printf("W - UP, S - DOWN, A - LEFT, D - RIGHT, E - TOGGLE TURBO\n");
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
