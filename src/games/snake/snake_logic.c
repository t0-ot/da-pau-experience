#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "engine/utils.h"
#include "engine/storage/highscore.h"
#include "games/snake/snake_logic.h"

void handleDeath(Segmented_entity *snake1, Point_list *fruit, Point *lastDirInput, int play_width, int play_height){
    reset_entity(snake1, 2, play_width/2, play_height/2);
    lastDirInput[0].posY = 0;
    lastDirInput[0].posX = 0;
    fruit->amt = 1;
    objectPointResize(fruit);
    spawnFruit(*fruit,  0, snake1->segments, play_width, play_height);
}

void spawnFruit(Point_list fruitView, int fruitNumber, Point_list snakeView, int spawnWidth, int spawnHeight) {
    do {
        fruitView.points[fruitNumber].posX = (rand() % (spawnWidth - 2)) + 1;
        fruitView.points[fruitNumber].posY = (rand() % (spawnHeight - 2)) + 1;
    } while (collisionCheckOne(&snakeView, &fruitView, 0, fruitNumber) || collisionCheckOne(&fruitView, &fruitView, 0, fruitNumber));
}

void baseEat(void *fruit, Segmented_entity *snake) {
    snake->segments.amt += ((defaultFruit *)((fruitData *)fruit)->fruitType)->scoreAmt;
}

void handleMilestoneFruit(fruitData *fruitAwardTarget, Point_list *snakeScore, int play_width, int play_height) {
    if (((snakeScore->amt -2) % 10) != 0)
        return;
    fruitAwardTarget->pointList.amt++;
    objectPointResize(&fruitAwardTarget->pointList);
    spawnFruit(fruitAwardTarget->pointList, fruitAwardTarget->pointList.amt-1, *snakeScore, play_width, play_height);
}

void handleFruitCollision(fruitData *collidedFruitData, Segmented_entity *snakeInfo, int collidedFruitNum, int play_width, int play_height) {
    collidedFruitData->onEat(collidedFruitData, snakeInfo);
    objectPointResize(&snakeInfo->segments);
    handleMilestoneFruit(collidedFruitData, &snakeInfo->segments, play_width, play_height);
    spawnFruit(collidedFruitData->pointList, collidedFruitNum-1, snakeInfo->segments, play_width, play_height);
}

int hiScoreCounter(int currentScore, int *highestScore) {
    if (currentScore-2 > *highestScore) {
        *highestScore = currentScore-2;
        highscore_save("snake", *highestScore);
    }
    return *highestScore;
}
