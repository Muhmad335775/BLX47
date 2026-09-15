#include "pulse_x_core.h"
#include "pulse_x_audio_engine.h"
#include "pulse_x_procedural_world.h"
#include "pulse_x_voice_scene_engine.h"
#include "pulse_x_render_engine.h"
#include "pulse_x_brand_output.h"

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
PxWorldDescriptor g_world_descriptor;
PxVoiceSceneDescriptor g_voice_scene_descriptor;
PxCinematicRenderDescriptor g_cinematic_render_descriptor;
PxBrandWatermarkDescriptor g_watermark_descriptor;
PxFinalVideoFrame g_final_video_frame;

pulse_x_audio::AnalysisAccumulator g_audio_accumulator;

int32_t g_frame_counter = 0;
bool g_world_generated = false;
bool g_voice_scene_generated = false;
bool g_cinematic_render_built = false;
bool g_watermark_applied = false;

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
    return 7;
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
    std::memset(&g_world_descriptor, 0, sizeof(g_world_descriptor));
    std::memset(&g_voice_scene_descriptor, 0, sizeof(g_voice_scene_descriptor));
    std::memset(&g_cinematic_render_descriptor, 0, sizeof(g_cinematic_render_descriptor));
    std::memset(&g_watermark_descriptor, 0, sizeof(g_watermark_descriptor));
    std::memset(&g_final_video_frame, 0, sizeof(g_final_video_frame));
    pulse_x_audio::reset(&g_audio_accumulator);
    g_frame_counter = 0;
    g_world_generated = false;
    g_voice_scene_generated = false;
    g_cinematic_render_built = false;
    g_watermark_applied = false;

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
    g_world_generated = false;
    g_voice_scene_generated = false;
    g_cinematic_render_built = false;
    g_watermark_applied = false;
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

int32_t px_core_generate_world(int32_t variation_seed) {
    if (!require_state(PX_STATE_GENERATION)) {
        return -1;
    }
    pulse_x_world::generate_world(&g_scene_descriptor, variation_seed, &g_world_descriptor);
    g_world_generated = true;
    return 0;
}

int32_t px_core_get_world_descriptor(PxWorldDescriptor* out_world) {
    if (out_world == nullptr) {
        set_error(PX_ERROR_INVALID_ARGUMENT);
        return -1;
    }
    if (!g_world_generated) {
        set_error(PX_ERROR_INVALID_STATE);
        return -2;
    }
    *out_world = g_world_descriptor;
    return 0;
}

int32_t px_core_get_daily_landmark(int32_t day_seed) {
    return pulse_x_world::select_daily_landmark(day_seed);
}

int32_t px_core_get_live_voice_scene(PxVoiceSceneDescriptor* out_scene) {
    if (out_scene == nullptr) {
        set_error(PX_ERROR_INVALID_ARGUMENT);
        return -1;
    }
    if (!require_state(PX_STATE_RECORDING)) {
        return -2;
    }

    PxAudioAnalysis partial_audio;
    PxSceneDescriptor partial_scene;
    std::memset(&partial_audio, 0, sizeof(partial_audio));
    std::memset(&partial_scene, 0, sizeof(partial_scene));

    pulse_x_audio::finalize(&g_audio_accumulator, g_config.sample_rate, &partial_audio);
    pulse_x_audio::derive_scene(&partial_audio, &partial_scene);

    pulse_x_voice_scene::build_live_scene(&partial_audio, &partial_scene, out_scene);
    return 0;
}

int32_t px_core_generate_voice_scene(int32_t device_tier) {
    if (!require_state(PX_STATE_GENERATION)) {
        return -1;
    }
    const PxDeviceTier tier = static_cast<PxDeviceTier>(device_tier);
    pulse_x_voice_scene::build_final_scene(
        &g_audio_analysis, &g_scene_descriptor, tier, &g_voice_scene_descriptor);
    g_voice_scene_generated = true;
    return 0;
}

