#ifndef PULSE_X_BRAND_TYPES_H
#define PULSE_X_BRAND_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Fixed-size data contracts for Paper 7 (PULSE-X BRAND + VIDEO OUTPUT). */

typedef struct PxBrandWatermarkDescriptor {
    int32_t enabled;
    float witch_flight_height;
    float witch_glow_intensity;
    int32_t witch_staff_seed;
    float logo_animation_phase;
    int32_t logo_position_corner;
    int32_t reserved[3];
} PxBrandWatermarkDescriptor;

typedef struct PxFinalVideoFrame {
    int32_t width;
    int32_t height;
    int32_t fps;
    int32_t duration_ms;
    int32_t has_watermark;
    int32_t reserved[3];
} PxFinalVideoFrame;

#ifdef __cplusplus
}
#endif

#endif /* PULSE_X_BRAND_TYPES_H */
