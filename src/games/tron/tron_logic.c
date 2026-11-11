#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "games/tron/tron_logic.h"
#include "games/tron/tron_internal.h"
#include "engine/utils.h"

/**
 * @brief Represents a coordinate used by the flood-fill algorithm.
 *
 * Stores grid positions while tracking accessible territory.
 */
typedef struct {
    int x; /**< Horizontal grid coordinate. */
    int y; /**< Vertical grid coordinate. */
} FillPoint;

// Helper function to check if a specific point matches another
bool same_point(Point a, Point b) {
    return a.posX == b.posX && a.posY == b.posY;
}

// Check if a point hits any current light trails or player segments
bool point_is_obstacle(Point *obstacles, int count, Point point) {
    for (int i = 0; i < count; i++) {
        if (same_point(obstacles[i], point)) {
            return true;
        }
    }
    return false;
}

// Bounded Flood Fill algorithm to grade available open territory
int check_breathing_room(Point start, Point *obstacles, int obstacle_count, int play_width, int play_height, int max_cells) {
    // 1. Instantly fail if the starting tile is out-of-bounds or an immediate crash
    if (start.posX < 0 || start.posX >= play_width || start.posY < 0 || start.posY >= play_height) {
        return 0;
    }
    if (point_is_obstacle(obstacles, obstacle_count, start)) {
        return 0;
    }

    // Allocate tracking arrays safely on the stack using our max cell constraint
    FillPoint *stack = malloc(max_cells * sizeof(FillPoint));
    FillPoint *visited = malloc(max_cells * sizeof(FillPoint));
    if (stack == NULL || visited == NULL) {
        free(stack);
        free(visited);
        return 0;
    }

    int stack_count = 0;
    int visited_count = 0;

    // Push initial starting coordinate
    stack[stack_count++] = (FillPoint){start.posX, start.posY};
    visited[visited_count++] = (FillPoint){start.posX, start.posY};

    int space_count = 0;

    // Process nodes until the stack empties or we hit our target comfort threshold
    while (stack_count > 0 && space_count < max_cells) {
        // Pop the top point off the stack
        FillPoint curr = stack[--stack_count];
        space_count++;

        // Explore all 4 adjacent neighbors (North, South, East, West)
        int dx[] = {0, 0, 1, -1};
        int dy[] = {1, -1, 0, 0};

        for (int i = 0; i < 4; i++) {
            int nx = curr.x + dx[i];
            int ny = curr.y + dy[i];
            Point neighbor = {nx, ny};

            // Enforce arena boundaries
            if (nx < 0 || nx >= play_width || ny < 0 || ny >= play_height) {
                continue;
            }

            // Enforce trail obstacle avoidance
            if (point_is_obstacle(obstacles, obstacle_count, neighbor)) {
                continue;
            }

            // Verify if we already cataloged this grid cell
            bool already_visited = false;
            for (int v = 0; v < visited_count; v++) {
                if (visited[v].x == nx && visited[v].y == ny) {
                    already_visited = true;
                    break;
                }
            }

            // If empty, unvisited, and we have stack room left, queue it
            if (!already_visited && visited_count < max_cells && stack_count < max_cells) {
                visited[visited_count++] = (FillPoint){nx, ny};
                stack[stack_count++] = (FillPoint){nx, ny};
            }
        }
    }

    free(stack);
    free(visited);
    return space_count;
}


