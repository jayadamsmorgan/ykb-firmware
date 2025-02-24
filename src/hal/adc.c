#include "hal/adc.h"

#include "hal/bits.h"
#include "hal/cortex.h"
#include "hal/hal_err.h"
#include "hal/systick.h"
#include "stm32wb55xx.h"
#include "stm32wbxx.h"
#include <stdint.h>
#include <stdio.h>

hal_err adc_init(adc_handle_t *handle) {

    if (handle->lock) {
        return ERR_ADC_INIT_BUSY;
    }
    handle->lock = true;

    adc_init_t *init = &handle->init;

    if (init->sequence_mode == ADC_SEQUENCE_DISCONTINUOUS &&
        init->conversion_mode == ADC_CONV_SEQUENCE) {
        return ERR_ADC_INIT_INV_SEQ_CONV;
    }

    if (handle->state == HAL_ADC_STATE_RESET) {

        // Enable ADC clock
        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_ADCEN);

        // Enable ADC interrupt
        cortex_nvic_set_priority(ADC1_IRQn, 0, 0);
        cortex_nvic_enable(ADC1_IRQn);

        // Clear errors + init lock
        handle->error = HAL_ADC_ERROR_NONE;
        handle->lock = false;
    }

    // Exit ADC deep power down mode
    if (READ_BIT(ADC1->CR, ADC_CR_DEEPPWD)) {
        CLEAR_BIT(ADC1->CR, ADC_CR_DEEPPWD);
    }

    // Enable ADC voltage regulator
    if (!READ_BIT(ADC1->CR, ADC_CR_ADVREGEN)) {

        SET_BIT(ADC1->CR, ADC_CR_ADVREGEN);

        /* Note: Variable divided by 2 to compensate partially              */
        /*       CPU processing cycles, scaling in us split to not          */
        /*       exceed 32 bits register capacity and handle low frequency. */
        __IO uint32_t wait =
            ((20UL / 10UL) * ((SystemCoreClock / (100000UL * 2UL)) + 1UL));
        while (wait != 0UL) {
            wait--;
        }
    }

    // Double check voltage regulator enabled
    if (!READ_BIT(ADC1->CR, ADC_CR_ADVREGEN)) {

        SET_BIT(handle->error, HAL_ADC_ERROR_INTERNAL);
        SET_BIT(handle->state, HAL_ADC_STATE_ERROR_INTERNAL);

        return ERR_ADC_INIT_ADVREGEN_FAULT;
    }

    // Making sure no regular conversions are ongoing
    if (adc_conversion_ongoing_regular()) {

        SET_BIT(handle->error, HAL_ADC_ERROR_INTERNAL);
        SET_BIT(handle->state, HAL_ADC_STATE_ERROR_INTERNAL);

        return ERR_ADC_INIT_CONV_ONGOING;
    }

    // Set ADC state
    MODIFY_REG(handle->state, HAL_ADC_STATE_REG_BUSY,
               HAL_ADC_STATE_BUSY_INTERNAL);

    // ADC clock can be updated only when ADC is disabled
    if (!READ_BIT(ADC1->CR, ADC_CR_ADEN)) {
        MODIFY_BITS(ADC1_COMMON->CCR, ADC_CCR_CKMODE_Pos, init->clock_mode,
                    BITMASK_2BIT);
        MODIFY_BITS(ADC1_COMMON->CCR, ADC_CCR_PRESC_Pos, init->clock_prescaler,
                    BITMASK_4BIT);
    }

    uint32_t tmp_reg = 0;

    bool conversion_ongoing = adc_conversion_ongoing();

    //
    // START ADC1->CFGR
    //

    // Disable Injected Queue (default)
    SET_BIT(tmp_reg, ADC_CFGR_JQDIS);

    // DMA config
    if (init->dma_mode != ADC_DMA_DISABLE && !conversion_ongoing) {
        if (init->dma_mode == ADC_DMA_CIRCULAR) {
            SET_BIT(tmp_reg, ADC_CFGR_DMACFG);
        }
        SET_BIT(tmp_reg, ADC_CFGR_DMAEN);
    }

    // ADC resolution
    MODIFY_BITS(tmp_reg, ADC_CFGR_RES_Pos, init->resolution, BITMASK_2BIT);
    // DR register data align
    MODIFY_BITS(tmp_reg, ADC_CFGR_ALIGN_Pos, init->data_align, BITMASK_1BIT);

    // ADC trigger source config
    if (init->trigger_source != ADC_TRIGGER_SOFTWARE) {
        init->trigger_edge = ADC_TRIGGER_EDGE_RISING;
        tmp_reg |= init->trigger_source;
        MODIFY_BITS(tmp_reg, ADC_CFGR_EXTEN_Pos, init->trigger_edge,
                    BITMASK_2BIT);
    }

    // Other configurations:
    MODIFY_BITS(tmp_reg, ADC_CFGR_OVRMOD_Pos, init->overrun_mode, BITMASK_1BIT);
    MODIFY_BITS(tmp_reg, ADC_CFGR_CONT_Pos, init->conversion_mode,
                BITMASK_1BIT);
    if (!conversion_ongoing) {
        MODIFY_BITS(tmp_reg, ADC_CFGR_AUTDLY_Pos, init->lp_autowait,
                    BITMASK_1BIT);
    }
    MODIFY_BITS(tmp_reg, ADC_CFGR_DISCEN_Pos, init->sequence_mode,
                BITMASK_1BIT);
    MODIFY_BITS(tmp_reg, ADC_CFGR_DISCNUM_Pos, init->sequence_amount,
                BITMASK_3BIT);

    WRITE_REG(ADC1->CFGR, tmp_reg);

    //
    // END ADC1->CFGR
    //

    //
    // START ADC1->CFGR2
    //

    if (init->oversampling_mode != ADC_OVERSAMPLING_DISABLED &&
        !conversion_ongoing) {

        tmp_reg = 0;
        tmp_reg |= init->oversampling_mode;

        MODIFY_BITS(tmp_reg, ADC_CFGR2_OVSR_Pos, init->oversampling_ratio,
                    BITMASK_3BIT);
        MODIFY_BITS(tmp_reg, ADC_CFGR2_OVSS_Pos, init->oversampling_shift,
                    BITMASK_4BIT);
        MODIFY_BITS(tmp_reg, ADC_CFGR2_TROVS_Pos,
                    init->oversampling_triggered_mode, BITMASK_1BIT);
        MODIFY_BITS(tmp_reg, ADC_CFGR2_ROVSM_Pos,
                    init->oversampling_regular_mode, BITMASK_1BIT);

        WRITE_REG(ADC1->CFGR2, tmp_reg);
    }

    //
    // END ADC1->CFGR2
    //

    MODIFY_BITS(ADC1->SQR1, ADC_SQR1_L_Pos,
                init->regular_channel_sequence_length, BITMASK_4BIT);

    // Set ADC state
    MODIFY_REG(handle->state, HAL_ADC_STATE_BUSY_INTERNAL, HAL_ADC_STATE_READY);

    handle->lock = false;

    return OK;
}

