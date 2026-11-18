#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "engine/assets/texture.h"

/* Opaque asset manager. */
typedef struct AssetManager AssetManager;

/* Identifies an asset ownership group. */
typedef struct {
    uint32_t index;
    uint32_t generation;
} AssetGroupHandle;

#define ASSET_GROUP_INVALID ((AssetGroupHandle){0, 0})

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
AssetManager *asset_manager_create(void);
void asset_manager_destroy(AssetManager *manager);

/* Ownership groups. */
AssetGroupHandle asset_manager_create_group(
    AssetManager *manager
);

bool asset_manager_destroy_group(
    AssetManager *manager,
    AssetGroupHandle group
);

/* Texture management. */
TextureLoadResult asset_manager_load_texture(
    AssetManager *manager,
    AssetGroupHandle group,
    const char *path
);

#endif // ASSET_MANAGER_H
