#ifndef HAL_HSEM_H
#define HAL_HSEM_H

#include "hal_err.h"

#include "stm32wbxx.h"

#include <stddef.h>

typedef enum {
    HSEM_INDEX_1 = 0U,
    HSEM_INDEX_2 = 1U,
    HSEM_INDEX_3 = 2U,
    HSEM_INDEX_4 = 3U,
    HSEM_INDEX_5 = 4U,
    HSEM_INDEX_6 = 5U,
    HSEM_INDEX_7 = 6U,
    HSEM_INDEX_8 = 7U,
    HSEM_INDEX_9 = 8U,
    HSEM_INDEX_10 = 9U,
    HSEM_INDEX_11 = 10U,
    HSEM_INDEX_12 = 11U,
    HSEM_INDEX_13 = 12U,
    HSEM_INDEX_14 = 13U,
    HSEM_INDEX_15 = 14U,
    HSEM_INDEX_16 = 15U,
    HSEM_INDEX_17 = 16U,
    HSEM_INDEX_18 = 17U,
    HSEM_INDEX_19 = 18U,
    HSEM_INDEX_20 = 19U,
    HSEM_INDEX_21 = 20U,
    HSEM_INDEX_22 = 21U,
    HSEM_INDEX_23 = 22U,
    HSEM_INDEX_24 = 23U,
    HSEM_INDEX_25 = 24U,
    HSEM_INDEX_26 = 25U,
    HSEM_INDEX_27 = 26U,
    HSEM_INDEX_28 = 27U,
    HSEM_INDEX_29 = 28U,
    HSEM_INDEX_30 = 29U,
    HSEM_INDEX_31 = 30U,
    HSEM_INDEX_32 = 31U,
} hsem_index;

typedef void (*hsem_unlock_callback)(uint32_t status_reg);

#define HSEM_COUNT 32

#define HSEM_C1_COREID 0x4U << HSEM_CR_COREID_Pos
#define HSEM_C2_COREID 0x8U << HSEM_CR_COREID_Pos

static inline void hal_hsem_c1_enable_it(hsem_index index) {
    SET_BIT(HSEM->C1IER, 1U << index);
}

static inline void hal_hsem_c2_enable_it(hsem_index index) {
    SET_BIT(HSEM->C2IER, 1U << index);
}

static inline void hal_hsem_c1_disable_it(hsem_index index) {
    CLEAR_BIT(HSEM->C1IER, 1U << index);
}

static inline void hal_hsem_c2_disable_it(hsem_index index) {
    CLEAR_BIT(HSEM->C2IER, 1U << index);
}

static inline void hal_hsem_c1_disable_it_all() {
    WRITE_REG(HSEM->C1IER, 0U);
}

static inline void hal_hsem_c2_disable_it_all() {
    WRITE_REG(HSEM->C2IER, 0U);
}

static inline void hal_hsem_unlock(hsem_index index, uint8_t proc_id) {
    HSEM->R[index] = (proc_id | HSEM_CPU1_COREID);
}

hal_err hal_hsem_lock(hsem_index index, uint8_t proc_id);
hal_err hal_hsem_lock_fast(hsem_index index);

void hal_hsem_set_irq_callback(hsem_unlock_callback cb);

#endif // HAL_HSEM_H
