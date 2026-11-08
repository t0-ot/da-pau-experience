#ifndef SNAKE_LOGIC_H
#define SNAKE_LOGIC_H

#include "engine/canvas.h"
#include "engine/entity.h"

typedef struct {
    int8_t scoreAmt;
} defaultFruit;

typedef struct {
    int8_t points;
    int8_t thickness;
} fatFruit;

typedef struct {
    int8_t *speed;
    int8_t points;
} speedFruit;

typedef struct {
    void *fruitType;
    void (*onEat)(void *fruit, Segmented_entity *snake);
    Point_list pointList;
    Canvas visuals;
} fruitData;

int hiScoreCounter(int currentScore, int *highestScore);
void handleDeath(Segmented_entity *snake1, Point_list *fruit, Point *lastDirInput, int play_width, int play_height);
void spawnFruit(Point_list fruitView, int fruitNumber, Point_list snakeView, int spawnWidth, int spawnHeight);
void baseEat(void *fruit, Segmented_entity *snake);
void handleMilestoneFruit(fruitData *fruitAwardTarget, Point_list *snakeScore, int play_width, int play_height);
void handleFruitCollision(fruitData *collidedFruitData, Segmented_entity *snakeInfo, int collidedFruitNum, int play_width, int play_height);

#endif // SNAKE_LOGIC_H
