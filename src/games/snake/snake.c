#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "games/snake/snake.h"
#include "engine/entity.h"
#include "engine/terminal.h"
#include "engine/utils.h"
#include "games/snake/snake_render.h"

#define GAME_NAME "Da Pau Snake Game"

/**
 * @brief Private runtime data keeping track of snake game, rendering elements, and it's world.
 */
typedef struct {
    Text_layout titleText;
    Text_layout scoreTexts[2];
    ScoreText score_numbers[2];
    RenderObj scoreTextObj;
    RenderObj scoreNumObj;
    RenderObj hiScoreTextObj;
    RenderObj hiScoreNumObj;
    RenderObj titleTextObj;
    Segmented_entity snake1;
    defaultFruit baseFruit;
    fruitData normalFruitSet;
    RenderObj snakeObj1;
    RenderObj normFruitSetObj;
    RenderObj* screen_render_list[7];
    Point lastDirInput[1];
    int total_render_list;
    int currentHiScore;
    int isRunning;
    int play_width;
    int play_height;
} snake_runtime;

void snake_init(void *runtime_data) {
    snake_runtime *runtime = (snake_runtime *)runtime_data;
    *runtime = (snake_runtime){
        .titleText = {
            .text = GAME_NAME,
            .startX = get_centered_x(snake_module.game_width/2, snake_module.game_width, 17),
            .startY = (snake_module.game_height * 3) / 20,
            .color = {
                {.r = 255, .g = 105, .b = 180},
                {.r = 255, .g = 215, .b = 0}
            }
        },
        .scoreTexts = {
            [0].text = "-== SCORE ==-",
            [0].startX = get_centered_x(snake_module.game_width/2, snake_module.game_width, 13),
            [0].startY = (snake_module.game_height * 9) / 20,
            [1].text = "-+== HI-SCORE ==+-",
            [1].startX = get_centered_x(snake_module.game_width/2, snake_module.game_width, 18),
            [1].startY = (snake_module.game_height * 13) / 20,
            [0 ... 1].color = {
                [0].r = 255,
                [0].g = 215,
                [0].b = 0
            }
        },
        .scoreTextObj = {
            .stamp = text_stamper
        },
        .hiScoreTextObj = {
            .stamp = text_stamper
        },
        .titleTextObj = {
            .data = &runtime->titleText,
            .stamp = gradient_text_stamper
        },
        .snake1 = {
            .entity_visual = {
                [0 ... 1].color = {
                    .r = 0,
                    .g = 255,
                    .b = 0
                },
                [0].sym = 'M',
                [1].sym = 'O',
            },
            .segments.amt = 2,
            .alive = 1
        },
        .score_numbers = {
            [0 ... 1].score = 0,
            [0].y = (snake_module.game_height * 8) / 20,
            [1].y = (snake_module.game_height * 12) / 20,
            [0 ... 1].x = get_centered_x(snake_module.game_width/2, snake_module.game_width, 3)
        },
        .scoreNumObj = {
            .stamp = score_stamper
        },
        .hiScoreNumObj = {
            .stamp = score_stamper
        },
        .snakeObj1 = {
            .stamp = snakeStamper
        },
        .baseFruit = { .scoreAmt = 1},
        .normalFruitSet = {
            .pointList.amt = 1,
            .visuals = { .sym = 'P', .color.r =  0, .color.g = 255, .color.b = 0},
            .onEat = baseEat
        },
        .normFruitSetObj = {
            .stamp = fruitStamper
        },
        .lastDirInput = {{0, 0}},
        .currentHiScore = 0,
        .isRunning = 1,
        .play_width = (snake_module.game_width)/2,
        .play_height = (snake_module.game_height) - 1
    };

    runtime->scoreTextObj.data = &runtime->scoreTexts[0];
    runtime->hiScoreTextObj.data = &runtime->scoreTexts[1];
    runtime->titleTextObj.data = &runtime->titleText;
    runtime->scoreNumObj.data = &runtime->score_numbers[0];
    runtime->hiScoreNumObj.data = &runtime->score_numbers[1];
    runtime->snakeObj1.data = &runtime->snake1;
    runtime->normFruitSetObj.data = &runtime->normalFruitSet;

    runtime->screen_render_list[0] = &runtime->normFruitSetObj;
    runtime->screen_render_list[1] = &runtime->snakeObj1;
    runtime->screen_render_list[2] = &runtime->scoreTextObj;
    runtime->screen_render_list[3] = &runtime->hiScoreTextObj;
    runtime->screen_render_list[4] = &runtime->titleTextObj;
    runtime->screen_render_list[5] = &runtime->scoreNumObj;
    runtime->screen_render_list[6] = &runtime->hiScoreNumObj;
    runtime->total_render_list = sizeof(runtime->screen_render_list) / sizeof(runtime->screen_render_list[0]);

    runtime->snake1.segments.points = malloc(runtime->snake1.segments.amt * sizeof(Point));
    runtime->snake1.segments.points->posX = runtime->play_width / 2;
    runtime->snake1.segments.points->posY = runtime->play_height / 2;

    runtime->normalFruitSet.fruitType = &runtime->baseFruit;
    runtime->normalFruitSet.pointList.points = malloc(runtime->normalFruitSet.pointList.amt * sizeof(Point));
    for (int i = 0; i < runtime->normalFruitSet.pointList.amt; i++)
        spawnFruit(runtime->normalFruitSet.pointList, i, runtime->snake1.segments, 39, 19);
    set_raw_mode(1);
}

