#ifndef SNAKE_INTERNAL_H
#define SNAKE_INTERNAL_H

#include "engine/entity.h"
#include "engine/animation/animation.h"
#include "engine/assets/asset_manager.h"

/**
 * @brief Stores the runtime state of a Snake game session.
 *
 * Contains the snake entity, fruit data, movement state,
 * score information, and play area dimensions.
 */

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

 typedef struct {
    ResourceGroupHandle assets;

    TextureHandle fruit_texture;
} snake_textures;

typedef struct snake_runtime {
    snake_textures texture;
    Segmented_entity snake; /**< The player's snake entity. */
    defaultFruit baseFruit; /**< Properties of the standard fruit. */
    fruitData normalFruitSet; /**< Gameplay data and positions of active fruits. */

    Point last_dir_input; /**< Current movement direction of the snake. */
    bool is_moving;

    int currentHiScore; /**< Highest score achieved during this session. */
    int play_width; /**< Width of the playable grid in cells. */
    int play_height; /**< Height of the playable grid in cells. */
    int pending_growth;

    float fruit_random_timer;

    float move_timer; /**< Time accumulated toward the next movement. */
    float move_interval; /**< Time required between successive movements. */

    animation_clip fruit_clip;
    animation_player fruit_player;

     Point buffered_dir[5]; /**< Direction queued for the next player movement. */
    int buffered_dir_amt; /**< Whether a direction is currently queued. */

} snake_runtime;

#endif // SNAKE_INTERNAL_H
