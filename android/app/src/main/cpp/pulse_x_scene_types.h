#ifndef PULSE_X_SCENE_TYPES_H
#define PULSE_X_SCENE_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Fixed-size data contracts for Paper 5 (PULSE-X VOICE SCENE ENGINE).
 * All structs are fixed-size, no heap pointers, pre-allocated at
 * INITIALIZE per the Memory Model. */

typedef enum PxScenarioFamily {
    PX_SCENARIO_COMEDY = 0,
    PX_SCENARIO_MOCKING = 1,
    PX_SCENARIO_CRAZY = 2,
    PX_SCENARIO_HORROR = 3,
    PX_SCENARIO_CRYING = 4,
    PX_SCENARIO_RAGE = 5,
    PX_SCENARIO_DANCING = 6,
    PX_SCENARIO_STREET = 7,
    PX_SCENARIO_TRAVEL = 8,
    PX_SCENARIO_BEACH = 9,
    PX_SCENARIO_HOSPITAL = 10,
    PX_SCENARIO_FANTASY = 11,
    PX_SCENARIO_WITCH_SPECIAL = 12
} PxScenarioFamily;

typedef enum PxDeviceTier {
    PX_DEVICE_TIER_LOW = 0,
    PX_DEVICE_TIER_MID = 1,
    PX_DEVICE_TIER_HIGH = 2
} PxDeviceTier;

typedef struct PxVoiceMapping {
    float movement_amount;
    float visual_response;
    float animation_timing_scale;
    float effects_intensity;
    float scale_motion_lighting;
} PxVoiceMapping;

typedef struct PxVoiceSceneDescriptor {
    int32_t scenario_family;
    float mood_intensity;
    PxVoiceMapping mapping;
    int32_t is_live_preview;
    int32_t device_tier;
    int32_t variation_count;
    int32_t variation_families[3];
    int32_t reserved[4];
} PxVoiceSceneDescriptor;

typedef struct PxWitchSpecialDescriptor {
    float flight_height;
    float glow_intensity;
    int32_t staff_seed;
    int32_t reserved[3];
} PxWitchSpecialDescriptor;

#ifdef __cplusplus
}
#endif

#endif /* PULSE_X_SCENE_TYPES_H */
