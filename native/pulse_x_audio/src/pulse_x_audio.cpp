#include "pulse_x_audio.h"
#include <aaudio/AAudio.h>
#include <algorithm>
#include <cmath>

static AAudioStream* g_stream = nullptr;
static float* g_scratch_buffer = nullptr;  // pre-allocated once
static int g_scratch_size = 0;
static float g_intensity = 0.0f;
static int g_bpm = 120;

// Euclidean rhythm table, computed once at init, bound to BPM
static uint8_t g_euclid_pattern[16];
static int g_euclid_steps = 16;

static void build_euclidean_pattern(int pulses, int steps) {
    for (int i = 0; i < steps; i++) {
        g_euclid_pattern[i] = ((i * pulses) % steps) < pulses ? 1 : 0;
    }
}

int px_audio_init(int sample_rate, int buffer_frames) {
    g_scratch_size = buffer_frames;
    g_scratch_buffer = new float[buffer_frames]; // ONLY allocation, happens once here
    build_euclidean_pattern(4, g_euclid_steps);

    AAudioStreamBuilder* builder;
    AAudio_createStreamBuilder(&builder);
    AAudioStreamBuilder_setSampleRate(builder, sample_rate);
    AAudioStreamBuilder_setFormat(builder, AAUDIO_FORMAT_PCM_FLOAT);
    AAudioStreamBuilder_setChannelCount(builder, 1);
    AAudioStreamBuilder_openStream(builder, &g_stream);
    AAudioStreamBuilder_delete(builder);
    return 0;
}

int px_audio_set_intensity(float intensity) {
    g_intensity = std::clamp(intensity, 0.0f, 1.0f);
    return 0;
}

int px_audio_generate(float* out_buffer, int num_frames) {
    // no malloc/new here — writes directly into caller-owned buffer
    for (int i = 0; i < num_frames && i < g_scratch_size; i++) {
        int step = i % g_euclid_steps;
        float beat = g_euclid_pattern[step] ? 1.0f : 0.0f;
        out_buffer[i] = beat * g_intensity * std::sin(2.0f * 3.14159f * 220.0f * i / 48000.0f);
    }
    return num_frames;
}

int px_audio_start(void) {
    if (g_stream) return AAudioStream_requestStart(g_stream);
    return -1;
}

int px_audio_stop(void) {
    if (g_stream) return AAudioStream_requestStop(g_stream);
    return -1;
}

int px_audio_shutdown(void) {
    if (g_stream) { AAudioStream_close(g_stream); g_stream = nullptr; }
    delete[] g_scratch_buffer;
    g_scratch_buffer = nullptr;
    return 0;
}
