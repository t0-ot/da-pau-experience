#include "engine/storage/storage_path.h"
#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>

static const char *get_base_directory(void) {
#ifdef _WIN32
    return getenv("APPDATA");
#else
    const char *xdg_data_home = getenv("XDG_DATA_HOME");
    if (xdg_data_home != NULL && xdg_data_home[0] != '\0') {
        return xdg_data_home;
    }

    const char *home = getenv("HOME");
    if (home == NULL || home[0] == '\0') {
        return NULL;
    }

    return home;
#endif
}

bool storage_path_init(char *buffer, size_t buffer_size) {
    if (buffer == NULL || buffer_size == 0) {
        return false;
    }

    const char *base_directory = get_base_directory();
    if (base_directory == NULL) {
        return false;
    }

    int length = 0;

#ifdef _WIN32
    length = snprintf(buffer, buffer_size, "%s/DaPau", base_directory);
#else
    const char *xdg_data_home = getenv("XDG_DATA_HOME");
    if (xdg_data_home != NULL && xdg_data_home[0] != '\0') {
        length = snprintf(buffer, buffer_size, "%s/da-pau", base_directory);
    } else {
        length = snprintf(buffer, buffer_size, "%s/.local/share/da-pau", base_directory);
    }
#endif

    if (length < 0 || (size_t)length >= buffer_size) {
        return false;
    }

    // Raylib creates intermediate directories recursively and returns 0 on success
    return MakeDirectory(buffer) == 0;
}
