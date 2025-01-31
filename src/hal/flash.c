#include "hal/flash.h"

#include "hal/bits.h"
#include "stm32wbxx.h"

void flash_select_latency(flash_latency latency) {
    MODIFY_BITS(FLASH->ACR, FLASH_ACR_LATENCY_Pos, latency, BITMASK_2BIT);
}

void flash_enable_prefetch() { SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN); }
