#include "raylib.h"

#include <stddef.h>
#include <math.h>
#include "engine/renderer/renderer.h"
#include "games/tron/tron_render.h"
#include "games/tron/tron_internal.h"
#include "games/tron/tron_logic.h"

#define TRON_CELL_SIZE 20

static const Color ENEMY_COLORS[] = {
    GOLD,
    ORANGE,
    RED
};

static void draw_cycle(
    AssetManager *manager,
    const Segmented_entity *cycle,
    const Point *facing_direction,
    Color color,
    TextureHandle cycle_texture,
    Vector2 cycle_origin,
    Rectangle source_texture,
    int offset_x,
    int offset_y
) {
    if (!cycle->alive || cycle->segments.points == NULL)
        return;

    int dx = facing_direction->posX;
    int dy = facing_direction->posY;

    /* 1. Draw connected trail segments */
    for (int i = 0; i < cycle->segments.amt; i++) {
        int index = (cycle->tail + i) % cycle->capacity;

        if (index == cycle->head)
            continue;

        Point current = cycle->segments.points[index];

        int x = offset_x + current.posX * TRON_CELL_SIZE;
        int y = offset_y + current.posY * TRON_CELL_SIZE;

        const int thickness = 8;
        const int center = (TRON_CELL_SIZE - thickness) / 2;

        /* Draw the center of this segment. */
        DrawRectangle(
            x + center, y + center,
            thickness, thickness,
            color
        );

        /* Connect to the previous segment, if one exists. */
        if (i > 0) {
            int previous_index =
                (cycle->tail + i - 1) % cycle->capacity;

            Point previous = cycle->segments.points[previous_index];

            int dx = previous.posX - current.posX;
            int dy = previous.posY - current.posY;

            if (dx < 0)
                DrawRectangle(x, y + center, TRON_CELL_SIZE / 2, thickness, color);
            else if (dx > 0)
                DrawRectangle(x + TRON_CELL_SIZE / 2, y + center,
                              TRON_CELL_SIZE / 2, thickness, color);
            else if (dy < 0)
                DrawRectangle(x + center, y, thickness, TRON_CELL_SIZE / 2, color);
            else if (dy > 0)
                DrawRectangle(x + center, y + TRON_CELL_SIZE / 2,
                              thickness, TRON_CELL_SIZE / 2, color);
        }

        /* Connect to the next segment (the head is included). */
        int next_index = (cycle->tail + i + 1) % cycle->capacity;
        Point next = cycle->segments.points[next_index];

        int dx = next.posX - current.posX;
        int dy = next.posY - current.posY;

        if (dx < 0)
            DrawRectangle(x, y + center, TRON_CELL_SIZE / 2, thickness, color);
        else if (dx > 0)
            DrawRectangle(x + TRON_CELL_SIZE / 2, y + center,
                          TRON_CELL_SIZE / 2, thickness, color);
        else if (dy < 0)
            DrawRectangle(x + center, y, thickness, TRON_CELL_SIZE / 2, color);
        else if (dy > 0)
            DrawRectangle(x + center, y + TRON_CELL_SIZE / 2,
                          thickness, TRON_CELL_SIZE / 2, color);
    }

    /* 2. Draw cycle head on top */
    int x = offset_x + cycle->segments.points[cycle->head].posX * TRON_CELL_SIZE;
    int y = offset_y + cycle->segments.points[cycle->head].posY * TRON_CELL_SIZE;

    Rectangle destination_pos = {
        x + TRON_CELL_SIZE / 2,
        y + TRON_CELL_SIZE / 2,
        TRON_CELL_SIZE * 2,
        TRON_CELL_SIZE * 2
    };

    float rotation = atan2f((float)dy, (float)dx) * (180.0f / PI) + 90.0f;

    renderer_draw_texture_region(
        manager,
        cycle_texture,
        source_texture,
        destination_pos,
        cycle_origin,
        rotation
    );
}

void tron_render_game(
    AssetManager *manager,
    TextureHandle cycle_texture,
    const Segmented_entity *player,
    const Point *last_dir_input,
    const Segmented_entity enemies[],
    int enemy_count,
    int level,
    int level_starting,
    int turbo,
    int attempt_no
) {
    int board_width = TRON_WIDTH * TRON_CELL_SIZE;
    int board_height = TRON_HEIGHT * TRON_CELL_SIZE;

    int offset_x = (GetScreenWidth() - board_width) / 2;
    int offset_y = (GetScreenHeight() - board_height) / 2;

    DrawText("DA PAU TRON", 45, 60, 30, SKYBLUE);
    DrawText(TextFormat("LEVEL: %d - %s", level, get_level_name(level)), 45, 90, 20, GOLD);

    DrawText(turbo ? "TURBO: ON" : "TURBO: OFF", 45, 120, 18, turbo ? GREEN : GRAY);

    DrawRectangle(offset_x - 3, offset_y - 3, board_width + 6, board_height + 6, DARKGRAY);

    DrawRectangle(offset_x, offset_y, board_width, board_height, (Color){10, 14, 28, 255});
    Vector2 cycle_origin = {TRON_CELL_SIZE, TRON_CELL_SIZE};
    draw_cycle(manager, player, last_dir_input,  SKYBLUE, cycle_texture, cycle_origin, (Rectangle){0, 0, 128, 128},   offset_x, offset_y);

//    for (int i = 0; i < enemy_count && i < 3; i++)
//        draw_cycle(&enemies[i], ENEMY_COLORS[i], offset_x, offset_y);

    if (level_starting) {
        const char *intro = get_level_name(level);
        int width = MeasureText(intro, 40);

        DrawText(intro, (GetScreenWidth() - width) / 2, GetScreenHeight() / 2, 40, RAYWHITE);
    }
    DrawText(TextFormat("Attempt No: %d", attempt_no), 45, 200, 20, WHITE);
    DrawText("WASD / ARROWS: MOVE     E: TURBO     ESC: MENU", 30, GetScreenHeight() - 52, 18, GRAY);
}
