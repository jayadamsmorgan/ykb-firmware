#include "rtc.h"

#include "logging.h"

rtc_handle_t rtc_handle;

hal_err setup_rtc() {

    LOG_INFO("Setting up...");
    rtc_handle.instance = RTC;
    rtc_handle.init.hour_format = RTC_HOUR_FORMAT_24;
    rtc_handle.init.async_pre_divider = 0x0F;
    rtc_handle.init.sync_pre_divider = 0x7FFF;
    rtc_handle.init.output = RTC_OUTPUT_DISABLE;
    rtc_handle.init.output_polarity = RTC_OUTPUT_POLARITY_HIGH;
    rtc_handle.init.output_type = RTC_OUTPUT_TYPE_OPENDRAIN;
    rtc_handle.init.output_remap = RTC_OUTPUT_REMAP_DISABLE;

    hal_err err = hal_rtc_init(&rtc_handle);
    if (err) {
        LOG_CRITICAL("Unable to init: Error %d", err);
        return err;
    }

    LOG_INFO("Setup complete.");

    return OK;
}
