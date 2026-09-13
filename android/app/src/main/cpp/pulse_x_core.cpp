#include "pulse_x_core.h"

#include <atomic>
#include <android/log.h>

#define PX_LOG_TAG "PulseXCore"
#define PX_LOGI(...) __android_log_print(ANDROID_LOG_INFO, PX_LOG_TAG, __VA_ARGS__)

namespace {
std::atomic<bool> g_initialized{false};
}

extern "C" {

int32_t px_core_version(void) {
    return 1;
}

int32_t px_core_init(void) {
    if (g_initialized.exchange(true)) {
        return 0;
    }
    PX_LOGI("PULSE-X CORE initialized (Paper 1 stub)");
    return 0;
}

void px_core_shutdown(void) {
    if (!g_initialized.exchange(false)) {
        return;
    }
    PX_LOGI("PULSE-X CORE shut down");
}

int32_t px_core_feed_test_audio(const float* samples, int32_t sample_count) {
    if (!g_initialized.load()) {
        return -1;
    }
    if (samples == nullptr || sample_count <= 0) {
        return -2;
    }
    return 0;
}

} // extern "C"
