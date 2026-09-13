#ifndef PULSE_X_TYPES_H
#define PULSE_X_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PxEngineConfig {
    int32_t sample_rate;
    int32_t max_recording_seconds;
    int32_t render_width;
    int32_t render_height;
    int32_t reserved[4];
} PxEngineConfig;

typedef struct PxSessionState {
    int32_t state;
    int32_t error_code;
} PxSessionState;

typedef struct PxAudioAnalysis {
    float rms_level;
    float peak_level;
    float dominant_frequency_hz;
    int32_t sample_count;
} PxAudioAnalysis;

typedef struct PxSceneDescriptor {
    float energy;
    float tempo_bpm;
    int32_t scene_id;
    int32_t reserved[4];
} PxSceneDescriptor;

typedef struct PxRenderFrame {
    int32_t width;
    int32_t height;
    int32_t frame_index;
    int32_t is_valid;
} PxRenderFrame;

typedef struct PxVideoFrame {
    int32_t width;
    int32_t height;
    int64_t timestamp_us;
    int32_t reserved;
} PxVideoFrame;

typedef struct PxWatermarkState {
    int32_t enabled;
    int32_t reserved[3];
} PxWatermarkState;

typedef struct PxExportResult {
    int32_t success;
    int32_t error_code;
    char output_path[256];
} PxExportResult;

typedef struct PxStorageState {
    int64_t bytes_used;
    int64_t bytes_available;
    int32_t reserved[2];
} PxStorageState;

#ifdef __cplusplus
}
#endif

#endif /* PULSE_X_TYPES_H */
