#pragma once
#include <cstdint>

extern "C" {

typedef enum {
    PX_STATE_UNINIT = 0,
    PX_STATE_READY,
    PX_STATE_RUNNING,
    PX_STATE_SHUTDOWN
} PxCoreState;

typedef enum {
    PX_OK = 0,
    PX_ERR_NULL_PTR = 1,
    PX_ERR_BAD_STATE = 2,
    PX_ERR_INTEGRITY_FAILED = 3
} PxStatus;

// Core lifecycle — no malloc/new happens inside tick()
PxStatus px_core_init(void);
PxStatus px_core_run_integrity_check(void);   // one-time, first launch only
PxStatus px_core_tick(float dt_seconds);
PxStatus px_core_shutdown(void);
PxCoreState px_core_get_state(void);

}
