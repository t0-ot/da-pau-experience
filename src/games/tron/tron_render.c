#include "raylib.h"

#include <stddef.h>
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
    const Segmented_entity *cycle,
    Color color,
    int offset_x,
    int offset_y
) {
    if (!cycle->alive || cycle->segments.points == NULL)
        return;

    for (int i = cycle->segments.amt - 1; i >= 0; i--) {
        int x = offset_x + cycle->segments.points[i].posX * TRON_CELL_SIZE;
        int y = offset_y + cycle->segments.points[i].posY * TRON_CELL_SIZE;

        DrawRectangle(
            x + 1, y + 1,
            TRON_CELL_SIZE - 2,
            TRON_CELL_SIZE - 2,
            color
        );
    }
}

void tron_render_game(
    const Segmented_entity *player,
    const Segmented_entity enemies[],
    int enemy_count,
    int level,
    int level_starting,
    int turbo
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

    draw_cycle(player, SKYBLUE, offset_x, offset_y);

    for (int i = 0; i < enemy_count && i < 3; i++)
        draw_cycle(&enemies[i], ENEMY_COLORS[i], offset_x, offset_y);

    if (level_starting) {
        const char *intro = get_level_name(level);
        int width = MeasureText(intro, 40);

        DrawText(intro, (GetScreenWidth() - width) / 2, GetScreenHeight() / 2, 40, RAYWHITE);
    }

    DrawText("WASD / ARROWS: MOVE     E: TURBO     ESC: MENU", 30, GetScreenHeight() - 52, 18, GRAY);
}
