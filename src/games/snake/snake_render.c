#include "games/snake/snake_render.h"
#include "games/snake/snake_logic.h"

void snakeStamper(void *data, CanvasBuffer *canvas) {
    snakeData *p = (snakeData *)data;
    for (int i = p->pointList.amt - 1; i >= 0; i--) {
        int x = p->pointList.points[i].posX;
        int y = p->pointList.points[i].posY;
        char sym = (i == 0) ? 'M' : 'O';

        // Use your cellColorAssignment helper with the session's canvas!
        cellColorAssignment(canvas, x, y, sym, 0, 255, 0);
    }
}

void fruitStamper(void *data, CanvasBuffer *canvas) {
    fruitData *p = (fruitData *)data;
    for (int i = 0; i < p->pointList.amt; i++) {
        int x = p->pointList.points[i].posX;
        int y = p->pointList.points[i].posY;

        cellColorAssignment(canvas, x, y, p->visuals.sym,
                            p->visuals.color.r,
                            p->visuals.color.g,
                            p->visuals.color.b);
    }
}
