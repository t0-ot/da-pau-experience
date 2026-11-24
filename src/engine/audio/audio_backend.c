#include "audio_backend.h"
#include "raylib.h"

struct MusicResource {
    Music music_data;
};

struct SoundResource {
    Sound sound_data;
};

/* -------------------------------------------------- */
/* Music Related Func                                 */
/* -------------------------------------------------- */
AudioBackendResult audio_backend_load_music(const char *path, SoundResource **out_resource) {

}

/* -------------------------------------------------- */
/* Sound Related Func                                 */
/* -------------------------------------------------- */
AudioBackendResult audio_backend_load_sound(const char *path, SoundResource **out_resource) {

}
