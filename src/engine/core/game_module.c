#include "engine/core/game_module.h"

#include <stdio.h>

static const char *module_result_to_string(const ModuleResult result) {
    switch (result) {
        case MODULE_INIT_SUCCESS:
            return "Initialization successful";

        case MODULE_INIT_ERROR_STATE_DATA:
            return "Invalid module state";

        case MODULE_INIT_ERROR_ALLOCATION:
            return "Memory allocation failed";

        case MODULE_INIT_ERROR_RESOURCE_LOAD:
            return "Failed to load module resources";

        case MODULE_INIT_ERROR_INITIALIZATION:
            return "Module initialization failed";

        default:
            return "Unknown module initialization error";
    }
}

void module_print_init_error(ModuleResult result, const char *error_message) {
    fprintf(stderr, "[MODULE_INIT_ERROR] %s", module_result_to_string(result));

    if (error_message != NULL && error_message[0] != '\0')
        fprintf(stderr, " (%s)", error_message);

    fprintf(stderr, "\n");
}