void enemy_lightcycle_ai(Enemy_list *view_enemies, Point_list *player, int distraction_modifier, int play_width,int play_height) {
    for (int i = 0; i < view_enemies->enemy_amt; i++) {
        if (!(view_enemies->enemy[i].alive))
            continue;

        // Extend and shift the enemy's trail.
        view_enemies->enemy[i].segments.amt++;
        objectPointResize(&view_enemies->enemy[i].segments);
        shift_entity_segments(&view_enemies->enemy[i]);

        // Capture starting position (the current head of the enemy cycle)
        Point enemy_start = view_enemies->enemy[i].segments.points[0];

        // =========================================
        // 1. BUILD THE OBSTACLE LIST
        // =========================================
        int obstacle_count = player->amt - 1;
        for (int k = 0; k < view_enemies->enemy_amt; k++) {
            if (!(view_enemies->enemy[k].alive))
                continue;
            if (k == i) {
                obstacle_count += view_enemies->enemy[k].segments.amt - 1;
            } else {
                obstacle_count += view_enemies->enemy[k].segments.amt;
            }
        }

        Point *obstacles = malloc(obstacle_count * sizeof(Point));
        if (obstacles == NULL && obstacle_count > 0) {
            fprintf(stderr, "Failed to allocate obstacles!\n");
            exit(EXIT_FAILURE);
        }

        int obstacle_index = 0;

        // Populate obstacles with the player's tail
        for (int j = 1; j < player->amt; j++) {
            obstacles[obstacle_index++] = player->points[j];
        }

        // Populate obstacles with enemy trails
        for (int k = 0; k < view_enemies->enemy_amt; k++) {
            if (!(view_enemies->enemy[k].alive))
                continue;
            int start = (k == i) ? 1 : 0;
            for (int j = start; j < view_enemies->enemy[k].segments.amt; j++) {
                obstacles[obstacle_index++] = view_enemies->enemy[k].segments.points[j];
            }
        }

        // =========================================
        // 2. RUN BREATHING ROOM CHECKS FOR ALL 4 MOVES
        // =========================================
        Point move_up    = {enemy_start.posX,     enemy_start.posY - 1};
        Point move_down  = {enemy_start.posX,     enemy_start.posY + 1};
        Point move_left  = {enemy_start.posX - 1, enemy_start.posY};
        Point move_right = {enemy_start.posX + 1, enemy_start.posY};

        // Grade each square by counting accessible tiles
        int up_score    = check_breathing_room(move_up,    obstacles, obstacle_count, play_width, play_height, 35);
        int down_score  = check_breathing_room(move_down,  obstacles, obstacle_count, play_width, play_height, 35);
        int left_score  = check_breathing_room(move_left,  obstacles, obstacle_count, play_width, play_height, 35);
        int right_score = check_breathing_room(move_right, obstacles, obstacle_count, play_width, play_height, 35);

        // Get the player's head position to calculate distance
        Point player_head = player->points[0];

        // Group options into an array so we can loop through them easily
        Point options[] = {move_up, move_down, move_left, move_right};
        int scores[]    = {up_score, down_score, left_score, right_score};

        int best_score = -1;
        int best_dist  = 999999;
        Point best_move = enemy_start;

        /* Introduce a 15% chance per frame that the AI gets distracted
         and just picks ANY safe space instead of perfectly copying you.*/
        bool is_distracted = (rand() % 100) < distraction_modifier;

        for (int j = 0; j < 4; j++) {
            // If this move instantly crashes, skip it completely
            if (scores[j] == 0) continue;

            int current_dist = calc_point_dist(&options[j], &player_head);

            if (is_distracted) {
                // If distracted, survival is still priority #1,
                // but we randomize the tie-breaker instead of targeting the player.
                if (scores[j] > best_score || (scores[j] == best_score && (rand() % 2 == 0))) {
                    best_score = scores[j];
                    best_move  = options[j];
                }
            } else {
                // Standard Smart/Aggressive AI behavior
                if (scores[j] > best_score) {
                    best_score = scores[j];
                    best_dist  = current_dist;
                    best_move  = options[j];
                }
                else if (scores[j] == best_score) {
                    if (current_dist < best_dist) {
                        best_dist  = current_dist;
                        best_move  = options[j];
                    }
                }
            }
        }

        // =========================================
        // 3. EXECUTE MOVEMENT
        // =========================================
        // Apply our best calculated position directly to the cycle's head if the best move is the same coordinate as the current position it means they are dead
        if (best_move.posX == view_enemies->enemy[i].segments.points[0].posX &&
            best_move.posY == view_enemies->enemy[i].segments.points[0].posY) {

            view_enemies->enemy[i].alive = false;
        }
        view_enemies->enemy[i].segments.points[0] = best_move;

        // Housekeeping
        free(obstacles);


    }
}

static void start_level_intro(tron_runtime *runtime) {
    runtime->level_starting = 1;
    runtime->intro_timer = 3.0f;
}

void handle_player_death(tron_runtime *runtime, int play_width) {
        runtime->enemy_distracted = 75;
        runtime->enemy_move_timer = 0.0f,
        runtime->enemy_move_interval = 0.20f,
        runtime->level = 1,
        start_level_intro(runtime);

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

    for (int i = 0; i < 1 + runtime->toggle_turbo; i++) {
        if (runtime->last_dir_input.posX == 0 &&
            runtime->last_dir_input.posY == 0)
            break;

        runtime->player.segments.amt++;
        objectPointResize(&runtime->player.segments);

        shift_entity_segments(&runtime->player);

        move_entity_segments(runtime->player.segments.points, &runtime->last_dir_input, play_width, play_height);

        runtime->player.alive = check_seg_entity_life(&runtime->player.segments, &runtime->last_dir_input, play_width, play_height);

        for (int j = 0; j < runtime->view_enemies.enemy_amt; j++) {
            int collision = collision_check_both(&runtime->view_enemies.enemy[j].segments, &runtime->player.segments, 0);

            if (collision > 0)
                runtime->player.alive = 0;
        }

        if (!runtime->player.alive) {
            handle_player_death(runtime, play_width);
            break;
        }
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

int all_enemies_dead(tron_runtime *runtime) {
    for (int i = 0; i < runtime->view_enemies.enemy_amt; i++) {
        if (runtime->view_enemies.enemy[i].alive)
            return 0;
    }
    return 1;
}

const char *get_level_name(int level) {
    if (level < 1 || level > LEVEL_COUNT)
        return "UNKNOWN";

    return level_names[level - 1];
}
