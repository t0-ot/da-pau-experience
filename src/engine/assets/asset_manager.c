
#include <stdlib.h>
#include <string.h>

#include "engine/assets/asset_manager.h"
#include "../resources/resource_group_manager_internal.h"
#include "asset_manager_internal.h"
#include "texture_backend.h"

#define FALLBACK_TEXTURE_DIR "assets/null.png"

/* -------------------------------------------------- */
/* Internal structures                                */
/* -------------------------------------------------- */

typedef struct {
    uint32_t generation;
    bool active;

    char *path;
    TextureResource *resource;

    ResourceGroupHandle owner;
    AssetError load_error;
} TextureSlot;

struct AssetManager {
    ResourceGroupManager *groups;

    TextureSlot *textures;
    TextureResource *fallback_texture;
    size_t texture_count;
    size_t texture_capacity;
};

/* -------------------------------------------------- */
/* Internal helpers                                   */
/* -------------------------------------------------- */

static char *copy_string(const char *source) {
    size_t length = strlen(source) + 1;

    char *copy = malloc(length);

    if (copy != NULL)
        memcpy(copy, source, length);

    return copy;
}

static uint32_t next_generation(uint32_t generation) {
    generation++;

    if (generation == 0)
        generation = 1;

    return generation;
}

static bool texture_is_valid(const AssetManager *manager, TextureHandle texture) {
    if (manager == NULL || texture.generation == 0)
        return false;

    if (texture.index >= manager->texture_count)
        return false;

    const TextureSlot *slot = &manager->textures[texture.index];

    return slot->active && slot->generation == texture.generation;
}

static TextureLoadResult make_error(AssetError error) {
    TextureLoadResult result = {
        .handle = TEXTURE_HANDLE_INVALID,
        .error = error,
        .reused = false
    };

    return result;
}

static bool grow_texture_slots(AssetManager *manager) {
    size_t old_capacity = manager->texture_capacity;

    if (old_capacity > SIZE_MAX / 2)
        return false;

    size_t new_capacity =
        old_capacity == 0 ? 8 : old_capacity * 2;

    if (new_capacity > UINT32_MAX ||
        new_capacity > SIZE_MAX / sizeof(TextureSlot))
        return false;

    TextureSlot *slots = realloc(
        manager->textures,
        new_capacity * sizeof(*slots)
    );

    if (slots == NULL)
        return false;

    manager->textures = slots;

    for (size_t i = old_capacity; i < new_capacity; i++) {
        slots[i] = (TextureSlot){0};
        slots[i].generation = 1;
    }

    manager->texture_capacity = new_capacity;

    return true;
}

/* -------------------------------------------------- */
/* Texture cleanup                                    */
/* -------------------------------------------------- */

static void unload_texture_slot(TextureSlot *slot) {
    if (slot == NULL || !slot->active)
        return;

    texture_backend_unload(slot->resource);

    free(slot->path);

    slot->resource = NULL;
    slot->path = NULL;
    slot->active = false;

    slot->generation = next_generation(slot->generation);
}

static void asset_manager_cleanup_group(void *context, ResourceGroupHandle group) {
    AssetManager *manager = context;

    if (manager == NULL)
        return;

    for (size_t i = 0; i < manager->texture_count; i++) {
        TextureSlot *texture = &manager->textures[i];

        if (!texture->active)
            continue;

        if (texture->owner.index == group.index &&
            texture->owner.generation == group.generation) {
            unload_texture_slot(texture);
        }
    }
}

/* -------------------------------------------------- */
/* Manager lifecycle                                  */
/* -------------------------------------------------- */

AssetManager *asset_manager_create(ResourceGroupManager *groups) {
    if (groups == NULL)
        return NULL;

    AssetManager *manager = calloc(1, sizeof(AssetManager));

    if (manager == NULL)
        return NULL;

    manager->groups = groups;

    if (texture_backend_load(FALLBACK_TEXTURE_DIR, &manager->fallback_texture) != TEXTURE_BACKEND_SUCCESS) {
        free(manager);
        return NULL;
    }

    /* Register for resource group destruction notifications. */
    if (!resource_group_manager_register_cleanup(groups, asset_manager_cleanup_group, manager)) {
        texture_backend_unload(manager->fallback_texture);
        free(manager);
        return NULL;
    }

    return manager;
}

