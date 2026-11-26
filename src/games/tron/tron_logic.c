#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include "games/tron/tron_logic.h"
#include "games/tron/tron_internal.h"
#include "engine/utils.h"

const char *level_names[] = {
    "CMSC-10",
    "CMSC-11",
    "CMSC-56",
    "KAS-1",
    "MATH-18",
    "PE-1",
    "SCI-10",
    "NSTP-1"
};

/**
 * @brief Represents a coordinate used by the flood-fill algorithm.
 *
 * Stores grid positions while tracking accessible territory.
 */
/* -------------------------------------------------- */
/* Enemy AI helpers                                   */
/* -------------------------------------------------- */

static bool same_point(Point a, Point b) {
    return a.posX == b.posX && a.posY == b.posY;
}

static bool point_is_obstacle(
    const Point *obstacles,
    size_t obstacle_count,
    Point point
) {
    for (size_t i = 0; i < obstacle_count; i++) {
        if (same_point(obstacles[i], point)) {
            return true;
        }
    }

    return false;
}

static bool point_was_visited(
    const Point *queue,
    size_t queue_count,
    int x,
    int y
) {
    for (size_t i = 0; i < queue_count; i++) {
        if (queue[i].posX == x && queue[i].posY == y) {
            return true;
        }
    }

    return false;
}

/**
 * @brief Counts the reachable free cells around a candidate move.
 *
 * Uses a bounded breadth-first search. The search stops after
 * max_cells have been explored, since additional space does not
 * improve the AI's evaluation.
 */
static int check_breathing_room(
    Point start,
    const Point *obstacles,
    size_t obstacle_count,
    int play_width,
    int play_height,
    int max_cells
) {
    if (max_cells <= 0 || play_width <= 0 || play_height <= 0) {
        return 0;
    }

    if (start.posX < 0 || start.posX >= play_width ||
        start.posY < 0 || start.posY >= play_height) {
        return 0;
    }

    if (obstacle_count > 0 && obstacles == NULL) {
        return 0;
    }

    if (point_is_obstacle(obstacles, obstacle_count, start)) {
        return 0;
    }

    const size_t capacity = (size_t)max_cells;

    if (capacity > SIZE_MAX / sizeof(Point)) {
        return 0;
    }

    /*
     * The queue also acts as the visited list.
     * A cell is recorded when it is enqueued, so it cannot
     * be added more than once.
     */
    Point *queue = malloc(capacity * sizeof(*queue));

    if (queue == NULL) {
        return 0;
    }

    static const int directions[4][2] = {
        { 0, -1},  // North
        { 0,  1},  // South
        { 1,  0},  // East
        {-1,  0}   // West
    };

    size_t queue_count = 1;
    size_t next_point = 0;

    queue[0] = (Point){start.posX, start.posY};

    while (next_point < queue_count && next_point < capacity) {
        Point current = queue[next_point++];

        for (int i = 0; i < 4; i++) {
            int next_x = current.posX + directions[i][0];
            int next_y = current.posY + directions[i][1];

            // Reject positions outside the arena.
            if (next_x < 0 || next_x >= play_width ||
                next_y < 0 || next_y >= play_height) {
                continue;
            }

            Point neighbor = {next_x, next_y};

            // Reject occupied positions.
            if (point_is_obstacle(obstacles, obstacle_count, neighbor)) {
                continue;
            }

            // Reject positions already discovered.
            if (point_was_visited(queue, queue_count, next_x, next_y)) {
                continue;
            }

            // Stop adding cells once the search reaches its limit.
            if (queue_count >= capacity) {
                continue;
            }

            queue[queue_count++] = (Point){next_x, next_y};
        }
    }

    free(queue);

    return (int)next_point;
}

/**
 * @brief Validates a circular entity segment buffer.
 */
static bool valid_segment_buffer(
    int amount,
    int capacity,
    int head,
    int tail,
    const Point *points
) {
    return amount > 0 &&
           capacity > 0 &&
           amount <= capacity &&
           head >= 0 && head < capacity &&
           tail >= 0 && tail < capacity &&
           points != NULL;
}