static inline volatile uint32_t *
channel_offset_type_register(adc_channel_offset_type type) {
    switch (type) {
    case ADC_CHANNEL_OFFSET_1:
        return &ADC1->OFR1;
    case ADC_CHANNEL_OFFSET_2:
        return &ADC1->OFR2;
    case ADC_CHANNEL_OFFSET_3:
        return &ADC1->OFR3;
    case ADC_CHANNEL_OFFSET_4:
        return &ADC1->OFR4;
    default:
        return NULL;
    }
}

static inline uint8_t channel_offset_read_channel(uint32_t reg) {
    return ((reg >> ADC_OFR1_OFFSET1_CH_Pos) & BITMASK_5BIT);
}

hal_err adc_config_channel(adc_handle_t *handle,
                           const adc_channel_config_t *channel_config) {

    if (handle->lock) {
        return ERR_ADC_CHCONF_BUSY;
    }
    handle->lock = true;

    // Check channel offset
    uint16_t max_offset = 0xFFFU >> (handle->init.resolution * 2);
    if (channel_config->offset > max_offset) {
        handle->lock = false;
        return ERR_ADC_CHCONF_INV_OFFSET;
    }

    /* if ROVSE is set, the value of the OFFSETy_EN bit in ADCx_OFRy register is
       ignored (considered as reset) */
    if (channel_config->offset_type != ADC_CHANNEL_OFFSET_NONE &&
        handle->init.oversampling_mode != ADC_OVERSAMPLING_DISABLED) {

        handle->lock = false;
        return ERR_ADC_CHCONF_INV_OFFSETTYPE;
    }

    // Only channels 1-15 support differrential mode
    if (channel_config->mode == ADC_CHANNEL_DIFFERENTIAL &&
        (channel_config->channel == 0U || channel_config->channel > 15U)) {

        handle->lock = false;
        return ERR_ADC_CHCONF_INV_MODE4CH;
    }

    // No regular conversion allowed during configuration
    if (adc_conversion_ongoing_regular()) {
        handle->lock = false;
        return ERR_ADC_CHCONF_CONV_ONGOING;
    }

    MODIFY_BITS(*channel_config->rank.reg, channel_config->rank.offset,
                channel_config->channel, BITMASK_4BIT);

    // Channel sampling time & offset can't be modified during conversion
    if (!adc_conversion_ongoing()) {

        // Sampling time
        if (channel_config->channel < 10) {
            MODIFY_BITS(ADC1->SMPR1, (channel_config->channel) * 3,
                        channel_config->sampling_time, BITMASK_3BIT);
        } else if (channel_config->channel <= 18) {
            MODIFY_BITS(ADC1->SMPR2, (channel_config->channel - 10) * 3,
                        channel_config->sampling_time, BITMASK_3BIT);
        }

        // Offset
        if (channel_config->offset_type != ADC_CHANNEL_OFFSET_NONE) {
            volatile uint32_t *ofr_reg =
                channel_offset_type_register(channel_config->offset_type);

            uint8_t offset_shift = handle->init.resolution * 2;

            MODIFY_BITS(*ofr_reg, ADC_OFR1_OFFSET1_Pos,
                        (channel_config->offset << offset_shift),
                        BITMASK_12BIT);
            MODIFY_BITS(*ofr_reg, ADC_OFR1_OFFSET1_CH_Pos,
                        channel_config->channel, BITMASK_4BIT);

            SET_BIT(*ofr_reg, ADC_OFR1_OFFSET1_EN);
        } else {
            // Scan OFR registers for the channel and turn offset off
            if (channel_offset_read_channel(ADC1->OFR1) ==
                channel_config->channel) {
                ADC1->OFR1 = 0U;
            }
            if (channel_offset_read_channel(ADC1->OFR2) ==
                channel_config->channel) {
                ADC1->OFR2 = 0U;
            }
            if (channel_offset_read_channel(ADC1->OFR3) ==
                channel_config->channel) {
                ADC1->OFR3 = 0U;
            }
            if (channel_offset_read_channel(ADC1->OFR4) ==
                channel_config->channel) {
                ADC1->OFR4 = 0U;
            }
        }

        // Mode and internal measurment channels
        // can't be updated while ADC is on
        if (!(READ_BIT(ADC1->CR, ADC_CR_ADEN))) {

            // Mode
            MODIFY_BITS(ADC1->DIFSEL, channel_config->channel,
                        channel_config->mode, BITMASK_1BIT);

            // Internal channels
            switch (channel_config->channel) {

            case ADC_CHANNEL_TEMPSENSOR:
                if (!(ADC1_COMMON->CCR & ADC_CCR_TSEN)) {
                    SET_BIT(ADC1_COMMON->CCR, ADC_CCR_TSEN);
                    /* Note: Variable divided by 2 to compensate partially */
                    /*    CPU processing cycles, scaling in us split to not */
                    /*       exceed 32 bits register capacity and handle low
                     * frequency. */
                    __IO uint32_t wait_loop_index;
                    wait_loop_index =
                        ((120UL / 10UL) *
                         ((SystemCoreClock / (100000UL * 2UL)) + 1UL));
                    while (wait_loop_index != 0UL) {
                        wait_loop_index--;
                    }
                }
                break;

            case ADC_CHANNEL_VBAT:
                SET_BIT(ADC1_COMMON->CCR, ADC_CCR_VBATEN);
                break;

            case ADC_CHANNEL_VREFINT:
                SET_BIT(ADC1_COMMON->CCR, ADC_CCR_VREFEN);
                break;

            default:
                break;
            }
        }
    }

    handle->lock = false;
    return OK;
}

