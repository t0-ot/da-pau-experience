#ifndef AUDIO_DEVICE_H
#define AUDIO_DEVICE_H

#include <stdbool.h>

/* Audio device lifecycle. */
bool audio_device_init(void);
void audio_device_close(void);

#endif // AUDIO_DEVICE_H
