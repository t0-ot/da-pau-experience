#include "raylib.h"

#include "games/snake/snake_render.h"
#include "engine/utils.h"

#define CELL_SIZE 30

void snake_render_game(
    const Segmented_entity *snake,
    const fruitData *fruit,
    int score,
    int high_score,
    int play_width,
    int play_height
) {
    int board_width = play_width * CELL_SIZE;
    int board_height = play_height * CELL_SIZE;

    int offset_x = 50;
    int offset_y = (GetScreenHeight() - board_height) / 2;


    DrawRectangle(offset_x - 3, offset_y - 3, board_width + 6, board_height + 6, DARKGRAY);

    int panel_x = offset_x + board_width + 50;
    DrawText("DA PAU SNAKE GAME", panel_x, offset_y + 7, 40, PINK);

    /* Draw a small decorative underline. */
    DrawLine(panel_x, offset_y+60, panel_x + MeasureText("DA PAU SNAKE GAME", 40), offset_y + 60, GOLD);

    DrawRectangle(panel_x, 150, 440, 200, DARKGRAY);
    DrawRectangle(panel_x+6, 150, 434, 200, (Color){20, 25, 30, 255});
    DrawText("STATISTICS", panel_x+20, 170, 30, GOLD);
    DrawText(TextFormat("SCORE: %d", score), panel_x+40, 240, 20, GOLD);

    DrawText( TextFormat("HI-SCORE: %d", high_score), panel_x+40, 270, 20, GOLD);

    DrawRectangle(offset_x, offset_y, board_width, board_height, (Color){20, 25, 30, 255});

    /* Draw fruit. */
    for (int i = 0; i < fruit->pointList.amt; i++) {
        int x = offset_x + fruit->pointList.points[i].posX * CELL_SIZE;
        int y = offset_y + fruit->pointList.points[i].posY * CELL_SIZE;

        DrawRectangle(x + 3, y + 3, CELL_SIZE - 6, CELL_SIZE - 6, RED);
    }
    for (int i = snake->segments.amt - 1; i >= 0; i--) {
        int x = offset_x + snake->segments.points[i].posX * CELL_SIZE;
        int y = offset_y + snake->segments.points[i].posY * CELL_SIZE;

        Color segment_color = (i == 0) ? LIME : DARKGREEN;

        DrawRectangle(x + 2, y + 2, CELL_SIZE - 4, CELL_SIZE - 4, segment_color);
    }

    DrawText("WASD / ARROWS: MOVE     ESC: MENU", panel_x, 370, 18, GRAY);
    DrawFPS(panel_x, 400);
}
