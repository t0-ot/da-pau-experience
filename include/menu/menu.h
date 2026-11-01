/**
 * @file menu.h
 * @brief Public interface and runtime definitions for the main menu module.
 */

#ifndef MENU_H
#define MENU_H

#include "../../include/engine/session.h"
#include "../../include/engine/canvas.h"

/**
 * @brief The public module interface for the main menu state.
 */
extern const game_module menu_module;

typedef enum {
    MENU_CHOICE_NONE,
    MENU_CHOICE_PLAY_SNAKE,
    MENU_CHOICE_QUIT
} menu_choice_type;

/**
 * @brief Private runtime data keeping track of menu state, rendering elements, and animations.
 */
typedef struct {
    Canvas menuCell[GAME_HEIGHT][GAME_WIDTH];
    OptionText optionChoices[2];
    BaseText titleText;
    float time;
    float duration;
    float startPos;
    float targetPos;
    float change;
    int total_choices;
    int highlighted_choice;
    int isAnimatingOut;
    menu_choice_type selected_choice;
} menu_runtime;

/**
 * @brief Retrieves the final menu choice selected by the user before exiting the menu module.
 *
 * @param runtime_data Pointer to the active menu runtime data structure.
 * @return menu_choice_type The specific choice selected (e.g., play snake or quit).
 */
menu_choice_type menu_get_exit_choice(void *runtime_data);

#endif // MENU_H
