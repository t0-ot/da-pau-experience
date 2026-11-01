#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "../../include/snake/snake.h"
#include "../../include/engine/canvas.h"
#include "../../include/engine/terminal.h"
#include "../../include/engine/utils.h"
#include "../../include/engine/session.h"
#include "../../include/snake/snake_render.h"

#define GAME_NAME "Da Pau Eating Experience"

void snake_init(void *runtime_data) {
    snake_runtime *runtime = (snake_runtime *)runtime_data;
    *runtime = (snake_runtime){
        .titleText = {
            .text = GAME_NAME,
            .startX = HALF_GAMEVIEW_BOUNDARY + 9,
            .startY = 3,
            .color = {
                {.r = 255, .g = 105, .b = 180},
                {.r = 255, .g = 215, .b = 0}
            }
        },
        .scoreTexts = {
            [0].text = "-== SCORE ==-",
            [0].startX = 53,
            [0].startY = 9,
            [1].text = "-+== HI-SCORE ==+-",
            [1].startX = 50,
            [1].startY = 13,
            [0 ... 1].color = {
                [0].r = 255,
                [0].g = 215,
                [0].b = 0
            }
        },
        .scoreTextObj = {
            .stamp = canvasTextStamper
        },
        .hiScoreTextObj = {
            .stamp = canvasTextStamper
        },
        .titleTextObj = {
            .data = &runtime->titleText,
            .stamp = canvasTextStamperGradient
        },
        .snake1 = {
            .pointList.amt = 2,
            .alive = 1
        },
        .scoreNumbers = {
            [0 ... 1].score = 0,
            [0].y = 8,
            [1].y = 12,
            [0 ... 1].x = 58
        },
        .scoreNumObj = {
            .stamp = canvasScoreStamper
        },
        .hiScoreNumObj = {
            .stamp = canvasScoreStamper
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
        .isRunning = 1
    };

    runtime->scoreTextObj.data = &runtime->scoreTexts[0];
    runtime->hiScoreTextObj.data = &runtime->scoreTexts[1];
    runtime->titleTextObj.data = &runtime->titleText;
    runtime->scoreNumObj.data = &runtime->scoreNumbers[0];
    runtime->hiScoreNumObj.data = &runtime->scoreNumbers[1];
    runtime->snakeObj1.data = &runtime->snake1;
    runtime->normFruitSetObj.data = &runtime->normalFruitSet;

    runtime->screenRenderList[0] = &runtime->normFruitSetObj;
    runtime->screenRenderList[1] = &runtime->snakeObj1;
    runtime->screenRenderList[2] = &runtime->scoreTextObj;
    runtime->screenRenderList[3] = &runtime->hiScoreTextObj;
    runtime->screenRenderList[4] = &runtime->titleTextObj;
    runtime->screenRenderList[5] = &runtime->scoreNumObj;
    runtime->screenRenderList[6] = &runtime->hiScoreNumObj;
    runtime->totalRenderList = sizeof(runtime->screenRenderList) / sizeof(runtime->screenRenderList[0]);

    runtime->snake1.pointList.points = malloc(runtime->snake1.pointList.amt * sizeof(Point));
    runtime->snake1.pointList.points->posX = 20;
    runtime->snake1.pointList.points->posY = 10;

    runtime->normalFruitSet.fruitType = &runtime->baseFruit;
    runtime->normalFruitSet.pointList.points = malloc(runtime->normalFruitSet.pointList.amt * sizeof(Point));
    for (int i = 0; i < runtime->normalFruitSet.pointList.amt; i++)
        spawnFruit(runtime->normalFruitSet.pointList, i, runtime->snake1.pointList, GAME_WIDTH - 41, GAME_HEIGHT - 1);
    enable_raw_mode();
}

module_action snake_upd(void *runtime_data) {
    snake_runtime *runtime = (snake_runtime *)runtime_data;

    char input = inputHandler();
    if (input == 'q')
        return MODULE_FINISHED;

    int moveBlocked = movementInputHandler(runtime->lastDirInput, input);
    int collideWithFruit = collisionCheckOne(&runtime->normalFruitSet.pointList, &runtime->snake1.pointList, 0, 0);
    if (collideWithFruit > 0)
        handleFruitCollision(&runtime->normalFruitSet, &runtime->snake1, collideWithFruit);

    shiftSnakeSegments(&runtime->snake1);
    snakeDirectionHandler(runtime->snake1.pointList.points, runtime->lastDirInput);
    runtime->snake1.alive = checkLife(runtime->snake1.pointList, GAME_WIDTH - 1, HALF_GAMEVIEW_BOUNDARY);

    if (!(runtime->snake1.alive) && !moveBlocked && !(runtime->lastDirInput->posX == 0 && runtime->lastDirInput->posY == 0))
        handleDeath(&runtime->snake1, &runtime->normalFruitSet.pointList, runtime->lastDirInput);
    runtime->scoreNumbers[0].score = runtime->snake1.pointList.amt;
    runtime->scoreNumbers[1].score = hiScoreCounter(runtime->snake1.pointList.amt, &runtime->currentHiScore);
    return MODULE_CONTINUE;
}

void snake_render(void *runtime_data) {
    snake_runtime *runtime = (snake_runtime *)runtime_data;
    printf("\033[?25l");
    renderFrame(runtime->cell, runtime->screenRenderList, runtime->totalRenderList);
    printf("W - UP, S - DOWN, A - LEFT, D - RIGHT\n");
    fflush(stdout);
    usleep(100000);
}

void snake_clean_up(void *runtime_data) {
    snake_runtime *runtime = (snake_runtime *)runtime_data;
    free(runtime->snake1.pointList.points);
    free(runtime->normalFruitSet.pointList.points);
    free(runtime);
    disable_raw_mode();
    printf("\033[?25h");
}

const game_module snake_module = {
    .init = snake_init,
    .upd = snake_upd,
    .render = snake_render,
    .clean_up = snake_clean_up
};

int hiScoreCounter(int currentScore, int *highestScore) {
    if (currentScore > *highestScore)
        *highestScore = currentScore;
    return *highestScore;
}
