#ifndef GAME_H
#define GAME_H

#include "../../include/engine/canvas.h"
#include "../../include/engine/session.h"
#include "../../include/snake/snake_logic.h"

/**
 * @brief The public module interface for the Snake game.
 */
extern const game_module snake_module;

//Full runtime struct for snake game and it's world
typedef struct {
    Canvas cell[GAME_HEIGHT][GAME_WIDTH];
    BaseText titleText;
    BaseText scoreTexts[2];
    ScoreText scoreNumbers[2];
    RenderObj scoreTextObj;
    RenderObj scoreNumObj;
    RenderObj hiScoreTextObj;
    RenderObj hiScoreNumObj;
    RenderObj titleTextObj;
    snakeData snake1;
    defaultFruit baseFruit;
    fruitData normalFruitSet;
    RenderObj snakeObj1;
    RenderObj normFruitSetObj;
    RenderObj* screenRenderList[7];
    Point lastDirInput[1];
    int totalRenderList;
    int currentHiScore;
    int isRunning;
} snake_runtime;

int hiScoreCounter(int currentScore, int *highestScore);
void gameUpdateLogic(snake_runtime *session);
void snake_init(void *data);
module_action snake_upd(void *runtime_data);
void snake_render(void *runtime_data);
void snake_clean_up(void *runtime_data);

#endif // GAME_H
