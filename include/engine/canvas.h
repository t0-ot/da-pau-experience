#ifndef CANVAS_H
#define CANVAS_H

#include <stdint.h>
#include <stdbool.h>
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
} Point_list;

typedef struct {
    char sym;
    RGB color;
} Canvas;

typedef struct {
    int width;
    int height;
    Canvas *cells;
} Canvas_buffer;

typedef void(*Stamp_func)(void *data, Canvas_buffer *canvas);

typedef struct {
    void *data;
    Stamp_func stamp;
} RenderObj;

typedef struct {
    char *text;
    int8_t startX;
    int8_t startY;
    RGB color[MAX_STATES];
} Text_layout;

typedef struct {
    int16_t score;
    int8_t x;
    int8_t y;
} ScoreText;

typedef struct {
    Text_layout textData;
    bool isSelected;
} Option_text;

typedef struct {
    Option_text *option_choice;
    int option_amt;
} Option_list;


static inline Canvas *canvas_get(Canvas_buffer *canvas, int x, int y) {
    return &canvas->cells[y * canvas->width + x];
}
Canvas_buffer *canvas_create(int width, int height);
void canvas_destroy(Canvas_buffer *canvas);
void canvas_intialize(Canvas_buffer *canvas, int dividerPos);
void render_frame(Canvas_buffer *canvas, RenderObj* screenRenderList[], int totalRenderList, int div_pos);
void opt_choice_stamper(void *data, Canvas_buffer *canvas);
void gradient_text_stamper(void *data, Canvas_buffer *canvas);
void text_stamper(void *data, Canvas_buffer *canvas);
void score_stamper(void *data, Canvas_buffer *canvas);
void canvas_print(Canvas_buffer *canvas);
void cell_color_assign(Canvas_buffer *canvas, int x, int y, char ch, uint8_t r, uint8_t g, uint8_t b);

#endif // CANVAS_H
