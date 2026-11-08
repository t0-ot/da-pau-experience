#ifndef UTILS_H
#define UTILS_H

#include "engine/canvas.h"

float ease(float time, float startValue, float change, float duration);
int objectPointResize(Point_list *data);
int calc_point_dist(Point *current_coord, Point *target_coord);
int get_centered_x(int left_wall, int right_wall, int text_length);
int get_centered_y(int top_wall, int bottom_wall, int text_length);
int collisionCheckOne(Point_list *objView1, Point_list *objView2, int countFrom, int checkIndexObj2);
int collision_check_both(Point_list *objView1, Point_list *objView2, int countFrom);

#endif // UTILS_H
