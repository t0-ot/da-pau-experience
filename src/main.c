#include <stdlib.h>
#include <time.h>
#include "../include/menu/menu.h"
#include "../include/snake/snake.h"
#include "../include/engine/session.h"

// Declare the external module defined in snake.c
extern const game_module menu_module;
extern const game_module snake_module;

//Route lookup table for extensible game launching
typedef struct {
    menu_choice_type choice;
    const game_module *module;
    size_t runtime_size;
} Game_Entry;

static const Game_Entry GAME_ENTRIES[] = {
    { MENU_CHOICE_PLAY_SNAKE, &snake_module, sizeof(snake_runtime) },
};
#define NUM_GAMES (sizeof(GAME_ENTRIES) / sizeof(Game_Entry))

int main(void) {
    srand(time(NULL));
    game_session session;
    session_init(&session);

    session_set_module(&session, &menu_module, sizeof(menu_runtime));
    module_action action = session_run(&session);

    while (session.is_running) {
        if (action == MODULE_QUIT) {
            session.current_module->clean_up(session.current_runtime);
            session.is_running = 0;
            break;
        }

        if (action == MODULE_FINISHED) {
            if (session.current_module == &menu_module) {
                menu_choice_type selected_choice = menu_get_exit_choice(session.current_runtime);
                for (size_t i = 0; i < NUM_GAMES; i++) {
                    if (selected_choice == GAME_ENTRIES[i].choice)
                        session_set_module(&session, GAME_ENTRIES[i].module, GAME_ENTRIES[i].runtime_size);
                }
            } else
                session_set_module(&session, &menu_module, sizeof(menu_runtime));
            action = session_run(&session);
        }
    }

    return 0;
}
