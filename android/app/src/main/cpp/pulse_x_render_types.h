#ifndef PULSE_X_RENDER_TYPES_H
#define PULSE_X_RENDER_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Fixed-size data contracts for Paper 6 (PULSE-X CINEMATIC RENDERER).
 * All structs are fixed-size, no heap pointers, pre-allocated at
 * INITIALIZE per the Memory Model. */

typedef enum PxRenderBackend {
    PX_RENDER_BACKEND_VULKAN = 0,
    PX_RENDER_BACKEND_OPENGL_ES = 1
} PxRenderBackend;

typedef enum PxDepthLayer {
    PX_DEPTH_BACKGROUND = 0,
    PX_DEPTH_ENVIRONMENT = 1,
    PX_DEPTH_MIDGROUND = 2,
    PX_DEPTH_SUBJECT = 3,
    PX_DEPTH_FOREGROUND = 4,
    PX_DEPTH_LAYER_COUNT = 5
} PxDepthLayer;

/* Bitmask flags for the procedural cinematic FX library. */
typedef enum PxCinematicFxFlags {
    PX_FX_MOTION_BLUR = 1 << 0,
    PX_FX_CAMERA_SHAKE = 1 << 1,
    PX_FX_SPEED_RAMP = 1 << 2,
    PX_FX_SHOCKWAVE = 1 << 3,
    PX_FX_FILM_GRAIN = 1 << 4,
    PX_FX_CHROMATIC_ABERRATION = 1 << 5,
    PX_FX_FLICKER = 1 << 6,
    PX_FX_FOG = 1 << 7,
    PX_FX_LENS_FLARE = 1 << 8,
    PX_FX_VOLUMETRIC_LIGHT = 1 << 9,
    PX_FX_EMBERS = 1 << 10,
    PX_FX_SLOW_MOTION_BLOOM = 1 << 11
} PxCinematicFxFlags;

typedef struct PxDepthLayerParams {
    float parallax_factor;
    float blur_amount;
    float opacity;
    int32_t reserved;
} PxDepthLayerParams;

typedef struct PxCameraParams {
    float position_x;
    float position_y;
    float position_z;
    float field_of_view_deg;
    float pan_speed;
    float tilt_speed;
    float dolly_speed;
} PxCameraParams;

typedef struct PxLightingParams {
    float key_light_intensity;
    float fill_light_intensity;
    float rim_light_intensity;
    float color_temperature_shift;
} PxLightingParams;

typedef struct PxCinematicRenderDescriptor {
    int32_t backend;
    int32_t device_tier;
    int32_t fx_flags;
    PxDepthLayerParams depth_layers[PX_DEPTH_LAYER_COUNT];
    PxCameraParams camera;
    PxLightingParams lighting;
    float atmosphere_density;
    int32_t particle_count;
    int32_t target_fps;
    int32_t is_large_scene;
    int32_t reserved[4];
} PxCinematicRenderDescriptor;

#ifdef __cplusplus
}
#endif

#endif /* PULSE_X_RENDER_TYPES_H */
