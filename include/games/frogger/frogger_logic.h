#ifndef FROGGER_LOGIC_H_INCLUDED
#define FROGGER_LOGIC_H_INCLUDED

#include "engine/core/game_input.h"

typedef struct frogger_runtime frogger_runtime;
typedef struct StreetRow StreetRow;

void frogger_init_state(frogger_runtime *runtime);

void frogger_update_logic(
    frogger_runtime *runtime,
    const GameInput *input,
    float delta_time
);

const StreetRow *frogger_get_row(const frogger_runtime *runtime, int world_row);
#endif // FROGGER_LOGIC_H_INCLUDED
