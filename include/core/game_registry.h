/**
 * @file game_registry.h
 * @brief Centralized registry and routing mechanism for game modules.
 *
 * Acts as the single source of truth for mapping menu choices to game modules
 * and determining the next module transition state for the engine core.
 */

#ifndef GAME_REGISTRY_H
#define GAME_REGISTRY_H

#include "engine/session.h"
#include "games/menu/menu.h"

/**
 * @brief Represents a registered game mapping entry.
 */
typedef struct {
    menu_choice_type choice;
    const game_module *module;
    size_t runtime_size;
} Game_Entry;

/**
 * @brief Determines the next module to run based on the current module and its exit action.
 *
 * @param current_mod Pointer to the module that just finished executing.
 * @param runtime Pointer to the current module's active runtime data.
 * @param action The completion action returned by the session run.
 * @return const Game_Entry* Pointer to the next game entry, or NULL to signal a complete exit.
 */
const Game_Entry *game_registry_get_next(const game_module *current_mod, void *runtime, module_action action);

/**
 * @brief Retrieves a game entry corresponding to a specific menu choice.
 *
 * @param choice The menu choice enum value.
 * @return const Game_Entry* Pointer to the matching game entry, or NULL if not found.
 */
const Game_Entry *game_registry_get_entry(menu_choice_type choice);

/**
 * @brief Fetches the root application launcher/menu entry point.
 *
 * @return const Game_Entry* Pointer to the menu launcher entry.
 */
const Game_Entry *game_registry_fetch_menu(void);

#endif // GAME_REGISTRY_H
