#include "audio_backend.h"
#include "raylib.h"

#include <stdlib.h>

struct MusicResource {
    Music music_data;
};

struct SoundResource {
    Sound sound_data;
};

/* -------------------------------------------------- */
/* Music Related Func                                 */
/* -------------------------------------------------- */
AudioBackendResult audio_backend_load_music(const char *path, MusicResource **out_resource) {
    if (path == NULL || path[0] == '\0' || out_resource == NULL)
        return AUDIO_BACKEND_FAILURE;

    *out_resource = NULL;

    if (!FileExists(path))
        return AUDIO_BACKEND_FILE_NOT_FOUND;

    MusicResource *resource = malloc(sizeof(*resource));

    if (resource == NULL)
        return AUDIO_BACKEND_OUT_OF_MEMORY;

    resource->music_data = LoadMusicStream(path);

    if (!IsMusicValid(resource->music_data)) {
        free(resource);
        return AUDIO_BACKEND_FAILURE;
    }

    *out_resource = resource;

    return AUDIO_BACKEND_SUCCESS;
}

void audio_backend_unload_music(MusicResource *resource) {

}

/* -------------------------------------------------- */
/* Sound Related Func                                 */
/* -------------------------------------------------- */
AudioBackendResult audio_backend_load_sound(const char *path, SoundResource **out_resource) {
    if (path == NULL || path[0] == '\0' || out_resource == NULL)
        return AUDIO_BACKEND_FAILURE;

    *out_resource = NULL;

    if (!FileExists(path))
        return AUDIO_BACKEND_FILE_NOT_FOUND;

    SoundResource *resource = malloc(sizeof(*resource));

    if (resource == NULL)
        return AUDIO_BACKEND_OUT_OF_MEMORY;

    resource->sound_data = LoadSound(path);

    if (!IsSoundValid(resource->sound_data)) {
        free(resource);
        return AUDIO_BACKEND_FAILURE;
    }

    *out_resource = resource;

    return AUDIO_BACKEND_SUCCESS;
}
