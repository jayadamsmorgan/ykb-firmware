#include "crs.h"

#include "logging.h"

hal_err setup_crs() {

    LOG_DEBUG("CRS: Setting up...");

    crs_enable();
    LOG_TRACE_EXPECTED("CRS: Enabled:", crs_enabled(), true);

    crs_config_t config;
    config.source = CRS_SYNC_SOURCE_USB;
    config.polarity = CRS_SYNC_POLARITY_RISING;
    config.divider = CRS_SYNC_DIV_NONE;
    config.limit = CRS_FREQ_LIMIT_DEFAULT;
    config.reload = 48000000U / 1000U - 1U;
    crs_config(&config);
#ifdef DEBUG
    crs_config_update(&config);
#endif // DEBUG
    LOG_TRACE_EXPECTED("CRS: Source:", config.source, CRS_SYNC_SOURCE_USB);
    LOG_TRACE_EXPECTED("CRS: Polarity:", config.polarity,
                       CRS_SYNC_POLARITY_RISING);
    LOG_TRACE_EXPECTED("CRS: Divider:", config.divider, CRS_SYNC_DIV_NONE);
    LOG_TRACE_EXPECTED("CRS: Limit:", config.limit, CRS_FREQ_LIMIT_DEFAULT);
    LOG_TRACE_EXPECTED("CRS: Reload:", config.reload, 48000000U / 1000U - 1U);

    hal_err err = crs_set_smooth_trimming(CRS_SMOOTH_TRIM_DEFAULT);
    if (err) {
        LOG_CRITICAL("CRS: Smooth trimming error %d.", err);
        return err;
    }
    LOG_TRACE("CRS: Smooth trimming enabled.");

    crs_auto_trimming_enable();
    LOG_TRACE_EXPECTED("CRS: AutoTrimming:", crs_auto_trimming_enabled(), true);

    crs_frequency_error_counter_enable();
    LOG_TRACE_EXPECTED("CRS: FEC:", crs_frequency_error_counter_enabled(),
                       true);

    LOG_INFO("CRS: Setup complete.");

    return OK;
}
