#ifndef HAL_H
#define HAL_H

#include "hal/hal_err.h"
#include "stm32wbxx.h"

#ifndef __weak
#define __weak __attribute__((weak))
#endif // __weak

typedef FlagStatus flag_status;

static inline void system_init() { SystemInit(); }

hal_err hal_init();

#endif // HAL_H
