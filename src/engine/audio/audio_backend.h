#ifndef AUDIO_BACKEND_H
#define AUDIO_BACKEND_H

typedef struct MusicResource MusicResource;
typedef struct SoundResource SoundResource;

typedef enum {
    AUDIO_BACKEND_SUCCESS,
    AUDIO_BACKEND_FILE_NOT_FOUND,
    AUDIO_BACKEND_OUT_OF_MEMORY,
    AUDIO_BACKEND_FAILURE
} AudioBackendResult;

/* -------------------------------------------------- */
/* Music Related Func                                 */
/* -------------------------------------------------- */
AudioBackendResult audio_backend_load_music(const char *path, MusicResource **out_resource);
void audio_backend_unload_music(MusicResource *resource);
void audio_backend_update_music(const MusicResource *resource);
void audio_backend_pause_music(const MusicResource *resource);
void audio_backend_resume_music(const MusicResource *resource);
void audio_backend_play_music(const MusicResource *resource);
void audio_backend_stop_music(const MusicResource *resource);

/* -------------------------------------------------- */
/* Sound Related Func                                 */
/* -------------------------------------------------- */
AudioBackendResult audio_backend_load_sound(const char *path, SoundResource **out_resource);
void audio_backend_unload_sound(SoundResource *resource);
void audio_backend_pause_sound(const SoundResource *resource);
void audio_backend_resume_sound(const SoundResource *resource);
void audio_backend_play_sound(const SoundResource *resource);
void audio_backend_stop_sound(const SoundResource *resource);



#endif // AUDIO_BACKEND_H
