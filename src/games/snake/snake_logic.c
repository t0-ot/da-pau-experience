#include <string.h>
#include <stdlib.h>
#include "engine/utils.h"
#include "engine/storage/highscore.h"
#include "games/snake/snake_logic.h"

void handleDeath(Segmented_entity *snake1, Point_list *fruit, Point *lastDirInput, int play_width, int play_height, bool *has_started, int *pending_growth) {
    reset_entity(snake1, 2, play_width/2, play_height/2);
    lastDirInput[0] = (Point){1, 0};
    *has_started = false;
    fruit->amt = 1;
    *pending_growth = 0;
    objectPointResize(fruit);
    spawnFruit(*fruit,  0, snake1, play_width, play_height);
}

void spawnFruit(Point_list fruitView, int fruitNumber, Segmented_entity *snake, int spawnWidth, int spawnHeight) {
    do {
        fruitView.points[fruitNumber].posX = (rand() % (spawnWidth - 2)) + 1;
        fruitView.points[fruitNumber].posY = (rand() % (spawnHeight - 2)) + 1;
    } while (segmented_collision_with_pointlist(snake, &fruitView, fruitNumber) || collisionCheckOne(&fruitView, &fruitView, 0, fruitNumber));
}

void baseEat(void *fruit, Segmented_entity *snake, int *pending_growth) {
    *pending_growth += ((defaultFruit *)((fruitData *)fruit)->fruitType)->scoreAmt;
}

void handleMilestoneFruit(fruitData *fruitAwardTarget, Segmented_entity *snake, int play_width, int play_height, int pending_segments) {
    if ((((snake->segments.amt + pending_segments) -2) % 10) != 0)
        return;
    fruitAwardTarget->pointList.amt++;
    objectPointResize(&fruitAwardTarget->pointList);
    spawnFruit(fruitAwardTarget->pointList, fruitAwardTarget->pointList.amt-1, snake, play_width, play_height);
}

void handleFruitCollision(fruitData *collidedFruitData, Segmented_entity *snakeInfo, int collidedFruitNum, int play_width, int play_height, int *pending_growth) {
    collidedFruitData->onEat(collidedFruitData, snakeInfo, pending_growth);
    handleMilestoneFruit(collidedFruitData, snakeInfo, play_width, play_height, *pending_growth);
    spawnFruit(collidedFruitData->pointList, collidedFruitNum-1, snakeInfo, play_width, play_height);
}

int hiScoreCounter(int currentScore, int *highestScore) {
    if (currentScore-2 > *highestScore) {
        *highestScore = currentScore-2;
        highscore_save("snake", *highestScore);
    }
    return *highestScore;
}