/**
 * @brief Adds an amount to an obstacle count without overflowing.
 */
static bool add_obstacle_count(size_t *total, size_t amount) {
    if (amount > SIZE_MAX - *total) {
        return false;
    }

    *total += amount;
    return true;
}

/**
 * @brief Copies segments from a circular buffer into the obstacle list.
 *
 * Segments are copied starting at the tail, preserving the existing
 * ring-buffer traversal order.
 */
static void append_obstacle_segments(
    Point *obstacles,
    size_t *obstacle_index,
    const Point *points,
    size_t capacity,
    size_t tail,
    size_t amount
) {
    for (size_t i = 0; i < amount; i++) {
        size_t point_index = (tail + i) % capacity;

        obstacles[(*obstacle_index)++] = points[point_index];
    }
}

/**
 * @brief Terminates the game on invalid internal AI state.
 */
static void enemy_ai_fatal_error(const char *message) {
    fprintf(stderr, "Tron enemy AI error: %s\n", message);
    exit(EXIT_FAILURE);
}

/**
 * Enemy AI strategy:
 *
 * 1. Collect occupied cells from the player and other enemies.
 * 2. Evaluate each possible movement direction.
 * 3. Use flood fill to estimate available space.
 * 4. Prefer the direction with the most available space.
 * 5. Break ties using player distance or randomness when distracted.
 * 6. If no safe direction exists, mark the enemy as dead.
 */

