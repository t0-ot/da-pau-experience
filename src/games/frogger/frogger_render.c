/**
 * @file frogger_render.c
 * @brief Raylib rendering implementation for the endless Pinoy street game.
 */

#include "raylib.h"
#include "engine/utils.h"
#include "engine/animation/animation.h"
#include "games/frogger/frogger_render.h"
#include "games/frogger/frogger_internal.h"
#include "games/frogger/frogger_logic.h"

#define STREET_CELL_SIZE 40

static const Color JEEPNEY_COLORS[] = {
    YELLOW,
    ORANGE,
    RED,
    SKYBLUE,
    MAGENTA
};

static void draw_vehicle_or_plank(
    LaneObject obj,
    int draw_x,
    int draw_w,
    int row_y
) {
    if (obj.kind == OBJ_PLANK) {
        DrawRectangle(draw_x + 2, row_y + 6, draw_w - 4, STREET_CELL_SIZE - 12, BROWN);
        DrawRectangleLines(draw_x + 2, row_y + 6, draw_w - 4, STREET_CELL_SIZE - 12, BEIGE);
        return;
    }

    if (obj.kind == OBJ_JEEPNEY) {
        Color body = JEEPNEY_COLORS[obj.color_idx % 5];
        DrawRectangle(draw_x + 2, row_y + 4, draw_w - 4, STREET_CELL_SIZE - 8, body);
        DrawRectangle(draw_x + 6, row_y + 8, draw_w - 12, STREET_CELL_SIZE - 16, LIGHTGRAY);
        DrawRectangleLines(draw_x + 2, row_y + 4, draw_w - 4, STREET_CELL_SIZE - 8, WHITE);
    } else if (obj.kind == OBJ_BUS) {
        DrawRectangle(draw_x + 2, row_y + 3, draw_w - 4, STREET_CELL_SIZE - 6, (Color){220, 60, 40, 255});
        DrawRectangle(draw_x + 6, row_y + 7, draw_w - 12, 8, SKYBLUE);
    } else if (obj.kind == OBJ_TRICYCLE) {
        int half_h = (STREET_CELL_SIZE - 10) / 2;
        DrawRectangle(draw_x + 3, row_y + 4, draw_w - 6, half_h, BLUE);
        DrawRectangle(draw_x + 3, row_y + 5 + half_h, draw_w - 6, half_h, GOLD);
    } else {
        /* OBJ_MOTOR */
        DrawRectangle(draw_x + 6, row_y + 10, draw_w - 12, STREET_CELL_SIZE - 20, LIME);
    }
}

