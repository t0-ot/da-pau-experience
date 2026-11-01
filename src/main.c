#include <stdlib.h>
#include <time.h>
#include "../include/menu/menu.h"
#include "../include/snake/snake.h"
#include "../include/engine/session.h"

// Declare the external module defined in snake.c
extern const game_module menu_module;
extern const game_module snake_module;

int main(void) {
    srand(time(NULL));
    game_session session;
    session_init(&session);

    menu_runtime *menu_data = malloc(sizeof(menu_runtime));

    session_set_module(&session, &menu_module, menu_data);
    module_action action = session_run(&session);

    while (session.is_running) {
        if (action == MODULE_QUIT) {
            session.current_module->clean_up(session.current_runtime);
            session.is_running = 0;
            break;
        } else if (action == MODULE_FINISHED) {
            if (session.current_module != &menu_module) {
                menu_data = malloc(sizeof(menu_runtime));
                session_set_module(&session, &menu_module, menu_data);
                action = session_run(&session);
            } else {
                menu_choice_type selected_choice = menu_get_exit_choice(menu_data);
                if (selected_choice == MENU_CHOICE_PLAY_SNAKE) {
                    snake_runtime *snake_data = malloc(sizeof(snake_runtime));
                    session_set_module(&session, &snake_module, snake_data);
                    action = session_run(&session);
                }
            }
        }
    }

    return 0;
}
