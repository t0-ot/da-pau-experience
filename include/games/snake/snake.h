#ifndef GAME_H
#define GAME_H

#include "engine/canvas.h"
#include "engine/session.h"
#include "games/snake/snake_logic.h"

/**
 * @brief The public module interface for the Snake game.
 */
extern const game_module snake_module;

void snake_init(void *data);
module_action snake_upd(void *runtime_data);
void snake_render(void *runtime_data, CanvasBuffer *canvas);
void snake_clean_up(void *runtime_data);

#endif // GAME_H
