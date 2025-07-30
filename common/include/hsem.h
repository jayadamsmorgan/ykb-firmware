#ifndef HSEM_H
#define HSEM_H

#include "settings.h"

#define HSEM_MANAGER_ENABLED 1

#if defined(HSEM_MANAGER_ENABLED) && HSEM_MANAGER_ENABLED == 1

#include "hal_hsem.h"

// HSEM Management routine

typedef struct __hsem_t hsem_t;

typedef enum {
    HSEM_STATUS_RESET = 0U,
    HSEM_STATUS_FREE = 1U,
    HSEM_STATUS_LOCKED = 2U,
} hsem_status;

hsem_t *hsem_new(uint8_t proc_id);
void hsem_release(hsem_t *hsem);

hal_err hsem_lock(hsem_t *hsem);
hal_err hsem_unlock(hsem_t *hsem);

bool hsem_locked(hsem_t *hsem);
uint8_t hsem_proc_id(hsem_t *hsem);

#endif // HSEM_MANAGER_ENABLED && HSEM_MANAGER_ENABLED == 1

#endif // HSEM_H
