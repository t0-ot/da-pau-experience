#include "engine/storage/highscore.h"
#include "engine/storage/storage_path.h"
#include "raylib.h"

#include <stdlib.h>
#include <limits.h>

int highscore_load(const char *game_name) {
    if (game_name == NULL || game_name[0] == '\0') {
        return 0;
    }

    char directory[512];
    if (!storage_path_init(directory, sizeof(directory))) {
        return 0;
    }

    const char *file_path = TextFormat("%s/%s.txt", directory, game_name);

    if (!FileExists(file_path)) {
        return 0;
    }

    char *text = LoadFileText(file_path);
    if (text == NULL) {
        return 0;
    }
    char* first_invalid;
    long long score = strtoll(text, &first_invalid, 10);

    if (first_invalid == text || score <  INT_MIN || score > INT_MAX)
        score = 0;

    UnloadFileText(text);

    return (int)score;
}

void highscore_save(const char *game_name, int score) {
    if (game_name == NULL || game_name[0] == '\0' || score < 0) {
        return;
    }

    char directory[512];
    if (!storage_path_init(directory, sizeof(directory))) {
        return;
    }

    const char *file_path = TextFormat("%s/%s.txt", directory, game_name);
    const char *content = TextFormat("%d\n", score);

    SaveFileText(file_path, content);
}
