#ifndef ASSET_MANAGER_INTERNAL_H
#define ASSET_MANAGER_INTERNAL_H

#include "engine/assets/asset_manager.h"
#include "texture_backend.h"

TextureResource *asset_manager_resolve_texture(
    const AssetManager *manager,
    TextureHandle texture
);

#endif // ASSET_MANAGER_INTERNAL_H
