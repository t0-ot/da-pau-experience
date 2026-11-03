#include <stdio.h>
#include <unistd.h>
#include "engine/utils.h"
#include "games/tron/tron.h"
#include "games/snake/snake_logic.h"

#define GAME_NAME "Da Pau Tron Game"

typedef struct {
    BaseText title_text;
    snakeData player;
    RenderObj player_obj;
    RenderObj title_text_obj;
    RenderObj* screen_render_list[2];
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

    };
    runtime->title_text_obj.data = &runtime->title_text;
    runtime->player_obj.data = &runtime->player;

    runtime->screen_render_list[0] = &runtime->title_text_obj;
    runtime->screen_render_list[1] = &runtime->player_obj;
    runtime->total_render_list = sizeof(runtime->screen_render_list) / sizeof(runtime->screen_render_list[0]);

}

module_action tron_upd(void *runtime_data) {
    tron_runtime *runtime = (tron_runtime *)runtime_data;
}

void tron_render(void *runtime_data, CanvasBuffer *canvas) {
    tron_runtime *runtime = (tron_runtime *)runtime_data;
    printf("\033[?25l");
    renderFrame(canvas, runtime->screen_render_list, runtime->total_render_list, (canvas->width / 2));
    printf("W - UP, S - DOWN, A - LEFT, D - RIGHT\n");
    fflush(stdout);
    usleep(100000);
}

void tron_cleanup(void *runtime_data) {
    tron_runtime *runtime = (tron_runtime *)runtime_data;
}

const game_module tron_module = {
    .game_width = 80,
    .game_height = 20,
    .runtime_size = sizeof(tron_runtime)
};
