/**
 * @file session.h
 * @brief Public interface for managing the core game session loop and game module swapping.
 */

#ifndef SESSION_H
#define SESSION_H
#include <stddef.h>
#include "engine/canvas.h"

/**
 * @brief Actions that a game module can return to tell the session what to do next.
 */
typedef enum {
    MODULE_CONTINUE,
    MODULE_QUIT,
    MODULE_FINISHED
} module_action;

/**
 * @brief Agnostic game module structure defining the interface for a game state.
 */
typedef struct {
    int game_width;
    int game_height;
    size_t runtime_size;
    void (*init)(void *runtime_data);
    module_action (*upd)(void *runtime_data);
    void (*render)(void *runtime_data, CanvasBuffer *canvas);
    void (*clean_up)(void *runtime_data);
} game_module;

/**
 * @brief Master session container tracking the active game module and session state.
 */
typedef struct {
    const game_module *current_module;
    void *current_runtime;
    CanvasBuffer *canvas;
    int is_running;
} game_session;

/**
 * @brief Initializes the game session by zeroing out its memory.
 * @param session Pointer to the game_session struct to initialize.
 * @return void
 */

void session_init(game_session *session);

/**
 * @brief Swaps out the active module. Cleans up the old game if it's there,
 *        plugs in the new one, and runs its init function safely.
 * @param session Pointer to the active game session.
 * @param runtime Pointer to the new module's private data state.
 * @param size of runtime.
 * @return void
 */
void session_set_module(game_session *session, const game_module *module);

/**
 * @brief Starts main engine loop, running updates and rendering
 *        for the active module until it tells us to quit.
 * @param session Pointer to the active game session.
 * @return type module_action either MODULE_QUIT or MODULE_FINISHED
 */
module_action session_run(game_session *session);

/**
 * @brief Tears down the session safely, cleaning up whatever module is
 *        in use and nulling out pointers so nothing dangles.
 * @param session Pointer to the game session to clean up.
 * @return void
 */
void session_cleanup(game_session *session);

#endif // SESSION_H