hal_err adc_disable(adc_handle_t *handle) {

    bool adc_enabled = READ_BIT(ADC1->CR, ADC_CR_ADEN);
    bool adc_disable_is_ongoing = READ_BIT(ADC1->CR, ADC_CR_ADDIS);

    if (!adc_enabled || adc_disable_is_ongoing) {
        return OK;
    }

    if (adc_conversion_ongoing()) {
        SET_BIT(handle->state, HAL_ADC_STATE_ERROR_INTERNAL);
        SET_BIT(handle->error, HAL_ADC_ERROR_INTERNAL);
        return ERR_ADC_DISABLE_CONV_ONGOING;
    }

    uint32_t tick_start = systick_get_tick();

    while (READ_BIT(ADC1->CR, ADC_CR_ADEN)) {
        if (systick_get_tick() - tick_start > ADC_DISABLE_TIMEOUT) {
            if (READ_BIT(ADC1->CR, ADC_CR_ADEN)) {
                SET_BIT(handle->state, HAL_ADC_STATE_ERROR_INTERNAL);
                SET_BIT(handle->error, HAL_ADC_ERROR_INTERNAL);
                return ERR_ADC_DISABLE_TIMEOUT;
            }
        }
    }

    return OK;
}

hal_err adc_enable(adc_handle_t *handle) {

    // Nothing to do if already enabled
    if (READ_BIT(ADC1->CR, ADC_CR_ADEN)) {
        return OK;
    }

    // Requirements
    if ((ADC1->CR &
         (ADC_CR_ADCAL | ADC_CR_JADSTP | ADC_CR_ADSTP | ADC_CR_JADSTART |
          ADC_CR_ADSTART | ADC_CR_ADDIS | ADC_CR_ADEN))) {
        SET_BIT(handle->state, HAL_ADC_STATE_ERROR_INTERNAL);
        SET_BIT(handle->error, HAL_ADC_ERROR_INTERNAL);
        return ERR_ADC_ENABLE_REQUIREMENTS;
    }

    // Enable ADC
    SET_BIT(ADC1->CR, ADC_CR_ADEN);

    // Delay for temperature sensor buffer stabilization time
    if (READ_BIT(ADC1_COMMON->CCR, ADC_CCR_TSEN)) {
        uint32_t wait_loop_index =
            ((15UL / 10UL) * ((SystemCoreClock / (100000UL * 2UL)) + 1UL));
        while (wait_loop_index) {
            wait_loop_index--;
        }
    }

    uint32_t tick_start = systick_get_tick();

    while (READ_BIT(ADC1->ISR, ADC_ISR_ADRDY) == 0UL) {
        /*  If ADEN bit is set less than 4 ADC clock cycles after the ADCAL bit
            has been cleared (after a calibration), ADEN bit is reset by the
            calibration logic.
            The workaround is to continue setting ADEN until ADRDY is becomes 1.
            Additionally, ADC_ENABLE_TIMEOUT is defined to encompass this
            4 ADC clock cycle duration */
        if (!READ_BIT(ADC1->CR, ADC_CR_ADEN)) {
            SET_BIT(ADC1->CR, ADC_CR_ADEN);
        }

        if (systick_get_tick() - tick_start > ADC_ENABLE_TIMEOUT) {
            SET_BIT(handle->state, HAL_ADC_STATE_ERROR_INTERNAL);
            SET_BIT(handle->error, HAL_ADC_ERROR_INTERNAL);
            return ERR_ADC_ENABLE_TIMEOUT;
        }
    }

    return OK;
}

