#include "pulse_x_core.h"
#include "pulse_x_audio_engine.h"

#include <atomic>
#include <cstdio>
#include <cstring>
#include <android/log.h>

#define PX_LOG_TAG "PulseXCore"
#define PX_LOGI(...) __android_log_print(ANDROID_LOG_INFO, PX_LOG_TAG, __VA_ARGS__)
#define PX_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, PX_LOG_TAG, __VA_ARGS__)

namespace {

std::atomic<int32_t> g_state{PX_STATE_UNINITIALIZED};
std::atomic<int32_t> g_last_error{PX_ERROR_NONE};

PxEngineConfig g_config;
PxAudioAnalysis g_audio_analysis;
PxSceneDescriptor g_scene_descriptor;
PxRenderFrame g_render_frame;
PxExportResult g_export_result;
PxStorageState g_storage_state;

pulse_x_audio::AnalysisAccumulator g_audio_accumulator;

int32_t g_frame_counter = 0;

void set_error(PxErrorCode code) {
    g_last_error.store(code);
    g_state.store(PX_STATE_ERROR);
    PX_LOGE("CORE entered ERROR state, code=%d", static_cast<int32_t>(code));
}

bool require_state(PxState expected) {
    if (g_state.load() != expected) {
        set_error(PX_ERROR_INVALID_STATE);
        return false;
    }
    return true;
}

} // namespace

extern "C" {

int32_t px_core_version(void) {
    return 3;
}

int32_t px_core_init(const PxEngineConfig* config) {
    if (g_state.load() != PX_STATE_UNINITIALIZED) {
        set_error(PX_ERROR_ALREADY_INITIALIZED);
        return -1;
    }
    if (config == nullptr) {
        set_error(PX_ERROR_INVALID_ARGUMENT);
        return -2;
    }

    g_config = *config;

    std::memset(&g_audio_analysis, 0, sizeof(g_audio_analysis));
    std::memset(&g_scene_descriptor, 0, sizeof(g_scene_descriptor));
    std::memset(&g_render_frame, 0, sizeof(g_render_frame));
    std::memset(&g_export_result, 0, sizeof(g_export_result));
    std::memset(&g_storage_state, 0, sizeof(g_storage_state));
    pulse_x_audio::reset(&g_audio_accumulator);
    g_frame_counter = 0;

    g_last_error.store(PX_ERROR_NONE);
    g_state.store(PX_STATE_READY);
    PX_LOGI("CORE initialized, sample_rate=%d", g_config.sample_rate);
    return 0;
}

void px_core_shutdown(void) {
    g_state.store(PX_STATE_UNINITIALIZED);
    g_last_error.store(PX_ERROR_NONE);
    PX_LOGI("CORE shut down");
}

int32_t px_core_get_state(void) {
    return g_state.load();
}

int32_t px_core_get_last_error(void) {
    return g_last_error.load();
}

int32_t px_core_recover(void) {
    if (g_state.load() != PX_STATE_ERROR) {
        return -1;
    }
    g_last_error.store(PX_ERROR_NONE);
    g_state.store(PX_STATE_READY);
    PX_LOGI("CORE recovered to READY");
    return 0;
}

int32_t px_core_begin_recording(void) {
    if (!require_state(PX_STATE_READY)) {
        return -1;
    }
    pulse_x_audio::reset(&g_audio_accumulator);
    g_state.store(PX_STATE_RECORDING);
    return 0;
}

int32_t px_core_feed_audio(const float* samples, int32_t sample_count) {
    if (!require_state(PX_STATE_RECORDING)) {
        return -1;
    }
    if (samples == nullptr || sample_count <= 0) {
        set_error(PX_ERROR_INVALID_ARGUMENT);
        return -2;
    }

    pulse_x_audio::process_chunk(&g_audio_accumulator, samples, sample_count);
    return 0;
}

int32_t px_core_end_recording(void) {
    if (!require_state(PX_STATE_RECORDING)) {
        return -1;
    }
    g_state.store(PX_STATE_PROCESSING);

    pulse_x_audio::finalize(&g_audio_accumulator, g_config.sample_rate, &g_audio_analysis);
    pulse_x_audio::derive_scene(&g_audio_analysis, &g_scene_descriptor);

    g_state.store(PX_STATE_GENERATION);
    return 0;
}

int32_t px_core_get_audio_analysis(PxAudioAnalysis* out_analysis) {
    if (out_analysis == nullptr) {
        set_error(PX_ERROR_INVALID_ARGUMENT);
        return -1;
    }
    *out_analysis = g_audio_analysis;
    return 0;
}

int32_t px_core_get_scene_descriptor(PxSceneDescriptor* out_scene) {
    if (out_scene == nullptr) {
        set_error(PX_ERROR_INVALID_ARGUMENT);
        return -1;
    }
    *out_scene = g_scene_descriptor;
    return 0;
}

int32_t px_core_begin_render(void) {
    if (!require_state(PX_STATE_GENERATION)) {
        return -1;
    }
    g_render_frame.width = g_config.render_width;
    g_render_frame.height = g_config.render_height;
    g_render_frame.frame_index = g_frame_counter++;
    g_render_frame.is_valid = 1;
    g_state.store(PX_STATE_RENDER);
    return 0;
}

int32_t px_core_get_render_frame(PxRenderFrame* out_frame) {
    if (out_frame == nullptr) {
        set_error(PX_ERROR_INVALID_ARGUMENT);
        return -1;
    }
    *out_frame = g_render_frame;
    return 0;
}

int32_t px_core_begin_export(void) {
    if (!require_state(PX_STATE_RENDER)) {
        return -1;
    }
    g_export_result.success = 1;
    g_export_result.error_code = PX_ERROR_NONE;
    std::snprintf(g_export_result.output_path,
                  sizeof(g_export_result.output_path),
                  "export_%d.mp4", g_frame_counter);
    g_state.store(PX_STATE_EXPORT);
    return 0;
}

int32_t px_core_get_export_result(PxExportResult* out_result) {
    if (out_result == nullptr) {
        set_error(PX_ERROR_INVALID_ARGUMENT);
        return -1;
    }
    *out_result = g_export_result;
    return 0;
}

int32_t px_core_get_storage_state(PxStorageState* out_storage) {
    if (out_storage == nullptr) {
        set_error(PX_ERROR_INVALID_ARGUMENT);
        return -1;
    }
    *out_storage = g_storage_state;
    return 0;
}

int32_t px_core_return_to_ready(void) {
    if (!require_state(PX_STATE_EXPORT)) {
        return -1;
    }
    g_state.store(PX_STATE_READY);
    return 0;
}

} // extern "C"
