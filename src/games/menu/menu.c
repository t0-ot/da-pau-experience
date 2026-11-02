#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "games/menu/menu.h"
#include "engine/terminal.h"
#include "engine/utils.h"

#define MODULE_NAME "Da Pau Launching Experience"

void menu_init(void *runtime_data) {
    menu_runtime *runtime = (menu_runtime *)runtime_data;

    runtime->titleText = (BaseText){
        .text = MODULE_NAME,
        .startX = 27,
        .startY = 3,
        .color = {
            {.r = 255, .g = 105, .b = 180},
            {.r = 255, .g = 215, .b = 0}
        }
    };

    runtime->optionChoices[0] = (OptionText){
        .textData.text = "PLAY!",
        .textData.startX = 37,
        .textData.startY = 10,
        .optionAmt = 2,
        .textData.color = {{0, 255, 0}, {255, 255, 255}}
    };

    runtime->optionChoices[1] = (OptionText){
        .textData.text = "QUIT GAME",
        .textData.startX = 35,
        .textData.startY = 12,
        .optionAmt = 2,
        .textData.color = {{0, 255, 0}, {255, 255, 255}}
    };

    runtime->time = 0.0f;
    runtime->duration = 1.0f;
    runtime->startPos = 0.0f;
    runtime->targetPos = 40.0f;
    runtime->change = runtime->targetPos - runtime->startPos;
    runtime->total_choices = 2;
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
    if (menuInputHandler(&runtime->highlighted_choice, runtime->total_choices)) {
        // User pressed enter/selected an option, trigger transition out
        runtime->isAnimatingOut = 1;

        // Record what the user chose so game_registry can interpret it later
        if (runtime->highlighted_choice == (runtime->total_choices-1)) {
            runtime->selected_choice = MENU_CHOICE_QUIT;
            return MODULE_QUIT;
        } else {
            switch (runtime->highlighted_choice) {
                case 0:
                    runtime->selected_choice = MENU_CHOICE_PLAY_SNAKE;
            }
        }

        return MODULE_CONTINUE;
    }

    for (int i = 0; i < runtime->optionChoices[0].optionAmt; i++) {
        runtime->optionChoices[i].isSelected = 0;
        if (i == runtime->highlighted_choice)
            runtime->optionChoices[i].isSelected = 1;
    }

    return MODULE_CONTINUE;
}

void menu_render(void *runtime_data, CanvasBuffer *canvas) {
    menu_runtime *runtime = (menu_runtime *)runtime_data;
    printf("\033[H");

    if (runtime->isAnimatingOut) {
        int currentPos = (int)ease(runtime->time, runtime->startPos, runtime->change, runtime->duration);
        runtime->titleText.startX = (int)ease(runtime->time, 28, 21, runtime->duration);

        canvasIntializer(canvas, currentPos);
        canvasTextStamperGradient(&runtime->titleText, canvas);
        canvasViewPrint(canvas);
    } else {
        canvasIntializer(canvas, runtime->startPos);
        optionChoiceStamper(runtime->optionChoices, canvas);
        canvasTextStamperGradient(&runtime->titleText, canvas);
        canvasViewPrint(canvas);
    }

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
    .init = menu_init,
    .upd = menu_upd,
    .render = menu_render,
    .clean_up = menu_clean_up
};
