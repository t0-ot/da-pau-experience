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

void audio_backend_update_music(const MusicResource *resource) {
    if (resource == NULL)
        return;

    if (!IsMusicValid(resource->music_data))
        return;

    UpdateMusicStream(resource->music_data);
}

void audio_backend_play_music(const MusicResource *resource) {
    if (resource == NULL)
        return;
    if (!IsMusicValid(resource->music_data))
        return;

    PlayMusicStream(resource->music_data);
}

void audio_backend_pause_music(const MusicResource *resource) {
    if (resource == NULL)
        return;
    if (!IsMusicValid(resource->music_data))
        return;

    PauseMusicStream(resource->music_data);
}
void audio_backend_resume_music(const MusicResource *resource) {
    if (resource == NULL)
        return;
    if (!IsMusicValid(resource->music_data))
        return;

    ResumeMusicStream(resource->music_data);
}

void audio_backend_stop_music(const MusicResource *resource) {
    if (resource == NULL)
        return;
    if (!IsMusicValid(resource->music_data))
        return;

    StopMusicStream(resource->music_data);
}


void audio_backend_unload_music(MusicResource *resource) {
    if (resource == NULL)
        return;

    UnloadMusicStream(resource->music_data);
    free(resource);
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

void audio_backend_play_sound(const SoundResource *resource) {
    if (resource == NULL)
        return;
    if (!IsSoundValid(resource->sound_data))
        return;

    PlaySound(resource->sound_data);
}

void audio_backend_unload_sound(SoundResource *resource) {
    if (resource == NULL)
        return;

    UnloadSound(resource->sound_data);
    free(resource);
}

void audio_backend_pause_sound(const SoundResource *resource) {
    if (resource == NULL)
        return;
    if (!IsSoundValid(resource->sound_data))
        return;

    PauseSound(resource->sound_data);
}

void audio_backend_resume_sound(const SoundResource *resource) {
    if (resource == NULL)
        return;
    if (!IsSoundValid(resource->sound_data))
        return;

    ResumeSound(resource->sound_data);
}

void audio_backend_stop_sound(const SoundResource *resource) {
    if (resource == NULL)
        return;
    if (!IsSoundValid(resource->sound_data))
        return;

    StopSound(resource->sound_data);
}
