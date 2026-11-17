#include "engine/renderer/renderer.h"

#include "../assets/asset_manager_internal.h"
#include "../assets/texture_backend.h"
#include "raylib.h"

#include <stddef.h>

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

bool renderer_draw_texture(const AssetManager *manager, TextureHandle texture, float x, float y, float width, float height) {
    if (width <= 0 || height <= 0)
        return false;

    TextureResource *resource =
        asset_manager_resolve_texture(manager, texture);

    if (resource == NULL)
        return false;

    texture_backend_draw(
        resource,
        x,
        y,
        width,
        height
    );

    return true;
}

bool renderer_draw_texture_region(
    const AssetManager *manager,
    TextureHandle texture,
    Rectangle source,
    Rectangle destination,
    Vector2 origin,
    float rotation
) {
    if (source.width <= 0 || source.height <= 0 ||
        destination.width <= 0 || destination.height <= 0)
        return false;

    TextureResource *resource =
        asset_manager_resolve_texture(manager, texture);

    if (resource == NULL)
        return false;

    texture_backend_draw_region(
        resource,
        source,
        destination,
        origin,
        rotation
    );

    return true;
}
