#include "engine/storage/storage_path.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

/**
 * @brief Creates a directory if it does not already exist.
 *
 * @param path Path of the directory to create.
 * @return true if the directory exists or was created successfully.
 */
static bool create_directory(const char *path) {
#ifdef _WIN32
    if (CreateDirectoryA(path, NULL)) {
        return true;
    }

    return GetLastError() == ERROR_ALREADY_EXISTS;

#else
    if (mkdir(path, 0755) == 0) {
        return true;
    }

    return errno == EEXIST;
#endif
}


/**
 * @brief Retrieves the base application data directory.
 *
 * @return The base directory path, or NULL if unavailable.
 */
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

    /* Linux fallback handled by the caller. */
    return home;
#endif
}

/**
 * @brief Creates a directory and any missing parent directories.
 *
 * @param path Directory path to create.
 * @return true if the directory hierarchy exists or was created.
 */
static bool create_directory_recursive(const char *path) {
    char buffer[512];

    if (strlen(path) >= sizeof(buffer)) {
        return false;
    }

    strcpy(buffer, path);

    char *start = buffer + 1;

    #ifdef _WIN32
    if (buffer[1] == ':') {
        start = buffer + 3;
    }
    #endif

    for (char *p = start; *p != '\0'; p++) {
        if (*p == '/' || *p == '\\') {
            char separator = *p;
            *p = '\0';

            if (!create_directory(buffer)) {
                return false;
            }

            *p = separator;
        }
    }

    return create_directory(buffer);
}

bool storage_path_init(char *buffer, size_t buffer_size) {
    int length = 0;
    if (buffer == NULL || buffer_size == 0) {
        return false;
    }

    const char *base_directory = get_base_directory();

    if (base_directory == NULL) {
        return false;
    }

    char full_path[512];

#ifdef _WIN32
    length = snprintf(
        full_path,
        sizeof(full_path),
        "%s\\DaPau",
        base_directory
    );
#else
    const char *xdg_data_home = getenv("XDG_DATA_HOME");

    if (xdg_data_home != NULL && xdg_data_home[0] != '\0') {
        int length = snprintf(
            full_path,
            sizeof(full_path),
            "%s/da-pau",
            base_directory
        );

        if (length < 0 || (size_t)length >= sizeof(full_path)) {
            return false;
        }
    } else {
        length = snprintf(
            full_path,
            sizeof(full_path),
            "%s/.local/share/da-pau",
            base_directory
        );

        if (length < 0 || (size_t)length >= sizeof(full_path)) {
            return false;
        }
    }
#endif

    if (!create_directory_recursive(full_path)) {
        return false;
    }

    length = snprintf(buffer, buffer_size, "%s", full_path);

    return length >= 0 && (size_t)length < buffer_size;
}