void enemy_lightcycle_ai(
    Enemy_list *view_enemies,
    Segmented_entity *player,
    int distraction_modifier,
    int play_width,
    int play_height
) {
    if (view_enemies == NULL || player == NULL) {
        return;
    }

    if (view_enemies->enemy_amt <= 0 || view_enemies->enemy == NULL) {
        return;
    }

    if (!valid_segment_buffer(
            player->segments.amt,
            player->capacity,
            player->head,
            player->tail,
            player->segments.points)) {
        enemy_ai_fatal_error("Invalid player segment buffer");
    }

    for (int i = 0; i < view_enemies->enemy_amt; i++) {
        if (!view_enemies->enemy[i].alive) {
            continue;
        }

        /*
         * 1. PREPARE THE ENEMY
         *
         * Grow the trail before evaluating movement, preserving
         * the original behavior.
         */
        if (!valid_segment_buffer(
                view_enemies->enemy[i].segments.amt,
                view_enemies->enemy[i].capacity,
                view_enemies->enemy[i].head,
                view_enemies->enemy[i].tail,
                view_enemies->enemy[i].segments.points)) {
            enemy_ai_fatal_error("Invalid enemy segment buffer");
        }

        grow_entity(&view_enemies->enemy[i]);

        if (!valid_segment_buffer(
                view_enemies->enemy[i].segments.amt,
                view_enemies->enemy[i].capacity,
                view_enemies->enemy[i].head,
                view_enemies->enemy[i].tail,
                view_enemies->enemy[i].segments.points)) {
            enemy_ai_fatal_error("Invalid enemy buffer after growth");
        }

        Point enemy_start =
            view_enemies->enemy[i].segments.points[
                view_enemies->enemy[i].head
            ];

        /*
         * 2. COUNT ALL OBSTACLES
         *
         * Include:
         * - Every player segment.
         * - Every living enemy's segments.
         * - Exclude this enemy's own head.
         */
        size_t obstacle_count = 0;

        if (!add_obstacle_count(
                &obstacle_count,
                (size_t)player->segments.amt)) {
            enemy_ai_fatal_error("Obstacle count overflow");
        }

        for (int k = 0; k < view_enemies->enemy_amt; k++) {
            if (!view_enemies->enemy[k].alive) {
                continue;
            }

            if (!valid_segment_buffer(
                    view_enemies->enemy[k].segments.amt,
                    view_enemies->enemy[k].capacity,
                    view_enemies->enemy[k].head,
                    view_enemies->enemy[k].tail,
                    view_enemies->enemy[k].segments.points)) {
                enemy_ai_fatal_error("Invalid obstacle segment buffer");
            }

            size_t segment_count =
                (size_t)view_enemies->enemy[k].segments.amt;

            // The current enemy must not collide with its own head.
            if (k == i) {
                segment_count--;
            }

            if (!add_obstacle_count(&obstacle_count, segment_count)) {
                enemy_ai_fatal_error("Obstacle count overflow");
            }
        }

        /*
         * 3. BUILD THE OBSTACLE LIST
         */
        if (obstacle_count == 0) {
            enemy_ai_fatal_error("No obstacles to evaluate");
        }

        if (obstacle_count > SIZE_MAX / sizeof(Point)) {
            enemy_ai_fatal_error("Obstacle allocation size overflow");
        }

        Point *obstacles = malloc(obstacle_count * sizeof(*obstacles));

        if (obstacles == NULL) {
            enemy_ai_fatal_error("Failed to allocate obstacles");
        }

        size_t obstacle_index = 0;

        // Add the player's entire trail.
        append_obstacle_segments(
            obstacles,
            &obstacle_index,
            player->segments.points,
            (size_t)player->capacity,
            (size_t)player->tail,
            (size_t)player->segments.amt
        );

        // Add every living enemy's trail.
        for (int k = 0; k < view_enemies->enemy_amt; k++) {
            if (!view_enemies->enemy[k].alive) {
                continue;
            }

            size_t segment_count =
                (size_t)view_enemies->enemy[k].segments.amt;

            // Exclude this enemy's current head.
            if (k == i) {
                segment_count--;
            }

            append_obstacle_segments(
                obstacles,
                &obstacle_index,
                view_enemies->enemy[k].segments.points,
                (size_t)view_enemies->enemy[k].capacity,
                (size_t)view_enemies->enemy[k].tail,
                segment_count
            );
        }

        // The count and population passes must agree.
        if (obstacle_index != obstacle_count) {
            free(obstacles);
            enemy_ai_fatal_error("Obstacle count does not match populated list");
        }

        /*
         * 4. GENERATE POSSIBLE MOVES
         */
        Point possible_moves[4] = {
            {enemy_start.posX,     enemy_start.posY - 1},
            {enemy_start.posX,     enemy_start.posY + 1},
            {enemy_start.posX - 1, enemy_start.posY},
            {enemy_start.posX + 1, enemy_start.posY}
        };

        int move_scores[4];

        for (int j = 0; j < 4; j++) {
            move_scores[j] = check_breathing_room(
                possible_moves[j],
                obstacles,
                obstacle_count,
                play_width,
                play_height,
                35
            );
        }

        /*
         * 5. EVALUATE POSSIBLE MOVES
         */
        Point player_head =
            player->segments.points[player->head];

        bool is_distracted =
            (rand() % 100) < distraction_modifier;

        bool found_safe_move = false;

        int best_score = -1;
        int best_distance = INT_MAX;

        Point best_move = enemy_start;

        for (int j = 0; j < 4; j++) {
            // A score of zero means the move immediately crashes.
            if (move_scores[j] == 0) {
                continue;
            }

            int current_distance = 0;

            if (!is_distracted)
                current_distance = calc_point_dist(&possible_moves[j], &player_head);

            // Always accept the first safe move.
            if (!found_safe_move) {
                found_safe_move = true;
                best_score = move_scores[j];
                best_distance = current_distance;
                best_move = possible_moves[j];
                continue;
            }

            // Territory is always the primary priority.
            if (move_scores[j] > best_score) {
                best_score = move_scores[j];
                best_distance = current_distance;
                best_move = possible_moves[j];
                continue;
            }

            // Ignore moves with less available territory.
            if (move_scores[j] < best_score) {
                continue;
            }

            // Equal scores: distracted enemies choose randomly.
            if (is_distracted) {
                if (rand() % 2 == 0) {
                    best_move = possible_moves[j];
                }

                continue;
            }

            // Equal scores: normal enemies move toward the player.
            if (current_distance < best_distance) {
                best_distance = current_distance;
                best_move = possible_moves[j];
            }
        }

        /*
         * 6. EXECUTE THE SELECTED MOVE
         */
        if (!found_safe_move) {
            view_enemies->enemy[i].alive = false;
            best_move = enemy_start;
        }

        // Advance the circular buffer's head.
        view_enemies->enemy[i].head =
            (view_enemies->enemy[i].head + 1) %
            view_enemies->enemy[i].capacity;

        view_enemies->enemy[i].segments.points[
            view_enemies->enemy[i].head
        ] = best_move;

        free(obstacles);
    }
}


