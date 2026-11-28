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
typedef struct EngineContext EngineContext;

typedef enum ModuleResult : int {
    MODULE_INIT_SUCCESS,
    MODULE_INIT_ERROR_STATE_DATA,
    MODULE_INIT_ERROR_ALLOCATION,
    MODULE_INIT_ERROR_RESOURCE_LOAD,
    MODULE_INIT_ERROR_INITIALIZATION
} ModuleResult;

/**
 * @brief Describes the result of a game module's update operation.
 */
typedef enum ModuleAction : int {
    MODULE_CONTINUE, /**< Continue running the current module. */
    MODULE_QUIT,     /**< Request termination of the application. */
    MODULE_FINISHED  /**< Indicate that the current module has finished. */
} ModuleAction;

void module_print_init_error(ModuleResult result, const char *error_message);

/**
 * @brief Defines the lifecycle operations and state requirements of a game module.
 *
 * Each game module provides callbacks for initialization, updating,
 * rendering, and cleanup. The session manages the allocation of its state.
 */
typedef struct GameModule {
    /** @brief Size of the module's runtime state in bytes. */
    size_t state_size;

    /**
     * @brief Initializes the module's runtime state.
     * @param state Pointer to the allocated runtime state.
     */
    ModuleResult (*init)(void *state, EngineContext *engine);

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
        float delta_time,
        EngineContext *engine
    );

    /**
     * @brief Draws the current state of the module.
     * @param state Pointer to the module's runtime state.
     */
    void (*draw)(void *state, EngineContext *engine);

    /**
     * @brief Releases resources owned by the module's runtime state.
     * @param state Pointer to the module's runtime state.
     */
    void (*cleanup)(void *state, EngineContext *engine);

} GameModule;

#endif // GAME_MODULE_H
