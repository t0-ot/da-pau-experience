#include "engine/input/input.h"

#include "raylib.h"

void input_init(void) {
    // Prevent Raylib from automatically closing on ESC.
    SetExitKey(KEY_NULL);
}

GameInput input_poll(void) {
    GameInput input = {0};

    input.up_held    = IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W);
    input.down_held  = IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S);
    input.left_held  = IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A);
    input.right_held = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);

    input.up_pressed    = IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W);
    input.down_pressed  = IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S);
    input.left_pressed  = IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A);
    input.right_pressed = IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D);

    input.confirm = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);
    input.back    = IsKeyPressed(KEY_Q);
    input.quit    = IsKeyPressed(KEY_ESCAPE);
    input.action = IsKeyPressed(KEY_E) || IsKeyPressed(KEY_X);

    return input;
}
