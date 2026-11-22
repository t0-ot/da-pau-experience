#ifndef TEXTURE_BACKEND_H
#define TEXTURE_BACKEND_H

#include "raylib.h"

typedef struct TextureResource TextureResource;

typedef enum {
    TEXTURE_BACKEND_SUCCESS,
    TEXTURE_BACKEND_FILE_NOT_FOUND,
    TEXTURE_BACKEND_OUT_OF_MEMORY,
    TEXTURE_BACKEND_FAILURE
} TextureBackendResult;

TextureBackendResult texture_backend_load(const char *path, TextureResource **out_resource);

void texture_backend_unload(TextureResource *resource);

void texture_backend_draw(const TextureResource *resource, Rectangle destination);

void texture_backend_draw_region(const TextureResource *resource, Rectangle source, Rectangle destination, Vector2 origin, float rotation);

#endif // TEXTURE_BACKEND_H
