#include <stdio.h>
#include <stdlib.h>
#include "engine/canvas.h"

CanvasBuffer *canvas_create(int width, int height) {
    CanvasBuffer *canvas = malloc(sizeof(CanvasBuffer));
    canvas->width = width;
    canvas->height = height;
    canvas->cells = malloc(width * height * sizeof(Canvas)); // Allocates the flat grid!
    return canvas;
}

void canvas_destroy(CanvasBuffer *canvas) {
    if (canvas) {
        free(canvas->cells);
        free(canvas);
    }
}

void canvasScoreStamper(void *data, CanvasBuffer *canvas) {
    ScoreText *scoreDat = (ScoreText *)data;
    int score = scoreDat->score - 2;
    if (score < 0)
        score = 0;

    int hundreds = (score / 100) % 10;
    int tens     = (score / 10) % 10;
    int units    = score % 10;

    cellColorAssignment(canvas, scoreDat->x,     scoreDat->y, hundreds + '0', 255, 215, 0);
    cellColorAssignment(canvas, scoreDat->x + 1, scoreDat->y, tens     + '0', 255, 215, 0);
    cellColorAssignment(canvas, scoreDat->x + 2, scoreDat->y, units    + '0', 255, 215, 0);
}

void cellColorAssignment(CanvasBuffer *canvas, int x, int y, char ch, uint8_t r, uint8_t g, uint8_t b) {
    canvas_get(canvas, x, y)->sym = ch;
    canvas_get(canvas, x, y)->color.r = r;
    canvas_get(canvas, x, y)->color.g = g;
    canvas_get(canvas, x, y)->color.b = b;
}

void canvasIntializer(CanvasBuffer *canvas, int dividerPos) {
    for (int y = 0; y < canvas->height; y++) {
        for (int x = 0; x < canvas->width; x++) {
            if (x == 0 || x == dividerPos || y == 0 || y == canvas->height - 1 || x == canvas->width - 1) {
                cellColorAssignment(canvas, x, y, '#', 255, 105, 180);
            } else if (x < dividerPos) {
                cellColorAssignment(canvas, x, y, '.', 255, 182, 193);
            } else {
                cellColorAssignment(canvas, x, y, ' ', 255, 255, 255);
            }
        }
    }
}

void optionChoiceStamper(void *data, CanvasBuffer *canvas) {
    OptionText *optionChoice = (OptionText *)data;

    for (int k = 0; k < optionChoice->optionAmt; k++) {
        int len = strlen(optionChoice[k].textData.text);
        int hoverState = optionChoice[k].isSelected;
        int r = optionChoice[k].textData.color[hoverState].r;
        int g = optionChoice[k].textData.color[hoverState].g;
        int b = optionChoice[k].textData.color[hoverState].b;
        for (int i = 0; i < len; i++)
            cellColorAssignment(canvas, optionChoice[k].textData.startX+i, optionChoice[k].textData.startY, optionChoice[k].textData.text[i], r, g, b);
        if (!(hoverState))
            continue;
        BaseText hoverArrow[2] = {
            [0].text = ">",
            [1].text = "<",
            [0 ... 1].color = { {255, 255, 255} },
            [0 ... 1].startX = optionChoice[k].textData.startX,
            [0 ... 1].startY = optionChoice[k].textData.startY
        };
        hoverArrow[0].startX -= 3;
        hoverArrow[1].startX += len+2;
        for (int j = 0; j < 2; j++)
            canvasTextStamper(&hoverArrow[j], canvas);
    }
}

void canvasTextStamperGradient(void *data, CanvasBuffer *canvas) {
    BaseText *text = (BaseText *)data;
    int len = strlen(text->text);

    for (int i = 0; i < len; i++) {
        int r = text->color[0].r + (text->color[1].r - text->color[0].r) * i / (len - 1);
        int g = text->color[0].g + (text->color[1].g - text->color[0].g) * i / (len - 1);
        int b = text->color[0].b + (text->color[1].b - text->color[0].b) * i / (len - 1);
        cellColorAssignment(canvas, text->startX+i, text->startY, text->text[i], r, g, b);
    }
}

void canvasTextStamper(void *data, CanvasBuffer *canvas) {
    BaseText *text = (BaseText *)data;
    int len = strlen(text->text);

    for (int i = 0; i < len; i++)
        cellColorAssignment(canvas, text->startX+i, text->startY, text->text[i], text->color[0].r, text->color[0].g, text->color[0].b);
}

void renderFrame(CanvasBuffer *canvas, RenderObj* screenRenderList[], int totalRenderList) {
    printf("\033[H");
    canvasIntializer(canvas, canvas->width - 41);
    for (int i = 0; i < totalRenderList; i++)
        screenRenderList[i]->stamp(screenRenderList[i]->data, canvas);
    canvasViewPrint(canvas);
}

void canvasViewPrint(CanvasBuffer *canvas) {
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
