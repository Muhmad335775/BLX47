#ifndef PULSE_X_PROCEDURAL_WORLD_H
#define PULSE_X_PROCEDURAL_WORLD_H

#include <stdint.h>
#include "pulse_x_types.h"
#include "pulse_x_world_types.h"

/* Internal procedural world generator (Paper 4).
 * Not exposed via PX_EXPORT - implementation detail of CORE only.
 * Per Bridge Law: logic stays in its own paper, only integration
 * lives in CORE. Nothing outside pulse_x_core.cpp may call this. */

namespace pulse_x_world {

/* Deterministic, allocation-free xorshift32 PRNG. Same seed always
 * produces the same world, so "random" stays coherent and
 * reproducible rather than meaningless. */
struct WorldRng {
    uint32_t state;
};

void seed_rng(WorldRng* rng, uint32_t seed);
uint32_t next_u32(WorldRng* rng);
float next_float01(WorldRng* rng);

/* Generates a positive, always-safe global scene descriptor from
 * the audio-derived PxSceneDescriptor (energy/scene_id) plus a
 * caller-provided variation seed (e.g. day-of-year, or country
 * code hash). Never selects offensive content - by construction,
 * only the positive element set defined in pulse_x_world_types.h
 * is ever produced. */
void generate_world(const PxSceneDescriptor* scene,
                     int32_t variation_seed,
                     PxWorldDescriptor* out_world);

/* Selects one of the fixed special landmarks for a given day,
 * or PX_LANDMARK_NONE on days with no landmark scene. At most
 * one or two positive cinematic scenes are meant to surface per
 * day per the Daily Cinematic Scene rule - the caller (CORE)
 * decides cadence; this function only decides which landmark. */
int32_t select_daily_landmark(int32_t day_seed);

/* Populates procedural, non-repeating fireworks parameters
 * (launch point, trajectory, burst, timing, scale, particles,
 * lighting) from the rng. Random inputs always produce a
 * coherent, physically plausible firework - never a meaningless
 * or degenerate one (e.g. zero radius or zero particles). */
void generate_fireworks(WorldRng* rng, PxFireworksParams* out_params);

} // namespace pulse_x_world

#endif /* PULSE_X_PROCEDURAL_WORLD_H */
