
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

static char *copy_string(const char *source) {
    size_t length = strlen(source) + 1;

    char *copy = malloc(length);

    if (copy != NULL)
        memcpy(copy, source, length);

    return copy;
}

static uint32_t next_generation(uint32_t generation) {
    generation++;

    if (generation == 0)
        generation = 1;

    return generation;
}

static bool music_is_valid(const AudioManager *manager, MusicHandle music) {
    if (manager == NULL || music.generation == 0)
        return false;

    if (music.index >= manager->music_count)
        return false;

    const MusicSlot *slot = &manager->music[music.index];

    return slot->active && slot->generation == music.generation;
}

static bool sound_is_valid(const AudioManager *manager, SoundHandle sound) {
    if (manager == NULL || sound.generation == 0)
        return false;

    if (sound.index >= manager->sound_count)
        return false;

    const SoundSlot *slot = &manager->sounds[sound.index];

    return slot->active && slot->generation == sound.generation;
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

static bool grow_music_slots(AudioManager *manager) {
    size_t old_capacity = manager->music_capacity;

    if (old_capacity > SIZE_MAX / 2)
        return false;

    size_t new_capacity = old_capacity == 0 ? 8 : old_capacity * 2;

    if (new_capacity > UINT32_MAX ||
        new_capacity > SIZE_MAX / sizeof(MusicSlot))
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

    if (new_capacity > UINT32_MAX ||
        new_capacity > SIZE_MAX / sizeof(SoundSlot))
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
                audio_manager_stop_music(manager);
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

void audio_manager_update(AudioManager *manager) {
    if (manager == NULL)
        return;

    if (manager->music_state != MUSIC_STATE_PLAYING)
        return;

    if (!music_is_valid(manager, manager->active_music)) {
        manager->active_music = MUSIC_HANDLE_INVALID;
        manager->music_state = MUSIC_STATE_STOPPED;
        return;
    }

    MusicSlot *music = &manager->music[manager->active_music.index];

    if (music->resource == NULL) {
        manager->active_music = MUSIC_HANDLE_INVALID;
        manager->music_state = MUSIC_STATE_STOPPED;
        return;
    }

    audio_backend_update_music(music->resource);
}

void audio_manager_destroy(AudioManager *manager) {
    if (manager == NULL)
        return;

    resource_group_manager_unregister_cleanup(
        manager->groups,
        audio_manager_cleanup_group,
        manager
    );

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
/* Sound Management                                   */
/* -------------------------------------------------- */

SoundLoadResult audio_manager_load_sound(
    AudioManager *manager,
    ResourceGroupHandle group,
    const char *path
) {
    if (manager == NULL || path == NULL || path[0] == '\0')
        return make_sound_error(AUDIO_ERROR_INVALID_ARGUMENT);

    if (!resource_group_manager_is_valid(manager->groups, group))
        return make_sound_error(AUDIO_ERROR_INVALID_GROUP);

    /* Reuse an existing sound within this group. */
    for (size_t i = 0; i < manager->sound_count; i++) {
        const SoundSlot *slot = &manager->sounds[i];

        if (!slot->active)
            continue;

        if (slot->owner.index == group.index &&
            slot->owner.generation == group.generation &&
            strcmp(slot->path, path) == 0) {

            return (SoundLoadResult){
                .handle = {
                    .index = (uint32_t)i,
                    .generation = slot->generation
                },
                .error = slot->load_error,
                .reused = true
            };
        }
    }

    /* Find an available sound slot. */
    size_t index;

    for (index = 0; index < manager->sound_count; index++) {
        if (!manager->sounds[index].active)
            break;
    }

    bool new_slot = false;

    if (index == manager->sound_count) {
        if (manager->sound_count == manager->sound_capacity) {
            if (!grow_sound_slots(manager))
                return make_sound_error(AUDIO_ERROR_OUT_OF_MEMORY);
        }

        new_slot = true;
    }

    SoundSlot *slot = &manager->sounds[index];

    /* Copy the path before loading the resource. */
    char *path_copy = copy_string(path);

    if (path_copy == NULL)
        return make_sound_error(AUDIO_ERROR_OUT_OF_MEMORY);

    /* Load through the backend abstraction. */
    SoundResource *resource = NULL;

    AudioBackendResult backend_result =
        audio_backend_load_sound(path, &resource);

    AudioError load_error = AUDIO_ERROR_NONE;

    switch (backend_result) {
        case AUDIO_BACKEND_SUCCESS:
            break;

        case AUDIO_BACKEND_FILE_NOT_FOUND:
            load_error = AUDIO_ERROR_FILE_NOT_FOUND;
            break;

        case AUDIO_BACKEND_OUT_OF_MEMORY:
            free(path_copy);
            return make_sound_error(AUDIO_ERROR_OUT_OF_MEMORY);

        default:
            load_error = AUDIO_ERROR_BACKEND;
            break;
    }

    /* Commit the request, even if loading failed recoverably. */
    slot->path = path_copy;
    slot->resource = resource;
    slot->load_error = load_error;
    slot->owner = group;
    slot->active = true;

    if (new_slot)
        manager->sound_count++;

    return (SoundLoadResult){
        .handle = {
            .index = (uint32_t)index,
            .generation = slot->generation
        },
        .error = slot->load_error,
        .reused = false
    };
}

bool audio_manager_play_sound(AudioManager *manager, SoundHandle sound) {
    if (!sound_is_valid(manager, sound))
        return false;

    SoundSlot *slot = &manager->sounds[sound.index];

    if (slot->resource == NULL)
        return false;

    audio_backend_play_sound(slot->resource);

    return true;
}

/* -------------------------------------------------- */
/* Music Management                                   */
/* -------------------------------------------------- */

// cppcheck-suppress unusedFunction
MusicLoadResult audio_manager_load_music(
    AudioManager *manager,
    ResourceGroupHandle group,
    const char *path
) {
    if (manager == NULL || path == NULL || path[0] == '\0')
        return make_music_error(AUDIO_ERROR_INVALID_ARGUMENT);

    if (!resource_group_manager_is_valid(manager->groups, group))
        return make_music_error(AUDIO_ERROR_INVALID_GROUP);

    /* Reuse an existing music resource within this group. */
    for (size_t i = 0; i < manager->music_count; i++) {
        const MusicSlot *slot = &manager->music[i];

        if (!slot->active)
            continue;

        if (slot->owner.index == group.index &&
            slot->owner.generation == group.generation &&
            strcmp(slot->path, path) == 0) {

            return (MusicLoadResult){
                .handle = {
                    .index = (uint32_t)i,
                    .generation = slot->generation
                },
                .error = slot->load_error,
                .reused = true
            };
        }
    }

    /* Find an available music slot. */
    size_t index;

    for (index = 0; index < manager->music_count; index++) {
        if (!manager->music[index].active)
            break;
    }

    bool new_slot = false;

    if (index == manager->music_count) {
        if (manager->music_count == manager->music_capacity) {
            if (!grow_music_slots(manager))
                return make_music_error(AUDIO_ERROR_OUT_OF_MEMORY);
        }

        new_slot = true;
    }

    MusicSlot *slot = &manager->music[index];

    /* Copy the path before loading the resource. */
    char *path_copy = copy_string(path);

    if (path_copy == NULL)
        return make_music_error(AUDIO_ERROR_OUT_OF_MEMORY);

    /* Load through the backend abstraction. */
    MusicResource *resource = NULL;

    AudioBackendResult backend_result =
        audio_backend_load_music(path, &resource);

    AudioError load_error = AUDIO_ERROR_NONE;

    switch (backend_result) {
        case AUDIO_BACKEND_SUCCESS:
            break;

        case AUDIO_BACKEND_FILE_NOT_FOUND:
            load_error = AUDIO_ERROR_FILE_NOT_FOUND;
            break;

        case AUDIO_BACKEND_OUT_OF_MEMORY:
            free(path_copy);
            return make_music_error(AUDIO_ERROR_OUT_OF_MEMORY);

        default:
            load_error = AUDIO_ERROR_BACKEND;
            break;
    }

    /* Commit the request, even if loading failed recoverably. */
    slot->path = path_copy;
    slot->resource = resource;
    slot->load_error = load_error;
    slot->owner = group;
    slot->active = true;

    if (new_slot)
        manager->music_count++;

    return (MusicLoadResult){
        .handle = {
            .index = (uint32_t)index,
            .generation = slot->generation
        },
        .error = slot->load_error,
        .reused = false
    };
}

// cppcheck-suppress unusedFunction
bool audio_manager_play_music(AudioManager *manager, MusicHandle music) {
    if (!music_is_valid(manager, music))
        return false;

    MusicSlot *slot = &manager->music[music.index];

    if (slot->resource == NULL)
        return false;

    /* Avoid restarting the already-playing track. */
    if (manager->active_music.index == music.index &&
        manager->active_music.generation == music.generation) {

        if (manager->music_state == MUSIC_STATE_PLAYING)
            return true;

        if (manager->music_state == MUSIC_STATE_PAUSED) {
            audio_backend_resume_music(slot->resource);
            manager->music_state = MUSIC_STATE_PLAYING;
            return true;
        }
    }

    /* Stop the previous track before starting another. */
    audio_manager_stop_music(manager);

    audio_backend_play_music(slot->resource);

    manager->active_music = music;
    manager->music_state = MUSIC_STATE_PLAYING;

    return true;
}

// cppcheck-suppress unusedFunction
bool audio_manager_pause_music(AudioManager *manager) {
    if (manager == NULL)
        return false;

    if (manager->music_state != MUSIC_STATE_PLAYING)
        return false;

    if (!music_is_valid(manager, manager->active_music)) {
        manager->active_music = MUSIC_HANDLE_INVALID;
        manager->music_state = MUSIC_STATE_STOPPED;
        return false;
    }

    MusicSlot *slot = &manager->music[manager->active_music.index];

    if (slot->resource == NULL) {
        manager->active_music = MUSIC_HANDLE_INVALID;
        manager->music_state = MUSIC_STATE_STOPPED;
        return false;
    }

    audio_backend_pause_music(slot->resource);

    manager->music_state = MUSIC_STATE_PAUSED;

    return true;
}

// cppcheck-suppress unusedFunction
bool audio_manager_resume_music(AudioManager *manager) {
    if (manager == NULL)
        return false;

    if (manager->music_state != MUSIC_STATE_PAUSED)
        return false;

    if (!music_is_valid(manager, manager->active_music)) {
        manager->active_music = MUSIC_HANDLE_INVALID;
        manager->music_state = MUSIC_STATE_STOPPED;
        return false;
    }

    MusicSlot *slot = &manager->music[manager->active_music.index];

    if (slot->resource == NULL) {
        manager->active_music = MUSIC_HANDLE_INVALID;
        manager->music_state = MUSIC_STATE_STOPPED;
        return false;
    }

    audio_backend_resume_music(slot->resource);

    manager->music_state = MUSIC_STATE_PLAYING;

    return true;
}

// cppcheck-suppress staticFunction
bool audio_manager_stop_music(AudioManager *manager) {
    if (manager == NULL)
        return false;

    if (manager->music_state != MUSIC_STATE_STOPPED &&
        music_is_valid(manager, manager->active_music)) {

        MusicSlot *slot = &manager->music[manager->active_music.index];

        if (slot->resource != NULL)
            audio_backend_stop_music(slot->resource);
    }

    manager->active_music = MUSIC_HANDLE_INVALID;
    manager->music_state = MUSIC_STATE_STOPPED;

    return true;
}
