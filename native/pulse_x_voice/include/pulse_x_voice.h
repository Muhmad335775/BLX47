#pragma once
#include <cstdint>

extern "C" {

typedef struct {
    float rms;          // 0.0 - 1.0 volume level
    float pitch_hz;      // from autocorrelation
    bool voice_detected;
} PxVoiceFrame;

int px_voice_init(int sample_rate, int frame_size);
int px_voice_set_permission_granted(bool granted); // false -> non-reactive fallback
int px_voice_process_frame(const float* pcm_in, int num_samples, PxVoiceFrame* out_frame);
int px_voice_capture_for_export(const float* pcm_in, int num_samples); // hands off to Cinematic Engine buffer
int px_voice_shutdown(void);

}
