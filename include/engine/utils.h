#ifndef UTILS_H
#define UTILS_H

#include "engine/canvas.h"

float ease(float time, float startValue, float change, float duration);
int objectPointResize(ViewPointList *data);
int get_centered_x(int left_wall, int right_wall, int text_length);
int get_centered_y(int top_wall, int bottom_wall, int text_length);
int collisionCheckOne(ViewPointList *objView1, ViewPointList *objView2, int countFrom, int checkIndexObj2);

#endif // UTILS_H
