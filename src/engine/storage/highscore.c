#include "engine/storage/highscore.h"
#include "engine/storage/storage_path.h"

#include <stdio.h>
#include <string.h>

int highscore_load(const char *game_name) {
    if (game_name == NULL || game_name[0] == '\0') {
        return 0;
    }

    char directory[512];

    if (!storage_path_init(directory, sizeof(directory))) {
        return 0;
    }

    char file_path[512];

    int length = snprintf(file_path, sizeof(file_path), "%s/%s.txt", directory, game_name);

    if (length < 0 || (size_t)length >= sizeof(file_path)) {
        return 0;
    }

    FILE *file = fopen(file_path, "r");

    if (file == NULL) {
        return 0;
    }

    int score = 0;

    if (fscanf(file, "%d", &score) != 1 || score < 0) {
        score = 0;
    }

    fclose(file);

    return score;
}

void highscore_save(const char *game_name, int score) {
    if (game_name == NULL || game_name[0] == '\0' || score < 0) {
        return;
    }

    char directory[512];

    if (!storage_path_init(directory, sizeof(directory))) {
        return;
    }

    char file_path[512];

    int length = snprintf(
        file_path,
        sizeof(file_path),
        "%s/%s.txt",
        directory,
        game_name
    );

    if (length < 0 || (size_t)length >= sizeof(file_path)) {
        return;
    }

    FILE *file = fopen(file_path, "w");

    if (file == NULL) {
        return;
    }

    fprintf(file, "%d\n", score);

    fclose(file);
}
