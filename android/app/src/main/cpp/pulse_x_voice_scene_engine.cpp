#include "pulse_x_voice_scene_engine.h"

#include <cstring>

namespace pulse_x_voice_scene {

namespace {

struct SceneRng {
    uint32_t state;
};

void seed_rng(SceneRng* rng, uint32_t seed) {
    rng->state = seed != 0 ? seed : 0x85EBCA6Bu;
}

uint32_t next_u32(SceneRng* rng) {
    uint32_t x = rng->state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng->state = x;
    return x;
}

float next_float01(SceneRng* rng) {
    return static_cast<float>(next_u32(rng) & 0x00FFFFFF) /
           static_cast<float>(0x01000000);
}

constexpr int32_t kVariationPool[] = {
    PX_SCENARIO_COMEDY,   PX_SCENARIO_MOCKING, PX_SCENARIO_CRAZY,
    PX_SCENARIO_HORROR,   PX_SCENARIO_CRYING,  PX_SCENARIO_RAGE,
    PX_SCENARIO_DANCING,  PX_SCENARIO_STREET,  PX_SCENARIO_TRAVEL,
    PX_SCENARIO_BEACH,    PX_SCENARIO_HOSPITAL, PX_SCENARIO_FANTASY,
};
constexpr int32_t kVariationPoolCount =
    sizeof(kVariationPool) / sizeof(kVariationPool[0]);

int32_t max_variations_for_tier(PxDeviceTier tier) {
    switch (tier) {
        case PX_DEVICE_TIER_LOW:
            return 1;
        case PX_DEVICE_TIER_MID:
            return 2;
        case PX_DEVICE_TIER_HIGH:
        default:
            return 3;
    }
}

} // namespace

PxScenarioFamily select_scenario_family(const PxAudioAnalysis* audio,
                                         const PxSceneDescriptor* scene) {
    if (audio == nullptr || scene == nullptr) {
        return PX_SCENARIO_COMEDY;
    }

    const float energy = scene->energy;
    const float freq = audio->dominant_frequency_hz;
    const float peak = audio->peak_level;

    const bool high_energy = energy > 0.15f;
    const bool low_energy = energy <= 0.04f;
    const bool high_freq = freq > 220.0f;
    const bool low_freq = freq <= 90.0f && freq > 0.0f;
    const bool silent_freq = freq <= 0.0f;

    if (peak > 0.85f && high_energy) {
        return PX_SCENARIO_RAGE;
    }
    if (high_energy && high_freq) {
        return PX_SCENARIO_CRAZY;
    }
    if (high_energy && low_freq) {
        return PX_SCENARIO_DANCING;
    }
    if (high_energy) {
        return PX_SCENARIO_STREET;
    }
    if (low_energy && high_freq) {
        return PX_SCENARIO_CRYING;
    }
    if (low_energy && (low_freq || silent_freq)) {
        return PX_SCENARIO_HOSPITAL;
    }
    if (low_energy) {
        return PX_SCENARIO_COMEDY;
    }
    if (high_freq) {
        return PX_SCENARIO_HORROR;
    }
    if (low_freq) {
        return PX_SCENARIO_BEACH;
    }
    return PX_SCENARIO_MOCKING;
}

void compute_voice_mapping(const PxAudioAnalysis* audio,
                            const PxSceneDescriptor* scene,
                            PxVoiceMapping* out_mapping) {
    if (audio == nullptr || scene == nullptr || out_mapping == nullptr) {
        return;
    }

    std::memset(out_mapping, 0, sizeof(PxVoiceMapping));

    /* Energy -> movement */
    out_mapping->movement_amount = scene->energy > 1.0f ? 1.0f : scene->energy;

    /* Pitch (approximated via dominant frequency) -> visual response */
    const float normalized_freq =
        audio->dominant_frequency_hz > 500.0f
            ? 1.0f
            : audio->dominant_frequency_hz / 500.0f;
    out_mapping->visual_response = normalized_freq;

    /* Rhythm (approximated via peak-to-rms variability) -> animation timing */
    const float rhythm_signal =
        audio->rms_level > 0.0f
            ? (audio->peak_level / (audio->rms_level + 0.0001f))
            : 0.0f;
    out_mapping->animation_timing_scale =
        rhythm_signal > 4.0f ? 1.0f : rhythm_signal / 4.0f;

    /* Frequency -> effects intensity */
    out_mapping->effects_intensity = normalized_freq * out_mapping->movement_amount;
    if (out_mapping->effects_intensity > 1.0f) {
        out_mapping->effects_intensity = 1.0f;
    }

    /* Intensity (peak) -> scale/motion/lighting */
    out_mapping->scale_motion_lighting =
        audio->peak_level > 1.0f ? 1.0f : audio->peak_level;
}

void build_live_scene(const PxAudioAnalysis* partial_audio,
                       const PxSceneDescriptor* partial_scene,
                       PxVoiceSceneDescriptor* out_descriptor) {
    if (partial_audio == nullptr || partial_scene == nullptr ||
        out_descriptor == nullptr) {
        return;
    }

    std::memset(out_descriptor, 0, sizeof(PxVoiceSceneDescriptor));

    out_descriptor->scenario_family =
        select_scenario_family(partial_audio, partial_scene);
    out_descriptor->mood_intensity = partial_scene->energy > 1.0f
        ? 1.0f
        : partial_scene->energy;
    compute_voice_mapping(partial_audio, partial_scene, &out_descriptor->mapping);
    out_descriptor->is_live_preview = 1;
    out_descriptor->variation_count = 1;
    out_descriptor->variation_families[0] = out_descriptor->scenario_family;
}

void build_final_scene(const PxAudioAnalysis* audio,
                        const PxSceneDescriptor* scene,
                        PxDeviceTier device_tier,
                        PxVoiceSceneDescriptor* out_descriptor) {
    if (audio == nullptr || scene == nullptr || out_descriptor == nullptr) {
        return;
    }

    std::memset(out_descriptor, 0, sizeof(PxVoiceSceneDescriptor));

    const PxScenarioFamily primary = select_scenario_family(audio, scene);

    out_descriptor->scenario_family = primary;
    out_descriptor->mood_intensity = scene->energy > 1.0f ? 1.0f : scene->energy;
    compute_voice_mapping(audio, scene, &out_descriptor->mapping);
    out_descriptor->is_live_preview = 0;
    out_descriptor->device_tier = static_cast<int32_t>(device_tier);

    const int32_t max_variations = max_variations_for_tier(device_tier);
    out_descriptor->variation_count = max_variations;
    out_descriptor->variation_families[0] = primary;

    if (max_variations > 1) {
        SceneRng rng;
        const uint32_t seed =
            static_cast<uint32_t>(primary) * 2654435761u +
            static_cast<uint32_t>(audio->sample_count) +
            static_cast<uint32_t>(scene->energy * 100000.0f);
        seed_rng(&rng, seed);

        for (int32_t i = 1; i < max_variations && i < 3; i++) {
            const int32_t idx = static_cast<int32_t>(
                next_float01(&rng) * static_cast<float>(kVariationPoolCount));
            const int32_t clamped =
                idx >= kVariationPoolCount ? kVariationPoolCount - 1 : idx;
            out_descriptor->variation_families[i] = kVariationPool[clamped];
        }
    }
}

void get_witch_special(PxWitchSpecialDescriptor* out_witch) {
    if (out_witch == nullptr) {
        return;
    }
    std::memset(out_witch, 0, sizeof(PxWitchSpecialDescriptor));
    out_witch->flight_height = 0.75f;
    out_witch->glow_intensity = 0.9f;
    out_witch->staff_seed = 4471;
}

} // namespace pulse_x_voice_scene
