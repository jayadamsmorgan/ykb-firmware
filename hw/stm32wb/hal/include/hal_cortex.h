#ifndef HAL_CORTEX_H
#define HAL_CORTEX_H

#include "hal_err.h"
#include "stm32wbxx.h"

typedef enum {
    NVIC_PRIORITY_GROUP_0 = 0U,
    NVIC_PRIORITY_GROUP_1 = 1U,
    NVIC_PRIORITY_GROUP_2 = 2U,
    NVIC_PRIORITY_GROUP_3 = 3U,
    NVIC_PRIORITY_GROUP_4 = 4U,
} cortex_nvic_priority_group_t;

static inline void cortex_nvic_enable(IRQn_Type irqn) {
    NVIC_EnableIRQ(irqn);
}

static inline void cortex_nvic_disable(IRQn_Type irqn) {
    NVIC_DisableIRQ(irqn);
}

static inline void cortex_nvic_clear_pending(IRQn_Type irqn) {
    NVIC_ClearPendingIRQ(irqn);
}

static inline void cortex_nvic_set_pending(IRQn_Type irqn) {
    NVIC_SetPendingIRQ(irqn);
}

hal_err cortex_nvic_set_priority(IRQn_Type irqn, uint32_t preempt_priority,
                                 uint32_t sub_priority);

static inline void
cortex_nvic_set_priority_group(cortex_nvic_priority_group_t priority) {
    NVIC_SetPriorityGrouping(priority);
}

#endif // HAL_CORTEX_H
