#include "pulse_x_world_colors.h"
#include <algorithm>

// Simple deterministic PRNG — same seed always produces same sequence
static uint32_t g_rng_state = 0;

static void seed_rng(uint32_t seed) {
    g_rng_state = seed == 0 ? 1 : seed; // 0 is not a valid LCG seed
}

static uint32_t next_rand(void) {
    g_rng_state = g_rng_state * 1664525u + 1013904223u; // classic LCG
    return g_rng_state;
}

static float rand_range(float min_val, float max_val) {
    float t = (float)(next_rand() % 10000) / 10000.0f;
    return min_val + t * (max_val - min_val);
}

int px_world_colors_generate(uint32_t seed, PxLight* out_lights, int max_lights, int* out_count) {
    if (!out_lights || !out_count) return -1; // null check
    if (max_lights <= 0) return -1;

    seed_rng(seed);

    int count = std::min(max_lights, PX_MAX_LIGHTS);

    // Key light ALWAYS at index 0 — never skipped, never blank
    out_lights[0].hue = rand_range(0.0f, 360.0f);
    out_lights[0].saturation = rand_range(0.5f, 1.0f);
    out_lights[0].value = rand_range(0.7f, 1.0f);
    out_lights[0].is_key_light = true;

    for (int i = 1; i < count; i++) {
        out_lights[i].hue = rand_range(0.0f, 360.0f);       // bounded 0-360
        out_lights[i].saturation = rand_range(0.0f, 1.0f);
        out_lights[i].value = rand_range(0.3f, 1.0f);
        out_lights[i].is_key_light = false;
    }

    *out_count = count;
    return 0;
}
