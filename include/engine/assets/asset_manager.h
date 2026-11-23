#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "engine/resources/resource_group_manager.h"
#include "engine/assets/texture.h"

/* Opaque asset manager. */
typedef struct AssetManager AssetManager;

/* Errors encountered while loading a texture. */
typedef enum {
    ASSET_ERROR_NONE,
    ASSET_ERROR_INVALID_ARGUMENT,
    ASSET_ERROR_INVALID_GROUP,
    ASSET_ERROR_FILE_NOT_FOUND,
    ASSET_ERROR_OUT_OF_MEMORY,
    ASSET_ERROR_BACKEND
} AssetError;

/* Result returned when loading a texture. */
typedef struct {
    TextureHandle handle;
    AssetError error;
    bool reused;
} TextureLoadResult;

/* Manager lifecycle. */
AssetManager *asset_manager_create(ResourceGroupManager *groups);
void asset_manager_destroy(AssetManager *manager);

/* Texture management. */
TextureLoadResult asset_manager_load_texture(
    AssetManager *manager,
    ResourceGroupHandle group,
    const char *path
);

#endif // ASSET_MANAGER_H
