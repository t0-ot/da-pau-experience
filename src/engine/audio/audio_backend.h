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

/* -------------------------------------------------- */
/* Sound Related Func                                 */
/* -------------------------------------------------- */
AudioBackendResult audio_backend_load_sound(const char *path, SoundResource **out_resource);


#endif // AUDIO_BACKEND_H
