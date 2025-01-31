#include "hal/cortex.h"
#include "hal/hal_err.h"

void cortex_nvic_enable(IRQn_Type irqn) { NVIC_EnableIRQ(irqn); }

hal_err cortex_nvic_set_priority(IRQn_Type irqn, uint32_t preempt_priority,
                                 uint32_t sub_priority) {

    if (sub_priority > 0x10U) {
        return ERR_NVIC_SET_PRIO_SP_INV;
    }
    if (preempt_priority > 0x10U) {
        return ERR_NVIC_SET_PRIO_PP_INV;
    }

    uint32_t priority_group = NVIC_GetPriorityGrouping();
    NVIC_SetPriority(irqn, NVIC_EncodePriority(priority_group, preempt_priority,
                                               sub_priority));
    return OK;
}

void cortex_nvic_set_priority_group(cortex_nvic_priority_group_t priority) {
    NVIC_SetPriorityGrouping(priority);
}