void asset_manager_destroy(AssetManager *manager) {
    if (manager == NULL)
        return;

    resource_group_manager_unregister_cleanup(manager->groups, asset_manager_cleanup_group, manager);

    for (size_t i = 0; i < manager->texture_count; i++)
        unload_texture_slot(&manager->textures[i]);

    texture_backend_unload(manager->fallback_texture);

    free(manager->textures);
    free(manager);
}

/* -------------------------------------------------- */
/* Texture management                                 */
/* -------------------------------------------------- */


TextureLoadResult asset_manager_load_texture(
    AssetManager *manager,
    ResourceGroupHandle group,
    const char *path
) {
    if (manager == NULL || path == NULL || path[0] == '\0')
        return make_error(ASSET_ERROR_INVALID_ARGUMENT);

    if (!resource_group_manager_is_valid(manager->groups, group))
        return make_error(ASSET_ERROR_INVALID_GROUP);

    /* Reuse an existing texture within this group. */
    for (size_t i = 0; i < manager->texture_count; i++) {
        const TextureSlot *slot = &manager->textures[i];

        if (!slot->active)
            continue;

        if (slot->owner.index == group.index &&
            slot->owner.generation == group.generation &&
            strcmp(slot->path, path) == 0) {

            return (TextureLoadResult){
                .handle = {
                    .index = (uint32_t)i,
                    .generation = slot->generation
                },
                .error = slot->load_error,
                .reused = true
            };
        }
    }

    /* Find an available texture slot. */
    size_t index;

    for (index = 0; index < manager->texture_count; index++) {
        if (!manager->textures[index].active)
            break;
    }

    bool new_slot = false;

    if (index == manager->texture_count) {
        if (manager->texture_count == manager->texture_capacity) {
            if (!grow_texture_slots(manager))
                return make_error(ASSET_ERROR_OUT_OF_MEMORY);
        }

        new_slot = true;
    }

    TextureSlot *slot = &manager->textures[index];

    /* Copy the path before loading the resource. */
    char *path_copy = copy_string(path);

    if (path_copy == NULL)
        return make_error(ASSET_ERROR_OUT_OF_MEMORY);

    /* Load through the backend abstraction. */
    TextureResource *resource = NULL;

    TextureBackendResult backend_result =
        texture_backend_load(path, &resource);

    AssetError load_error = ASSET_ERROR_NONE;

    switch (backend_result) {
        case TEXTURE_BACKEND_SUCCESS:
            break;

        case TEXTURE_BACKEND_FILE_NOT_FOUND:
            load_error = ASSET_ERROR_FILE_NOT_FOUND;
            break;

        case TEXTURE_BACKEND_OUT_OF_MEMORY:
            free(path_copy);
            return make_error(ASSET_ERROR_OUT_OF_MEMORY);

        default:
            load_error = ASSET_ERROR_BACKEND;
            break;
    }

    /* Commit the request, even if loading failed recoverably. */
    slot->path = path_copy;
    slot->resource = resource;
    slot->load_error = load_error;
    slot->owner = group;
    slot->active = true;

    if (new_slot)
        manager->texture_count++;

    return (TextureLoadResult){
        .handle = {
            .index = (uint32_t)index,
            .generation = slot->generation
        },
        .error = slot->load_error,
        .reused = false
    };
}

TextureResource *asset_manager_resolve_texture(const AssetManager *manager, TextureHandle texture, bool *is_fallback) {
    if (is_fallback != NULL)
        *is_fallback = false;

    if (!texture_is_valid(manager, texture))
        return NULL;

    const TextureSlot *slot = &manager->textures[texture.index];

    if (slot->resource != NULL)
        return slot->resource;

    if (is_fallback != NULL)
        *is_fallback = true;

    return manager->fallback_texture;
}
