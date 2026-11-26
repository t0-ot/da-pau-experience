#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

typedef struct {
    uint32_t index;
    uint32_t generation;
} SoundHandle;

#define SOUND_HANDLE_INVALID ((SoundHandle){0, 0})

#endif // SOUND_H
