/**
 * @file game_module.h
 * @brief Declares the game module interface.
 *
 * Defines the common module structure and lifecycle callbacks
 * used by the application to manage individual games.
 */

#ifndef GAME_MODULE_H
#define GAME_MODULE_H

#include <stddef.h>
#include "game_input.h"

/**
 * @brief Describes the result of a game module's update operation.
 */
typedef enum {
    MODULE_CONTINUE, /**< Continue running the current module. */
    MODULE_QUIT,     /**< Request termination of the application. */
    MODULE_FINISHED  /**< Indicate that the current module has finished. */
} ModuleAction;

/**
 * @brief Defines the lifecycle operations and state requirements of a game module.
 *
 * Each game module provides callbacks for initialization, updating,
 * rendering, and cleanup. The session manages the allocation of its state.
 */
typedef struct {
    /** @brief Size of the module's runtime state in bytes. */
    size_t state_size;

    /**
     * @brief Initializes the module's runtime state.
     * @param state Pointer to the allocated runtime state.
     */
    void (*init)(void *state);

    /**
     * @brief Updates the module's game logic.
     * @param state Pointer to the module's runtime state.
     * @param input Current input snapshot.
     * @param delta_time Time elapsed since the previous frame, in seconds.
     * @return The action indicating whether the module should continue,
     * finish, or quit the application.
     */
    ModuleAction (*update)(
        void *state,
        const GameInput *input,
        float delta_time
    );

    /**
     * @brief Draws the current state of the module.
     * @param state Pointer to the module's runtime state.
     */
    void (*draw)(void *state);

    /**
     * @brief Releases resources owned by the module's runtime state.
     * @param state Pointer to the module's runtime state.
     */
    void (*cleanup)(void *state);

} GameModule;

#endif // GAME_MODULE_H
