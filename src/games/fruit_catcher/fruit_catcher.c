#include "raylib.h"
#include <stdio.h>
#include "games/fruit_catcher/fruit_catcher.h"
#include "engine/core/game_input.h"
#include "engine/core/game_types.h"


typedef struct{
    Point catcher;
    Point lastDirInput;
    float move_timer;
    float move_interval;
} catcher_runtime;



static void catcher_init(void *state) {
    catcher_runtime *runtime = state;

    runtime->catcher.posX = GetScreenWidth()/2 - 50;
    runtime->catcher.posY = 650;
}

static ModuleAction catcher_upd(void *state, const GameInput *input, float delta_time) {
    catcher_runtime *runtime = state;

    if(input->right) runtime->catcher.posX+=30;
    if(input->left) runtime->catcher.posX-=30;

    if (input->back)
        return MODULE_FINISHED;


    return MODULE_CONTINUE;
}

static void catcher_draw(void *state) {
    catcher_runtime *runtime = state;

    Color GENESIS = {0, 255, 100, 255};

    DrawRectangle(runtime->catcher.posX, runtime->catcher.posY, 100, 10, GENESIS);
    DrawText("Fruit Catcher", GetScreenWidth()/2 - (MeasureText("Fruit Catcher", 35)/2), 150, 35, WHITE);
}

static void catcher_cleanup(void *state) {
    catcher_runtime *runtime = state;
}

const GameModule catcher_module = {
    .state_size = sizeof(catcher_runtime),
    .init = catcher_init,
    .update = catcher_upd,
    .draw = catcher_draw,
    .cleanup = catcher_cleanup
};
