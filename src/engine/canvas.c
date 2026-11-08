#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engine/canvas.h"

Canvas_buffer *canvas_create(int width, int height) {
    Canvas_buffer *canvas = malloc(sizeof(Canvas_buffer));
    canvas->width = width;
    canvas->height = height;
    canvas->cells = malloc(width * height * sizeof(Canvas)); // Allocates the flat grid!
    return canvas;
}

void canvas_destroy(Canvas_buffer *canvas) {
    if (canvas) {
        free(canvas->cells);
        free(canvas);
    }
}

void score_stamper(void *data, Canvas_buffer *canvas) {
    ScoreText *scoreDat = (ScoreText *)data;
    int score = scoreDat->score - 2;
    if (score < 0)
        score = 0;

    int hundreds = (score / 100) % 10;
    int tens     = (score / 10) % 10;
    int units    = score % 10;

    cell_color_assign(canvas, scoreDat->x,     scoreDat->y, hundreds + '0', 255, 215, 0);
    cell_color_assign(canvas, scoreDat->x + 1, scoreDat->y, tens     + '0', 255, 215, 0);
    cell_color_assign(canvas, scoreDat->x + 2, scoreDat->y, units    + '0', 255, 215, 0);
}

void cell_color_assign(Canvas_buffer *canvas, int x, int y, char ch, uint8_t r, uint8_t g, uint8_t b) {
    canvas_get(canvas, x, y)->sym = ch;
    canvas_get(canvas, x, y)->color.r = r;
    canvas_get(canvas, x, y)->color.g = g;
    canvas_get(canvas, x, y)->color.b = b;
}

void canvas_intialize(Canvas_buffer *canvas, int dividerPos) {
    for (int y = 0; y < canvas->height; y++) {
        for (int x = 0; x < canvas->width; x++) {
            if (x == 0 || x == dividerPos || y == 0 || y == canvas->height - 1 || x == canvas->width - 1) {
                cell_color_assign(canvas, x, y, '#', 255, 105, 180);
            } else if (x < dividerPos) {
                cell_color_assign(canvas, x, y, '.', 255, 182, 193);
            } else {
                cell_color_assign(canvas, x, y, ' ', 255, 255, 255);
            }
        }
    }
}

void opt_choice_stamper(void *data, Canvas_buffer *canvas) {
    Option_list *option_list_data = (Option_list *)data;

    for (int k = 0; k < option_list_data->option_amt; k++) {
        int len = strlen(option_list_data->option_choice[k].textData.text);
        int hoverState = option_list_data->option_choice[k].isSelected;
        int r = option_list_data->option_choice[k].textData.color[hoverState].r;
        int g = option_list_data->option_choice[k].textData.color[hoverState].g;
        int b = option_list_data->option_choice[k].textData.color[hoverState].b;
        for (int i = 0; i < len; i++)
            cell_color_assign(canvas, option_list_data->option_choice[k].textData.startX+i,
                                option_list_data->option_choice[k].textData.startY,
                                option_list_data->option_choice[k].textData.text[i],
                                r, g, b);
        if (!(hoverState))
            continue;
        Text_layout hoverArrow[2] = {
            [0].text = ">",
            [1].text = "<",
            [0 ... 1].color = { {255, 255, 255} },
            [0 ... 1].startX = option_list_data->option_choice[k].textData.startX,
            [0 ... 1].startY = option_list_data->option_choice[k].textData.startY
        };
        hoverArrow[0].startX -= 3;
        hoverArrow[1].startX += len+2;
        for (int j = 0; j < 2; j++)
            text_stamper(&hoverArrow[j], canvas);
    }
}

void gradient_text_stamper(void *data, Canvas_buffer *canvas) {
    Text_layout *text = (Text_layout *)data;
    int len = strlen(text->text);

    for (int i = 0; i < len; i++) {
        int r = text->color[0].r + (text->color[1].r - text->color[0].r) * i / (len - 1);
        int g = text->color[0].g + (text->color[1].g - text->color[0].g) * i / (len - 1);
        int b = text->color[0].b + (text->color[1].b - text->color[0].b) * i / (len - 1);
        cell_color_assign(canvas, text->startX+i, text->startY, text->text[i], r, g, b);
    }
}

void text_stamper(void *data, Canvas_buffer *canvas) {
    Text_layout *text = (Text_layout *)data;
    int len = strlen(text->text);

    for (int i = 0; i < len; i++)
        cell_color_assign(canvas, text->startX+i, text->startY, text->text[i], text->color[0].r, text->color[0].g, text->color[0].b);
}

void render_frame(Canvas_buffer *canvas, RenderObj* screenRenderList[], int totalRenderList, int div_pos) {
    printf("\033[H");
    canvas_intialize(canvas, div_pos);
    for (int i = 0; i < totalRenderList; i++) {
        if (screenRenderList[i] != NULL)
            screenRenderList[i]->stamp(screenRenderList[i]->data, canvas);
    }
    canvas_print(canvas);
}

void canvas_print(Canvas_buffer *canvas) {
    int bufferPos = 0;
    char buffer[(canvas->width * 30) * canvas->height];
    for (int y = 0; y < canvas->height; y++) {
        for (int x = 0; x < canvas->width; x++) {
            Canvas *cell = canvas_get(canvas, x, y);
            bufferPos += sprintf(buffer + bufferPos, "\033[38;2;%d;%d;%dm%c\033[0m",
                                 cell->color.r, cell->color.g, cell->color.b, cell->sym);
        }
        sprintf(buffer + bufferPos, "\n");
        bufferPos++;
    }
    fwrite(buffer, sizeof(char), bufferPos, stdout);
}
