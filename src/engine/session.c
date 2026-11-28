#include <stdlib.h>
#include <stdio.h>

#include "engine/session.h"
#include "engine/input/input.h"
#include "engine/core/engine_context.h"
#include "engine/core/game_module.h"
#include "engine/renderer/renderer.h"
#include "audio/audio_manager_internal.h"

static const char *session_result_to_string(const SessionResult result) {
    switch (result) {
        case SESSION_SUCCESS:
            return "Session intialized Game Module successful";

        case SESSION_ERROR_INVALID_SESSION:
            return "Invalid Session passed";

        case SESSION_ERROR_INVALID_MODULE:
            return "Invalid module passed into session";

        case SESSION_ERROR_INVALID_MODULE_STATE:
            return "Module state has an invalid size";

        case SESSION_ERROR_MODULE_ALLOCATION:
            return "Session failed to allocate memory to incoming Game Module";

        case SESSION_ERROR_MODULE_INITIALIZATION:
            return "Module failed to initialize itself";

        default:
            return "Unknown Session error";
    }
}

void session_print_error(SessionResult result) {
    if (result == SESSION_SUCCESS)
        return;

    fprintf(stderr, "[SESSION_ERROR] %s\n",
        session_result_to_string(result));
}

void session_init(GameSession *session, EngineContext *engine) {
    if (session == NULL)
        return;

    session->current_module = NULL;
    session->current_state = NULL;
    session->engine = engine;
    session->is_running = true;
}

SessionResult session_set_module(GameSession *session, const GameModule *module) {
    if (session == NULL)
        return SESSION_ERROR_INVALID_SESSION;

    if (module == NULL || module->init == NULL || module->update == NULL)
        return SESSION_ERROR_INVALID_MODULE;

    if (module->state_size == 0)
        return SESSION_ERROR_INVALID_MODULE_STATE;

    void *new_state = calloc(1, module->state_size);

    if (new_state == NULL)
        return SESSION_ERROR_MODULE_ALLOCATION;

    ModuleResult result = module->init(new_state, session->engine);

    if (result != MODULE_INIT_SUCCESS) {
        module_print_init_error(result, "Incoming module activation failed");

        if (module->cleanup != NULL)
            module->cleanup(new_state, session->engine);

        free(new_state);
        return SESSION_ERROR_MODULE_INITIALIZATION;
    }

    if (session->current_module != NULL) {
        if (session->current_module->cleanup != NULL)
            session->current_module->cleanup(session->current_state, session->engine);

        free(session->current_state);
    }

    session->current_module = module;
    session->current_state = new_state;

    return SESSION_SUCCESS;
}

ModuleAction session_run(GameSession *session) {
    if (session == NULL || session->current_module == NULL ||
        session->current_state == NULL) {
        fprintf(stderr, "[FATAL ERROR] The game session or the current Game Module or its associated game state is NULL");
        return MODULE_QUIT;
    }

    while (session->is_running && !renderer_should_close()) {
        GameInput input = input_poll();

        if (input.quit) {
            session->is_running = false;
            return MODULE_QUIT;
        }

        float delta_time = renderer_get_delta_time();

        /* Update engine audio streaming. */
        audio_manager_update(session->engine->audio);

        ModuleAction action = session->current_module->update(
            session->current_state,
            &input,
            delta_time,
            session->engine
        );

        if (action != MODULE_CONTINUE) {
            if (action == MODULE_QUIT)
                session->is_running = false;

            return action;
        }

        renderer_begin_frame();

        if (session->current_module->draw != NULL)
            session->current_module->draw(session->current_state, session->engine);

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
        session->current_module->cleanup(session->current_state, session->engine);
    }

    free(session->current_state);

    session->current_state = NULL;
    session->current_module = NULL;
}
