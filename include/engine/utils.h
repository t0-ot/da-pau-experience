/**
 * @file utils.h
 * @brief Declares shared utility functions.
 *
 * Provides reusable helper functions used across the application.
 */

#ifndef UTILS_H
#define UTILS_H

#include "engine/core/game_types.h"

/**
 * @brief Calculates an eased value using a quadratic ease-in-out function.
 *
 * Interpolates from the starting value to the ending value over the
 * specified duration, accelerating during the first half and
 * decelerating during the second half.
 *
 * @param time Current elapsed time.
 * @param startValue Starting value of the interpolation.
 * @param change Total difference between the starting and ending values.
 * @param duration Total duration of the interpolation.
 * @return The interpolated value at the specified time.
 */
float ease(float time, float startValue, float change, float duration);

/**
 * @brief Resizes the allocated memory of a Point_list.
 *
 * Adjusts the points array to match the list's current amount.
 * If the amount is zero or negative, the allocated memory is freed
 * and the pointer is set to NULL.
 *
 * @param data Pointer to the Point_list to resize.
 * @return 1 when the operation completes successfully.
 */
int objectPointResize(Point_list *data);

/**
 * @brief Calculates the Manhattan distance between two points.
 *
 * Calculates the sum of the absolute differences between the
 * coordinates and multiplies the result by 10.
 *
 * @param current_coord Pointer to the first point.
 * @param target_coord Pointer to the second point.
 * @return The calculated distance, scaled by a factor of 10.
 */
int calc_point_dist(Point *current_coord, Point *target_coord);

/**
 * @brief Calculates the horizontal position for centered text.
 *
 * @param left_wall Left boundary of the region.
 * @param right_wall Right boundary of the region.
 * @param text_length Width of the text.
 * @return The horizontal coordinate at which the text should begin.
 */
int get_centered_x(int left_wall, int right_wall, int text_length);

/**
 * @brief Calculates the vertical position for centered text.
 *
 * @param top_wall Top boundary of the region.
 * @param bottom_wall Bottom boundary of the region.
 * @param text_length Height of the text.
 * @return The vertical coordinate at which the text should begin.
 */
int get_centered_y(int top_wall, int bottom_wall, int text_length);

/**
 * @brief Checks whether a point in the second list overlaps a point in the first list.
 *
 * Searches the first list starting at the specified index and compares
 * its points against the selected point in the second list.
 * If both pointers refer to the same list, the selected point is skipped.
 *
 * @param objView1 Point list to search.
 * @param objView2 Point list containing the point to check.
 * @param countFrom Index at which the search in the first list begins.
 * @param checkIndexObj2 Index of the point in the second list to check.
 * @return The matching index in the first list plus one, or 0 if no match is found.
 */
int collisionCheckOne(Point_list *objView1, Point_list *objView2, int countFrom, int checkIndexObj2);

/**
 * @brief Checks for overlapping points between two lists.
 *
 * Compares the points in both lists, beginning the search in the first
 * list at the specified index.
 *
 * @param objView1 First point list.
 * @param objView2 Second point list.
 * @param countFrom Index at which the search in the first list begins.
 * @return The matching index in the first list plus one, or 0 if no match is found.
 */
int collision_check_both(Point_list *objView1, Point_list *objView2, int countFrom);

#endif // UTILS_H
