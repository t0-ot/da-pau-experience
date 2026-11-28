/**
 * @file main.c
 * @brief Application entry point and engine initialization.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "engine/session.h"
#include "engine/core/engine_context.h"
#include "engine/renderer/renderer.h"
#include "engine/audio/audio_device.h"
#include "engine/input/input.h"
#include "engine/resources/resource_group_manager_internal.h"
#include "engine/assets/asset_manager_internal.h"
#include "engine/audio/audio_manager_internal.h"
#include "app/game_registry.h"

/**
 * @brief Application entry point.
 *
 * Initializes the random number generator, graphics window,
 * input subsystem, and game session. Runs the game module
 * lifecycle until the application exits.
 *
 * @return EXIT_SUCCESS if the application terminates normally,
 * otherwise EXIT_FAILURE if initialization fails.
 */
int main(void) {
    srand((unsigned int)time(NULL));
    bool initialization_failed = false;

    /* Initialize the graphics window. */
    if (!renderer_init(1280, 720, "Da Pau Launching Experience"))
        return EXIT_FAILURE;

    /* Create engine services. */
    ResourceGroupManager *groups = resource_group_manager_create();

    if (groups == NULL) {
        renderer_close();
        return EXIT_FAILURE;
    }

    if (!audio_device_init()) {
        resource_group_manager_destroy(groups);
        renderer_close();
        return EXIT_FAILURE;
    }
    /* Create the audio manager. */
    AudioManager *audio = audio_manager_create(groups);

    if (audio == NULL) {
        audio_device_close();
        resource_group_manager_destroy(groups);
        renderer_close();
        return EXIT_FAILURE;
    }

    /* Create the asset manager */
    AssetManager *assets = asset_manager_create(groups);

    if (assets == NULL) {
        audio_manager_destroy(audio);
        audio_device_close();
        resource_group_manager_destroy(groups);
        renderer_close();
        return EXIT_FAILURE;
    }

    /* Assemble the engine context. */
    EngineContext engine = {
        .assets = assets,
        .audio = audio,
        .groups = groups
    };

    /* Initialize centralized input handling. */
    input_init();

    /* Initialize the game session. */
    GameSession session;

    session_init(&session, &engine);

    /* Begin with the main menu. */
    const Game_Entry *next_game = game_registry_fetch_menu();

    while (session.is_running) {
        if (next_game == NULL) {
            fprintf(stderr, "[GAME_REGISTRY_ERROR] Failed to retrieve the next game module\n");
            initialization_failed = true;
            break;
        }

        SessionResult result = session_set_module(&session, next_game->module);

        if (result != SESSION_SUCCESS) {
            session_print_error(result);
            initialization_failed = true;
            session.is_running = false;
            break;
        }

        ModuleAction action = session_run(&session);

        if (action == MODULE_QUIT)
            break;

        next_game = game_registry_get_next(
            session.current_module,
            session.current_state,
            action
        );
    }

    /* Release session and graphics resources. */
    session_cleanup(&session);
    audio_manager_destroy(audio);
    asset_manager_destroy(assets);
    resource_group_manager_destroy(groups);

    audio_device_close();
    renderer_close();

    return initialization_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
