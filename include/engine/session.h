#ifndef SESSION_H
#define SESSION_H

#include <stdbool.h>

#include "engine/core/game_module.h"

/**
 * @brief Holds the state of the currently active game session.
 *
 * The session manages the active game module, its allocated runtime
 * state, and whether the application should continue running.
 */
typedef struct {
    const GameModule *current_module; /**< Currently active game module. */
    void *current_state;              /**< Runtime state allocated for the active module. */
    bool is_running;                  /**< Whether the session is still running. */
} GameSession;

/**
 * @brief Initializes a game session.
 *
 * @param session Pointer to the session to initialize.
 */
void session_init(GameSession *session);

/**
 * @brief Switches the active game module.
 *
 * Allocates runtime state for the new module and releases the previous
 * module's resources and allocated state.
 *
 * @param session Pointer to the active session.
 * @param module Game module to activate.
 * @return true if the module was successfully activated, false otherwise.
 */
bool session_set_module(
    GameSession *session,
    const GameModule *module
);

/**
 * @brief Runs the active game module's update and drawing loop.
 *
 * @param session Pointer to the active session.
 * @return The action returned by the active module or the session.
 */
ModuleAction session_run(GameSession *session);

/**
 * @brief Releases the active module's resources and session state.
 *
 * @param session Pointer to the session to clean up.
 */
void session_cleanup(GameSession *session);

#endif // SESSION_H