hal_err adc_start_calibration(adc_handle_t *handle, adc_channel_mode mode) {

    if (handle->lock) {
        return ERR_ADC_CALIB_BUSY;
    }
    handle->lock = true;

    hal_err err = adc_disable(handle);
    if (err) {
        handle->lock = false;
        return err;
    }

    MODIFY_REG(handle->state, HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
               HAL_ADC_STATE_BUSY_INTERNAL);

    // Start calibration
    if (mode == ADC_CHANNEL_DIFFERENTIAL) {
        SET_BIT(ADC1->CR, ADC_CR_ADCALDIF);
    }
    SET_BIT(ADC1->CR, ADC_CR_ADCAL);

    // Wait for calibration to finish
    uint32_t wait_loop_index = 0UL;
    while (READ_BIT(ADC1->CR, ADC_CR_ADCAL)) {
        wait_loop_index++;
        if (wait_loop_index >= ADC_CALIBRATION_TIMEOUT) {
            MODIFY_REG(handle->state, HAL_ADC_STATE_BUSY_INTERNAL,
                       HAL_ADC_STATE_ERROR_INTERNAL);
            handle->lock = false;
            return ERR_ADC_CALIB_TIMEOUT;
        }
    }

    MODIFY_REG(handle->state, HAL_ADC_STATE_BUSY_INTERNAL, HAL_ADC_STATE_READY);

    handle->lock = false;
    return OK;
}
