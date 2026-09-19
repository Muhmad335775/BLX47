#pragma once
#include <cstdint>

extern "C" {

typedef struct {
    float hue;         // 0-360
    float saturation;  // 0-1
    float value;       // 0-1
    bool is_key_light;
} PxLight;

#define PX_MAX_LIGHTS 8

int px_world_colors_generate(uint32_t seed, PxLight* out_lights, int max_lights, int* out_count);

}
