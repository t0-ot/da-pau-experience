#include <stdlib.h>
#include <time.h>
#include "engine/session.h"
#include "core/game_registry.h"

int main(void) {
    srand(time(NULL));
    game_session session;
    session_init(&session);

    const Game_Entry *retrieved_game = game_registry_fetch_menu();

    while (session.is_running) {
        session_set_module(&session, retrieved_game->module, retrieved_game->runtime_size);
        module_action action = session_run(&session);
        if (action == MODULE_QUIT) {
            session.current_module->clean_up(session.current_runtime);
            break;
        }
        retrieved_game = game_registry_get_next(session.current_module, session.current_runtime, action);

        if (retrieved_game == NULL) {
            session.current_module->clean_up(session.current_runtime);
            break;
        }
    }
    return 0;
}
