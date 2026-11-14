/**
 * @file game_registry.h
 * @brief Central registry and routing system for game modules.
 *
 * Maps menu selections to their corresponding game modules and
 * determines which module should execute after the current one.
 */

#ifndef GAME_REGISTRY_H
#define GAME_REGISTRY_H

#include "engine/core/game_module.h"
#include "games/menu/menu.h"

/**
 * @brief Represents a registered game module.
 */
typedef struct {
    menu_choice_type choice;  /**< Menu selection associated with this game. */
    const GameModule *module; /**< Game module associated with this entry. */
} Game_Entry;

/**
 * @brief Determines the next module following the current module's execution.
 *
 * If the menu has finished, its selected option determines the next module.
 * Other completed games return the application to the menu.
 *
 * @param current_mod Module that just finished executing.
 * @param state Runtime state belonging to the completed module.
 * @param action Action returned by the module.
 * @return Pointer to the next registry entry, or NULL to exit.
 */
const Game_Entry *game_registry_get_next(
    const GameModule *current_mod,
    void *state,
    ModuleAction action
);

/**
 * @brief Retrieves a registered game using its menu selection.
 *
 * @param choice Menu selection to look up.
 * @return Matching registry entry, or NULL if no match exists.
 */
const Game_Entry *game_registry_get_entry(menu_choice_type choice);

/**
 * @brief Retrieves the application's starting menu module.
 *
 * @return Pointer to the menu launcher entry.
 */
const Game_Entry *game_registry_fetch_menu(void);

#endif /* GAME_REGISTRY_H */
