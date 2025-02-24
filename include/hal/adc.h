#ifndef HAL_ADC_H
#define HAL_ADC_H

#include "hal/gpio.h"
#include "hal/hal_err.h"
#include "stm32wb55xx.h"
#include "stm32wbxx.h"
#include <stdint.h>

#define HAL_ADC_STATE_RESET (0x00000000UL)
#define HAL_ADC_STATE_READY (0x00000001UL)
#define HAL_ADC_STATE_BUSY_INTERNAL (0x00000002UL)
#define HAL_ADC_STATE_TIMEOUT (0x00000004UL)

#define HAL_ADC_STATE_ERROR_INTERNAL (0x00000010UL)
#define HAL_ADC_STATE_ERROR_CONFIG (0x00000020UL)
#define HAL_ADC_STATE_ERROR_DMA (0x00000040UL)

#define HAL_ADC_STATE_REG_BUSY (0x00000100UL)
#define HAL_ADC_STATE_REG_EOC (0x00000200UL)
#define HAL_ADC_STATE_REG_OVR (0x00000400UL)
#define HAL_ADC_STATE_REG_EOSMP (0x00000800UL)

#define HAL_ADC_STATE_INJ_BUSY (0x00001000UL)
#define HAL_ADC_STATE_INJ_EOC (0x00002000UL)
#define HAL_ADC_STATE_INJ_JQOVF (0x00004000UL)

#define HAL_ADC_STATE_AWD1 (0x00010000UL)
#define HAL_ADC_STATE_AWD2 (0x00020000UL)
#define HAL_ADC_STATE_AWD3 (0x00040000UL)

#define HAL_ADC_ERROR_NONE (0x00U)
#define HAL_ADC_ERROR_INTERNAL (0x01U)
#define HAL_ADC_ERROR_OVR (0x02U)
#define HAL_ADC_ERROR_DMA (0x04U)
#define HAL_ADC_ERROR_JQOVF (0x08U)

typedef enum {
    ADC_CLOCK_MODE_ASYNC = 0U,
    ADC_CLOCK_MODE_HCLK_DIV1 = 1U,
    ADC_CLOCK_MODE_HCLK_DIV2 = 2U,
    ADC_CLOCK_MODE_HCLK_DIV4 = 3U,
} adc_clock_mode;

typedef enum {
    ADC_CLOCK_PRESCALER_DIV1 = 0U,
    ADC_CLOCK_PRESCALER_DIV2 = 1U,
    ADC_CLOCK_PRESCALER_DIV4 = 2U,
    ADC_CLOCK_PRESCALER_DIV6 = 3U,
    ADC_CLOCK_PRESCALER_DIV8 = 4U,
    ADC_CLOCK_PRESCALER_DIV10 = 5U,
    ADC_CLOCK_PRESCALER_DIV12 = 6U,
    ADC_CLOCK_PRESCALER_DIV16 = 7U,
    ADC_CLOCK_PRESCALER_DIV32 = 8U,
    ADC_CLOCK_PRESCALER_DIV64 = 9U,
    ADC_CLOCK_PRESCALER_DIV128 = 10U,
    ADC_CLOCK_PRESCALER_DIV256 = 11U,
} adc_clock_prescaler;

typedef enum {
    ADC_SMP_2_5_CYCLES = 0U,
    ADC_SMP_6_5_CYCLES = 1U,
    ADC_SMP_12_5_CYCLES = 2U,
    ADC_SMP_24_5_CYCLES = 3U,
    ADC_SMP_47_5_CYCLES = 4U,
    ADC_SMP_92_5_CYCLES = 5U,
    ADC_SMP_247_5_CYCLES = 6U,
    ADC_SMP_640_5_CYCLES = 7U,
} adc_sampling_time;

typedef enum {
    ADC_RESOLUTION_12BIT = 0U,
    ADC_RESOLUTION_10BIT = 1U,
    ADC_RESOLUTION_8BIT = 2U,
    ADC_RESOLUTION_6BIT = 3U,
} adc_resolution;

typedef enum {
    ADC_DATA_ALIGN_RIGHT = 0U,
    ADC_DATA_ALIGN_LEFT = 1U,
} adc_data_align;

