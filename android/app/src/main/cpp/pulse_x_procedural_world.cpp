#include "pulse_x_procedural_world.h"

#include <cstring>

namespace pulse_x_world {

namespace {

/* Only ever positive elements. This set is fixed and exhaustive -
 * there is no path that can add an offensive element, since
 * nothing outside this array is ever selected. */
constexpr int32_t kPositiveElementPool[] = {
    PX_ELEMENT_SKY,
    PX_ELEMENT_MOON,
    PX_ELEMENT_STARS,
    PX_ELEMENT_SEA,
    PX_ELEMENT_BIRDS,
    PX_ELEMENT_BUTTERFLIES,
    PX_ELEMENT_ANGELS,
    PX_ELEMENT_FANTASY,
    PX_ELEMENT_FLAGS,
    PX_ELEMENT_CELEBRATION,
};
constexpr int32_t kPositiveElementCount =
    sizeof(kPositiveElementPool) / sizeof(kPositiveElementPool[0]);

constexpr int32_t kLandmarkPool[] = {
    PX_LANDMARK_BURJ_KHALIFA,
    PX_LANDMARK_EIFFEL_TOWER,
    PX_LANDMARK_STATUE_OF_LIBERTY,
    PX_LANDMARK_GREAT_PYRAMIDS_GIZA,
};
constexpr int32_t kLandmarkCount =
    sizeof(kLandmarkPool) / sizeof(kLandmarkPool[0]);

} // namespace

void seed_rng(WorldRng* rng, uint32_t seed) {
    rng->state = seed != 0 ? seed : 0x9E3779B9u;
}

uint32_t next_u32(WorldRng* rng) {
    uint32_t x = rng->state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng->state = x;
    return x;
}

float next_float01(WorldRng* rng) {
    return static_cast<float>(next_u32(rng) & 0x00FFFFFF) /
           static_cast<float>(0x01000000);
}

void generate_world(const PxSceneDescriptor* scene,
                     int32_t variation_seed,
                     PxWorldDescriptor* out_world) {
    if (scene == nullptr || out_world == nullptr) {
        return;
    }

    std::memset(out_world, 0, sizeof(PxWorldDescriptor));

    WorldRng rng;
    const uint32_t seed =
        static_cast<uint32_t>(variation_seed) ^
        static_cast<uint32_t>(scene->scene_id * 2654435761u) ^
        static_cast<uint32_t>(scene->energy * 1000.0f);
    seed_rng(&rng, seed);

    /* Energy from the audio scene drives how many positive
     * elements are layered in - higher energy, richer scene. */
    const int32_t min_elements = 2;
    const int32_t max_elements = kPositiveElementCount;
    const int32_t element_span = max_elements - min_elements;
    const int32_t element_count =
        min_elements +
        static_cast<int32_t>(next_float01(&rng) * static_cast<float>(element_span));

    int32_t flags = 0;
    for (int32_t i = 0; i < element_count; i++) {
        const int32_t idx =
            static_cast<int32_t>(next_float01(&rng) * static_cast<float>(kPositiveElementCount));
        const int32_t clamped_idx =
            idx >= kPositiveElementCount ? kPositiveElementCount - 1 : idx;
        flags |= kPositiveElementPool[clamped_idx];
    }
    out_world->element_flags = flags;

    out_world->landmark_id = PX_LANDMARK_NONE;
    out_world->variation_seed = variation_seed;

    out_world->atmosphere_intensity = 0.3f + (scene->energy * 0.7f);
    if (out_world->atmosphere_intensity > 1.0f) {
        out_world->atmosphere_intensity = 1.0f;
    }
    out_world->lighting_warmth = next_float01(&rng);

    if (scene->scene_id >= 2) {
        out_world->has_fireworks = 1;
        generate_fireworks(&rng, &out_world->fireworks);
    } else {
        out_world->has_fireworks = 0;
    }
}

int32_t select_daily_landmark(int32_t day_seed) {
    WorldRng rng;
    seed_rng(&rng, static_cast<uint32_t>(day_seed) * 40503u);

    /* Roughly one in three days surfaces a landmark scene, keeping
     * with "one or two positive national/landmark cinematic scenes
     * surfaced per day" without forcing one on every single day. */
    const float roll = next_float01(&rng);
    if (roll > 0.5f) {
        return PX_LANDMARK_NONE;
    }

    const int32_t idx =
        static_cast<int32_t>(next_float01(&rng) * static_cast<float>(kLandmarkCount));
    const int32_t clamped_idx = idx >= kLandmarkCount ? kLandmarkCount - 1 : idx;
    return kLandmarkPool[clamped_idx];
}

void generate_fireworks(WorldRng* rng, PxFireworksParams* out_params) {
    if (rng == nullptr || out_params == nullptr) {
        return;
    }

    out_params->launch_x = 0.2f + (next_float01(rng) * 0.6f);
    out_params->launch_y = 0.7f + (next_float01(rng) * 0.2f);
    out_params->burst_radius = 0.08f + (next_float01(rng) * 0.12f);
    out_params->burst_height = 0.3f + (next_float01(rng) * 0.4f);
    out_params->particle_count =
        60 + static_cast<int32_t>(next_float01(rng) * 140.0f);
    out_params->color_seed = static_cast<int32_t>(next_u32(rng) & 0x7FFFFFFF);
    out_params->timing_offset_ms =
        static_cast<int32_t>(next_float01(rng) * 800.0f);
    out_params->reserved = 0;
}

} // namespace pulse_x_world
