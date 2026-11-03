/**
 * @file menu.h
 * @brief Public interface and runtime definitions for the main menu module.
 */

#ifndef MENU_H
#define MENU_H

#include "engine/session.h"
#include "engine/canvas.h"

/**
 * @brief The public module interface for the main menu state.
 */
extern const game_module menu_module;

/**
 * @brief The choice flags. Order is important for the games
 */
typedef enum {
    MENU_CHOICE_PLAY_SNAKE,
    MENU_CHOICE_PLAY_TRON,
    MENU_CHOICE_NONE,
    MENU_CHOICE_QUIT
} menu_choice_type;

/**
 * @brief Retrieves the final menu choice selected by the user before exiting the menu module.
 *
 * @param runtime_data Pointer to the active menu runtime data structure.
 * @return menu_choice_type The specific choice selected (e.g., play snake or quit).
 */
menu_choice_type menu_get_exit_choice(void *runtime_data);

#endif // MENU_H
