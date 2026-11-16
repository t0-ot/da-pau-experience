
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "engine/assets/asset_manager.h"
#include "asset_manager_internal.h"
#include "texture_backend.h"

/* -------------------------------------------------- */
/* Internal structures                                */
/* -------------------------------------------------- */

typedef struct {
    uint32_t generation;
    bool active;

    char *path;
    TextureResource *resource;

    AssetGroupHandle owner;
} TextureSlot;

typedef struct {
    uint32_t generation;
    bool active;
} AssetGroupSlot;

struct AssetManager {
    TextureSlot *textures;
    size_t texture_count;
    size_t texture_capacity;

    AssetGroupSlot *groups;
    size_t group_count;
    size_t group_capacity;
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

static bool group_is_valid(
    const AssetManager *manager,
    AssetGroupHandle group
) {
    if (manager == NULL || group.generation == 0)
        return false;

    if (group.index >= manager->group_count)
        return false;

    const AssetGroupSlot *slot = &manager->groups[group.index];

    return slot->active && slot->generation == group.generation;
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

/* -------------------------------------------------- */
/* Slot allocation                                    */
/* -------------------------------------------------- */

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

static bool grow_group_slots(AssetManager *manager) {
    size_t old_capacity = manager->group_capacity;

    if (old_capacity > SIZE_MAX / 2)
        return false;

    size_t new_capacity =
        old_capacity == 0 ? 4 : old_capacity * 2;

    if (new_capacity > UINT32_MAX ||
        new_capacity > SIZE_MAX / sizeof(AssetGroupSlot))
        return false;

    AssetGroupSlot *slots = realloc(
        manager->groups,
        new_capacity * sizeof(*slots)
    );

    if (slots == NULL)
        return false;

    manager->groups = slots;

    for (size_t i = old_capacity; i < new_capacity; i++) {
        slots[i] = (AssetGroupSlot){
            .generation = 1,
            .active = false
        };
    }

    manager->group_capacity = new_capacity;

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

/* -------------------------------------------------- */
/* Manager lifecycle                                  */
/* -------------------------------------------------- */

AssetManager *asset_manager_create(void) {
    return calloc(1, sizeof(AssetManager));
}

void asset_manager_destroy(AssetManager *manager) {
    if (manager == NULL)
        return;

    for (size_t i = 0; i < manager->texture_count; i++)
        unload_texture_slot(&manager->textures[i]);

    free(manager->textures);
    free(manager->groups);
    free(manager);
}

/* -------------------------------------------------- */
/* Ownership groups                                   */
/* -------------------------------------------------- */

AssetGroupHandle asset_manager_create_group(AssetManager *manager) {
    if (manager == NULL)
        return ASSET_GROUP_INVALID;

    size_t index;

    for (index = 0; index < manager->group_count; index++) {
        if (!manager->groups[index].active)
            break;
    }

    if (index == manager->group_count) {
        if (manager->group_count == manager->group_capacity) {
            if (!grow_group_slots(manager))
                return ASSET_GROUP_INVALID;
        }

        manager->group_count++;
    }

    AssetGroupSlot *slot = &manager->groups[index];

    slot->active = true;

    AssetGroupHandle handle = {
        .index = (uint32_t)index,
        .generation = slot->generation
    };

    return handle;
}

bool asset_manager_destroy_group(AssetManager *manager, AssetGroupHandle group) {
    if (!group_is_valid(manager, group))
        return false;

    for (size_t i = 0; i < manager->texture_count; i++) {
        TextureSlot *texture = &manager->textures[i];

        if (!texture->active)
            continue;

        if (texture->owner.index == group.index &&
            texture->owner.generation == group.generation) {
            unload_texture_slot(texture);
        }
    }

    AssetGroupSlot *slot = &manager->groups[group.index];

    slot->active = false;
    slot->generation = next_generation(slot->generation);

    return true;
}

/* -------------------------------------------------- */
/* Texture management                                 */
/* -------------------------------------------------- */

TextureLoadResult asset_manager_load_texture(AssetManager *manager, AssetGroupHandle group, const char *path) {
    if (manager == NULL || path == NULL || path[0] == '\0')
        return make_error(ASSET_ERROR_INVALID_ARGUMENT);

    if (!group_is_valid(manager, group))
        return make_error(ASSET_ERROR_INVALID_GROUP);

    /* Reuse an existing texture within this group. */
    for (size_t i = 0; i < manager->texture_count; i++) {
        TextureSlot *slot = &manager->textures[i];

        if (!slot->active)
            continue;

        if (slot->owner.index == group.index &&
            slot->owner.generation == group.generation &&
            strcmp(slot->path, path) == 0) {

            TextureLoadResult result = {
                .handle = {
                    .index = (uint32_t)i,
                    .generation = slot->generation
                },
                .error = ASSET_ERROR_NONE,
                .reused = true
            };

            return result;
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

    TextureBackendResult backend_result = texture_backend_load(path, &resource);

    if (backend_result != TEXTURE_BACKEND_SUCCESS) {
        free(path_copy);

        if (backend_result == TEXTURE_BACKEND_FILE_NOT_FOUND)
            return make_error(ASSET_ERROR_FILE_NOT_FOUND);

        if (backend_result == TEXTURE_BACKEND_OUT_OF_MEMORY)
            return make_error(ASSET_ERROR_OUT_OF_MEMORY);

        return make_error(ASSET_ERROR_BACKEND);
    }

    /* Commit the successfully loaded texture to its slot. */
    slot->path = path_copy;
    slot->resource = resource;
    slot->owner = group;
    slot->active = true;

    if (new_slot)
        manager->texture_count++;

    TextureLoadResult result = {
        .handle = {
            .index = (uint32_t)index,
            .generation = slot->generation
        },
        .error = ASSET_ERROR_NONE,
        .reused = false
    };

    return result;
}

bool asset_manager_is_texture_valid(const AssetManager *manager, TextureHandle texture) {
    return texture_is_valid(manager, texture);
}

TextureResource *asset_manager_resolve_texture(const AssetManager *manager, TextureHandle texture) {
    if (!texture_is_valid(manager, texture))
        return NULL;

    return manager->textures[texture.index].resource;
}
