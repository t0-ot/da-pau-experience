/**
 * @file main.c
 * @brief Application entry point and engine initialization.
 */

#include <stdlib.h>
#include <time.h>

#include "engine/session.h"
#include "engine/renderer/renderer.h"
#include "engine/audio/audio_device.h"
#include "engine/input/input.h"
#include "engine/resources/resource_group_manager.h"
#include "engine/assets/asset_manager.h"
#include "engine/audio/audio_manager.h"
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

    while (session.is_running && next_game != NULL) {

        if (!session_set_module(&session, next_game->module)) {
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

    return EXIT_SUCCESS;
}
