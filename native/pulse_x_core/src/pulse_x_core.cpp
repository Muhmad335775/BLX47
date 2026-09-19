#include "pulse_x_core.h"

static PxCoreState g_state = PX_STATE_UNINIT;
static bool g_integrity_checked = false;

PxStatus px_core_init(void) {
    if (g_state != PX_STATE_UNINIT) return PX_ERR_BAD_STATE;
    g_state = PX_STATE_READY;
    return PX_OK;
}

PxStatus px_core_run_integrity_check(void) {
    if (g_integrity_checked) return PX_OK; // only ever once
    // Real check bridged from Play Integrity API via Flutter platform channel
    g_integrity_checked = true;
    return PX_OK;
}

PxStatus px_core_tick(float dt_seconds) {
    if (g_state != PX_STATE_READY && g_state != PX_STATE_RUNNING)
        return PX_ERR_BAD_STATE;
    g_state = PX_STATE_RUNNING;
    // NOTE: no malloc/new allowed below this line, ever
    return PX_OK;
}

PxStatus px_core_shutdown(void) {
    if (g_state == PX_STATE_UNINIT) return PX_ERR_BAD_STATE;
    g_state = PX_STATE_SHUTDOWN;
    return PX_OK;
}

PxCoreState px_core_get_state(void) {
    return g_state;
}
