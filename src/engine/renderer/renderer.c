#include "engine/renderer/renderer.h"

#include "raylib.h"

bool renderer_init(int width, int height, const char *title) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(width, height, title);

    return IsWindowReady();
}

void renderer_close(void) {
    CloseWindow();
}

bool renderer_should_close(void) {
    return WindowShouldClose();
}

void renderer_begin_frame(void) {
    BeginDrawing();
    ClearBackground((Color){20, 20, 20, 255});

    Rectangle border = { 10, 10, GetScreenWidth() - 20, GetScreenHeight() - 20};
    DrawRectangleLinesEx(border, 18, PINK);
}

void renderer_end_frame(void) {
    EndDrawing();
}

float renderer_get_delta_time(void) {
    return GetFrameTime();
}
