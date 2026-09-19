#pragma once
#include <cstdint>

extern "C" {

typedef struct {
    float intensity;      // 0.0 - 1.0, drives all layers
    int bpm;               // bound rhythm generation
} PxAudioParams;

// Lifecycle
int px_audio_init(int sample_rate, int buffer_frames); // pre-allocates buffer ONCE
int px_audio_start(void);
int px_audio_set_intensity(float intensity);            // clamped 0.0-1.0
int px_audio_generate(float* out_buffer, int num_frames); // NO allocation here
int px_audio_stop(void);
int px_audio_shutdown(void);

}
