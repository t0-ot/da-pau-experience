#ifndef MUSIC_H
#define MUSIC_H

#include <stdint.h>

typedef struct {
    uint32_t index;
    uint32_t generation;
} MusicHandle;

#define MUSIC_HANDLE_INVALID ((MusicHandle){0, 0})

#endif // MUSIC_H
