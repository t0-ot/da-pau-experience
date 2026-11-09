#include <stdio.h>
#include <stddef.h>

#include "core/game_registry.h"
#include "games/snake/snake.h"
#include "games/tron/tron.h"
#include "games/pacman/pacman.h"s

/**
 * @brief Registry table containing all playable game modules.
 */
static const Game_Entry GAME_ENTRIES[] = {
    { MENU_CHOICE_PLAY_SNAKE, &snake_module },
    { MENU_CHOICE_PLAY_TRON,  &tron_module },
    { MENU_CHOICE_PLAY_PACMAN, &pacman_module}
};

/**
 * @brief Root application entry point.
 */
static const Game_Entry MENU_LAUNCHER = {
    .choice = MENU_CHOICE_NONE,
    .module = &menu_module
};

#define NUM_GAMES (sizeof(GAME_ENTRIES) / sizeof(GAME_ENTRIES[0]))

const Game_Entry *game_registry_get_next(
    const GameModule *current_mod,
    void *state,
    ModuleAction action
) {
    (void)action;

    if (current_mod == &menu_module) {
        menu_choice_type choice = menu_get_exit_choice(state);

        const Game_Entry *next_game = game_registry_get_entry(choice);

        if (next_game == NULL) {
            if (choice == MENU_CHOICE_QUIT)
                return NULL;

            fprintf(
                stderr,
                "Error: Unrecognized game selection %d. Returning to menu.\n",
                choice
            );

            return game_registry_fetch_menu();
        }

        return next_game;
    }

    /* Completed games return to the menu. */
    return game_registry_fetch_menu();
}

const Game_Entry *game_registry_get_entry(menu_choice_type choice) {
    for (size_t i = 0; i < NUM_GAMES; i++) {
        if (GAME_ENTRIES[i].choice == choice)
            return &GAME_ENTRIES[i];
    }

    return NULL;
}

const Game_Entry *game_registry_fetch_menu(void) {
    return &MENU_LAUNCHER;
}
