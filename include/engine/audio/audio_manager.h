#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "engine/resources/resource_group_manager.h"
#include "engine/audio/music.h"
#include "engine/audio/sound.h"

/* Opaque audio manager. */
typedef struct AudioManager AudioManager;

/* Errors encountered while loading a piece of audio. */
typedef enum {
    AUDIO_ERROR_NONE,
    AUDIO_ERROR_INVALID_ARGUMENT,
    AUDIO_ERROR_INVALID_GROUP,
    AUDIO_ERROR_FILE_NOT_FOUND,
    AUDIO_ERROR_OUT_OF_MEMORY,
    AUDIO_ERROR_BACKEND
} AudioError;

/* Result returned from loading audio */
typedef struct {
    MusicHandle handle;
    AudioError error;
    bool reused;
} MusicLoadResult;

typedef struct {
    SoundHandle handle;
    AudioError error;
    bool reused;
} SoundLoadResult;

/* Audio manager lifecycle. */
AudioManager *audio_manager_create(ResourceGroupManager *groups);
void audio_manager_update(AudioManager *manager);
void audio_manager_destroy(AudioManager *manager);

/* Sound Management */
MusicLoadResult audio_manager_load_music(AudioManager *manager, ResourceGroupHandle group, const char *path);
bool audio_manager_play_sound(AudioManager *manager, SoundHandle sound);

/* Music Management */
SoundLoadResult audio_manager_load_sound(AudioManager *manager, ResourceGroupHandle group, const char *path);
bool audio_manager_play_music(AudioManager *manager, MusicHandle music);
bool audio_manager_pause_music(AudioManager *manager);
bool audio_manager_resume_music(AudioManager *manager);
bool audio_manager_stop_music(AudioManager *manager);

#endif // AUDIO_MANAGER_H
