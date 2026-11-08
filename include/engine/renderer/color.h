#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include <stdint.h>

/**
 * @brief Represents a color using red, green, and blue components.
 */
typedef struct {
    uint8_t r; /**< Red component (0–255). */
    uint8_t g; /**< Green component (0–255). */
    uint8_t b; /**< Blue component (0–255). */
} RGB;

#endif // COLOR_H_INCLUDED
