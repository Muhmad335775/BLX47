#ifndef PULSE_X_BRAND_OUTPUT_H
#define PULSE_X_BRAND_OUTPUT_H

#include <stdint.h>
#include "pulse_x_types.h"
#include "pulse_x_render_types.h"
#include "pulse_x_brand_types.h"

/* Internal Brand + Video Output layer (Paper 7). Not exposed via
 * PX_EXPORT - implementation detail of CORE only. Per Bridge Law:
 * logic stays in its own paper, only integration lives in CORE.
 * Nothing outside pulse_x_core.cpp may call this. */

namespace pulse_x_brand {

/* Builds the mandatory BLX47 watermark - reuses the same
 * witch-like figure from Paper 5's Mirror Engine special scenario,
 * plus the animated BLX47 logo. enabled is always 1; there is no
 * code path that sets it to 0 - the watermark cannot be disabled. */
void build_watermark_state(float witch_flight_height,
                            float witch_glow_intensity,
                            int32_t witch_staff_seed,
                            PxBrandWatermarkDescriptor* out_watermark);

/* Builds the final branded video frame descriptor from the
 * cinematic render descriptor and the recording's real duration.
 * has_watermark is always 1. */
void build_final_video_frame(const PxCinematicRenderDescriptor* render,
                              int32_t duration_ms,
                              PxFinalVideoFrame* out_frame);

} // namespace pulse_x_brand

#endif /* PULSE_X_BRAND_OUTPUT_H */
