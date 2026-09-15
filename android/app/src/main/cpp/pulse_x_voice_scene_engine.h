#ifndef PULSE_X_VOICE_SCENE_ENGINE_H
#define PULSE_X_VOICE_SCENE_ENGINE_H

#include <stdint.h>
#include "pulse_x_types.h"
#include "pulse_x_scene_types.h"

/* Internal Voice Scene Engine (Paper 5), including its Mirror Engine
 * sub-module. Not exposed via PX_EXPORT - implementation detail of
 * CORE only. Per Bridge Law: logic stays in its own paper, only
 * integration lives in CORE. Nothing outside pulse_x_core.cpp may
 * call this. */

namespace pulse_x_voice_scene {

/* Mirror Engine: automatically selects a scenario family and mood
 * intensity from the audio-derived signals only - no manual
 * scenario choice is ever required. Deterministic: same audio
 * input always mirrors to the same scenario family. */
PxScenarioFamily select_scenario_family(const PxAudioAnalysis* audio,
                                         const PxSceneDescriptor* scene);

/* Voice Mapping: Energy -> movement, Pitch -> visual response,
 * Rhythm -> animation timing, Frequency -> effects,
 * Intensity -> scale/motion/lighting. */
void compute_voice_mapping(const PxAudioAnalysis* audio,
                            const PxSceneDescriptor* scene,
                            PxVoiceMapping* out_mapping);

/* Live Sync: builds a scene preview directly from the in-progress
 * accumulator state (via the caller-supplied partial analysis),
 * so the scene reacts to voice while recording is still held -
 * not only after release. */
void build_live_scene(const PxAudioAnalysis* partial_audio,
                       const PxSceneDescriptor* partial_scene,
                       PxVoiceSceneDescriptor* out_descriptor);

/* Multi-Output Mode: fills up to device-tier-capped variation
 * families from the same underlying audio, alongside the primary
 * scenario family. Low tier = 1 variation (primary only), high
 * tier = up to 3. */
void build_final_scene(const PxAudioAnalysis* audio,
                        const PxSceneDescriptor* scene,
                        PxDeviceTier device_tier,
                        PxVoiceSceneDescriptor* out_descriptor);

/* Fixed special scenario descriptor for the flying witch-like
 * figure, reused identically by the mandatory Paper 7 watermark. */
void get_witch_special(PxWitchSpecialDescriptor* out_witch);

} // namespace pulse_x_voice_scene

#endif /* PULSE_X_VOICE_SCENE_ENGINE_H */
