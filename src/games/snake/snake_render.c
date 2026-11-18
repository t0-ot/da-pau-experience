#include "raylib.h"

#include "games/snake/snake_render.h"
#include "engine/core/engine_context.h"
#include "engine/renderer/renderer.h"
#include "engine/utils.h"

#define CELL_SIZE 30

void snake_render_game(snake_runtime *snake_data, const EngineContext *engine) {
    int board_width = snake_data->play_width * CELL_SIZE;
    int board_height = snake_data->play_height * CELL_SIZE;

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
    DrawText(TextFormat("SCORE: %d", snake_data->snake.segments.amt-2), panel_x+40, 240, 20, GOLD);

    DrawText( TextFormat("HI-SCORE: %d", snake_data->currentHiScore), panel_x+40, 270, 20, GOLD);

    DrawRectangle(offset_x, offset_y, board_width, board_height, (Color){20, 25, 30, 255});

    /* Draw fruit. */
    for (int i = 0; i < snake_data->normalFruitSet.pointList.amt; i++) {
        int x = offset_x + snake_data->normalFruitSet.pointList.points[i].posX * CELL_SIZE;
        int y = offset_y + snake_data->normalFruitSet.pointList.points[i].posY * CELL_SIZE;

//        DrawRectangle(x + 3, y + 3, CELL_SIZE - 6, CELL_SIZE - 6, RED);
        renderer_draw_texture(engine->assets, snake_data->texture.fruit_texture, x-16, y-19, CELL_SIZE*2, CELL_SIZE*2);
    }
    for (int i = 0; i < snake_data->snake.segments.amt; i++) {
        int index = (snake_data->snake.tail + i) % snake_data->snake.capacity;
        int x = offset_x + snake_data->snake.segments.points[index].posX * CELL_SIZE;
        int y = offset_y + snake_data->snake.segments.points[index].posY * CELL_SIZE;

        Color segment_color = (index == snake_data->snake.head) ? LIME : DARKGREEN;

        DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, segment_color);
    }

    DrawText("WASD / ARROWS: MOVE     ESC: MENU", panel_x, 370, 18, GRAY);
}
