#ifndef RESOURCE_GROUP_MANAGER_H
#define RESOURCE_GROUP_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

/* Opaque resource group manager. */
typedef struct ResourceGroupManager ResourceGroupManager;

/* Identifies a resource ownership group. */
typedef struct {
    uint32_t index;
    uint32_t generation;
} ResourceGroupHandle;

#define RESOURCE_GROUP_INVALID ((ResourceGroupHandle){0, 0})

/* Resource group management. */
ResourceGroupHandle resource_group_manager_create_group(
    ResourceGroupManager *manager
);

bool resource_group_manager_destroy_group(ResourceGroupManager *manager, ResourceGroupHandle group);

#endif
