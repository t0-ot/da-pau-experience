#include "engine/audio/audio_manager.h"
#include "../resources/resource_group_manager_internal.h"
#include "audio_backend.h"

#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------- */
/* Internal structures                                */
/* -------------------------------------------------- */

typedef enum {
    MUSIC_STATE_STOPPED,
    MUSIC_STATE_PLAYING,
    MUSIC_STATE_PAUSED
} MusicPlaybackState;

typedef struct {
    uint32_t generation;
    bool active;

    char *path;
    MusicResource *resource;

    ResourceGroupHandle owner;
    AudioError load_error;
} MusicSlot;

typedef struct {
    uint32_t generation;
    bool active;

    char *path;
    SoundResource *resource;

    ResourceGroupHandle owner;
    AudioError load_error;
} SoundSlot;

struct AudioManager {
    ResourceGroupManager *groups;

    MusicSlot *music;
    SoundSlot *sounds;

    size_t music_count;
    size_t music_capacity;

    size_t sound_count;
    size_t sound_capacity;

    MusicHandle active_music;
    MusicPlaybackState music_state;
};

/* -------------------------------------------------- */
/* Internal helpers                                   */
/* -------------------------------------------------- */

static uint32_t next_generation(uint32_t generation) {
    generation++;

    if (generation == 0)
        generation = 1;

    return generation;
}

static bool grow_music_slots(AudioManager *manager) {
    size_t old_capacity = manager->music_capacity;

    if (old_capacity > SIZE_MAX / 2)
        return false;

    size_t new_capacity = old_capacity == 0 ? 8 : old_capacity * 2;

    if (new_capacity > UINT32_MAX || new_capacity > SIZE_MAX / sizeof(MusicSlot))
        return false;

    MusicSlot *slots = realloc(manager->music, new_capacity * sizeof(*slots));

    if (slots == NULL)
        return false;

    manager->music = slots;

    for (size_t i = old_capacity; i < new_capacity; i++) {
        slots[i] = (MusicSlot){0};
        slots[i].generation = 1;
    }

    manager->music_capacity = new_capacity;
    return true;
}

static bool grow_sound_slots(AudioManager *manager) {
    size_t old_capacity = manager->sound_capacity;

    if (old_capacity > SIZE_MAX / 2)
        return false;

    size_t new_capacity = old_capacity == 0 ? 8 : old_capacity * 2;

    if (new_capacity > UINT32_MAX || new_capacity > SIZE_MAX / sizeof(SoundSlot))
        return false;

    SoundSlot *slots = realloc(manager->sounds, new_capacity * sizeof(*slots));

    if (slots == NULL)
        return false;

    manager->sounds = slots;

    for (size_t i = old_capacity; i < new_capacity; i++) {
        slots[i] = (SoundSlot){0};
        slots[i].generation = 1;
    }

    manager->sound_capacity = new_capacity;
    return true;
}

static MusicLoadResult make_music_error(AudioError error) {
    MusicLoadResult result = {
        .handle = MUSIC_HANDLE_INVALID,
        .error = error,
        .reused = false
    };
    return result;
}

static SoundLoadResult make_sound_error(AudioError error) {
    SoundLoadResult result = {
        .handle = SOUND_HANDLE_INVALID,
        .error = error,
        .reused = false
    };
    return result;
}

/* -------------------------------------------------- */
/* Audio Cleanup                                      */
/* -------------------------------------------------- */

static void unload_music_slot(MusicSlot *slot) {
    if (slot == NULL || !slot->active)
        return;

    audio_backend_unload_music(slot->resource);

    free(slot->path);

    slot->resource = NULL;
    slot->path = NULL;
    slot->active = false;

    slot->generation = next_generation(slot->generation);
}

static void unload_sound_slot(SoundSlot *slot) {
    if (slot == NULL || !slot->active)
        return;

    audio_backend_unload_sound(slot->resource);

    free(slot->path);

    slot->resource = NULL;
    slot->path = NULL;
    slot->active = false;

    slot->generation = next_generation(slot->generation);
}

static void audio_manager_cleanup_group(void *context, ResourceGroupHandle group) {
    AudioManager *manager = context;

    if (manager == NULL)
        return;

    for (size_t i = 0; i < manager->music_count; i++) {
        MusicSlot *music = &manager->music[i];

        if (!music->active)
            continue;

        if (music->owner.index == group.index &&
            music->owner.generation == group.generation) {

            if (manager->active_music.index == i &&
                manager->active_music.generation == music->generation) {

                audio_backend_stop_music(music->resource);

                manager->active_music = MUSIC_HANDLE_INVALID;
                manager->music_state = MUSIC_STATE_STOPPED;
            }

            unload_music_slot(music);
        }
    }

    for (size_t i = 0; i < manager->sound_count; i++) {
        SoundSlot *sound = &manager->sounds[i];

        if (!sound->active)
            continue;

        if (sound->owner.index == group.index &&
            sound->owner.generation == group.generation) {
            unload_sound_slot(sound);
        }
    }
}

/* -------------------------------------------------- */
/* Manager lifecycle                                  */
/* -------------------------------------------------- */

AudioManager *audio_manager_create(ResourceGroupManager *groups) {
    if (groups == NULL)
        return NULL;

    AudioManager *manager = calloc(1, sizeof(AudioManager));

    if (manager == NULL)
        return NULL;

    manager->groups = groups;
    manager->active_music = MUSIC_HANDLE_INVALID;
    manager->music_state = MUSIC_STATE_STOPPED;

    if (!resource_group_manager_register_cleanup(groups, audio_manager_cleanup_group, manager)) {
        free(manager);
        return NULL;
    }

    return manager;
}

void audio_manager_destroy(AudioManager *manager) {
    if (manager == NULL)
        return;

    resource_group_manager_unregister_cleanup(manager->groups, audio_manager_cleanup_group, manager);

    audio_manager_stop_music(manager);

    for (size_t i = 0; i < manager->music_count; i++)
        unload_music_slot(&manager->music[i]);

    for (size_t i = 0; i < manager->sound_count; i++)
        unload_sound_slot(&manager->sounds[i]);


    free(manager->music);
    free(manager->sounds);
    free(manager);

}

/* -------------------------------------------------- */
/* Audio management                                   */
/* -------------------------------------------------- */

/* Sound Management */
SoundLoadResult audio_manager_load_sound(AudioManager *manager, ResourceGroupHandle group, const char *path) {
    if (manager == NULL || path == NULL || path[0] == '\0')
        return make_sound_error(AUDIO_ERROR_INVALID_ARGUMENT);

}

bool audio_manager_play_sound(AudioManager *manager, SoundHandle sound) {

}


/* Music Management */
MusicLoadResult audio_manager_load_music(AudioManager *manager, ResourceGroupHandle group, const char *path) {
}
bool audio_manager_play_music(AudioManager *manager, MusicHandle music) {
}
bool audio_manager_pause_music(AudioManager *manager) {
}
bool audio_manager_resume_music(AudioManager *manager) {
}
bool audio_manager_stop_music(AudioManager *manager) {
}
