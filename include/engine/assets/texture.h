#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct {
    uint32_t index;
    uint32_t generation;
} TextureHandle;

#define TEXTURE_HANDLE_INVALID ((TextureHandle){0, 0})

#endif // TEXTURE_H
