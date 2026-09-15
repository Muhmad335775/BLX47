#include "pulse_x_render_engine.h"

#include <cstring>

namespace pulse_x_render {

PxRenderBackend select_backend(PxDeviceTier device_tier) {
    if (device_tier == PX_DEVICE_TIER_LOW) {
        return PX_RENDER_BACKEND_OPENGL_ES;
    }
    return PX_RENDER_BACKEND_VULKAN;
}

void apply_device_tiering(PxDeviceTier device_tier,
                           PxCinematicRenderDescriptor* out_descriptor) {
    if (out_descriptor == nullptr) {
        return;
    }

    out_descriptor->device_tier = static_cast<int32_t>(device_tier);
    out_descriptor->backend = static_cast<int32_t>(select_backend(device_tier));

    switch (device_tier) {
        case PX_DEVICE_TIER_LOW:
            out_descriptor->target_fps = 30;
            out_descriptor->particle_count = 80;
            break;
        case PX_DEVICE_TIER_MID:
            out_descriptor->target_fps = 60;
            out_descriptor->particle_count = 220;
            break;
        case PX_DEVICE_TIER_HIGH:
        default:
            out_descriptor->target_fps = 60;
            out_descriptor->particle_count = 500;
            break;
    }
}

void build_depth_layers(const PxVoiceSceneDescriptor* voice_scene,
                         PxCinematicRenderDescriptor* out_descriptor) {
    if (voice_scene == nullptr || out_descriptor == nullptr) {
        return;
    }

    const float movement = voice_scene->mapping.movementAmount;
    const float visual = voice_scene->mapping.visualResponse;

    static const float kParallaxBase[PX_DEPTH_LAYER_COUNT] = {
        0.05f, 0.15f, 0.35f, 0.70f, 1.00f
    };
    static const float kBlurBase[PX_DEPTH_LAYER_COUNT] = {
        0.60f, 0.35f, 0.15f, 0.00f, 0.10f
    };
    static const float kOpacityBase[PX_DEPTH_LAYER_COUNT] = {
        1.00f, 1.00f, 1.00f, 1.00f, 0.85f
    };

    for (int32_t i = 0; i < PX_DEPTH_LAYER_COUNT; i++) {
        PxDepthLayerParams* layer = &out_descriptor->depth_layers[i];
        layer->parallax_factor = kParallaxBase[i] * (0.5f + movement);
        layer->blur_amount = kBlurBase[i] * (1.0f - visual * 0.5f);
        if (layer->blur_amount < 0.0f) {
            layer->blur_amount = 0.0f;
        }
        layer->opacity = kOpacityBase[i];
        layer->reserved = 0;
    }
}

void build_camera(const PxVoiceSceneDescriptor* voice_scene,
                   PxCameraParams* out_camera) {
    if (voice_scene == nullptr || out_camera == nullptr) {
        return;
    }

    std::memset(out_camera, 0, sizeof(PxCameraParams));

    out_camera->position_x = 0.0f;
    out_camera->position_y = 1.6f;
    out_camera->position_z = -3.5f;
    out_camera->field_of_view_deg = 45.0f + (voice_scene->mapping.effectsIntensity * 20.0f);

    out_camera->pan_speed =
        voice_scene->mapping.animationTimingScale * 0.6f;
    out_camera->tilt_speed =
        voice_scene->mapping.visualResponse * 0.4f;
    out_camera->dolly_speed =
        voice_scene->mapping.scaleMotionLighting * 0.5f;
}

void build_lighting(const PxWorldDescriptor* world,
                     PxLightingParams* out_lighting) {
    if (world == nullptr || out_lighting == nullptr) {
        return;
    }

    std::memset(out_lighting, 0, sizeof(PxLightingParams));

    out_lighting->key_light_intensity = 0.5f + (world->atmosphereIntensity * 0.5f);
    out_lighting->fill_light_intensity = 0.3f + (world->lightingWarmth * 0.3f);
    out_lighting->rim_light_intensity = world->atmosphereIntensity * 0.4f;
    out_lighting->color_temperature_shift = (world->lightingWarmth - 0.5f) * 2.0f;
}

int32_t select_fx_flags(PxScenarioFamily family, int32_t has_fireworks) {
    int32_t flags = 0;

    switch (family) {
        case PX_SCENARIO_RAGE:
        case PX_SCENARIO_CRAZY:
        case PX_SCENARIO_STREET:
        case PX_SCENARIO_DANCING:
            flags |= PX_FX_MOTION_BLUR | PX_FX_CAMERA_SHAKE | PX_FX_SPEED_RAMP;
            break;
        case PX_SCENARIO_HORROR:
        case PX_SCENARIO_CRYING:
        case PX_SCENARIO_HOSPITAL:
            flags |= PX_FX_FILM_GRAIN | PX_FX_CHROMATIC_ABERRATION |
                     PX_FX_FLICKER | PX_FX_FOG;
            break;
        case PX_SCENARIO_FANTASY:
        case PX_SCENARIO_TRAVEL:
        case PX_SCENARIO_BEACH:
        case PX_SCENARIO_WITCH_SPECIAL:
            flags |= PX_FX_LENS_FLARE | PX_FX_VOLUMETRIC_LIGHT |
                     PX_FX_EMBERS | PX_FX_SLOW_MOTION_BLOOM;
            break;
        case PX_SCENARIO_COMEDY:
        case PX_SCENARIO_MOCKING:
        default:
            flags |= PX_FX_LENS_FLARE;
            break;
    }

    if (has_fireworks) {
        flags |= PX_FX_SHOCKWAVE | PX_FX_EMBERS;
    }

    return flags;
}

void build_render_descriptor(const PxVoiceSceneDescriptor* voice_scene,
                              const PxWorldDescriptor* world,
                              PxDeviceTier device_tier,
                              PxCinematicRenderDescriptor* out_descriptor) {
    if (voice_scene == nullptr || world == nullptr || out_descriptor == nullptr) {
        return;
    }

    std::memset(out_descriptor, 0, sizeof(PxCinematicRenderDescriptor));

    apply_device_tiering(device_tier, out_descriptor);
    build_depth_layers(voice_scene, out_descriptor);
    build_camera(voice_scene, &out_descriptor->camera);
    build_lighting(world, &out_descriptor->lighting);

    out_descriptor->fx_flags = select_fx_flags(
        static_cast<PxScenarioFamily>(voice_scene->scenarioFamily),
        world->hasFireworks);

    out_descriptor->atmosphere_density = world->atmosphereIntensity;
    out_descriptor->is_large_scene = 1;
}

} // namespace pulse_x_render
