#ifndef SNAKE_INTERNAL_H
#define SNAKE_INTERNAL_H

#include "engine/entity.h"

/**
 * @brief Stores the runtime state of a Snake game session.
 *
 * Contains the snake entity, fruit data, movement state,
 * score information, and play area dimensions.
 */
typedef struct snake_runtime {
    Segmented_entity snake; /**< The player's snake entity. */
    defaultFruit baseFruit; /**< Properties of the standard fruit. */
    fruitData normalFruitSet; /**< Gameplay data and positions of active fruits. */

    Point last_dir_input; /**< Current movement direction of the snake. */

    int currentHiScore; /**< Highest score achieved during this session. */
    int play_width; /**< Width of the playable grid in cells. */
    int play_height; /**< Height of the playable grid in cells. */

    float move_timer; /**< Time accumulated toward the next movement. */
    float move_interval; /**< Time required between successive movements. */

     Point buffered_dir[5]; /**< Direction queued for the next player movement. */
    int buffered_dir_amt; /**< Whether a direction is currently queued. */

} snake_runtime;

#endif // SNAKE_INTERNAL_H
