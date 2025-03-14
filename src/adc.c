#include "adc.h"

#include "error_handler.h"
#include "hal/adc.h"
#include "hal/cortex.h"
#include "hal/dma.h"
#include "hal/systick.h"
#include "logging.h"
#include "utils/utils.h"
#include <stdint.h>

#define ADC_CHANNELS_USED 4 // 3 MUXes + 1 tempsensor
#define DMA_BUF_LEN ADC_CHANNELS_USED
#define CALLBACKS_BUF_LEN ADC_CHANNELS_USED

static __IO uint16_t dma_buffer[DMA_BUF_LEN];

static __IO bool *data_ready_flags[CALLBACKS_BUF_LEN];
static __IO uint8_t data_ready_flag_count = 0U;

hal_err adc_register_data_ready_flag(volatile bool *flag) {
    if (data_ready_flag_count >= CALLBACKS_BUF_LEN) {
        return -1501;
    }

    data_ready_flags[data_ready_flag_count] = flag;

    data_ready_flag_count++;

    return OK;
}

static void conversion_complete(adc_conversion_trigger trigger) {
    UNUSED(trigger);
    LOG_TRACE("ADC: Conversion complete: %d, %d, %d, %d", dma_buffer[0],
              dma_buffer[1], dma_buffer[2], dma_buffer[3]);

    for (uint8_t i = 0; i < data_ready_flag_count; i++) {
        if (data_ready_flags[i]) {
            *data_ready_flags[i] = true;
        }
    }
}

hal_err setup_adc() {

    hal_err err;

    LOG_INFO("ADC: Setting up...");

    LOG_TRACE("ADC: Setting up DMA...");
    dma_mux_enable();
    dma1_enable();
    err = cortex_nvic_set_priority(DMA1_Channel1_IRQn, 0, 0);
    if (err) {
        LOG_ERROR("ADC: Unable to set up DMA: Error %d", err);
        return err;
    }
    cortex_nvic_enable(DMA1_Channel1_IRQn);
    LOG_TRACE("ADC: DMA setup complete.");

    dma_handle_t dma_handle;
    dma_handle.instance = DMA1_Channel1;
    dma_handle.init.request = DMA_REQUEST_ADC1;
    dma_handle.init.direction = DMA_TRANSFER_PERIPH_TO_MEMORY;
    dma_handle.init.peripheral_align = DMA_PERIPHERAL_DATA_ALIGN_HALFWORD;
    dma_handle.init.memory_align = DMA_MEMORY_DATA_ALIGN_HALFWORD;
    dma_handle.init.mode = DMA_MODE_CIRCULAR;
    dma_handle.init.priority = DMA_PRIORITY_HIGH;

    LOG_TRACE("ADC: DMA init...");
    err = dma_init(&dma_handle);
    if (err) {
        LOG_ERROR("ADC: Unable to init DMA: Error: %d", err);
        return err;
    }
    adc_link_dma(&dma_handle);
    LOG_TRACE("ADC: DMA init OK.");

    LOG_TRACE("ADC: Starting init...");
    adc_init_t init;
    init.clock_prescaler = ADC_CLOCK_PRESCALER_DIV4;
    init.resolution = ADC_RESOLUTION_10BIT;
    init.data_align = ADC_DATA_ALIGN_RIGHT;
    init.eoc_flag = ADC_EOC_SINGLE_CONVERSION;
    init.lp_autowait = ADC_LP_AUTOWAIT_DISABLE;
    init.conversion_mode = ADC_CONVERSION_SINGLE;
    init.regular_channel_sequence_length = 3;
    init.sequence_mode = ADC_SEQUENCE_DISCONTINUOUS;
    init.trigger_source = ADC_TRIGGER_SOFTWARE;
    init.trigger_edge = ADC_TRIGGER_EDGE_NONE;
    init.dma_mode = ADC_DMA_ONE_SHOT;
    init.overrun_mode = ADC_OVERRUN_DATA_OVERWRITTEN;
    init.oversampling_mode = ADC_OVERSAMPLING_DISABLED;
    err = adc_init(&init);
    if (err) {
        LOG_CRITICAL("ADC: Unable to init: Error %d", err);
        return err;
    }
    LOG_TRACE("ADC: Init OK.");

    LOG_TRACE("ADC: Setting up callbacks...");
    adc_callbacks_t adc_callbacks;
    adc_callbacks.conversion_complete = conversion_complete;
    adc_set_callbacks(adc_callbacks);

    LOG_TRACE("ADC: Starting calibration...");
    err = adc_start_calibration(ADC_CHANNEL_SINGLE_ENDED);
    if (err) {
        LOG_CRITICAL("ADC: Unable to calibrate: Error %d", err);
        return err;
    }
    LOG_TRACE("ADC: Calibration complete.");

    LOG_TRACE("ADC: Starting DMA...");
    err = adc_start_dma((uint32_t *)dma_buffer, sizeof(dma_buffer));
    if (err) {
        LOG_CRITICAL("ADC: Unable to start with DMA: Error: %d", err);
    }
    LOG_TRACE("ADC: DMA started successfully.");

    LOG_INFO("ADC: Setup complete.");

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