typedef enum {
    ADC_EOC_SINGLE_CONVERSION = ADC_ISR_EOC,
    ADC_EOC_SEQ_CONVERSION = ADC_ISR_EOS,
} adc_eoc_flag;

typedef enum {
    ADC_CONV_SINGLE = 0U,
    ADC_CONV_SEQUENCE = 1U,
} adc_conversion_mode;

typedef enum {
    ADC_SEQUENCE_COMPLETE = 0U,
    ADC_SEQUENCE_DISCONTINUOUS = 1U,
} adc_sequence_mode;

typedef enum {
    ADC_LP_AUTOWAIT_DISABLE = 0U,
    ADC_LP_AUTOWAIT_ENABLE = 1U,
} adc_lp_autowait;

typedef enum {
    ADC_TRIGGER_SOFTWARE = -1,
    ADC_TRIGGER_TIM1_TRGO = ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_0,
    ADC_TRIGGER_TIM1_TRGO2 = ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_1,
    ADC_TRIGGER_TIM1_CH1 = 0U,
    ADC_TRIGGER_TIM1_CH2 = ADC_CFGR_EXTSEL_0,
    ADC_TRIGGER_TIM1_CH3 = ADC_CFGR_EXTSEL_1,
    ADC_TRIGGER_TIM2_TRGO =
        ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_0,
    ADC_TRIGGER_TIM2_CH2 = ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_0,
    ADC_TRIGGER_EXTI11 = ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_1,
} adc_trigger_source;

typedef enum {
    ADC_TRIGGER_EDGE_NONE = 0U,
    ADC_TRIGGER_EDGE_RISING = 1U,
    ADC_TRIGGER_EDGE_FALLING = 2U,
    ADC_TRIGGER_EDGE_RISING_FALLING = 3U,
} adc_trigger_edge;

typedef enum {
    ADC_DMA_DISABLE,
    ADC_DMA_ONE_SHOT,
    ADC_DMA_CIRCULAR,
} adc_dma_mode;

typedef enum {
    ADC_OVERRUN_DATA_PRESERVED,
    ADC_OVERRUN_DATA_OVERWRITTEN,
} adc_overrun_mode;

typedef enum {
    ADC_OVERSAMPLING_DISABLED = 0U,
    ADC_OVERSAMPLING_REGULAR = 1U,
    ADC_OVERSAMPLING_INJECTED = 2U,
    ADC_OVERSAMPLING_REGULAR_INJECTED = 3U,
} adc_oversampling_mode;

typedef enum {
    ADC_OVERSAMPLING_RATIO_2X = 0U,
    ADC_OVERSAMPLING_RATIO_4X = 1U,
    ADC_OVERSAMPLING_RATIO_8X = 2U,
    ADC_OVERSAMPLING_RATIO_16X = 3U,
    ADC_OVERSAMPLING_RATIO_32X = 4U,
    ADC_OVERSAMPLING_RATIO_64X = 5U,
    ADC_OVERSAMPLING_RATIO_128X = 6U,
    ADC_OVERSAMPLING_RATIO_256X = 7U,
} adc_oversampling_ratio;

typedef enum {
    ADC_OVERSAMPLING_NO_SHIFT = 0U,
    ADC_OVERSAMPLING_SHIFT_1BIT = 1U,
    ADC_OVERSAMPLING_SHIFT_2BITS = 2U,
    ADC_OVERSAMPLING_SHIFT_3BITS = 3U,
    ADC_OVERSAMPLING_SHIFT_4BITS = 4U,
    ADC_OVERSAMPLING_SHIFT_5BITS = 5U,
    ADC_OVERSAMPLING_SHIFT_6BITS = 6U,
    ADC_OVERSAMPLING_SHIFT_7BITS = 7U,
    ADC_OVERSAMPLING_SHIFT_8BITS = 8U,
} adc_oversampling_shift;

typedef enum {
    ADC_OVERSAMPLING_TRIGGERED_DISABLED = 0U,
    ADC_OVERSAMPLING_TRIGGERED_ENABLED = 1U,
} adc_oversampling_triggered_mode;

