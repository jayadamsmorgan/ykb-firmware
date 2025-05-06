#include "hal_power.h"

#include "stm32wbxx.h"

void hal_power_enable_vdd_usb() { SET_BIT(PWR->CR2, PWR_CR2_USV); }

void hal_power_disable_vdd_usb() { CLEAR_BIT(PWR->CR2, PWR_CR2_USV); }
