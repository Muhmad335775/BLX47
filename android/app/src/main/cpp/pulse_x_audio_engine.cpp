#include "pulse_x_audio_engine.h"

#include <cmath>
#include <cstring>

namespace pulse_x_audio {

void reset(AnalysisAccumulator* acc) {
    std::memset(acc, 0, sizeof(AnalysisAccumulator));
    acc->has_prev_sample = false;
}

void process_chunk(AnalysisAccumulator* acc, const float* samples, int32_t count) {
    if (acc == nullptr || samples == nullptr || count <= 0) {
        return;
    }

    for (int32_t i = 0; i < count; i++) {
        const float s = samples[i];

        acc->sum_squares += s * s;

        const float abs_s = s < 0.0f ? -s : s;
        if (abs_s > acc->peak) {
            acc->peak = abs_s;
        }

        if (acc->has_prev_sample) {
            const bool sign_changed =
                (acc->prev_sample >= 0.0f && s < 0.0f) ||
                (acc->prev_sample < 0.0f && s >= 0.0f);
            if (sign_changed) {
                acc->zero_crossings += 1;
            }
        }
        acc->prev_sample = s;
        acc->has_prev_sample = true;
    }

    acc->total_samples += count;
}

void finalize(const AnalysisAccumulator* acc,
              int32_t sample_rate,
              PxAudioAnalysis* out_analysis) {
    if (acc == nullptr || out_analysis == nullptr) {
        return;
    }

    out_analysis->sample_count = acc->total_samples;
    out_analysis->peak_level = acc->peak;

    if (acc->total_samples > 0) {
        out_analysis->rms_level = std::sqrt(
            acc->sum_squares / static_cast<float>(acc->total_samples));
    } else {
        out_analysis->rms_level = 0.0f;
    }

    /* Lightweight, fully offline frequency approximation using
     * zero-crossing rate. This is not full FFT spectral analysis;
     * it is a cheap real-time estimate for scene-reactivity. */
    if (sample_rate > 0 && acc->total_samples > 0) {
        const float duration_seconds =
            static_cast<float>(acc->total_samples) / static_cast<float>(sample_rate);
        if (duration_seconds > 0.0f) {
            out_analysis->dominant_frequency_hz =
                (static_cast<float>(acc->zero_crossings) / 2.0f) / duration_seconds;
        } else {
            out_analysis->dominant_frequency_hz = 0.0f;
        }
    } else {
        out_analysis->dominant_frequency_hz = 0.0f;
    }
}

void derive_scene(const PxAudioAnalysis* analysis, PxSceneDescriptor* out_scene) {
    if (analysis == nullptr || out_scene == nullptr) {
        return;
    }

    out_scene->energy = analysis->rms_level;
    out_scene->tempo_bpm = 0.0f;

    if (analysis->rms_level > 0.2f) {
        out_scene->scene_id = 2;
    } else if (analysis->rms_level > 0.05f) {
        out_scene->scene_id = 1;
    } else {
        out_scene->scene_id = 0;
    }
}

} // namespace pulse_x_audio