typedef enum {
    ADC_OVERSAMPLING_REGULAR_CONTINUED = 0U,
    ADC_OVERSAMPLING_REGULAR_RESUMED = 1U,
} adc_oversampling_regular_mode;

typedef enum {
    ADC_1_CHANNEL = 0U,
    ADC_2_CHANNELS = 1U,
    ADC_3_CHANNELS = 2U,
    ADC_4_CHANNELS = 3U,
    ADC_5_CHANNELS = 4U,
    ADC_6_CHANNELS = 5U,
    ADC_7_CHANNELS = 6U,
    ADC_8_CHANNELS = 7U,
} adc_sequence_channel_amount;

typedef enum {
    ADC_CHANNEL_SEQUENCE_LENGTH_1_CONVERSION = 0U,
    ADC_CHANNEL_SEQUENCE_LENGTH_2_CONVERSIONS = 1U,
    ADC_CHANNEL_SEQUENCE_LENGTH_3_CONVERSIONS = 2U,
    ADC_CHANNEL_SEQUENCE_LENGTH_4_CONVERSIONS = 3U,
    ADC_CHANNEL_SEQUENCE_LENGTH_5_CONVERSIONS = 4U,
    ADC_CHANNEL_SEQUENCE_LENGTH_6_CONVERSIONS = 5U,
    ADC_CHANNEL_SEQUENCE_LENGTH_7_CONVERSIONS = 6U,
    ADC_CHANNEL_SEQUENCE_LENGTH_8_CONVERSIONS = 7U,
    ADC_CHANNEL_SEQUENCE_LENGTH_9_CONVERSIONS = 8U,
    ADC_CHANNEL_SEQUENCE_LENGTH_10_CONVERSIONS = 9U,
    ADC_CHANNEL_SEQUENCE_LENGTH_11_CONVERSIONS = 10U,
    ADC_CHANNEL_SEQUENCE_LENGTH_12_CONVERSIONS = 11U,
    ADC_CHANNEL_SEQUENCE_LENGTH_13_CONVERSIONS = 12U,
    ADC_CHANNEL_SEQUENCE_LENGTH_14_CONVERSIONS = 13U,
    ADC_CHANNEL_SEQUENCE_LENGTH_15_CONVERSIONS = 14U,
    ADC_CHANNEL_SEQUENCE_LENGTH_16_CONVERSIONS = 15U,
} adc_regular_channel_sequence_length;

typedef struct {

    adc_clock_prescaler clock_prescaler;

    adc_clock_mode clock_mode;

    adc_regular_channel_sequence_length regular_channel_sequence_length;

    adc_conversion_mode conversion_mode;

    adc_data_align data_align;

    adc_dma_mode dma_mode;

    adc_eoc_flag eoc_flag;

    adc_lp_autowait lp_autowait;

    adc_overrun_mode overrun_mode;

    adc_oversampling_mode oversampling_mode;

    adc_oversampling_ratio oversampling_ratio;

    adc_oversampling_shift oversampling_shift;

    adc_oversampling_regular_mode oversampling_regular_mode;

    adc_oversampling_triggered_mode oversampling_triggered_mode;

    adc_resolution resolution;

    adc_sequence_mode sequence_mode;

    adc_sequence_channel_amount sequence_amount;

    adc_trigger_edge trigger_edge;

    adc_trigger_source trigger_source;

} adc_init_t;

typedef struct {

    adc_init_t init;

    // TODO: DMA handle here

    __IO uint32_t state;
    __IO uint32_t error;

    bool lock;

} adc_handle_t;

typedef enum {
    ADC_CHANNEL_0 = 0U,
    ADC_CHANNEL_1 = 1U,
    ADC_CHANNEL_2 = 2U,
    ADC_CHANNEL_3 = 3U,
    ADC_CHANNEL_4 = 4U,
    ADC_CHANNEL_5 = 5U,
    ADC_CHANNEL_6 = 6U,
    ADC_CHANNEL_7 = 7U,
    ADC_CHANNEL_8 = 8U,
    ADC_CHANNEL_9 = 9U,
    ADC_CHANNEL_10 = 10U,
    ADC_CHANNEL_11 = 11U,
    ADC_CHANNEL_12 = 12U,
    ADC_CHANNEL_13 = 13U,
    ADC_CHANNEL_14 = 14U,
    ADC_CHANNEL_15 = 15U,
    ADC_CHANNEL_16 = 16U,
    ADC_CHANNEL_17 = 17U,
    ADC_CHANNEL_18 = 18U,
    ADC_CHANNEL_VREFINT = 19U,
    ADC_CHANNEL_TEMPSENSOR = 20U,
    ADC_CHANNEL_VBAT = 21U,
} adc_channel;

