#include "hal/flash.h"
#include "hal/bits.h"
#include "stm32wb55xx.h"

void flash_select_latency(flash_latency latency) {
    MODIFY_BITS(FLASH->ACR, FLASH_ACR_LATENCY_Pos, latency, BITMASK_2BIT);
}
