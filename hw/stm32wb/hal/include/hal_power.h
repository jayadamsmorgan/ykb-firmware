#ifndef HAL_POWER_H
#define HAL_POWER_H

#include "stm32wbxx.h"

static inline void hal_power_enable_vdd_usb() {
    SET_BIT(PWR->CR2, PWR_CR2_USV);
}

static inline void hal_power_disable_vdd_usb() {
    CLEAR_BIT(PWR->CR2, PWR_CR2_USV);
}

static inline void hal_power_enable_backup_access() {
    SET_BIT(PWR->CR1, PWR_CR1_DBP);
}

static inline void hal_power_disable_backup_access() {
    CLEAR_BIT(PWR->CR1, PWR_CR1_DBP);
}

#endif // HAL_POWER_H
