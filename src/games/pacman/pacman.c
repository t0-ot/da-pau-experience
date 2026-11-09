#include "raylib.h"
#include "games/pacman/pacman.h"
#include "engine/core/game_input.h"
#include "engine/core/game_types.h"

typedef struct{
    Point mc;
} pacman_runtime;

static void pacman_init(void *state) {
    pacman_runtime *runtime = state;

    runtime->mc.posX = 10;
    runtime->mc.posY = 10;
}

static ModuleAction pacman_upd(void *state, const GameInput *input, float delta_time) {
    pacman_runtime *runtime = state;

    runtime->mc.posX++;
    return MODULE_CONTINUE;

}

static void pacman_draw(void *state) {
     pacman_runtime *runtime = state;

     DrawRectangle(0, 0, 100, 10, RED);


}

static void pacman_cleanup(void *state) {
     pacman_runtime *runtime = state;

}

const GameModule pacman_module = {
    .state_size = sizeof(pacman_runtime),
    .init = pacman_init,
    .update = pacman_upd,
    .draw = pacman_draw,
    .cleanup = pacman_cleanup
};
