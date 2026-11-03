#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "games/menu/menu.h"
#include "engine/terminal.h"
#include "engine/utils.h"

#define MODULE_NAME "Da Pau Launching Experience"

/**
 * @brief Private runtime data keeping track of menu state, rendering elements, and animations.
 */
typedef struct {
    OptionText optionChoices[3];
    option_list total_option_list;
    BaseText titleText;
    RenderObj title_text_obj;
    RenderObj option_choices_obj;
    RenderObj* render_list[2];
    int total_render_list;
    float time;
    float duration;
    float startPos;
    float targetPos;
    float change;
    int highlighted_choice;
    int isAnimatingOut;
    menu_choice_type selected_choice;
} menu_runtime;

typedef struct {
    const char *text;
    menu_choice_type choice;
    int width;    // Used for centering math
    int y_offset; // Relative spacing modifier
} MenuConfig;

// Define menu structure sequentially
static const MenuConfig MENU_REGISTRY[] = {
    { .text = "SNAKE",     .choice = MENU_CHOICE_PLAY_SNAKE, .width = 5, .y_offset = 0 },
    { .text = "TRON",      .choice = MENU_CHOICE_PLAY_TRON,  .width = 4, .y_offset = 1 },
    { .text = "QUIT GAME", .choice = MENU_CHOICE_QUIT,       .width = 9, .y_offset = 3 }
};

#define TOTAL_MENU_ITEMS (sizeof(MENU_REGISTRY) / sizeof(MENU_REGISTRY[0]))


void menu_init(void *runtime_data) {
    menu_runtime *runtime = (menu_runtime *)runtime_data;

    runtime->titleText = (BaseText){
        .text = MODULE_NAME,
        .startX = get_centered_x(0, menu_module.game_width, 27),
        .startY = (menu_module.game_height * 3) / 20,
        .color = { {.r = 255, .g = 105, .b = 180}, {.r = 255, .g = 215, .b = 0} }
    };

    runtime->title_text_obj = (RenderObj){ .stamp = canvasTextStamperGradient };
    runtime->option_choices_obj = (RenderObj){ .stamp = optionChoiceStamper };

    // Dynamic Initialization Loop
    for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
        runtime->optionChoices[i] = (OptionText){
            .textData.text = MENU_REGISTRY[i].text,
            .textData.startX = get_centered_x(0, menu_module.game_width, MENU_REGISTRY[i].width),
            // Calculates height scaling based on your y_offset properties
            .textData.startY = get_centered_y(i == 0 ? 0 : 1, menu_module.game_height, 1) + MENU_REGISTRY[i].y_offset,
            .textData.color = {{0, 255, 0}, {255, 255, 255}}
        };
    }

    runtime->title_text_obj.data = &runtime->titleText;
    runtime->option_choices_obj.data = &runtime->total_option_list;
    runtime->render_list[0] = &runtime->title_text_obj;
    runtime->render_list[1] = &runtime->option_choices_obj;
    runtime->total_render_list = sizeof(runtime->render_list) / sizeof(runtime->render_list[0]);

    runtime->total_option_list.option_choice = runtime->optionChoices;
    runtime->total_option_list.option_amt = TOTAL_MENU_ITEMS;

    runtime->time = 0.0f;
    runtime->duration = 1.0f;
    runtime->startPos = 0.0f;
    runtime->targetPos = 40.0f;
    runtime->change = runtime->targetPos - runtime->startPos;
    runtime->selected_choice = MENU_CHOICE_NONE;
    runtime->highlighted_choice = 0;
    runtime->isAnimatingOut = 0;

    set_raw_mode(1);
}


module_action menu_upd(void *runtime_data) {
    menu_runtime *runtime = (menu_runtime *)runtime_data;

    if (runtime->isAnimatingOut) {
        if (runtime->time <= runtime->duration) {
            runtime->time += 0.03f;
            return MODULE_CONTINUE; // Keep looping render/upd for animation
        }
        return MODULE_FINISHED;
    }

    if (menuInputHandler(&runtime->highlighted_choice, runtime->total_option_list.option_amt)) {
        runtime->isAnimatingOut = 1;

        // Dynamic mapping assignment
        runtime->selected_choice = MENU_REGISTRY[runtime->highlighted_choice].choice;

        if (runtime->selected_choice == MENU_CHOICE_QUIT) {
            return MODULE_QUIT;
        }
        return MODULE_CONTINUE;
    }

    for (int i = 0; i < runtime->total_option_list.option_amt; i++) {
        runtime->optionChoices[i].isSelected = (i == runtime->highlighted_choice);
    }

    return MODULE_CONTINUE;
}

void menu_render(void *runtime_data, CanvasBuffer *canvas) {
    menu_runtime *runtime = (menu_runtime *)runtime_data;
    printf("\033[H");
    int currentPos = 0;
    if (runtime->isAnimatingOut) {
        currentPos = (int)ease(runtime->time, runtime->startPos, runtime->change, runtime->duration);
        runtime->titleText.startX = (int)ease(runtime->time, 28, 21, runtime->duration);
        runtime->render_list[1] = NULL;
    }
    renderFrame(canvas, runtime->render_list, runtime->total_render_list, currentPos);
    fflush(stdout);
    usleep(30000);
}

void menu_clean_up(void *runtime_data) {
    if (runtime_data == NULL) return;
    set_raw_mode(0);

    free(runtime_data);
}

menu_choice_type menu_get_exit_choice(void *runtime_data) {
    if (runtime_data == NULL) return MENU_CHOICE_NONE;
    return ((menu_runtime *)runtime_data)->selected_choice;
}

const game_module menu_module = {
    .game_width = 80,
    .game_height = 20,
    .runtime_size = sizeof(menu_runtime),
    .init = menu_init,
    .upd = menu_upd,
    .render = menu_render,
    .clean_up = menu_clean_up
};
