
/**
 * @file menu.h
 * @brief Public interface for the main menu module.
 *
 * Defines menu selections and exposes the menu module to the
 * game registry.
 */

#ifndef MENU_H
#define MENU_H

#include "engine/core/game_module.h"

/**
 * @brief Available selections in the main menu.
 */
typedef enum {
    MENU_CHOICE_PLAY_SNAKE, /**< Launch Snake. */
    MENU_CHOICE_PLAY_TRON,  /**< Launch Tron. */
    MENU_CHOICE_NONE,       /**< No selection has been made. */
    MENU_CHOICE_QUIT        /**< Exit the application. */
} menu_choice_type;

/**
 * @brief Main menu game module.
 *
 * The session manages the memory allocated for its runtime state.
 */
extern const GameModule menu_module;

/**
 * @brief Retrieves the selection made before the menu finished.
 *
 * @param state Pointer to the menu's runtime state.
 * @return The selected menu choice, or MENU_CHOICE_NONE if state is NULL.
 */
menu_choice_type menu_get_exit_choice(void *state);

#endif /* MENU_H */