int32_t px_core_get_voice_scene(PxVoiceSceneDescriptor* out_scene) {
    if (out_scene == nullptr) {
        set_error(PX_ERROR_INVALID_ARGUMENT);
        return -1;
    }
    if (!g_voice_scene_generated) {
        set_error(PX_ERROR_INVALID_STATE);
        return -2;
    }
    *out_scene = g_voice_scene_descriptor;
    return 0;
}

int32_t px_core_get_witch_special(PxWitchSpecialDescriptor* out_witch) {
    if (out_witch == nullptr) {
        set_error(PX_ERROR_INVALID_ARGUMENT);
        return -1;
    }
    pulse_x_voice_scene::get_witch_special(out_witch);
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
    g_cinematic_render_built = false;
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

int32_t px_core_build_cinematic_render(int32_t device_tier) {
    if (!require_state(PX_STATE_RENDER)) {
        return -1;
    }
    if (!g_world_generated || !g_voice_scene_generated) {
        set_error(PX_ERROR_INVALID_STATE);
        return -2;
    }

    const PxDeviceTier tier = static_cast<PxDeviceTier>(device_tier);
    pulse_x_render::build_render_descriptor(
        &g_voice_scene_descriptor, &g_world_descriptor, tier,
        &g_cinematic_render_descriptor);
    g_cinematic_render_built = true;
    return 0;
}

int32_t px_core_get_cinematic_render(PxCinematicRenderDescriptor* out_render) {
    if (out_render == nullptr) {
        set_error(PX_ERROR_INVALID_ARGUMENT);
        return -1;
    }
    if (!g_cinematic_render_built) {
        set_error(PX_ERROR_INVALID_STATE);
        return -2;
    }
    *out_render = g_cinematic_render_descriptor;
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
    g_watermark_applied = false;
    g_state.store(PX_STATE_EXPORT);
    return 0;
}

int32_t px_core_apply_watermark(void) {
    if (!require_state(PX_STATE_EXPORT)) {
        return -1;
    }

    PxWitchSpecialDescriptor witch;
    pulse_x_voice_scene::get_witch_special(&witch);
    pulse_x_brand::build_watermark_state(
        witch.flight_height, witch.glow_intensity, witch.staff_seed,
        &g_watermark_descriptor);

    int32_t duration_ms = 0;
    if (g_audio_analysis.sample_count > 0 && g_config.sample_rate > 0) {
        duration_ms = static_cast<int32_t>(
            (static_cast<int64_t>(g_audio_analysis.sample_count) * 1000) /
            g_config.sample_rate);
    }
    pulse_x_brand::build_final_video_frame(
        &g_cinematic_render_descriptor, duration_ms, &g_final_video_frame);

    g_watermark_applied = true;
    return 0;
}

int32_t px_core_get_watermark_state(PxBrandWatermarkDescriptor* out_watermark) {
    if (out_watermark == nullptr) {
        set_error(PX_ERROR_INVALID_ARGUMENT);
        return -1;
    }
    if (!g_watermark_applied) {
        set_error(PX_ERROR_INVALID_STATE);
        return -2;
    }
    *out_watermark = g_watermark_descriptor;
    return 0;
}

int32_t px_core_get_final_video_frame(PxFinalVideoFrame* out_frame) {
    if (out_frame == nullptr) {
        set_error(PX_ERROR_INVALID_ARGUMENT);
        return -1;
    }
    if (!g_watermark_applied) {
        set_error(PX_ERROR_INVALID_STATE);
        return -2;
    }
    *out_frame = g_final_video_frame;
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
    const int32_t current = g_state.load();
    if (current == PX_STATE_RENDER || current == PX_STATE_EXPORT) {
        g_state.store(PX_STATE_READY);
        return 0;
    }
    set_error(PX_ERROR_INVALID_STATE);
    return -1;
}

} // extern "C"
