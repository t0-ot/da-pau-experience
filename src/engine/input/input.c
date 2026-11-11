#include "engine/input/input.h"

#include "raylib.h"

void input_init(void) {
    // Prevent Raylib from automatically closing on ESC.
    SetExitKey(KEY_NULL);
}

GameInput input_poll(void) {
    GameInput input = {0};

    input.up    = IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W);
    input.down  = IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S);
    input.left  = IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A);
    input.right = IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D);

    input.confirm = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);
    input.back    = IsKeyPressed(KEY_Q);
    input.quit    = IsKeyPressed(KEY_ESCAPE);
    input.action = IsKeyPressed(KEY_E) || IsKeyPressed(KEY_X);

    return input;
}
