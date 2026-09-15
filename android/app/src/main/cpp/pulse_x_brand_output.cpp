#include "pulse_x_brand_output.h"

#include <cstring>

namespace pulse_x_brand {

void build_watermark_state(float witch_flight_height,
                            float witch_glow_intensity,
                            int32_t witch_staff_seed,
                            PxBrandWatermarkDescriptor* out_watermark) {
    if (out_watermark == nullptr) {
        return;
    }
    std::memset(out_watermark, 0, sizeof(PxBrandWatermarkDescriptor));
    out_watermark->enabled = 1;
    out_watermark->witch_flight_height = witch_flight_height;
    out_watermark->witch_glow_intensity = witch_glow_intensity;
    out_watermark->witch_staff_seed = witch_staff_seed;
    out_watermark->logo_animation_phase = 0.0f;
    out_watermark->logo_position_corner = 3;
}

void build_final_video_frame(const PxCinematicRenderDescriptor* render,
                              int32_t duration_ms,
                              PxFinalVideoFrame* out_frame) {
    if (render == nullptr || out_frame == nullptr) {
        return;
    }
    std::memset(out_frame, 0, sizeof(PxFinalVideoFrame));
    out_frame->width = 1080;
    out_frame->height = 1920;
    out_frame->fps = render->target_fps > 0 ? render->target_fps : 30;
    out_frame->duration_ms = duration_ms;
    out_frame->has_watermark = 1;
}

} // namespace pulse_x_brand
