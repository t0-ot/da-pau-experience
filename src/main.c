#include <stdlib.h>
#include <time.h>
#include "engine/session.h"
#include "core/game_registry.h"

int main(void) {
    srand(time(NULL));
    game_session session;
    session_init(&session);

    const Game_Entry *retrieved_game = game_registry_fetch_menu();

    while (session.is_running && retrieved_game != NULL) {
        // session_set_module automatically cleans up the PREVIOUS module & canvas!
        session_set_module(&session, retrieved_game->module);

        module_action action = session_run(&session);

        if (action == MODULE_QUIT)
            break;

        retrieved_game = game_registry_get_next(session.current_module, session.current_runtime, action);
    }

    session_cleanup(&session);

    return 0;
}
