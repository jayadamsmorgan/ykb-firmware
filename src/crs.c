#include "crs.h"

hal_err crs_setup() {

    crs_enable();

    crs_config(CRS_SYNC_POLARITY_RISING, CRS_SYNC_SOURCE_USB, CRS_SYNC_DIV_NONE,
               CRS_FREQ_LIMIT_DEFAULT, 48000000 / 1000 - 1U);

    hal_err err = crs_set_smooth_trimming(CRS_SMOOTH_TRIM_DEFAULT);
    if (err) {
        return err;
    }

    crs_enable_auto_trimming();
    crs_enable_frequency_error_counter();

    return OK;
}
