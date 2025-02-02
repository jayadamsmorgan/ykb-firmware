#ifndef HAL_CRS_H
#define HAL_CRS_H

#include "hal/hal_err.h"

#include <stdint.h>

#define CRS_FREQ_LIMIT_DEFAULT 0x00000022U
#define CRS_SMOOTH_TRIM_DEFAULT 0x00000020U

typedef enum {
    CRS_SYNC_POLARITY_RISING = 0U,
    CRS_SYNC_POLARITY_FALLING = 1U,
} crs_sync_polarity;

typedef enum {
    CRS_SYNC_SOURCE_GPIO = 0U,
    CRS_SYNC_SOURCE_LSE = 1U,
    CRS_SYNC_SOURCE_USB = 2U,
    CRS_SYNC_SOURCE_IN3 = 3U, // Not sure what is this yet...
} crs_sync_source;

typedef enum {
    CRS_SYNC_DIV_NONE = 0U,
    CRS_SYNC_DIV_2 = 1U,
    CRS_SYNC_DIV_4 = 2U,
    CRS_SYNC_DIV_8 = 3U,
    CRS_SYNC_DIV_16 = 4U,
    CRS_SYNC_DIV_32 = 5U,
    CRS_SYNC_DIV_64 = 6U,
    CRS_SYNC_DIV_128 = 7U,
} crs_sync_divider;

typedef uint8_t crs_frequency_limit_t;

typedef uint16_t crs_counter_reload_t;

typedef uint8_t crs_smooth_trim_t;

void crs_enable();

void crs_enable_auto_trimming();

void crs_enable_frequency_error_counter();

void crs_config(crs_sync_polarity polarity, crs_sync_source source,
                crs_sync_divider divider, crs_frequency_limit_t limit,
                crs_counter_reload_t reload);

// Trim should be a 6-bit value
hal_err crs_set_smooth_trimming(crs_smooth_trim_t trim);

#endif // HAL_CRS_H
