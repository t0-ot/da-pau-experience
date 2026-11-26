#ifndef AUDIO_MANAGER_INTERNAL_H
#define AUDIO_MANAGER_INTERNAL_H

#include "engine/audio/audio_manager.h"

/* Audio manager lifecycle. */
AudioManager *audio_manager_create(ResourceGroupManager *groups);
void audio_manager_update(AudioManager *manager);
void audio_manager_destroy(AudioManager *manager);

#endif // AUDIO_MANAGER_INTERNAL_H
