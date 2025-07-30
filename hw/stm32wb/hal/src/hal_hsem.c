#include "hal_hsem.h"

#include "utils/utils.h"

static hsem_unlock_callback hsem_cb = NULL;

hal_err hal_hsem_lock(hsem_index index, uint8_t proc_id) {
    uint32_t tmp = (proc_id | HSEM_CPU1_COREID | HSEM_R_LOCK);
    WRITE_REG(HSEM->R[index], tmp);

    if (HSEM->R[index] != tmp) {
        return ERR_HSEM_LOCK_FAIL;
    }

    return OK;
}

hal_err hal_hsem_lock_fast(hsem_index index) {
    if (HSEM->RLR[index] != (HSEM_CPU1_COREID | HSEM_RLR_LOCK)) {
        return ERR_HSEM_LOCK_FAST_FAIL;
    }

    return OK;
}

void hal_hsem_set_irq_callback(hsem_unlock_callback cb) {
    hsem_cb = cb;
}

__weak void HSEM_IRQ_Handler(void) {
    uint32_t status_reg = HSEM->C1MISR;
    CLEAR_BIT(HSEM->C1IER, status_reg);
    WRITE_REG(HSEM->C1ICR, status_reg);
    if (hsem_cb) {
        hsem_cb(status_reg);
    }
}
