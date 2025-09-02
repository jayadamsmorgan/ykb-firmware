#ifndef HAL_POWER_H
#define HAL_POWER_H

#include "hal_bits.h"

#include "stm32wbxx.h"

typedef enum {
    POWER_MODE_STOP0 = 0U,
    POWER_MODE_STOP1 = 1U,
    POWER_MODE_STOP2 = 2U,
    POWER_LP_MODE_STANDBY = 3U,
    POWER_LP_MODE_SHUTDOWN = 4U,
} hal_power_lp_mode;

static inline void hal_power_c1_set_lp_mode(hal_power_lp_mode mode) {
    MODIFY_BITS(PWR->CR1, PWR_CR1_LPMS_Pos, mode, BITMASK_3BIT);
}

static inline hal_power_lp_mode hal_power_c1_get_lp_mode() {
    return READ_BITS(PWR->CR1, PWR_CR1_LPMS_Pos, BITMASK_3BIT);
}

static inline void hal_power_c1_lp_enable() {
    SET_BIT(PWR->CR1, PWR_CR1_LPR);
}

static inline void hal_power_c1_lp_disable() {
    CLEAR_BIT(PWR->CR1, PWR_CR1_LPR);
}

static inline bool hal_power_c1_lp_enabled() {
    return READ_BIT(PWR->CR1, PWR_CR1_LPR);
}

static inline void hal_power_c2_set_lp_mode(hal_power_lp_mode mode) {
    MODIFY_BITS(PWR->C2CR1, PWR_C2CR1_LPMS_Pos, mode, BITMASK_3BIT);
}

static inline hal_power_lp_mode hal_power_c2_get_lp_mode() {
    return READ_BITS(PWR->C2CR1, PWR_C2CR1_LPMS_Pos, BITMASK_3BIT);
}

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