typedef struct {
    volatile uint32_t *reg;
    uint8_t offset;
} adc_channel_rank_t;

#define __ADC_CHANNEL_RANK(REG, OFFSET)                                        \
    (adc_channel_rank_t) { .reg = REG, .offset = OFFSET }

#define ADC_CHANNEL_RANK_1 __ADC_CHANNEL_RANK(&ADC1->SQR1, 6)
#define ADC_CHANNEL_RANK_2 __ADC_CHANNEL_RANK(&ADC1->SQR1, 12)
#define ADC_CHANNEL_RANK_3 __ADC_CHANNEL_RANK(&ADC1->SQR1, 18)
#define ADC_CHANNEL_RANK_4 __ADC_CHANNEL_RANK(&ADC1->SQR1, 24)
#define ADC_CHANNEL_RANK_5 __ADC_CHANNEL_RANK(&ADC1->SQR2, 0)
#define ADC_CHANNEL_RANK_6 __ADC_CHANNEL_RANK(&ADC1->SQR2, 6)
#define ADC_CHANNEL_RANK_7 __ADC_CHANNEL_RANK(&ADC1->SQR2, 12)
#define ADC_CHANNEL_RANK_8 __ADC_CHANNEL_RANK(&ADC1->SQR2, 18)
#define ADC_CHANNEL_RANK_9 __ADC_CHANNEL_RANK(&ADC1->SQR2, 24)
#define ADC_CHANNEL_RANK_10 __ADC_CHANNEL_RANK(&ADC1->SQR3, 0)
#define ADC_CHANNEL_RANK_11 __ADC_CHANNEL_RANK(&ADC1->SQR3, 6)
#define ADC_CHANNEL_RANK_12 __ADC_CHANNEL_RANK(&ADC1->SQR3, 12)
#define ADC_CHANNEL_RANK_13 __ADC_CHANNEL_RANK(&ADC1->SQR3, 18)
#define ADC_CHANNEL_RANK_14 __ADC_CHANNEL_RANK(&ADC1->SQR3, 24)
#define ADC_CHANNEL_RANK_15 __ADC_CHANNEL_RANK(&ADC1->SQR4, 0)
#define ADC_CHANNEL_RANK_16 __ADC_CHANNEL_RANK(&ADC1->SQR4, 6)

typedef enum {
    ADC_CHANNEL_SINGLE_ENDED = 0U,
    ADC_CHANNEL_DIFFERENTIAL = 1U,
} adc_channel_mode;

typedef enum {
    ADC_CHANNEL_OFFSET_NONE,
    ADC_CHANNEL_OFFSET_1,
    ADC_CHANNEL_OFFSET_2,
    ADC_CHANNEL_OFFSET_3,
    ADC_CHANNEL_OFFSET_4,
} adc_channel_offset_type;

typedef struct {

    adc_channel channel;

    adc_channel_rank_t rank;

    adc_sampling_time sampling_time;

    adc_channel_mode mode;

    adc_channel_offset_type offset_type;

    uint16_t offset;

} adc_channel_config_t;

static inline bool adc_conversion_ongoing_regular() {
    return READ_BIT(ADC1->CR, ADC_CR_ADSTART) == ADC_CR_ADSTART;
}

static inline bool adc_conversion_ongoing_injected() {
    return READ_BIT(ADC1->CR, ADC_CR_JADSTART) == ADC_CR_JADSTART;
}

static inline bool adc_conversion_ongoing() {
    return adc_conversion_ongoing_regular() ||
           adc_conversion_ongoing_injected();
}

hal_err adc_init(adc_handle_t *handle);

hal_err adc_config_channel(adc_handle_t *handle,
                           const adc_channel_config_t *channel_config);

#endif // HAL_ADC_H
