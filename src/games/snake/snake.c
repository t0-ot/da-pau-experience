#include <stdlib.h>
#include "raylib.h"

#include "games/snake/snake.h"
#include "engine/input/movement.h"
#include "games/snake/snake_logic.h"
#include "games/snake/snake_render.h"
#include "games/snake/snake_internal.h"
#include "engine/core/game_input.h"
#include "engine/storage/highscore.h"
#include "engine/utils.h"

#define SNAKE_WIDTH 22
#define SNAKE_HEIGHT 20
#define SNAKE_MOVE_INTERVAL 0.12f
#define MAX_INPUT_BUFFER 5

static void snake_init(void *state) {
    snake_runtime *runtime = state;

    runtime->play_width = SNAKE_WIDTH;
    runtime->play_height = SNAKE_HEIGHT;
    runtime->move_interval = SNAKE_MOVE_INTERVAL;
    runtime->move_timer = 0.0f;
    runtime->currentHiScore = highscore_load("snake");

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
    runtime->snake.segments.points[1] = (Point){start_x, start_y};

    runtime->snake.alive = true;

    /* Start moving right, with the tail behind the head. */
    runtime->last_dir_input = (Point){0, -1};

    runtime->baseFruit.scoreAmt = 1;
    runtime->normalFruitSet.fruitType = &runtime->baseFruit;
    runtime->normalFruitSet.onEat = baseEat;

    spawnFruit(runtime->normalFruitSet.pointList, 0, runtime->snake.segments, runtime->play_width, runtime->play_height);
}

static ModuleAction snake_update(void *state, const GameInput *input, float delta_time) {
    snake_runtime *runtime = state;

    if (runtime == NULL || input == NULL ||
        runtime->snake.segments.points == NULL ||
        runtime->normalFruitSet.pointList.points == NULL)
        return MODULE_QUIT;

    if (input->back)
        return MODULE_FINISHED;

    if (runtime->last_dir_input.posX == 0 &&
        runtime->last_dir_input.posY == 0) {
        return MODULE_CONTINUE;
    }
    runtime->move_timer += delta_time;
    latch_movement_no_reversal(&runtime->last_dir_input, runtime->buffered_dir, input, &runtime->buffered_dir_amt);


    while (runtime->move_timer >= runtime->move_interval) {
        runtime->move_timer -= runtime->move_interval;

        consume_buffer_input(&runtime->last_dir_input, runtime->buffered_dir, &runtime->buffered_dir_amt);
        if (!move_entity_segments(
                runtime->snake.segments.points,
                &runtime->last_dir_input,
                runtime->play_width,
                runtime->play_height)) {
            runtime->snake.alive = false;
        }

        if (runtime->snake.alive) {
            runtime->snake.alive = check_seg_entity_life(
                &runtime->snake.segments,
                &runtime->last_dir_input,
                runtime->play_width,
                runtime->play_height
            );
        }

        if (!runtime->snake.alive) {
            handleDeath(
                &runtime->snake,
                &runtime->normalFruitSet.pointList,
                &runtime->last_dir_input,
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
        shift_entity_segments(&runtime->snake);
    }
    runtime->currentHiScore = hiScoreCounter(
        runtime->snake.segments.amt,
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