module_action snake_upd(void *runtime_data) {
    snake_runtime *runtime = (snake_runtime *)runtime_data;

    char input = inputHandler();
    if (input == 'q')
        return MODULE_FINISHED;

    movement_input_handler(runtime->lastDirInput, input);
    int collideWithFruit = collisionCheckOne(&runtime->normalFruitSet.pointList, &runtime->snake1.segments, 0, 0);
    if (collideWithFruit > 0)
        handleFruitCollision(&runtime->normalFruitSet, &runtime->snake1, collideWithFruit, runtime->play_width, runtime->play_height);

    shift_entity_segments(&runtime->snake1);
    move_entity_segments(runtime->snake1.segments.points, runtime->lastDirInput, runtime->play_width, runtime->play_height);
    runtime->snake1.alive = check_seg_entity_life(&runtime->snake1.segments, runtime->lastDirInput, runtime->play_width, runtime->play_height);
    if (!runtime->snake1.alive)
        handleDeath(&runtime->snake1, &runtime->normalFruitSet.pointList, runtime->lastDirInput, runtime->play_width, runtime->play_height);
    runtime->score_numbers[0].score = runtime->snake1.segments.amt;
    runtime->score_numbers[1].score = hiScoreCounter(runtime->snake1.segments.amt, &runtime->currentHiScore);
    return MODULE_CONTINUE;
}

void snake_render(void *runtime_data, Canvas_buffer *canvas) {
    snake_runtime *runtime = (snake_runtime *)runtime_data;
    render_frame(canvas, runtime->screen_render_list, runtime->total_render_list, (canvas->width / 2));
    printf("W - UP, S - DOWN, A - LEFT, D - RIGHT\n");
    fflush(stdout);
    usleep(100000);
}

void snake_clean_up(void *runtime_data) {
    snake_runtime *runtime = (snake_runtime *)runtime_data;

    free(runtime->snake1.segments.points);
    free(runtime->normalFruitSet.pointList.points);
    free(runtime);
    set_raw_mode(0);
}

const game_module snake_module = {
    .game_width = 80,
    .game_height = 20,
    .runtime_size = sizeof(snake_runtime),
    .init = snake_init,
    .upd = snake_upd,
    .render = snake_render,
    .clean_up = snake_clean_up
};
