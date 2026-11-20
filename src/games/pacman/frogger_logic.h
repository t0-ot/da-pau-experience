#ifndef FROGGER_LOGIC_H_INCLUDED
#define FROGGER_LOGIC_H_INCLUDED

#include "engine/core/game_input.h"

typedef struct frogger_runtime frogger_runtime;

void frogger_init_state(frogger_runtime *runtime);

void frogger_update_logic(
    frogger_runtime *runtime,
    const GameInput *input,
    float delta_time
);

const Street_row *frogger_get_row(const frogger_runtime *runtime, int world_row);

#endif // FROGGER_LOGIC_H_INCLUDED
