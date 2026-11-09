#include "games/pacman/pacman.h"
#include "engine/core/game_input.h"
#include "engine/core/game_types.h"


typedef struct{
} pacman_runtime;

static void pacman_init(void *state) {
    pacman_runtime *runtime = state;


}

static ModuleAction pacman_upd(void *state, const GameInput *input, float delta_time) {
    pacman_runtime *runtime = state;

}

static void pacman_draw(void *state) {
     pacman_runtime *runtime = state;


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
