#include "adc.h"

#include "logging.h"

hal_err setup_adc() {

    hal_err err;

    LOG_INFO("Setting up...");

    LOG_TRACE("Starting init...");
    adc_init_t init;
    init.clock_mode = ADC_CLOCK_MODE_HCLK_DIV4;
    init.clock_prescaler = ADC_CLOCK_PRESCALER_DIV1;
    init.resolution = ADC_RESOLUTION_10BIT;
    init.data_align = ADC_DATA_ALIGN_RIGHT;
    init.eoc_flag = ADC_EOC_SINGLE_CONVERSION;
    init.lp_autowait = ADC_LP_AUTOWAIT_DISABLE;
    init.conversion_mode = ADC_CONVERSION_SINGLE;
    init.regular_channel_sequence_length =
        ADC_CHANNEL_SEQUENCE_LENGTH_1_CONVERSION;
    init.sequence_mode = ADC_SEQUENCE_DISCONTINUOUS;
    init.trigger_source = ADC_TRIGGER_SOFTWARE;
    init.trigger_edge = ADC_TRIGGER_EDGE_NONE;
    init.dma_mode = ADC_DMA_DISABLE;
    init.overrun_mode = ADC_OVERRUN_DATA_OVERWRITTEN;
    init.oversampling_mode = ADC_OVERSAMPLING_DISABLED;
    LOG_TRACE("Setting up callbacks...");
    err = adc_init(&init);
    if (err) {
        LOG_CRITICAL("Unable to init: Error %d", err);
        return err;
    }
    LOG_TRACE("Init OK.");

    LOG_TRACE("Starting calibration...");
    err = adc_start_calibration(ADC_CHANNEL_SINGLE_ENDED);
    if (err) {
        LOG_CRITICAL("Unable to calibrate: Error %d", err);
        return err;
    }
    LOG_TRACE("Calibration complete.");

    LOG_INFO("Setup complete.");

    return OK;
}
