#ifndef PULSE_X_AUDIO_ENGINE_H
#define PULSE_X_AUDIO_ENGINE_H

#include <stdint.h>
#include "pulse_x_types.h"

/* Internal audio analysis engine (Paper 3).
 * Not exposed via PX_EXPORT - implementation detail of CORE only.
 * Per Bridge Law: logic stays in its own paper, only integration
 * lives in CORE. Nothing outside pulse_x_core.cpp may call this. */

namespace pulse_x_audio {

struct AnalysisAccumulator {
    float sum_squares;
    float peak;
    int32_t zero_crossings;
    float prev_sample;
    bool has_prev_sample;
    int32_t total_samples;
};

void reset(AnalysisAccumulator* acc);

void process_chunk(AnalysisAccumulator* acc, const float* samples, int32_t count);

void finalize(const AnalysisAccumulator* acc,
              int32_t sample_rate,
              PxAudioAnalysis* out_analysis);

void derive_scene(const PxAudioAnalysis* analysis,
                   PxSceneDescriptor* out_scene);

} // namespace pulse_x_audio

#endif /* PULSE_X_AUDIO_ENGINE_H */
