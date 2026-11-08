#include "engine/core/input.h"

#include "raylib.h"

void input_init(void) {
    // Prevent Raylib from automatically closing on ESC.
    SetExitKey(KEY_NULL);
}

GameInput input_poll(void) {
    GameInput input = {0};

    input.up    = IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W);
    input.down  = IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S);
    input.left  = IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A);
    input.right = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);

    input.confirm = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);
    input.back    = IsKeyPressed(KEY_ESCAPE);
    input.quit    = IsKeyPressed(KEY_Q);
    input.action = IsKeyPressed(KEY_E) || IsKeyPressed(KEY_X);

    return input;
}
