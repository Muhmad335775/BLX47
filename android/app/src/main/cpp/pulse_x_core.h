#ifndef PULSE_X_CORE_H
#define PULSE_X_CORE_H

#include <stdint.h>
#include "pulse_x_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
  #define PX_EXPORT __declspec(dllexport)
#else
  #define PX_EXPORT __attribute__((visibility("default")))
#endif

/* PULSE-X CORE public C API - the single bridge.
 * Every system communicates through this surface only.
 * Papers 3-8 are implemented behind CORE and are never
 * called directly by the Flutter/Dart UI.
 *
 * Flow: INITIALIZE -> READY -> RECORDING -> PROCESSING ->
 *       GENERATION -> RENDER -> EXPORT -> READY
 * Any stage may transition to ERROR, with a defined
 * recovery path back to READY. */

typedef enum PxState {
    PX_STATE_UNINITIALIZED = 0,
    PX_STATE_READY = 1,
    PX_STATE_RECORDING = 2,
    PX_STATE_PROCESSING = 3,
    PX_STATE_GENERATION = 4,
    PX_STATE_RENDER = 5,
    PX_STATE_EXPORT = 6,
    PX_STATE_ERROR = 7
} PxState;

typedef enum PxErrorCode {
    PX_ERROR_NONE = 0,
    PX_ERROR_NOT_INITIALIZED = 1,
    PX_ERROR_INVALID_STATE = 2,
    PX_ERROR_INVALID_ARGUMENT = 3,
    PX_ERROR_ALREADY_INITIALIZED = 4
} PxErrorCode;

PX_EXPORT int32_t px_core_version(void);

PX_EXPORT int32_t px_core_init(const PxEngineConfig* config);

PX_EXPORT void px_core_shutdown(void);

PX_EXPORT int32_t px_core_get_state(void);

PX_EXPORT int32_t px_core_get_last_error(void);

PX_EXPORT int32_t px_core_recover(void);

PX_EXPORT int32_t px_core_begin_recording(void);

PX_EXPORT int32_t px_core_feed_audio(const float* samples, int32_t sample_count);

PX_EXPORT int32_t px_core_end_recording(void);

PX_EXPORT int32_t px_core_get_audio_analysis(PxAudioAnalysis* out_analysis);

PX_EXPORT int32_t px_core_get_scene_descriptor(PxSceneDescriptor* out_scene);

PX_EXPORT int32_t px_core_begin_render(void);

PX_EXPORT int32_t px_core_get_render_frame(PxRenderFrame* out_frame);

PX_EXPORT int32_t px_core_begin_export(void);

PX_EXPORT int32_t px_core_get_export_result(PxExportResult* out_result);

PX_EXPORT int32_t px_core_get_storage_state(PxStorageState* out_storage);

PX_EXPORT int32_t px_core_return_to_ready(void);

#ifdef __cplusplus
}
#endif

#endif /* PULSE_X_CORE_H */