static void start_level_intro(tron_runtime *runtime) {
    runtime->level_starting = 1;
    runtime->intro_timer = 3.0f;
}

/**
 * @brief Resets the player and enemy state after player death.
 * @param runtime Pointer to the Tron runtime state.
 * @param play_width Width of the playable area in grid cells.
 */
static void handle_player_death(tron_runtime *runtime, int play_width) {
        runtime->enemy_distracted = 75;
        runtime->enemy_move_timer = 0.0f,
        runtime->enemy_move_interval = 0.20f,
        runtime->level = 1,
        start_level_intro(runtime);

        runtime->toggle_turbo = 0;
        runtime->last_dir_input.posX = 0;
        runtime->last_dir_input.posY = -1;
        reset_entity(&runtime->player, 1, play_width / 2, 25);

        for (int i = 0; i < runtime->view_enemies.enemy_amt; i++) {
            runtime->view_enemies.enemy[i].alive = 1;
            reset_entity(&runtime->view_enemies.enemy[i], 1, enemy_start_x[i], enemy_start_y[i]);
        }
        runtime->view_enemies.enemy_amt = 1;
}

void update_tron_player(
    tron_runtime *runtime,
    int dx,
    int dy,
    int play_width,
    int play_height
) {
    if ((dx != 0 || dy != 0) &&
        !(dx == -runtime->last_dir_input.posX &&
          dy == -runtime->last_dir_input.posY)) {
        runtime->last_dir_input.posX = dx;
        runtime->last_dir_input.posY = dy;
    }

    move_entity_segments(&runtime->player, &runtime->last_dir_input, play_width, play_height);
    grow_entity(&runtime->player);
    runtime->player.alive = check_seg_entity_life(&runtime->player, &runtime->last_dir_input, play_width, play_height);

    for (int j = 0; j < runtime->view_enemies.enemy_amt; j++) {
        //TODO FIX ENEMY COLLISION CHECK WITH PROPER RING BUFFER COLLISION CHECK
        int collision = collision_check_both(&runtime->view_enemies.enemy[j].segments, &runtime->player.segments, 0);

        if (collision > 0)
            runtime->player.alive = 0;
    }

    if (!runtime->player.alive) {
        handle_player_death(runtime, play_width);
        runtime->attempt_number++;
    }
}

void advance_tron_level(tron_runtime *runtime, int play_width) {
    runtime->view_enemies.enemy_amt = (runtime->level % 3) +1;
    runtime->level++;
    start_level_intro(runtime);
    runtime->last_dir_input.posX = 0;
    runtime->last_dir_input.posY = -1;
    reset_entity(&runtime->player, 1, play_width / 2, 25);
    for (int j = 0; j < runtime->view_enemies.enemy_amt; j++) {
        reset_entity(&runtime->view_enemies.enemy[j], 1, enemy_start_x[j], enemy_start_y[j]);
    }
    int distract_deduct = 0;
    if (runtime->level < 6) {
        distract_deduct = 2;
        runtime->enemy_move_interval = 0.20f;
    } else if (runtime->level < 11) {
        distract_deduct = 3;
        runtime->enemy_move_interval = 0.15f;
    } else {
        distract_deduct = 5;
        runtime->enemy_move_interval = 0.10f;
    }
    runtime->enemy_distracted = (runtime->enemy_distracted-distract_deduct == 0) ? 0 : runtime->enemy_distracted-distract_deduct;
}

bool all_enemies_dead(const tron_runtime *runtime) {
    for (int i = 0; i < runtime->view_enemies.enemy_amt; i++) {
        if (runtime->view_enemies.enemy[i].alive)
            return false;
    }
    return true;
}

const char *get_level_name(int level) {
    if (level < 1 || level > (int)LEVEL_COUNT)
        return "UNKNOWN";

    return level_names[level - 1];
}