void frogger_render_game(const frogger_runtime *runtime) {
    int board_width  = FROGGER_COLS * STREET_CELL_SIZE;
    int board_height = FROGGER_VISIBLE_ROWS * STREET_CELL_SIZE;

    int offset_x = 50;
    int offset_y = (GetScreenHeight() - board_height) / 2;

    /* Outer board frame matching Snake's visual style */
    DrawRectangle(offset_x - 3, offset_y - 3, board_width + 6, board_height + 6, DARKGRAY);

    /* Right-hand HUD panel */
    int panel_x = offset_x + board_width + 50;
    DrawText("TAWID KALSADA", panel_x, offset_y + 7, 40, PINK);
    DrawLine(
        panel_x,
        offset_y + 60,
        panel_x + MeasureText("TAWID KALSADA", 40),
        offset_y + 60,
        GOLD
    );

    DrawRectangle(panel_x, 150, 440, 210, DARKGRAY);
    DrawRectangle(panel_x + 6, 150, 434, 210, (Color){20, 25, 30, 255});
    DrawText("STATISTICS", panel_x + 20, 170, 30, GOLD);
    DrawText(TextFormat("SCORE: %d", runtime->score), panel_x + 40, 230, 20, GOLD);
    DrawText(TextFormat("HI-SCORE: %d", runtime->current_hi_score), panel_x + 40, 260, 20, GOLD);
    DrawText(TextFormat("SPEED: %.1fx", runtime->scroll_speed), panel_x + 40, 290, 20, SKYBLUE);
        /* Distance margin before falling behind the camera (NADAPA meter) */
    float margin = ((float)runtime->player.posY - (runtime->camera_row - 0.35f)) / ((float)FROGGER_VISIBLE_ROWS * 0.55f);
    if (margin < 0.0f) margin = 0.0f;
    if (margin > 1.0f) margin = 1.0f;

    Color bar_color = GREEN;
    if (margin < 0.25f) {
        bar_color = RED;
    } else if (margin < 0.50f) {
        bar_color = ORANGE;
    }

    DrawText("ORAS / STAMINA:", panel_x + 40, 322, 16, LIGHTGRAY);
    DrawRectangleLines(panel_x + 200, 322, 180, 16, GRAY);
    DrawRectangle(panel_x + 202, 324, (int)(176.0f * margin), 12, bar_color);


    /* Clip scrolling street rows strictly inside the play board */
    BeginScissorMode(offset_x, offset_y, board_width, board_height);

    int min_row = (int)runtime->camera_row - 1;
    int max_row = (int)runtime->camera_row + FROGGER_VISIBLE_ROWS + 1;

    for (int w_row = min_row; w_row <= max_row; w_row++) {
        if (w_row < 0)
            continue;

        const StreetRow *row = frogger_get_row(runtime, w_row);
        float screen_row_offset = (float)w_row - runtime->camera_row;
        int row_y = offset_y + board_height - (int)((screen_row_offset + 1.0f) * STREET_CELL_SIZE);

        /* Terrain background */
        Color bg = (Color){68, 72, 82, 255}; /* Sidewalk concrete */
        if (row->type == LANE_KALSADA) {
            bg = (Color){26, 28, 34, 255};   /* Asphalt road */
        } else if (row->type == LANE_BAHA) {
            bg = (Color){22, 68, 112, 255};  /* Floodwater / estero */
        }

        DrawRectangle(offset_x, row_y, board_width, STREET_CELL_SIZE, bg);

        /* Dashed asphalt lane lines */
        if (row->type == LANE_KALSADA) {
            for (int c = 0; c < FROGGER_COLS; c += 2) {
                DrawRectangle(
                    offset_x + c * STREET_CELL_SIZE + 8,
                    row_y + STREET_CELL_SIZE - 3,
                    STREET_CELL_SIZE / 2,
                    2,
                    (Color){90, 90, 70, 255}
                );
            }
        }

        /* Sidewalk street posts / barriers */
        if (row->type == LANE_SIDEWALK) {
            for (int c = 0; c < FROGGER_COLS; c++) {
                if (row->blocked_cols[c]) {
                    int post_x = offset_x + c * STREET_CELL_SIZE;
                    DrawRectangle(post_x + 8, row_y + 8, STREET_CELL_SIZE - 16, STREET_CELL_SIZE - 16, DARKBROWN);
                    DrawRectangleLines(post_x + 8, row_y + 8, STREET_CELL_SIZE - 16, STREET_CELL_SIZE - 16, ORANGE);
                }
            }
        }

        /* Moving vehicles or floating planks */
        for (int i = 0; i < row->object_count; i++) {
            LaneObject obj = row->objects[i];
            int obj_x = offset_x + (int)(obj.x * STREET_CELL_SIZE);
            int obj_w = (int)(obj.width * STREET_CELL_SIZE);

            draw_vehicle_or_plank(obj, obj_x, obj_w, row_y);
        }
    }

    /* Compute hop squash/stretch using ease() and animation frame index */
    float hop_expand = 0.0f;
    if (runtime->hop_ease_timer > 0.0f && runtime->hop_ease_duration > 0.0f) {
        float elapsed = runtime->hop_ease_duration - runtime->hop_ease_timer;
        hop_expand = ease(elapsed, 4.0f, -4.0f, runtime->hop_ease_duration);
    }

    Rectangle src_rect = {0};
    animation_player_get_source(&runtime->hop_anim, &src_rect);

    float player_screen_row = (float)runtime->player.posY - runtime->camera_row;
    int px = offset_x + (int)(runtime->player_float_x * STREET_CELL_SIZE) - (int)hop_expand;
    int py = offset_y + board_height - (int)((player_screen_row + 1.0f) * STREET_CELL_SIZE) - (int)hop_expand;
    int p_size = STREET_CELL_SIZE - 12 + (int)(hop_expand * 2.0f);

    Color player_tint = (runtime->hop_anim.current_frame % 2 == 0) ? GREEN : LIME;
    DrawRectangle(px + 6, py + 6, p_size, p_size, player_tint);
    DrawRectangleLines(px + 6, py + 6, p_size, p_size, RAYWHITE);

    /* Game Over Overlay */
       /* Game Over Overlay */
    if (runtime->game_over) {
        DrawRectangle(offset_x, offset_y, board_width, board_height, Fade(BLACK, 0.72f));

        const char *msg1 = "GAME OVER!";
        Color msg_color = RED;

        if (runtime->death_reason == DEATH_NASAGASAAN) {
            msg1 = "NASAGASAAN!";
            msg_color = RED;
        } else if (runtime->death_reason == DEATH_NALUNOD) {
            msg1 = "NALUNOD!";
            msg_color = SKYBLUE;
        } else if (runtime->death_reason == DEATH_NADAPA) {
            msg1 = "NADAPA!";
            msg_color = ORANGE;
        }

        const char *msg2 = "PRESS ENTER / SPACE TO RETRY";
        int w1 = MeasureText(msg1, 36);
        int w2 = MeasureText(msg2, 18);

        int x1 = offset_x + (board_width - w1) / 2;
        int x2 = offset_x + (board_width - w2) / 2;

        DrawText(msg1, x1, offset_y + board_height / 2 - 30, 36, msg_color);
        DrawText(msg2, x2, offset_y + board_height / 2 + 18, 18, WHITE);
    }


    EndScissorMode();

    DrawText("WASD / ARROWS: MOVE     Q: MENU     ESC: QUIT", panel_x, 385, 18, GRAY);
}
