/**
 * @file session.c
 * @brief Implementation of the game engine's session state machine and loop.
 */

#include <string.h>
#include <stdlib.h>
#include "../../include/engine/session.h"
#include "../../include/engine/guard.h"

void session_init(game_session *session) {
    REQUIRE_NOT_NULLED(session);
    memset(session, 0, sizeof(game_session));
}

void session_set_module(game_session *session, const game_module *module, size_t runtime_size) {
    REQUIRE_NOT_NULLED(session);
    REQUIRE_NOT_NULLED(module);

    //If there was a game before we clean it up first.
    if (session->current_module != NULL && session->current_module->clean_up != NULL) {
        session->current_module->clean_up(session->current_runtime);
        session->current_runtime = NULL;
    }

    void *runtime_data = malloc(runtime_size);
    REQUIRE_NOT_NULLED(runtime_data);


    //Update with the new game
    session->current_module = module;
    session->current_runtime = runtime_data;

    //check if current_module intialization function isn't NULL before running intialization to avoid seg fault
    if (session->current_module->init != NULL)
        session->current_module->init(session->current_runtime);
}

module_action session_run(game_session *session) {
    REQUIRE_NOT_NULLED(session);
    REQUIRE_NOT_NULLED(session->current_module);
    session->is_running = 1;

    while (session->is_running) {

        module_action action = session->current_module->upd(session->current_runtime);

        if (action != MODULE_CONTINUE)
            return action;

        session->current_module->render(session->current_runtime);
    }
    return MODULE_QUIT;
}

void session_cleanup(game_session *session) {
    if (session == NULL)
        return;

    if (session->current_module != NULL) {
        if (session->current_module->clean_up != NULL && session->current_runtime != NULL)
            session->current_module->clean_up(session->current_runtime);

        session->current_module = NULL;
        session->current_runtime = NULL;
    }
}
