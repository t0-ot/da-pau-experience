/**
 * @file renderer.h
 * @brief Declares the application's rendering and window management functions.
 *
 * Provides the interface for initializing the rendering environment,
 * managing drawing frames, and closing the application window.
 */

#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

/**
 * @brief Initializes the application's rendering environment.
 *
 * Creates the application window using the specified dimensions and title.
 *
 * @param width Width of the application window in pixels.
 * @param height Height of the application window in pixels.
 * @param title Title displayed on the application window.
 * @return true if initialization succeeds, false otherwise.
 */
bool renderer_init(int width, int height, const char *title);

/**
 * @brief Closes the application window and rendering environment.
 */
void renderer_close(void);

/**
 * @brief Checks whether the application window should close.
 *
 * @return true if the window should close, false otherwise.
 */
bool renderer_should_close(void);

/**
 * @brief Begins a new rendering frame.
 *
 * Starts the drawing process, clears the background using the
 * application's default dark color, and draws the pink window border.
 */
void renderer_begin_frame(void);

/**
 * @brief Ends the current rendering frame.
 *
 * Completes the drawing process and presents the rendered frame.
 */
void renderer_end_frame(void);

/**
 * @brief Retrieves the time elapsed during the previous frame.
 *
 * @return The frame time in seconds.
 */
float renderer_get_delta_time(void);

#endif // RENDERER_H
