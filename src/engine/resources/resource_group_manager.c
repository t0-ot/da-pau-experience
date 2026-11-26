#include "engine/resources/resource_group_manager.h"
#include "resource_group_manager_internal.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    ResourceGroupCleanupCallback callback;
    void *context;
} ResourceGroupCleanupEntry;

typedef struct {
    uint32_t generation;
    bool active;
} ResourceGroupSlot;

struct ResourceGroupManager {
    ResourceGroupSlot *groups;
    size_t group_count;
    size_t group_capacity;

    ResourceGroupCleanupEntry *callbacks;
    size_t callback_count;
    size_t callback_capacity;
};

/* -------------------------------------------------- */
/* Internal helpers                                   */
/* -------------------------------------------------- */

bool resource_group_manager_is_valid(const ResourceGroupManager *manager, ResourceGroupHandle group) {
    if (manager == NULL || group.generation == 0)
        return false;

    if (group.index >= manager->group_count)
        return false;

    const ResourceGroupSlot *slot = &manager->groups[group.index];

    return slot->active && slot->generation == group.generation;
}

static uint32_t next_generation(uint32_t generation) {
    generation++;

    if (generation == 0)
        generation = 1;

    return generation;
}

/* -------------------------------------------------- */
/* Slot allocation                                    */
/* -------------------------------------------------- */

static bool grow_group_slots(ResourceGroupManager *manager) {
    size_t old_capacity = manager->group_capacity;

    if (old_capacity > SIZE_MAX / 2)
        return false;

    size_t new_capacity =
        old_capacity == 0 ? 4 : old_capacity * 2;

    if (new_capacity > UINT32_MAX ||
        new_capacity > SIZE_MAX / sizeof(ResourceGroupSlot))
        return false;

    ResourceGroupSlot *slots = realloc(
        manager->groups,
        new_capacity * sizeof(*slots)
    );

    if (slots == NULL)
        return false;

    manager->groups = slots;

    for (size_t i = old_capacity; i < new_capacity; i++) {
        slots[i] = (ResourceGroupSlot){
            .generation = 1,
            .active = false
        };
    }

    manager->group_capacity = new_capacity;

    return true;
}

static bool grow_cleanup_callbacks(ResourceGroupManager *manager) {
    size_t old_capacity = manager->callback_capacity;

    if (old_capacity > SIZE_MAX / 2)
        return false;

    size_t new_capacity =
        old_capacity == 0 ? 4 : old_capacity * 2;

    if (new_capacity > SIZE_MAX / sizeof(ResourceGroupCleanupEntry))
        return false;

    ResourceGroupCleanupEntry *callbacks = realloc(
        manager->callbacks,
        new_capacity * sizeof(*callbacks)
    );

    if (callbacks == NULL)
        return false;

    manager->callbacks = callbacks;
    manager->callback_capacity = new_capacity;

    return true;
}

/* -------------------------------------------------- */
/* Ownership groups                                   */
/* -------------------------------------------------- */

ResourceGroupHandle resource_group_manager_create_group(ResourceGroupManager *manager) {
    if (manager == NULL)
        return RESOURCE_GROUP_INVALID;

    size_t index;

    for (index = 0; index < manager->group_count; index++) {
        if (!manager->groups[index].active)
            break;
    }

    if (index == manager->group_count) {
        if (manager->group_count == manager->group_capacity) {
            if (!grow_group_slots(manager))
                return RESOURCE_GROUP_INVALID;
        }

        manager->group_count++;
    }

    ResourceGroupSlot *slot = &manager->groups[index];

    slot->active = true;

    ResourceGroupHandle handle = {
        .index = (uint32_t)index,
        .generation = slot->generation
    };

    return handle;
}

bool resource_group_manager_destroy_group(ResourceGroupManager *manager, ResourceGroupHandle group) {
    if (!resource_group_manager_is_valid(manager, group))
        return false;

    /* Notify every registered resource manager. */
    for (size_t i = 0; i < manager->callback_count; i++) {
        ResourceGroupCleanupEntry *entry = &manager->callbacks[i];

        entry->callback(entry->context, group);
    }

    /* Invalidate the group after its resources are cleaned up. */
    ResourceGroupSlot *slot = &manager->groups[group.index];

    slot->active = false;
    slot->generation = next_generation(slot->generation);

    return true;
}

ResourceGroupManager *resource_group_manager_create(void) {
    return calloc(1, sizeof(ResourceGroupManager));
}

void resource_group_manager_destroy(ResourceGroupManager *manager) {
    if (manager == NULL)
        return;

    free(manager->groups);
    free(manager->callbacks);
    free(manager);
}

bool resource_group_manager_register_cleanup(ResourceGroupManager *manager, ResourceGroupCleanupCallback callback, void *context) {
    if (manager == NULL || callback == NULL)
        return false;

    if (manager->callback_count == manager->callback_capacity) {
        if (!grow_cleanup_callbacks(manager))
            return false;
    }

    manager->callbacks[manager->callback_count++] = (ResourceGroupCleanupEntry){.callback = callback, .context = context};

    return true;
}

void resource_group_manager_unregister_cleanup(ResourceGroupManager *manager, ResourceGroupCleanupCallback callback, const void *context) {
    if (manager == NULL || callback == NULL)
        return;

    for (size_t i = 0; i < manager->callback_count; i++) {
        const ResourceGroupCleanupEntry *entry = &manager->callbacks[i];

        if (entry->callback == callback && entry->context == context) {
            manager->callbacks[i] =
                manager->callbacks[manager->callback_count - 1];

            manager->callback_count--;
            return;
        }
    }
}
