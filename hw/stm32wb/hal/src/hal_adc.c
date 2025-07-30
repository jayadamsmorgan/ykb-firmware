#include "hal_adc.h"

#include "hal_bits.h"
#include "hal_cortex.h"
#include "hal_dma.h"
#include "hal_err.h"
#include "hal_gpio.h"
#include "hal_systick.h"
#include "stm32wbxx.h"
#include "utils/utils.h"

#include <stdint.h>
#include <stdio.h>

static volatile adc_handle_t hal_adc_handle;

hal_err adc_init(const adc_init_t *init) {

    if (!init) {
        return ERR_ADC_INIT_ARGNULL;
    }

    volatile adc_handle_t *handle = &hal_adc_handle;

    if (handle->lock) {
        return ERR_ADC_INIT_BUSY;
    }
    handle->lock = true;

    handle->init = *init;

    if (init->sequence_mode == ADC_SEQUENCE_DISCONTINUOUS &&
        init->conversion_mode == ADC_CONVERSION_SEQUENCE) {
        return ERR_ADC_INIT_INV_SEQ_CONV;
    }

    if (handle->state == HAL_ADC_STATE_RESET) {

        // Enable ADC clock
        clock_adc_enable();

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
        handle->init.trigger_edge = ADC_TRIGGER_EDGE_RISING;
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

hal_err adc_config_channel(const adc_channel_config_t *channel_config) {

    volatile adc_handle_t *handle = &hal_adc_handle;

    if (handle->lock) {
        return ERR_ADC_CHCONF_BUSY;
    }
    handle->lock = true;

    // Check channel offset
    uint16_t max_offset = 0xFFFU >> (handle->init.resolution * 2);
    if (channel_config->offset_type != ADC_CHANNEL_OFFSET_NONE &&
        channel_config->offset > max_offset) {
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

hal_err adc_disable() {

    volatile adc_handle_t *handle = &hal_adc_handle;

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

    SET_BIT(ADC1->CR, ADC_CR_ADDIS);

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

hal_err adc_enable() {

    volatile adc_handle_t *handle = &hal_adc_handle;

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

hal_err adc_start_calibration(adc_channel_mode mode) {

    volatile adc_handle_t *handle = &hal_adc_handle;

    if (handle->lock) {
        return ERR_ADC_CALIB_BUSY;
    }
    handle->lock = true;

    hal_err err = adc_disable();
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

hal_err adc_conversion_stop(adc_conversion_group group) {

    if (!adc_conversion_ongoing()) {
        return OK;
    }

    volatile adc_handle_t *handle = &hal_adc_handle;

    if ((ADC1->CFGR & ADC_CFGR_JAUTO) &&
        handle->init.conversion_mode == ADC_CONVERSION_SEQUENCE &&
        handle->init.lp_autowait == ADC_LP_AUTOWAIT_ENABLE) {

        group = ADC_CONVERSION_GROUP_REGULAR;

        uint32_t conversion_timeout = 0UL;

        while (!(ADC1->ISR & ADC_ISR_JEOS)) {
            if (conversion_timeout >= ADC_CONVERSION_TIMEOUT * 4UL) {
                SET_BIT(handle->state, HAL_ADC_STATE_ERROR_INTERNAL);
                SET_BIT(handle->error, HAL_ADC_ERROR_INTERNAL);
                return ERR_ADC_STOPCONV_TIMEOUT;
            }
            conversion_timeout++;
        }
        SET_BIT(ADC1->ISR, ADC_ISR_JEOS);
    }

    if (group != ADC_CONVERSION_GROUP_INJECTED) {
        if (adc_conversion_ongoing_regular()) {
            if (!(ADC1->CR & ADC_CR_ADDIS)) {
                SET_BIT(ADC1->CR, ADC_CR_ADSTP);
            }
        }
    }

    if (group != ADC_CONVERSION_GROUP_REGULAR) {
        if (adc_conversion_ongoing_injected()) {
            if (!(ADC1->CR & ADC_CR_ADDIS)) {
                SET_BIT(ADC1->CR, ADC_CR_JADSTP);
            }
        }
    }

    uint32_t tick_start = systick_get_tick();

    while (ADC1->CR & group) {
        if ((systick_get_tick() - tick_start) > ADC_STOP_CONVERSION_TIMEOUT) {
            if (ADC1->CR & group) {
                SET_BIT(handle->state, HAL_ADC_STATE_ERROR_INTERNAL);
                SET_BIT(handle->error, HAL_ADC_ERROR_INTERNAL);
                return ERR_ADC_STOPCONV_TIMEOUT;
            }
        }
    }

    return OK;
}

hal_err adc_start() {
    volatile adc_handle_t *handle = &hal_adc_handle;

    if (adc_conversion_ongoing_regular() || handle->lock) {
        return ERR_ADC_START_BUSY;
    }

    handle->lock = true;

    hal_err err;

    err = adc_enable();
    if (err) {
        handle->lock = false;
        return err;
    }

    MODIFY_REG(handle->state,
               HAL_ADC_STATE_READY | HAL_ADC_STATE_REG_EOC |
                   HAL_ADC_STATE_REG_OVR | HAL_ADC_STATE_REG_EOSMP,
               HAL_ADC_STATE_REG_BUSY);

    if (handle->state & HAL_ADC_STATE_INJ_BUSY) {
        CLEAR_BIT(handle->error, (HAL_ADC_ERROR_OVR | HAL_ADC_ERROR_DMA));
    } else {
        handle->error = HAL_ADC_ERROR_NONE;
    }

    WRITE_REG(ADC1->ISR, (ADC_ISR_EOC | ADC_ISR_EOS | ADC_ISR_OVR));

    handle->lock = false;

    MODIFY_REG(ADC1->CR,
               (ADC_CR_ADCAL | ADC_CR_JADSTP | ADC_CR_ADSTP | ADC_CR_JADSTART |
                ADC_CR_ADSTART | ADC_CR_ADDIS | ADC_CR_ADEN),
               ADC_CR_ADSTART);

    return OK;
}

hal_err adc_start_it() {

    volatile adc_handle_t *handle = &hal_adc_handle;

    if (adc_conversion_ongoing_regular()) {
        return ERR_ADC_STARTIT_CONV_ONGOING;
    }

    if (handle->lock) {
        return ERR_ADC_STARTIT_BUSY;
    }
    handle->lock = true;

    hal_err err = adc_enable();
    if (err) {
        handle->lock = false;
        return err;
    }

    MODIFY_REG(handle->state,
               HAL_ADC_STATE_READY | HAL_ADC_STATE_REG_EOC |
                   HAL_ADC_STATE_REG_OVR | HAL_ADC_STATE_REG_EOSMP,
               HAL_ADC_STATE_REG_BUSY);

    if ((handle->state & HAL_ADC_STATE_INJ_BUSY)) {
        CLEAR_BIT(handle->error, (HAL_ADC_ERROR_OVR | HAL_ADC_ERROR_DMA));
    } else {
        handle->error = HAL_ADC_ERROR_NONE;
    }

    WRITE_REG(ADC1->ISR, ADC_ISR_EOC | ADC_ISR_EOS | ADC_ISR_OVR);

    handle->lock = false;

    CLEAR_BIT(ADC1->IER, ADC_IER_EOCIE | ADC_IER_EOSIE | ADC_IER_OVRIE);

    switch (handle->init.eoc_flag) {
    case ADC_EOC_SINGLE_CONVERSION:
        SET_BIT(ADC1->IER, ADC_IER_EOCIE);
        break;
    case ADC_EOC_SEQ_CONVERSION:
        SET_BIT(ADC1->IER, ADC_IER_EOSIE);
        break;
    }

    if (handle->init.overrun_mode == ADC_OVERRUN_DATA_PRESERVED) {
        SET_BIT(ADC1->IER, ADC_IER_OVRIE);
    }

    SET_BIT(ADC1->CR, ADC_CR_ADSTART);

    return OK;
}

hal_err adc_stop_it() {

    volatile adc_handle_t *handle = &hal_adc_handle;

    if (handle->lock) {
        return ERR_ADC_STOPIT_BUSY;
    }
    handle->lock = true;

    hal_err err;

    err = adc_conversion_stop(ADC_CONVERSION_GROUP_REGULAR_INJECTED);
    if (err) {
        return err;
    }

    CLEAR_BIT(ADC1->IER, ADC_IER_EOCIE | ADC_IER_EOSIE | ADC_IER_OVRIE);

    err = adc_disable();
    if (err) {
        return err;
    }

    MODIFY_REG(handle->state, HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
               HAL_ADC_STATE_READY);

    handle->lock = false;
    return OK;
}

void adc_dma_conversion_complete(dma_handle_t *dma_handle) {

    volatile adc_handle_t *adc_handle = &hal_adc_handle;

    if (adc_handle->state & HAL_ADC_STATE_ERROR_INTERNAL) {
        adc_handle->callbacks.error_callback(adc_handle->error);
        return;
    }
    if (adc_handle->state & HAL_ADC_STATE_ERROR_DMA) {
        dma_handle->xfer_error_callback(dma_handle);
        return;
    }

    SET_BIT(adc_handle->state, HAL_ADC_STATE_REG_EOC);

    if (!(ADC1->ISR & ADC_ISR_EOS)) {
        if (!READ_BIT(ADC1->CFGR, ADC_CFGR_DMACFG)) {
            CLEAR_BIT(adc_handle->state, HAL_ADC_STATE_REG_BUSY);
            if ((adc_handle->state & HAL_ADC_STATE_INJ_BUSY) == 0UL) {
                SET_BIT(adc_handle->state, HAL_ADC_STATE_READY);
            }
        }
        adc_handle->callbacks.conversion_complete(ADC_CONVERSION_TRIGGER_EOS);
        return;
    }

    if (READ_BIT(ADC1->CFGR, ADC_CFGR_EXTEN)) {
        adc_handle->callbacks.conversion_complete(ADC_CONVERSION_TRIGGER_EOS);
        return;
    }

    if (READ_BIT(ADC1->CFGR, ADC_CFGR_CONT) == 0UL) {
        CLEAR_BIT(adc_handle->state, HAL_ADC_STATE_REG_BUSY);
        if ((adc_handle->state & HAL_ADC_STATE_INJ_BUSY) == 0UL) {
            SET_BIT(adc_handle->state, HAL_ADC_STATE_READY);
        }
    }
    adc_handle->callbacks.conversion_complete(ADC_CONVERSION_TRIGGER_EOS);
}

void adc_dma_conversion_half_complete(dma_handle_t *dma_handle) {
    UNUSED(dma_handle);

    hal_adc_handle.callbacks.conversion_half_complete();
}

void adc_dma_error(dma_handle_t *dma_handle) {

    UNUSED(dma_handle);

    volatile adc_handle_t *adc_handle = &hal_adc_handle;

    SET_BIT(adc_handle->state, HAL_ADC_STATE_ERROR_DMA);
    SET_BIT(adc_handle->error, HAL_ADC_ERROR_DMA);

    adc_handle->callbacks.error_callback(adc_handle->error);
}

hal_err adc_start_dma(uint32_t *dma_buf, uint32_t dma_buf_len) {

    volatile adc_handle_t *handle = &hal_adc_handle;

    if (adc_conversion_ongoing_regular() || handle->lock) {
        return ERR_ADC_STARTDMA_BUSY;
    }

    if (!dma_buf || dma_buf_len == 0) {
        return ERR_ADC_STARTDMA_BADARGS;
    }

    handle->lock = true;

    hal_err err;

    err = adc_enable();
    if (err) {
        handle->lock = false;
        return err;
    }

    MODIFY_REG(handle->state,
               HAL_ADC_STATE_READY | HAL_ADC_STATE_REG_EOC |
                   HAL_ADC_STATE_REG_OVR | HAL_ADC_STATE_REG_EOSMP,
               HAL_ADC_STATE_REG_BUSY);

    if (handle->state & HAL_ADC_STATE_INJ_BUSY) {
        CLEAR_BIT(handle->error, (HAL_ADC_ERROR_OVR | HAL_ADC_ERROR_DMA));
    } else {
        handle->error = HAL_ADC_ERROR_NONE;
    }

    handle->dma_handle->xfer_complete_callback = adc_dma_conversion_complete;
    handle->dma_handle->xfer_half_complete_callback =
        adc_dma_conversion_half_complete;
    handle->dma_handle->xfer_error_callback = adc_dma_error;

    WRITE_REG(ADC1->ISR, (ADC_ISR_EOC | ADC_ISR_EOS | ADC_ISR_OVR));

    handle->lock = false;

    SET_BIT(ADC1->IER, ADC_IER_OVRIE);

    SET_BIT(ADC1->CFGR, ADC_CFGR_DMAEN);

    err = dma_start_it(handle->dma_handle, (uint32_t)&ADC1->DR,
                       (uint32_t)dma_buf, dma_buf_len);
    if (err) {
        return err;
    }

    MODIFY_REG(ADC1->CR,
               (ADC_CR_ADCAL | ADC_CR_JADSTP | ADC_CR_ADSTP | ADC_CR_JADSTART |
                ADC_CR_ADSTART | ADC_CR_ADDIS | ADC_CR_ADEN),
               ADC_CR_ADSTART);

    return OK;
}

void adc_set_callbacks(adc_callbacks_t callbacks) {
    hal_adc_handle.callbacks = callbacks;
}

__weak void ADC1_IRQHandler(void) {

    volatile adc_handle_t *handle = &hal_adc_handle;

    adc_conversion_trigger trigger = ADC_CONVERSION_TRIGGER_EOC;

    if ((ADC1->IER & ADC_IER_EOSMPIE) && (ADC1->ISR & ADC_ISR_EOSMP)) {
        if (!(handle->state & HAL_ADC_STATE_ERROR_INTERNAL)) {
            SET_BIT(handle->state, HAL_ADC_STATE_REG_EOSMP);
        }
        if (handle->callbacks.sampling_complete) {
            handle->callbacks.sampling_complete();
        }
        SET_BIT(ADC1->ISR, ADC_ISR_EOSMP);
    }

    if (((ADC1->IER & ADC_IER_EOCIE) && (ADC1->ISR & ADC_ISR_EOC)) ||
        ((ADC1->IER & ADC_IER_EOSIE) && (ADC1->ISR & ADC_ISR_EOS))) {

        if (!(handle->state & HAL_ADC_ERROR_INTERNAL)) {
            SET_BIT(handle->state, HAL_ADC_STATE_REG_EOC);
        }

        if ((READ_BIT(ADC1->CFGR, ADC_CFGR_EXTEN) == (0UL & ADC_CFGR_EXTEN)) &&
            READ_BIT(ADC1->CFGR, ADC_CFGR_CONT) &&
            READ_BIT(ADC1->ISR, ADC_ISR_EOS)) {
            if (!adc_conversion_ongoing_regular()) {
                CLEAR_BIT(ADC1->IER, ADC_IER_EOCIE | ADC_IER_EOSIE);

                CLEAR_BIT(handle->state, HAL_ADC_STATE_REG_BUSY);
                if (!(handle->state & HAL_ADC_STATE_INJ_BUSY)) {
                    SET_BIT(handle->state, HAL_ADC_STATE_READY);
                }
            } else {
                SET_BIT(handle->state, HAL_ADC_STATE_ERROR_INTERNAL);
                SET_BIT(handle->error, HAL_ADC_ERROR_INTERNAL);
            }
        }

        if (READ_BIT(ADC1->ISR, ADC_ISR_EOS)) {
            trigger = ADC_CONVERSION_TRIGGER_EOS;
        }
        if (handle->callbacks.conversion_complete) {
            handle->callbacks.conversion_complete(trigger);
        }

        SET_BIT(ADC1->ISR, ADC_ISR_EOC | ADC_ISR_EOS);
    }

    if (((ADC1->IER & ADC_IER_JEOCIE) && (ADC1->ISR & ADC_ISR_JEOC)) ||
        ((ADC1->IER & ADC_IER_JEOSIE) && (ADC1->ISR & ADC_ISR_JEOS))) {

        if (!(handle->state & HAL_ADC_ERROR_INTERNAL)) {
            SET_BIT(handle->state, HAL_ADC_STATE_INJ_EOC);
        }

        bool inj_is_trigger_source_sw_start = ((
            READ_BIT(ADC1->JSQR, ADC_JSQR_JEXTEN) == (0UL & ADC_JSQR_JEXTEN)));
        bool reg_is_trigger_source_sw_start =
            ((READ_BIT(ADC1->CFGR, ADC_CFGR_EXTEN) == (0UL & ADC_CFGR_EXTEN)));

        if (inj_is_trigger_source_sw_start &&
            (!(READ_BIT(ADC1->CFGR, ADC_CFGR_JAUTO)) ||
             (reg_is_trigger_source_sw_start &&
              !(READ_BIT(ADC1->CFGR, ADC_CFGR_CONT))))) {

            if (READ_BIT(ADC1->ISR, ADC_ISR_JEOS)) {

                if (!(READ_BIT(ADC1->CFGR, ADC_CFGR_JQM))) {

                    if (!adc_conversion_ongoing()) {

                        CLEAR_BIT(ADC1->IER, ADC_IER_JEOCIE | ADC_IER_JEOSIE);
                        if (!(handle->state & HAL_ADC_STATE_REG_BUSY)) {
                            SET_BIT(handle->state, HAL_ADC_STATE_READY);
                        }

                    } else {
                        SET_BIT(handle->state, HAL_ADC_STATE_ERROR_INTERNAL);
                        SET_BIT(handle->error, HAL_ADC_ERROR_INTERNAL);
                    }
                }
            }
        }

        if (READ_BIT(ADC1->ISR, ADC_ISR_JEOS)) {
            trigger = ADC_CONVERSION_TRIGGER_EOS;
        }

        if (handle->callbacks.injected_conversion_complete) {
            handle->callbacks.injected_conversion_complete(trigger);
        }

        SET_BIT(ADC1->ISR, ADC_ISR_JEOC | ADC_ISR_JEOS);
    }

    if ((ADC1->IER & ADC_IER_AWD1IE) && (ADC1->ISR & ADC_ISR_AWD1)) {
        SET_BIT(handle->state, HAL_ADC_STATE_AWD1);
        if (handle->callbacks.awd1_callback) {
            handle->callbacks.awd1_callback();
        }
        SET_BIT(ADC1->ISR, ADC_ISR_AWD1);
    }

    if ((ADC1->IER & ADC_IER_AWD2IE) && (ADC1->ISR & ADC_ISR_AWD2)) {
        SET_BIT(handle->state, HAL_ADC_STATE_AWD2);
        if (handle->callbacks.awd2_callback) {
            handle->callbacks.awd2_callback();
        }
        SET_BIT(ADC1->ISR, ADC_ISR_AWD2);
    }

    if ((ADC1->IER & ADC_IER_AWD3IE) && (ADC1->ISR & ADC_ISR_AWD3)) {
        SET_BIT(handle->state, HAL_ADC_STATE_AWD3);
        if (handle->callbacks.awd3_callback) {
            handle->callbacks.awd3_callback();
        }
        SET_BIT(ADC1->ISR, ADC_ISR_AWD3);
    }

    if ((ADC1->IER & ADC_IER_OVRIE) && (ADC1->ISR & ADC_ISR_OVR)) {

        bool ovr_err = false;
        if (handle->init.overrun_mode == ADC_OVERRUN_DATA_PRESERVED) {
            ovr_err = true;
        } else if (READ_BIT(ADC1->CFGR, ADC_CFGR_DMAEN | ADC_CFGR_DMACFG)) {
            ovr_err = true;
        }

        if (ovr_err) {
            SET_BIT(handle->state, HAL_ADC_STATE_REG_OVR);
            SET_BIT(handle->error, HAL_ADC_ERROR_OVR);

            handle->callbacks.error_callback(handle->error);

            SET_BIT(ADC1->ISR, ADC_ISR_OVR);
        }
    }

    if ((ADC1->IER & ADC_IER_JQOVFIE) && (ADC1->ISR & ADC_ISR_JQOVF)) {
        SET_BIT(handle->state, HAL_ADC_STATE_INJ_JQOVF);
        SET_BIT(handle->error, HAL_ADC_ERROR_JQOVF);
        SET_BIT(ADC1->ISR, ADC_ISR_JQOVF);

        if (handle->callbacks.injected_queue_overflow) {
            handle->callbacks.injected_queue_overflow();
        }
    }
}

adc_handle_t *adc_get_handle() {
    return (adc_handle_t *)&hal_adc_handle;
}

void adc_link_dma(dma_handle_t *handle) {
    hal_adc_handle.dma_handle = handle;
    handle->parent = (void *)&hal_adc_handle;
}
