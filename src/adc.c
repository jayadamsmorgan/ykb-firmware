#include "adc.h"

#include "error_handler.h"
#include "hal/adc.h"
#include "hal/cortex.h"
#include "hal/gpio.h"
#include "hal/systick.h"
#include "logging.h"
#include "utils/utils.h"
#include <stdint.h>

#define ADC_CHANNELS_USED 3

#ifndef ADC_DEFAULT_SAMPLING
#define ADC_DEFAULT_SAMPLING ADC_SMP_92_5_CYCLES;
#endif // ADC_DEFAULT_SAMPLING

static uint8_t adc_ranks_index = 0;
static const adc_channel_rank_t adc_ranks[ADC_CHANNELS_USED] = {
    ADC_CHANNEL_RANK_1, ADC_CHANNEL_RANK_2, ADC_CHANNEL_RANK_3};

hal_err adc_register_channel(const gpio_pin_t *pin, uint8_t *rank_number) {
    if (adc_ranks_index >= ADC_CHANNELS_USED) {
        return -1;
    }

    if (!pin || pin->adc_chan == ADC_CHANNEL_NONE) {
        return -2;
    }

    adc_channel_config_t config;
    config.channel = pin->adc_chan;
    config.rank = adc_ranks[adc_ranks_index];
    config.mode = ADC_CHANNEL_SINGLE_ENDED;
    config.sampling_time = ADC_DEFAULT_SAMPLING;
    config.offset_type = ADC_CHANNEL_OFFSET_NONE;
    config.offset = 0;

    hal_err err = adc_config_channel(&config);
    if (err) {
        return err;
    }

    if (rank_number) {
        *rank_number = adc_ranks_index;
    }

    adc_ranks_index++;

    return OK;
}

hal_err setup_adc() {

    hal_err err;

    LOG_INFO("ADC: Setting up...");

    LOG_TRACE("ADC: Starting init...");
    adc_init_t init;
    init.clock_mode = ADC_CLOCK_MODE_HCLK_DIV4;
    init.clock_prescaler = ADC_CLOCK_PRESCALER_DIV1;
    init.resolution = ADC_RESOLUTION_10BIT;
    init.data_align = ADC_DATA_ALIGN_RIGHT;
    init.eoc_flag = ADC_EOC_SINGLE_CONVERSION;
    init.lp_autowait = ADC_LP_AUTOWAIT_DISABLE;
    init.conversion_mode = ADC_CONVERSION_SINGLE;
    init.regular_channel_sequence_length = ADC_CHANNELS_USED - 1;
    init.sequence_mode = ADC_SEQUENCE_DISCONTINUOUS;
    init.trigger_source = ADC_TRIGGER_SOFTWARE;
    init.trigger_edge = ADC_TRIGGER_EDGE_NONE;
    init.dma_mode = ADC_DMA_DISABLE;
    init.overrun_mode = ADC_OVERRUN_DATA_OVERWRITTEN;
    init.oversampling_mode = ADC_OVERSAMPLING_DISABLED;
    LOG_TRACE("ADC: Setting up callbacks...");
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
