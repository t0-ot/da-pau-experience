
/**
 * @file menu.c
 * @brief Main menu logic and Raylib rendering.
 */

#include <stdbool.h>
#include <stddef.h>

#include "raylib.h"

#include "games/menu/menu.h"
#include "engine/utils.h"
#include "engine/core/game_input.h"

/** @brief Window-independent title displayed by the menu. */
#define MODULE_NAME "-+ Da Pau Launching Experience +-"

/** @brief Time between repeated menu navigation inputs. */
#define MENU_INPUT_INTERVAL 0.18f

/**
 * @brief Configuration for an individual menu option.
 */
typedef struct {
    const char *text; /**< Text displayed for the menu option. */
    menu_choice_type choice; /**< Action associated with the option. */
} MenuConfig;

/**
 * @brief Runtime state owned by the main menu module.
 */
typedef struct {
    int highlighted_choice; /**< Index of the currently highlighted option. */
    menu_choice_type selected_choice; /**< Choice confirmed by the player. */

    float input_cooldown; /**< Remaining delay before another navigation input. */
    float animation_time; /**< Elapsed time of the exit animation. */
    float animation_duration; /**< Total duration of the exit animation. */

    bool is_animating_out; /**< Whether the menu is transitioning out. */
} MenuRuntime;

/**
 * @brief Static menu option definitions.
 */
static const MenuConfig MENU_REGISTRY[] = {
    { "SNAKE",     MENU_CHOICE_PLAY_SNAKE },
    { "TRON",      MENU_CHOICE_PLAY_TRON },
    { "PACMAN",    MENU_CHOICE_PLAY_PACMAN},
    { "QUIT GAME", MENU_CHOICE_QUIT }
};

/**
 * @brief Number of available options in the menu registry.
 */
#define TOTAL_MENU_ITEMS (sizeof(MENU_REGISTRY) / sizeof(MENU_REGISTRY[0]))

/**
 * @brief Initializes the menu runtime state.
 *
 * @param state Pointer to allocated menu runtime memory.
 */
static void menu_init(void *state) {
    MenuRuntime *runtime = state;

    runtime->highlighted_choice = 0;
    runtime->selected_choice = MENU_CHOICE_NONE;

    runtime->input_cooldown = 0.0f;
    runtime->animation_time = 0.0f;
    runtime->animation_duration = 0.65f;

    runtime->is_animating_out = false;
}

/**
 * @brief Updates menu navigation and selection.
 *
 * @param state Pointer to menu runtime state.
 * @param input Current frame's input snapshot.
 * @param delta_time Time elapsed since the previous frame.
 * @return MODULE_FINISHED when an option is selected; otherwise MODULE_CONTINUE.
 */
static ModuleAction menu_update(
    void *state,
    const GameInput *input,
    float delta_time
) {
    MenuRuntime *runtime = state;

    if (runtime == NULL || input == NULL)
        return MODULE_QUIT;

    if (runtime->is_animating_out) {
        runtime->animation_time += delta_time;

        if (runtime->animation_time >= runtime->animation_duration)
            return MODULE_FINISHED;

        return MODULE_CONTINUE;
    }

    if (runtime->input_cooldown > 0.0f)
        runtime->input_cooldown -= delta_time;

    if (runtime->input_cooldown <= 0.0f) {
        if (input->up) {
            runtime->highlighted_choice--;

            if (runtime->highlighted_choice < 0)
                runtime->highlighted_choice = (int)TOTAL_MENU_ITEMS - 1;

            runtime->input_cooldown = MENU_INPUT_INTERVAL;
        }
        else if (input->down) {
            runtime->highlighted_choice++;

            if (runtime->highlighted_choice >= (int)TOTAL_MENU_ITEMS)
                runtime->highlighted_choice = 0;

            runtime->input_cooldown = MENU_INPUT_INTERVAL;
        }
    }

    if (input->confirm) {
        runtime->selected_choice =
            MENU_REGISTRY[runtime->highlighted_choice].choice;

        runtime->is_animating_out = true;
        runtime->animation_time = 0.0f;
    }

    return MODULE_CONTINUE;
}

/**
 * @brief Draws the menu using Raylib.
 *
 * @param state Pointer to menu runtime state.
 */
static void menu_draw(void *state) {
    MenuRuntime *runtime = state;

    if (runtime == NULL)
        return;

    const int screen_width = GetScreenWidth();
    const int screen_height = GetScreenHeight();

    const char *title = MODULE_NAME;
    const int title_size = 32;

    int title_width = MeasureText(title, title_size);

    int title_x = (screen_width - title_width) / 2;
    int title_y = screen_height / 5;

    if (runtime->is_animating_out) {
        float movement = ease(runtime->animation_time, 0.0f, 100.0f, runtime->animation_duration);

        if (runtime->selected_choice == MENU_CHOICE_PLAY_SNAKE) {
            title_x += (int)movement;
        }
        else if (runtime->selected_choice == MENU_CHOICE_PLAY_TRON) {
            title_y -= (int)movement;
        }
    }

    Color pink = { 255, 105, 180, 255 };
    Color gold = { 255, 215, 0, 255 };

    DrawText(title, title_x, title_y, title_size, pink);

    /* Draw a small decorative underline. */
    DrawLine(title_x, title_y + title_size + 8, title_x + title_width, title_y + title_size + 8, gold);

    /* Hide the options while the menu transitions out. */
    if (runtime->is_animating_out)
        return;

    const int option_size = 28;
    const int option_spacing = 55;

    int first_option_y = screen_height / 2;

    for (size_t i = 0; i < TOTAL_MENU_ITEMS; i++) {
        const char *option = MENU_REGISTRY[i].text;

        int option_width = MeasureText(option, option_size);
        int option_x = (screen_width - option_width) / 2;
        int option_y = first_option_y + (int)i * option_spacing;

        Color option_color = ((int)i == runtime->highlighted_choice) ? GREEN : WHITE;

        DrawText(option, option_x, option_y, option_size, option_color);
    }

    DrawText("Use UP/DOWN to navigate and ENTER to select", 40, screen_height - 50, 16, GRAY );
}

/**
 * @brief Cleans up resources owned by the menu.
 *
 * The session owns and frees the runtime state itself.
 *
 * @param state Pointer to menu runtime state.
 */
menu_choice_type menu_get_exit_choice(void *state) {
    if (state == NULL)
        return MENU_CHOICE_NONE;

    const MenuRuntime *runtime = state;
    return runtime->selected_choice;
}

/**
 * @brief Main menu module definition.
 */
const GameModule menu_module = {
    .state_size = sizeof(MenuRuntime),
    .init = menu_init,
    .update = menu_update,
    .draw = menu_draw,
};
