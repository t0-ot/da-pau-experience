#include "engine/entity.h"
#include "games/snake/snake_render.h"
#include "games/snake/snake_logic.h"

void snakeStamper(void *data, Canvas_buffer *canvas) {
    Segmented_entity *p = (Segmented_entity *)data;
    for (int i = p->segments.amt - 1; i >= 0; i--) {
        int x = p->segments.points[i].posX;
        int y = p->segments.points[i].posY;
        char sym = (i == 0) ? 'M' : 'O';

        // Use cellColorAssignment helper with the session's canvas
        cell_color_assign(canvas, x, y, sym, 0, 255, 0);
    }
}

void fruitStamper(void *data, Canvas_buffer *canvas) {
    fruitData *p = (fruitData *)data;
    for (int i = 0; i < p->pointList.amt; i++) {
        int x = p->pointList.points[i].posX;
        int y = p->pointList.points[i].posY;

        cell_color_assign(canvas, x, y, p->visuals.sym,
                            p->visuals.color.r,
                            p->visuals.color.g,
                            p->visuals.color.b);
    }
}
