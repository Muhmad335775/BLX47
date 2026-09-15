#ifndef PULSE_X_RENDER_ENGINE_H
#define PULSE_X_RENDER_ENGINE_H

#include <stdint.h>
#include "pulse_x_types.h"
#include "pulse_x_scene_types.h"
#include "pulse_x_world_types.h"
#include "pulse_x_render_types.h"

/* Internal PULSE-X RENDER engine (Paper 6). Not exposed via
 * PX_EXPORT - implementation detail of CORE only. Per Bridge Law:
 * logic stays in its own paper, only integration lives in CORE.
 * Nothing outside pulse_x_core.cpp may call this.
 *
 * This layer computes the full cinematic render descriptor
 * (depth, camera, lighting, FX flags, particle/atmosphere budget)
 * from the voice scene + procedural world. It does not issue any
 * Vulkan/OpenGL ES draw calls itself - the actual GPU backend
 * (Vulkan primary, OpenGL ES 3.2 fallback) consumes this
 * descriptor to draw. */

namespace pulse_x_render {

/* Chooses Vulkan vs OpenGL ES fallback for the given device tier.
 * Low-tier devices fall back to OpenGL ES 3.2 automatically. */
PxRenderBackend select_backend(PxDeviceTier device_tier);

/* Caps target FPS and particle/atmosphere budget by device tier,
 * so the same scene renders at scaled detail rather than dropping
 * frames on weaker hardware. */
void apply_device_tiering(PxDeviceTier device_tier,
                           PxCinematicRenderDescriptor* out_descriptor);

/* Builds the five-layer depth composition
 * (BACKGROUND -> ENVIRONMENT -> MIDGROUND -> SUBJECT -> FOREGROUND)
 * driven by the voice scene's movement/visual-response mapping. */
void build_depth_layers(const PxVoiceSceneDescriptor* voice_scene,
                         PxCinematicRenderDescriptor* out_descriptor);

/* Builds camera parameters (position, FOV, pan/tilt/dolly speed)
 * from the voice scene's animation timing and scale/motion/
 * lighting mapping. */
void build_camera(const PxVoiceSceneDescriptor* voice_scene,
                   PxCameraParams* out_camera);

/* Builds key/fill/rim lighting and color temperature from the
 * procedural world's atmosphere and lighting warmth. */
void build_lighting(const PxWorldDescriptor* world,
                     PxLightingParams* out_lighting);

/* Selects the procedural Cinematic FX Library flags for a given
 * scenario family (Action / Horror / Epic groupings from the
 * paper), all generated in-engine - no copied filter packs. */
int32_t select_fx_flags(PxScenarioFamily family, int32_t has_fireworks);

/* Full pipeline: composes depth, camera, lighting, FX, and
 * device-tiered budget into a single cinematic render descriptor.
 * This is the "Large cinematic scene, not a tiny avatar" target -
 * is_large_scene is always 1. */
void build_render_descriptor(const PxVoiceSceneDescriptor* voice_scene,
                              const PxWorldDescriptor* world,
                              PxDeviceTier device_tier,
                              PxCinematicRenderDescriptor* out_descriptor);

} // namespace pulse_x_render

#endif /* PULSE_X_RENDER_ENGINE_H */
