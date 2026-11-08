#ifndef STORAGE_PATH_H
#define STORAGE_PATH_H

/**
 * @file storage_path.h
 * @brief Provides functions for resolving and creating the application's data directory.
 */

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Resolves and creates the application's data directory.
 *
 * @param buffer Destination buffer for the resulting directory path.
 * @param buffer_size Capacity of the destination buffer.
 * @return true if the directory is available, false otherwise.
 */
bool storage_path_init(char *buffer, size_t buffer_size);

#endif // STORAGE_PATH_H
