#include <stdlib.h>
#include "raylib.h"

#include "games/snake/snake.h"
#include "games/snake/snake_logic.h"
#include "games/snake/snake_render.h"
#include "engine/core/game_input.h"
#include "engine/utils.h"

#define SNAKE_WIDTH 22
#define SNAKE_HEIGHT 20
#define SNAKE_MOVE_INTERVAL 0.12f

/**
 * @brief Stores the runtime state of a Snake game session.
 *
 * Contains the snake entity, fruit data, movement state,
 * score information, and play area dimensions.
 */
typedef struct {
    Segmented_entity snake; /**< The player's snake entity. */
    defaultFruit baseFruit; /**< Properties of the standard fruit. */
    fruitData normalFruitSet; /**< Gameplay data and positions of active fruits. */

    Point lastDirInput; /**< Current movement direction of the snake. */

    int currentHiScore; /**< Highest score achieved during this session. */
    int play_width; /**< Width of the playable grid in cells. */
    int play_height; /**< Height of the playable grid in cells. */

    float move_timer; /**< Time accumulated toward the next movement. */
    float move_interval; /**< Time required between successive movements. */
} snake_runtime;

static void snake_set_direction(snake_runtime *runtime, const GameInput *input) {
    int dx = 0;
    int dy = 0;

    if (input->up)         dy = -1;
    else if (input->down)  dy = 1;
    else if (input->left)  dx = -1;
    else if (input->right) dx = 1;

    if (dx == 0 && dy == 0)
        return;

    /* Prevent the snake from reversing directly into itself. */
    if (dx == -runtime->lastDirInput.posX &&
        dy == -runtime->lastDirInput.posY)
        return;

    runtime->lastDirInput.posX = dx;
    runtime->lastDirInput.posY = dy;
}

static void snake_init(void *state) {
    snake_runtime *runtime = state;

    runtime->play_width = SNAKE_WIDTH;
    runtime->play_height = SNAKE_HEIGHT;
    runtime->move_interval = SNAKE_MOVE_INTERVAL;
    runtime->move_timer = 0.0f;
    runtime->currentHiScore = 0;

    runtime->snake.segments.amt = 2;
    runtime->snake.segments.points =
        calloc(2, sizeof(Point));

    runtime->normalFruitSet.pointList.amt = 1;
    runtime->normalFruitSet.pointList.points =
        calloc(1, sizeof(Point));

    if (runtime->snake.segments.points == NULL ||
        runtime->normalFruitSet.pointList.points == NULL) {
        runtime->snake.alive = false;
        return;
    }

    int start_x = runtime->play_width / 2;
    int start_y = runtime->play_height / 2;

    runtime->snake.segments.points[0] = (Point){start_x, start_y};
    runtime->snake.segments.points[1] = (Point){start_x - 1, start_y};

    runtime->snake.alive = true;

    /* Start moving right, with the tail behind the head. */
    runtime->lastDirInput = (Point){0, 0};

    runtime->baseFruit.scoreAmt = 1;
    runtime->normalFruitSet.fruitType = &runtime->baseFruit;
    runtime->normalFruitSet.onEat = baseEat;

    spawnFruit(runtime->normalFruitSet.pointList, 0, runtime->snake.segments, runtime->play_width, runtime->play_height);
}

static ModuleAction snake_update(void *state,
                                 const GameInput *input,
                                 float delta_time) {
    snake_runtime *runtime = state;

    if (runtime == NULL || input == NULL ||
        runtime->snake.segments.points == NULL ||
        runtime->normalFruitSet.pointList.points == NULL)
        return MODULE_QUIT;

    if (input->back)
        return MODULE_FINISHED;

    snake_set_direction(runtime, input);
    if (runtime->lastDirInput.posX == 0 &&
        runtime->lastDirInput.posY == 0) {
        return MODULE_CONTINUE;
    }
    runtime->move_timer += delta_time;

    if (runtime->move_timer >= runtime->move_interval) {
        runtime->move_timer -= runtime->move_interval;

        shift_entity_segments(&runtime->snake);

        if (!move_entity_segments(
                runtime->snake.segments.points,
                &runtime->lastDirInput,
                runtime->play_width,
                runtime->play_height)) {
            runtime->snake.alive = false;
        }

        if (runtime->snake.alive) {
            runtime->snake.alive = check_seg_entity_life(
                &runtime->snake.segments,
                &runtime->lastDirInput,
                runtime->play_width,
                runtime->play_height
            );
        }

        if (!runtime->snake.alive) {
            handleDeath(
                &runtime->snake,
                &runtime->normalFruitSet.pointList,
                &runtime->lastDirInput,
                runtime->play_width,
                runtime->play_height
            );

            runtime->snake.segments.points[1] = (Point){
                runtime->snake.segments.points[0].posX - 1,
                runtime->snake.segments.points[0].posY
            };

            runtime->move_timer = 0.0f;
        } else {
            int fruit_collision = collisionCheckOne(
                &runtime->normalFruitSet.pointList,
                &runtime->snake.segments,
                0,
                0
            );

            if (fruit_collision > 0) {
                handleFruitCollision(
                    &runtime->normalFruitSet,
                    &runtime->snake,
                    fruit_collision,
                    runtime->play_width,
                    runtime->play_height
                );
            }
        }
    }
    runtime->currentHiScore = hiScoreCounter(
        runtime->snake.segments.amt-2,
        &runtime->currentHiScore
    );
    return MODULE_CONTINUE;
}

static void snake_draw(void *state) {
    snake_runtime *runtime = state;

    if (runtime == NULL || !runtime->snake.segments.points ||
        !runtime->normalFruitSet.pointList.points)
        return;

    snake_render_game(
        &runtime->snake,
        &runtime->normalFruitSet,
        runtime->snake.segments.amt-2,
        runtime->currentHiScore,
        runtime->play_width,
        runtime->play_height
    );

    runtime->currentHiScore = hiScoreCounter(
        runtime->snake.segments.amt,
        &runtime->currentHiScore
    );
}

static void snake_cleanup(void *state) {
    snake_runtime *runtime = state;

    if (runtime == NULL)
        return;

    free(runtime->snake.segments.points);
    runtime->snake.segments.points = NULL;

    free(runtime->normalFruitSet.pointList.points);
    runtime->normalFruitSet.pointList.points = NULL;
}

const GameModule snake_module = {
    .state_size = sizeof(snake_runtime),
    .init = snake_init,
    .update = snake_update,
    .draw = snake_draw,
    .cleanup = snake_cleanup
};
