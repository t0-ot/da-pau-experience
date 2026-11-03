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

void menu_init(void *runtime_data) {
    menu_runtime *runtime = (menu_runtime *)runtime_data;

    runtime->titleText = (BaseText){
        .text = MODULE_NAME,
        .startX = get_centered_x(0, menu_module.game_width, 27),
        .startY = (menu_module.game_height * 3) / 20,
        .color = {
            {.r = 255, .g = 105, .b = 180},
            {.r = 255, .g = 215, .b = 0}
        }
    };
    runtime->title_text_obj = (RenderObj){
        .stamp = canvasTextStamperGradient
    },
    runtime->option_choices_obj = (RenderObj){
        .stamp = optionChoiceStamper
    };

    runtime->optionChoices[0] = (OptionText){
        .textData.text = "SNAKE",
        .textData.startX = get_centered_x(0, menu_module.game_width, 5),
        .textData.startY = get_centered_y(0, menu_module.game_height, 1),
        .textData.color = {{0, 255, 0}, {255, 255, 255}}
    };

    runtime->optionChoices[1] = (OptionText){
        .textData.text = "TRON",
        .textData.startX = get_centered_x(0, menu_module.game_width, 4),
        .textData.startY = get_centered_y(1, menu_module.game_height, 1) + 1,
        .textData.color = {{0, 255, 0}, {255, 255, 255}}
    };
    runtime->optionChoices[2] = (OptionText){
        .textData.text = "QUIT GAME",
        .textData.startX = get_centered_x(0, menu_module.game_width, 9),
        .textData.startY = get_centered_y(1, menu_module.game_height, 1) + 3,
        .textData.color = {{0, 255, 0}, {255, 255, 255}}
    };
    runtime->title_text_obj.data = &runtime->titleText;
    runtime->option_choices_obj.data = &runtime->total_option_list;

    runtime->render_list[0] = &runtime->title_text_obj;
    runtime->render_list[1] = &runtime->option_choices_obj;
    runtime->total_render_list = sizeof(runtime->render_list) / sizeof(runtime->render_list[0]);

    runtime->total_option_list.option_choice = runtime->optionChoices;
    runtime->total_option_list.option_amt = sizeof(runtime->optionChoices) / sizeof(runtime->optionChoices[0]);
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

    // If we finished the selection and are running the exit transition
    if (runtime->isAnimatingOut) {
        if (runtime->time <= runtime->duration) {
            runtime->time += 0.03f;
            return MODULE_CONTINUE; // Keep looping render/upd for animation
        }
        return MODULE_FINISHED;
    }

    // Normal interactive menu loop
    if (menuInputHandler(&runtime->highlighted_choice, runtime->total_option_list.option_amt)) {
        // User pressed enter/selected an option, trigger transition out
        runtime->isAnimatingOut = 1;

        // Record what the user chose so game_registry can interpret it later
        if (runtime->highlighted_choice == (runtime->total_option_list.option_amt-1)) {
            runtime->selected_choice = MENU_CHOICE_QUIT;
            return MODULE_QUIT;
        } else {
            switch (runtime->highlighted_choice) {
                case 0:
                    runtime->selected_choice = MENU_CHOICE_PLAY_SNAKE;
                    break;
                case 1:
                    runtime->selected_choice = MENU_CHOICE_PLAY_TRON;
                    break;
            }
        }

        return MODULE_CONTINUE;
    }

    for (int i = 0; i < runtime->total_option_list.option_amt; i++) {
        runtime->optionChoices[i].isSelected = 0;
        if (i == runtime->highlighted_choice)
            runtime->optionChoices[i].isSelected = 1;
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
