#include <stdio.h>
#include "core/game_registry.h"
#include "games/snake/snake.h"

// Registry table mapping menu choice selections to their respective modules and runtime sizes
static const Game_Entry GAME_ENTRIES[] = {
    { MENU_CHOICE_PLAY_SNAKE, &snake_module, sizeof(snake_runtime) },
};

// Static definition for the root game launcher/menu module
static const Game_Entry MENU_LAUNCHER = {
    .choice = MENU_CHOICE_NONE,
    .module = &menu_module,
    .runtime_size = sizeof(menu_runtime)
};

#define NUM_GAMES (sizeof(GAME_ENTRIES) / sizeof(Game_Entry))

const Game_Entry *game_registry_get_next(const game_module *current_mod, void *runtime, module_action action) {
    // If the menu finished, inspect its choice and route accordingly
    if (current_mod == game_registry_fetch_menu()->module) {
        menu_choice_type choice = menu_get_exit_choice(runtime);
        const Game_Entry *retrieved_game = game_registry_get_entry(choice);

        if (retrieved_game == NULL) {
            if (choice == MENU_CHOICE_QUIT)
                return NULL; // Signal main.c to exit

            fprintf(stderr, "Error: Unrecognized game state %d. Returning to menu.\n", choice);
            return game_registry_fetch_menu();
        }
        return retrieved_game;
    }

    // If ANY other game finished, default back to the menu launcher
    return game_registry_fetch_menu();
}
// Searches the registry table for a game entry matching the given menu choice
const Game_Entry *game_registry_get_entry(menu_choice_type choice) {
    for (size_t i = 0; i < NUM_GAMES; i++) {
        if (GAME_ENTRIES[i].choice == choice)
            return &GAME_ENTRIES[i];
    }
    return NULL;
}
// Returns the default entry point for the application (the menu launcher)
const Game_Entry *game_registry_fetch_menu(void) {
    return &MENU_LAUNCHER;
}


