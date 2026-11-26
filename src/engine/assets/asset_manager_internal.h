#ifndef ASSET_MANAGER_INTERNAL_H
#define ASSET_MANAGER_INTERNAL_H

#include "engine/assets/asset_manager.h"
#include "texture_backend.h"

/* Manager lifecycle. */
AssetManager *asset_manager_create(ResourceGroupManager *groups);
void asset_manager_destroy(AssetManager *manager);

TextureResource *asset_manager_resolve_texture(const AssetManager *manager, TextureHandle texture, bool *is_fallback);

#endif // ASSET_MANAGER_INTERNAL_H
