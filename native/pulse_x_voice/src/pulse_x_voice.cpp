#include "pulse_x_voice.h"
#include <cmath>
#include <algorithm>

static bool g_permission_granted = false;
static float* g_autocorr_buffer = nullptr; // pre-allocated once at init
static int g_frame_size = 0;
static int g_sample_rate = 48000;

// Export handoff buffer — owned here, read by Card 9 (Cinematic Engine)
static float* g_export_buffer = nullptr;
static int g_export_capacity = 0;
static int g_export_write_pos = 0;

int px_voice_init(int sample_rate, int frame_size) {
    g_sample_rate = sample_rate;
    g_frame_size = frame_size;
    g_autocorr_buffer = new float[frame_size]; // ONLY allocation, once
    g_export_capacity = sample_rate * 60 * 3;  // up to 3 min session (Card 11)
    g_export_buffer = new float[g_export_capacity];
    g_export_write_pos = 0;
    return 0;
}

int px_voice_set_permission_granted(bool granted) {
    g_permission_granted = granted;
    return 0;
}

static float compute_rms(const float* pcm, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) sum += pcm[i] * pcm[i];
    return std::sqrt(sum / n);
}

static float compute_pitch_autocorrelation(const float* pcm, int n, int sample_rate) {
    int best_lag = -1;
    float best_corr = 0.0f;
    for (int lag = 50; lag < n / 2; lag++) {
        float corr = 0.0f;
        for (int i = 0; i < n - lag; i++) corr += pcm[i] * pcm[i + lag];
        if (corr > best_corr) { best_corr = corr; best_lag = lag; }
    }
    if (best_lag <= 0) return 0.0f;
    return (float)sample_rate / (float)best_lag;
}

int px_voice_process_frame(const float* pcm_in, int num_samples, PxVoiceFrame* out_frame) {
    if (!out_frame) return -1; // null check

    if (!g_permission_granted) {
        out_frame->rms = 0.0f;
        out_frame->pitch_hz = 0.0f;
        out_frame->voice_detected = false;
        return 0; // non-reactive lighting fallback
    }

    out_frame->rms = std::clamp(compute_rms(pcm_in, num_samples), 0.0f, 1.0f);
    out_frame->pitch_hz = compute_pitch_autocorrelation(pcm_in, num_samples, g_sample_rate);
    out_frame->voice_detected = out_frame->rms > 0.02f;
    return 0;
}

int px_voice_capture_for_export(const float* pcm_in, int num_samples) {
    if (!g_permission_granted) return -1;
    int remaining = g_export_capacity - g_export_write_pos;
    int to_copy = std::min(num_samples, remaining);
    for (int i = 0; i < to_copy; i++)
        g_export_buffer[g_export_write_pos + i] = pcm_in[i]; // no network, local only
    g_export_write_pos += to_copy;
    return to_copy;
}

int px_voice_shutdown(void) {
    delete[] g_autocorr_buffer; g_autocorr_buffer = nullptr;
    delete[] g_export_buffer; g_export_buffer = nullptr;
    g_export_write_pos = 0;
    return 0;
}
