
/**
 * @file snake_logic.h
 * @brief Gameplay data and operations for Snake.
 */

#ifndef SNAKE_LOGIC_H
#define SNAKE_LOGIC_H

#include "engine/entity.h"

typedef struct snake_runtime snake_runtime;

/**
 * @brief Properties of the standard fruit.
 */
typedef struct {
    int8_t scoreAmt; /**< Number of segments added when the fruit is eaten. */
} defaultFruit;

/**
 * @brief Stores fruit gameplay data and its positions.
 *
 * Rendering information is deliberately excluded.
 */
typedef struct {
    void *fruitType; /**< Pointer to the specific fruit type and its properties. */
    void (*onEat)(void *fruit, Segmented_entity *snake, int *pending_growth); /**< Eating behavior assigned to the fruit. */
    Point_list pointList; /**< Positions of the fruit instances. */
} fruitData;


/**
 * @brief Updates and returns the highest score achieved.
 *
 * @param currentScore The snake's current segment count.
 * @param highestScore Pointer to the stored highest score.
 * @return The updated highest score.
 */
int hiScoreCounter(int currentScore, int *highestScore);

/**
 * @brief Resets the snake and respawns its fruit after death.
 *
 * Resets the snake to the center of the play area, clears its
 * movement direction, and spawns a new fruit.
 *
 * @param snake Pointer to the snake entity.
 * @param fruit Pointer to the fruit position list.
 * @param lastDirInput Pointer to the snake's last movement direction.
 * @param play_width Width of the play area.
 * @param play_height Height of the play area.
 */
void handleDeath(Segmented_entity *snake1, Point_list *fruit, Point *lastDirInput, int play_width, int play_height, bool *has_started, int *pending_growth);

/**
 * @brief Generates a fruit position that does not overlap the snake.
 *
 * Randomly selects a position within the play area and repeats
 * the process if the position collides with the snake.
 *
 * @param fruitView Fruit position list receiving the new position.
 * @param fruitNumber Index of the fruit to reposition.
 * @param snakeView Current positions of the snake's segments.
 * @param spawnWidth Width of the play area.
 * @param spawnHeight Height of the play area.
 */
void spawnFruit(Point_list fruitView, int fruitNumber, Segmented_entity *snake, int spawnWidth, int spawnHeight);

/**
 * @brief Applies the standard fruit's score to the snake.
 *
 * Increases the snake's segment count by the score amount
 * configured in the fruit's defaultFruit data.
 *
 * @param fruit Pointer to the fruit's gameplay data.
 * @param snake Pointer to the snake entity receiving the score.
 */
void baseEat(void *fruit, Segmented_entity *snake, int *pending_growth);

/**
 * @brief Spawns an additional fruit when the snake reaches a milestone.
 *
 * Checks whether the snake's segment count meets the milestone
 * condition and, if so, adds another fruit to the fruit position list.
 *
 * @param fruitAwardTarget Pointer to the fruit data receiving the new fruit.
 * @param snakeScore Pointer to the snake's segment list.
 * @param play_width Width of the play area.
 * @param play_height Height of the play area.
 */
void handleMilestoneFruit(fruitData *fruitAwardTarget, Segmented_entity *snake, int play_width, int play_height, int pending_segments);

/**
 * @brief Handles the snake's collision with a fruit.
 *
 * Executes the fruit's assigned eating behavior, resizes the snake's
 * segment list, checks for milestone fruit generation, and respawns
 * the collected fruit.
 *
 * @param collidedFruitData Pointer to the fruit data being collected.
 * @param snakeInfo Pointer to the snake entity.
 * @param collidedFruitNum Number identifying the collected fruit.
 * @param play_width Width of the play area.
 * @param play_height Height of the play area.
 */
void handleFruitCollision(fruitData *collidedFruitData, Segmented_entity *snakeInfo, int collidedFruitNum, int play_width, int play_height, int *pending_growth);

#endif // SNAKE_LOGIC_H
