#ifndef HAL_CORTEX_H
#define HAL_CORTEX_H

#include "hal/hal_err.h"
#include "stm32wbxx.h"

typedef enum {
    NVIC_PRIORITY_GROUP_0,
    NVIC_PRIORITY_GROUP_1,
    NVIC_PRIORITY_GROUP_2,
    NVIC_PRIORITY_GROUP_3,
    NVIC_PRIORITY_GROUP_4,
} cortex_nvic_priority_group_t;

void cortex_nvic_enable(IRQn_Type irqn);

hal_err cortex_nvic_set_priority(IRQn_Type irqn, uint32_t preempt_priority,
                                 uint32_t sub_priority);

void cortex_nvic_set_priority_group(cortex_nvic_priority_group_t priority);

#endif // HAL_CORTEX_H
