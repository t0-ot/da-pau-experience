#ifndef CANVAS_H
#define CANVAS_H

#include <stdint.h>
#include <string.h>
#define MAX_STATES 2

typedef struct {
    uint8_t r, g, b;
} RGB;

typedef struct {
    int8_t posX;
    int8_t posY;
} Point;

typedef struct {
    Point *points;
    int16_t amt;
} ViewPointList;

typedef struct {
    char sym;
    RGB color;
} Canvas;

typedef struct {
    int width;
    int height;
    Canvas *cells;
} CanvasBuffer;

typedef void(*StampFunc)(void *data, CanvasBuffer *canvas);

typedef struct {
    void *data;
    StampFunc stamp;
} RenderObj;

typedef struct {
    char *text;
    int8_t startX;
    int8_t startY;
    RGB color[MAX_STATES];
} BaseText;

typedef struct {
    int16_t score;
    int8_t x;
    int8_t y;
} ScoreText;

typedef struct {
    BaseText textData;
    bool isSelected;
    int8_t optionAmt;
} OptionText;


static inline Canvas *canvas_get(CanvasBuffer *canvas, int x, int y) {
    return &canvas->cells[y * canvas->width + x];
}
CanvasBuffer *canvas_create(int width, int height);
void canvas_destroy(CanvasBuffer *canvas);
void canvasIntializer(CanvasBuffer *canvas, int dividerPos);
void renderFrame(CanvasBuffer *canvas, RenderObj* screenRenderList[], int totalRenderList);
void optionChoiceStamper(void *data, CanvasBuffer *canvas);
void canvasTextStamperGradient(void *data, CanvasBuffer *canvas);
void canvasTextStamper(void *data, CanvasBuffer *canvas);
void canvasScoreStamper(void *data, CanvasBuffer *canvas);
void canvasViewPrint(CanvasBuffer *canvas);
void cellColorAssignment(CanvasBuffer *canvas, int x, int y, char ch, uint8_t r, uint8_t g, uint8_t b);

#endif // CANVAS_H
