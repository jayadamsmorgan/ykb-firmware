#ifndef HAL_FLASH_H
#define HAL_FLASH_H

typedef enum {
    FLASH_LATENCY_ZERO_WAIT = 0U,
    FLASH_LATENCY_ONE_WAIT = 1U,
    FLASH_LATENCT_TWO_WAIT = 2U,
    FLASH_LATENCY_THREE_WAIT = 3U,
} flash_latency;

void flash_enable_prefetch();

void flash_select_latency(flash_latency latency);

#endif // HAL_FLASH_H
