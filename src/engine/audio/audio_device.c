#include "engine/audio/audio_device.h"

#include "raylib.h"

bool audio_device_init(void) {
    InitAudioDevice();

    return IsAudioDeviceReady();
}

void audio_device_close(void) {
    CloseAudioDevice();
}
