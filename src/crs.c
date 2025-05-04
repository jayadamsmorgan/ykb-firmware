#include "crs.h"

#include "hal/crs.h"
#include "logging.h"

#define RELOAD_VALUE 48000000U / 1000U - 1U

hal_err setup_crs() {

    LOG_DEBUG("Setting up...");

    crs_enable();
    ASSERT(crs_enabled());

    crs_config_t config;
    config.source = CRS_SYNC_SOURCE_USB;
    config.polarity = CRS_SYNC_POLARITY_RISING;
    config.divider = CRS_SYNC_DIV_NONE;
    config.limit = CRS_FREQ_LIMIT_DEFAULT;
    config.reload = RELOAD_VALUE;
    crs_config(&config);

    crs_config_update(&config);
    ASSERT(config.source == CRS_SYNC_SOURCE_USB);
    ASSERT(config.polarity == CRS_SYNC_POLARITY_RISING);
    ASSERT(config.divider == CRS_SYNC_DIV_NONE);
    ASSERT(config.limit == CRS_FREQ_LIMIT_DEFAULT);
    ASSERT(config.reload == RELOAD_VALUE);

    hal_err err = crs_set_smooth_trimming(CRS_SMOOTH_TRIM_DEFAULT);
    if (err) {
        LOG_CRITICAL("Smooth trimming error %d.", err);
        return err;
    }
    LOG_TRACE("Smooth trimming enabled.");

    crs_auto_trimming_enable();
    ASSERT(crs_auto_trimming_enabled());

    crs_frequency_error_counter_enable();
    ASSERT(crs_frequency_error_counter_enabled());

    LOG_INFO("Setup complete.");

    return OK;
}
