#ifndef PULSE_X_CORE_H
#define PULSE_X_CORE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
  #define PX_EXPORT __declspec(dllexport)
#else
  #define PX_EXPORT __attribute__((visibility("default")))
#endif

/* Public C API of PULSE-X CORE.
 * This is the only boundary the Flutter/Dart UI is allowed to call.
 * Papers 3-8 are implemented behind this boundary and are never
 * called directly from Dart. */

PX_EXPORT int32_t px_core_version(void);

PX_EXPORT int32_t px_core_init(void);

PX_EXPORT void px_core_shutdown(void);

PX_EXPORT int32_t px_core_feed_test_audio(const float* samples, int32_t sample_count);

#ifdef __cplusplus
}
#endif

#endif /* PULSE_X_CORE_H */
