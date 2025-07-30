#include "hsem.h"

#include "hal_clock.h"

#include "error_handler.h"
#include "settings.h"

#if defined(HSEM_MANAGER_ENABLED) && HSEM_MANAGER_ENABLED == 1

struct __hsem_t {
    hsem_index index;
    __IO hsem_status status;
    __IO uint8_t proc_id;
};

static hsem_t hsem_pool[HSEM_COUNT] = {0};

static hsem_index last_freed_hsem_index = 0U;
static hsem_index last_taken_hsem_index = HSEM_COUNT - 1;
static hsem_index hsem_taken_count = 0U;

hal_err hsem_init() {
    clock_hsem_enable();
    for (size_t i = 0; i < HSEM_COUNT; i++) {
        hsem_pool[i].index = i;
        hsem_pool[i].status = HSEM_STATUS_RESET;
    }
    return OK;
}

hsem_t *hsem_new(uint8_t proc_id) {

    __disable_irq();

    if (hsem_taken_count == HSEM_COUNT)
        goto fail;

    size_t index;

    index = last_freed_hsem_index;
    if (hsem_pool[index].status == HSEM_STATUS_RESET)
        goto success;

    index = (last_taken_hsem_index + 1) % (HSEM_COUNT);
    if (hsem_pool[index].status == HSEM_STATUS_RESET)
        goto success;

    // Last resort, iterate through pool:
    for (size_t i = 0; i < HSEM_COUNT; i++) {
        if (hsem_pool[i].status == HSEM_STATUS_RESET) {
            index = i;
            goto success;
        }
    }

fail:
    __enable_irq();
    return NULL;

success:
    hsem_pool[index].status = HSEM_STATUS_FREE;
    last_taken_hsem_index = index;
    hsem_taken_count++;
    hsem_pool[index].proc_id = proc_id;

    hal_hsem_c1_enable_it(index);
    __enable_irq();

    return &hsem_pool[index];
}

void hsem_release(hsem_t *hsem) {

    __disable_irq();

    if (!hsem || hsem->status == HSEM_STATUS_RESET)
        goto irq_en;

    if (hsem->status == HSEM_STATUS_LOCKED)
        hsem_unlock(hsem);

    hsem->status = HSEM_STATUS_RESET;
    last_freed_hsem_index = hsem->index;
    hsem_taken_count--;

irq_en:
    __enable_irq();
    return;
}

hal_err hsem_lock(hsem_t *hsem) {

    __disable_irq();

    hal_err err = OK;

    if (!hsem) {
        err = ERR_HSEM_LOCK_ARGNULL;
        goto cleanup;
    }

    if (hsem->status == HSEM_STATUS_LOCKED) {
        err = ERR_HSEM_LOCK_ALREADY_LOCKED;
        goto cleanup;
    }

    err = hal_hsem_lock(hsem->index, hsem->proc_id);
    if (err)
        goto cleanup;

    hsem->status = HSEM_STATUS_LOCKED;

cleanup:
    __enable_irq();
    return err;
}

bool hsem_locked(hsem_t *hsem) {
    return hsem->status == HSEM_STATUS_LOCKED;
}

uint8_t hsem_proc_id(hsem_t *hsem) {
    return hsem->proc_id;
}

hal_err hsem_unlock(hsem_t *hsem) {

    __disable_irq();

    hal_err err = OK;

    if (!hsem) {
        err = ERR_HSEM_UNLOCK_ARGNULL;
        goto cleanup;
    }

    if (hsem->status == HSEM_STATUS_FREE) {
        err = ERR_HSEM_UNLOCK_ALREADY_UNLOCKED;
        goto cleanup;
    }

    hal_hsem_unlock(hsem->index, hsem->proc_id);

    hsem->status = HSEM_STATUS_FREE;

cleanup:
    __enable_irq();
    return err;
}

#endif // HSEM_MANAGER_ENABLED && HSEM_MANAGER_ENABLED == 1
