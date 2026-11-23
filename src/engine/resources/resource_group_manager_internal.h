#ifndef RESOURCE_GROUP_MANAGER_INTERNAL_H
#define RESOURCE_GROUP_MANAGER_INTERNAL_H

#include "engine/resources/resource_group_manager.h"

typedef void (*ResourceGroupCleanupCallback)(
    void *context,
    ResourceGroupHandle group
);

bool resource_group_manager_is_valid(
    const ResourceGroupManager *manager,
    ResourceGroupHandle group
);

bool resource_group_manager_register_cleanup(
    ResourceGroupManager *manager,
    ResourceGroupCleanupCallback callback,
    void *context
);

void resource_group_manager_unregister_cleanup(
    ResourceGroupManager *manager,
    ResourceGroupCleanupCallback callback,
    void *context
);

#endif //RESOURCE_GROUP_MANAGER_INTERNAL_H
