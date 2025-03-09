#include "adc.h"

#include "error_handler.h"
#include "hal/adc.h"
#include "hal/systick.h"
#include "logging.h"

#define ADC_READ_BLOCKING_TIMEOUT 500

static volatile bool adc_conversion_complete = false;

static volatile uint32_t adc_read_value;

void adc_conversion_callback(adc_conversion_trigger trigger) {

    LOG_TRACE("ADC: Conversion callback triggered.");

    // TODO: Not working as expected:
    if (trigger == ADC_CONVERSION_TRIGGER_EOS) {
        return;
    }

    adc_read_value = adc_get_value();
    adc_conversion_complete = true;

    LOG_TRACE("ADC: Conversion callback complete.");
}

hal_err setup_adc() {

    LOG_INFO("ADC: Setting up...");

    adc_init_t init;

    init.dma_mode = ADC_DMA_DISABLE;
    init.eoc_flag = ADC_EOC_SINGLE_CONVERSION;
    init.clock_mode = ADC_CLOCK_MODE_HCLK_DIV4;
    init.data_align = ADC_DATA_ALIGN_RIGHT;
    init.resolution = ADC_RESOLUTION_10BIT;
    init.lp_autowait = ADC_LP_AUTOWAIT_DISABLE;
    init.overrun_mode = ADC_OVERRUN_DATA_OVERWRITTEN;
    init.trigger_edge = ADC_TRIGGER_EDGE_NONE;
    init.trigger_source = ADC_TRIGGER_SOFTWARE;
    init.sequence_mode = ADC_SEQUENCE_COMPLETE;
    init.conversion_mode = ADC_CONVERSION_SINGLE;
    init.sequence_amount = ADC_SEQUENCE_1_CHANNEL;
    init.oversampling_mode = ADC_OVERSAMPLING_DISABLED;
    init.regular_channel_sequence_length =
        ADC_CHANNEL_SEQUENCE_LENGTH_1_CONVERSION;

    adc_callbacks_t callbacks;
    callbacks.regular_conversion_complete = adc_conversion_callback;
    adc_set_callbacks(callbacks);
    LOG_TRACE("ADC: Callbacks are set.");

    hal_err err;

    LOG_TRACE("ADC: Starting init...");
    err = adc_init(&init);
    if (err) {
        LOG_CRITICAL("ADC: Unable to init: Error %d", err);
        return err;
    }
    LOG_TRACE("ADC: Init OK.");

    LOG_TRACE("ADC: Starting calibration...");
    err = adc_start_calibration(ADC_CHANNEL_SINGLE_ENDED);
    if (err) {
        LOG_CRITICAL("ADC: Unable to calibrate: Error %d", err);
        return err;
    }
    LOG_TRACE("ADC: Calibration complete.");

    LOG_INFO("ADC: Setup complete.");

    return OK;
}

hal_err adc_read_blocking(uint32_t *value) {

    hal_err err;

    LOG_DEBUG("ADC: Triggering conversion...");
    err = adc_start_it();
    if (err) {
        LOG_ERROR("ADC: Error triggering conversion: Error %d", err);
        return err;
    }
    LOG_TRACE("ADC: Conversion triggered. Waiting for it...");

    uint32_t tick_start = systick_get_tick();
    while (!adc_conversion_complete) {
        if (systick_get_tick() - tick_start >= ADC_READ_BLOCKING_TIMEOUT) {
            if (!adc_conversion_complete) { // Preemption check
                LOG_ERROR("ADC: Conversion timeout.");
                return ERR_ADC_RB_TIMEOUT;
            }
        }
    }

    if (value) {
        *value = adc_read_value;
    }

    LOG_DEBUG("ADC: Conversion complete.");

    return OK;
}

hal_err setup_tempsensor() {

    LOG_INFO("TEMPSENS: Setting up...");

    adc_channel_config_t channel_config;

    channel_config.channel = ADC_CHANNEL_TEMPSENSOR;
    channel_config.mode = ADC_CHANNEL_SINGLE_ENDED;
    channel_config.rank = ADC_CHANNEL_RANK_1;
    channel_config.sampling_time = ADC_SMP_92_5_CYCLES;
    channel_config.offset_type = ADC_CHANNEL_OFFSET_NONE;
    channel_config.offset = 0;

    hal_err err = adc_config_channel(&channel_config);
    if (err) {
        LOG_CRITICAL("TEMPSENS: Unable to config ADC channel: %d", err);
        return err;
    }

    LOG_INFO("TEMPSENS: Setup complete.");
    return OK;
}
