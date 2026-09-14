#ifndef PULSE_X_WORLD_TYPES_H
#define PULSE_X_WORLD_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Fixed-size data contracts for Paper 4 (PULSE-X PROCEDURAL WORLD).
 * All structs are fixed-size, no heap pointers, so they can be
 * pre-allocated once at INITIALIZE per the Memory Model. */

typedef enum PxLandmarkId {
    PX_LANDMARK_NONE = 0,
    PX_LANDMARK_BURJ_KHALIFA = 1,
    PX_LANDMARK_EIFFEL_TOWER = 2,
    PX_LANDMARK_STATUE_OF_LIBERTY = 3,
    PX_LANDMARK_GREAT_PYRAMIDS_GIZA = 4
} PxLandmarkId;

/* Bitmask flags for positive global scene elements. */
typedef enum PxSceneElementFlags {
    PX_ELEMENT_SKY = 1 << 0,
    PX_ELEMENT_MOON = 1 << 1,
    PX_ELEMENT_STARS = 1 << 2,
    PX_ELEMENT_SEA = 1 << 3,
    PX_ELEMENT_BIRDS = 1 << 4,
    PX_ELEMENT_BUTTERFLIES = 1 << 5,
    PX_ELEMENT_ANGELS = 1 << 6,
    PX_ELEMENT_FANTASY = 1 << 7,
    PX_ELEMENT_FLAGS = 1 << 8,
    PX_ELEMENT_CELEBRATION = 1 << 9
} PxSceneElementFlags;

typedef struct PxFireworksParams {
    float launch_x;
    float launch_y;
    float burst_radius;
    float burst_height;
    int32_t particle_count;
    int32_t color_seed;
    int32_t timing_offset_ms;
    int32_t reserved;
} PxFireworksParams;

typedef struct PxWorldDescriptor {
    int32_t element_flags;
    int32_t landmark_id;
    int32_t has_fireworks;
    int32_t variation_seed;
    PxFireworksParams fireworks;
    float atmosphere_intensity;
    float lighting_warmth;
    int32_t reserved[4];
} PxWorldDescriptor;

#ifdef __cplusplus
}
#endif

#endif /* PULSE_X_WORLD_TYPES_H */
