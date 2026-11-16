#include <stdlib.h>
#include "raylib.h"
#include "texture_backend.h"

struct TextureResource {
    Texture2D texture;
};

TextureBackendResult texture_backend_load(
    const char *path,
    TextureResource **out_resource
) {
    if (path == NULL || out_resource == NULL)
        return TEXTURE_BACKEND_FAILURE;

    *out_resource = NULL;

    if (!FileExists(path))
        return TEXTURE_BACKEND_FILE_NOT_FOUND;

    TextureResource *resource = malloc(sizeof(*resource));

    if (resource == NULL)
        return TEXTURE_BACKEND_OUT_OF_MEMORY;

    resource->texture = LoadTexture(path);

    if (resource->texture.id == 0) {
        free(resource);
        return TEXTURE_BACKEND_FAILURE;
    }

    *out_resource = resource;

    return TEXTURE_BACKEND_SUCCESS;
}

void texture_backend_unload(TextureResource *resource) {
    if (resource == NULL)
        return;

    UnloadTexture(resource->texture);
    free(resource);
}

void texture_backend_draw(
    const TextureResource *resource,
    float x,
    float y,
    float width,
    float height
) {
    if (resource == NULL || width <= 0 || height <= 0)
        return;

    Rectangle source = {
        0,
        0,
        (float)resource->texture.width,
        (float)resource->texture.height
    };

    Rectangle destination = {
        x,
        y,
        width,
        height
    };

    DrawTexturePro(
        resource->texture,
        source,
        destination,
        (Vector2){0, 0},
        0.0f,
        WHITE
    );
}
