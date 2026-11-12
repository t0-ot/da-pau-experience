#include <stdlib.h>

#include "engine/session.h"
#include "engine/input/input.h"
#include "engine/renderer/renderer.h"

void session_init(GameSession *session) {
    if (session == NULL)
        return;

    session->current_module = NULL;
    session->current_state = NULL;
    session->is_running = true;
}

bool session_set_module(
    GameSession *session,
    const GameModule *module
) {
    if (session == NULL || module == NULL || module->state_size == 0)
        return false;

    void *new_state = calloc(1, module->state_size);

    if (new_state == NULL)
        return false;

    if (session->current_module != NULL) {
        if (session->current_module->cleanup != NULL)
            session->current_module->cleanup(session->current_state);

        free(session->current_state);
    }

    session->current_module = module;
    session->current_state = new_state;

    if (module->init != NULL)
        module->init(session->current_state);

    return true;
}

ModuleAction session_run(GameSession *session) {
    if (session == NULL || session->current_module == NULL ||
        session->current_state == NULL)
        return MODULE_QUIT;

    while (session->is_running && !renderer_should_close()) {
        GameInput input = input_poll();

        if (input.quit) {
            session->is_running = false;
            return MODULE_QUIT;
        }

        float delta_time = renderer_get_delta_time();

        ModuleAction action = session->current_module->update(
            session->current_state,
            &input,
            delta_time
        );

        if (action != MODULE_CONTINUE) {
            if (action == MODULE_QUIT)
                session->is_running = false;

            return action;
        }

        renderer_begin_frame();

        if (session->current_module->draw != NULL)
            session->current_module->draw(session->current_state);

        renderer_end_frame();
    }

    session->is_running = false;
    return MODULE_QUIT;
}

void session_cleanup(GameSession *session) {
    if (session == NULL)
        return;

    session->is_running = false;

    if (session->current_module != NULL &&
        session->current_module->cleanup != NULL) {
        session->current_module->cleanup(session->current_state);
    }

    free(session->current_state);

    session->current_state = NULL;
    session->current_module = NULL;
}
