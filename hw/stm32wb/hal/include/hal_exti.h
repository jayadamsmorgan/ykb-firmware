#ifndef HAL_EXTI_H
#define HAL_EXTI_H

#include "stm32wbxx.h"

typedef enum {
    EXTI_EVENT_0 = 0U,
    EXTI_EVENT_1 = 1U,
    EXTI_EVENT_2 = 2U,
    EXTI_EVENT_3 = 3U,
    EXTI_EVENT_4 = 4U,
    EXTI_EVENT_5 = 5U,
    EXTI_EVENT_6 = 6U,
    EXTI_EVENT_7 = 7U,
    EXTI_EVENT_8 = 8U,
    EXTI_EVENT_9 = 9U,
    EXTI_EVENT_10 = 10U,
    EXTI_EVENT_11 = 11U,
    EXTI_EVENT_12 = 12U,
    EXTI_EVENT_13 = 13U,
    EXTI_EVENT_14 = 14U,
    EXTI_EVENT_15 = 15U,
    EXTI_EVENT_16 = 16U,
    EXTI_EVENT_17 = 17U,
    EXTI_EVENT_18 = 18U,
    EXTI_EVENT_19 = 19U,
    EXTI_EVENT_20 = 20U,
    EXTI_EVENT_21 = 21U,
    EXTI_EVENT_22 = 22U,
    EXTI_EVENT_23 = 23U,
    EXTI_EVENT_24 = 24U,
    EXTI_EVENT_25 = 25U,
    EXTI_EVENT_26 = 26U,
    EXTI_EVENT_27 = 27U,
    EXTI_EVENT_28 = 28U,
    EXTI_EVENT_29 = 29U,
    EXTI_EVENT_30 = 30U,
    EXTI_EVENT_31 = 31U,
    EXTI_EVENT_32 = 32U,
    EXTI_EVENT_33 = 33U,
    EXTI_EVENT_34 = 34U,
    EXTI_EVENT_35 = 35U,
    EXTI_EVENT_36 = 36U,
    EXTI_EVENT_37 = 37U,
    EXTI_EVENT_38 = 38U,
    EXTI_EVENT_39 = 39U,
    EXTI_EVENT_40 = 40U,
    EXTI_EVENT_41 = 41U,
    EXTI_EVENT_42 = 42U,
    EXTI_EVENT_43 = 43U,
    EXTI_EVENT_44 = 44U,
    EXTI_EVENT_45 = 45U,
    EXTI_EVENT_46 = 46U,
    EXTI_EVENT_47 = 47U,
    EXTI_EVENT_48 = 48U,
} exti_event;

static inline void hal_exti_enable_it(exti_event event) {
    if (event > EXTI_EVENT_31) {
        SET_BIT(EXTI->IMR2, 1U << (event - 32));
    } else {
        SET_BIT(EXTI->IMR1, 1U << event);
    }
}

static inline void hal_exti_disable_it(exti_event event) {
    if (event > EXTI_EVENT_31) {
        CLEAR_BIT(EXTI->IMR2, 1U << (event - 32));
    } else {
        CLEAR_BIT(EXTI->IMR1, 1U << event);
    }
}

static inline bool hal_exti_enabled_it(exti_event event) {
    if (event > EXTI_EVENT_31) {
        return READ_BIT(EXTI->IMR2, 1U << (event - 32));
    } else {
        return READ_BIT(EXTI->IMR1, 1U << event);
    }
}

static inline void hal_exti_enable_rising(exti_event event) {
    if (event > EXTI_EVENT_31) {
        SET_BIT(EXTI->RTSR2, 1U << (event - 32));
    } else {
        SET_BIT(EXTI->RTSR1, 1U << event);
    }
}

static inline void hal_exti_disable_rising(exti_event event) {
    if (event > EXTI_EVENT_31) {
        CLEAR_BIT(EXTI->RTSR2, 1U << (event - 32));
    } else {
        CLEAR_BIT(EXTI->RTSR1, 1U << event);
    }
}

static inline bool hal_exti_is_rising(exti_event event) {
    if (event > EXTI_EVENT_31) {
        return READ_BIT(EXTI->RTSR2, 1U << (event - 32));
    } else {
        return READ_BIT(EXTI->RTSR1, 1U << event);
    }
}

static inline void hal_exti_clear_event(exti_event event) {
    if (event > EXTI_EVENT_31) {
        WRITE_REG(EXTI->PR2, 1U << (event - 32));
    } else {
        WRITE_REG(EXTI->PR1, 1U << event);
    }
}

#endif // HAL_EXTI_H
