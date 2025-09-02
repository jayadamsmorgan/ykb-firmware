#include "hsem.h"

#include "hal_clock.h"

#include "error_handler.h"
#include "logging.h"
#include "settings.h"
#include "utils/utils.h"

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
    LOG_INFO("Setting up...");
    LOG_TRACE("Enabling clocks...");
    clock_hsem_enable();
    LOG_TRACE("Initializing HSEM pool...");
    for (size_t i = 0; i < HSEM_COUNT; i++) {
        hsem_pool[i].index = i;
        hsem_pool[i].status = HSEM_STATUS_RESET;
    }
    LOG_INFO("Setup complete.");
    return OK;
}

hsem_t *hsem_new(uint8_t proc_id) {

    LOG_TRACE("Issuing new HSEM...");

    HAL_ENTER_CRITICAL_SECTION();

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
    LOG_TRACE("Unable to issue HSEM.");
    HAL_EXIT_CRITICIAL_SECTION();
    return NULL;

success:
    LOG_TRACE("Issued HSEM with pool index %zu", index);
    hsem_pool[index].status = HSEM_STATUS_FREE;
    last_taken_hsem_index = index;
    hsem_taken_count++;
    hsem_pool[index].proc_id = proc_id;

    hal_hsem_c1_enable_it(index);
    HAL_EXIT_CRITICIAL_SECTION();

    return &hsem_pool[index];
}

hal_err hsem_release(hsem_t *hsem) {

    if (!hsem) {
        LOG_TRACE("HSEM is NULL");
        return ERR_HSEM_RELEASE_ARGNULL;
    }

    HAL_ENTER_CRITICAL_SECTION();

    LOG_TRACE("Releasing HSEM with pool index %zu...", hsem->index);

    hal_err err = OK;
    if (hsem->status == HSEM_STATUS_LOCKED) {
        // Unlock if locked before returning to the pool
        err = hsem_unlock(hsem);
        if (err) {
            LOG_ERROR("Unable to unlock HSEM: Error %d", err);
            goto irq_en;
        }
        hsem->status = HSEM_STATUS_RESET;
    }
    last_freed_hsem_index = hsem->index;
    hsem_taken_count--;

    LOG_TRACE("HSEM with pool index %zu released.", hsem->index);

irq_en:
    HAL_EXIT_CRITICIAL_SECTION();
    return err;
}

hal_err hsem_lock(hsem_t *hsem) {

    if (!hsem) {
        LOG_TRACE("HSEM is NULL");
        return ERR_HSEM_LOCK_ARGNULL;
    }

    LOG_TRACE("Locking HSEM with pool index %zu...", hsem->index);

    HAL_ENTER_CRITICAL_SECTION();

    hal_err err = OK;

    if (hsem->status == HSEM_STATUS_LOCKED) {
        LOG_TRACE("HSEM already unlocked.");
        err = ERR_HSEM_LOCK_ALREADY_LOCKED;
        goto cleanup;
    }

    err = hal_hsem_lock(hsem->index, hsem->proc_id);
    if (err) {
        LOG_ERROR("Unable to lock HSEM: Error %d", err);
        goto cleanup;
    }

    hsem->status = HSEM_STATUS_LOCKED;

    LOG_TRACE("HSEM with pool index %zu locked.", hsem->index);

cleanup:
    HAL_EXIT_CRITICIAL_SECTION();
    return err;
}

bool hsem_locked(hsem_t *hsem) {
    return hsem->status == HSEM_STATUS_LOCKED;
}

uint8_t hsem_proc_id(hsem_t *hsem) {
    return hsem->proc_id;
}

hal_err hsem_unlock(hsem_t *hsem) {

    if (!hsem) {
        LOG_TRACE("HSEM is NULL.");
        return ERR_HSEM_UNLOCK_ARGNULL;
    }

    HAL_ENTER_CRITICAL_SECTION();

    LOG_TRACE("Unlocking HSEM with pool index %zu", hsem->index);

    hal_err err = OK;

    if (hsem->status == HSEM_STATUS_FREE) {
        LOG_TRACE("HSEM is already unlocked.");
        err = ERR_HSEM_UNLOCK_ALREADY_UNLOCKED;
        goto cleanup;
    }

    hal_hsem_unlock(hsem->index, hsem->proc_id);

    hsem->status = HSEM_STATUS_FREE;

    LOG_TRACE("HSEM with pool index %zu unlocked.", hsem->index);

cleanup:
    HAL_EXIT_CRITICIAL_SECTION();
    return err;
}

#endif // HSEM_MANAGER_ENABLED && HSEM_MANAGER_ENABLED == 1
