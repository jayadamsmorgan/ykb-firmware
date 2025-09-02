#include "hal_rtc.h"

#include "hal_systick.h"

static inline hal_err hal_rtc_enter_init_mode(rtc_handle_t *handle) {
    if (READ_BIT(handle->instance->ISR, RTC_ISR_INITF))
        return OK;

    SET_BIT(handle->instance->ISR, RTC_ISR_INIT);

    uint32_t tick_start = systick_get_tick();

    while (!READ_BIT(handle->instance->ISR, RTC_ISR_INITF)) {
        if (systick_get_tick() - tick_start > RTC_TIMEOUT_VALUE) {
            handle->state = RTC_STATE_ERROR;
            return ERR_RTC_INIT_WPRE_TIMEOUT;
        }
    }

    return OK;
}

static inline hal_err hal_rtc_wait_for_sync(rtc_handle_t *handle) {

    // Reset ISR register
    WRITE_REG(handle->instance->ISR, 130911U);

    uint32_t tick_start = systick_get_tick();

    while (!READ_BIT(handle->instance->ISR, RTC_ISR_RSF)) {
        if (systick_get_tick() - tick_start > RTC_TIMEOUT_VALUE)
            return ERR_RTC_INIT_WPRX_TIMEOUT;
    }

    return OK;
}

static inline hal_err hal_rtc_exit_init_mode(rtc_handle_t *handle) {

    CLEAR_BIT(handle->instance->ISR, RTC_ISR_INIT);

    hal_err err = OK;

    if (!READ_BIT(handle->instance->CR, RTC_CR_BYPSHAD))
        err = hal_rtc_wait_for_sync(handle);

    return err;
}

static inline hal_err hal_rtc_calendar_init(rtc_handle_t *handle) {

    if (READ_BIT(handle->instance->ISR, RTC_ISR_INITS)) {
        return OK;
    }

    hal_rtc_disable_write_protection(handle);

    hal_err err;

    err = hal_rtc_enter_init_mode(handle);
    if (err) {
        goto done;
    }

    CLEAR_BIT(handle->instance->CR, RTC_CR_FMT | RTC_CR_OSEL | RTC_CR_POL);

    MODIFY_BITS(handle->instance->CR, RTC_CR_FMT_Pos, handle->init.hour_format,
                BITMASK_1BIT);
    MODIFY_BITS(handle->instance->CR, RTC_CR_OSEL_Pos, handle->init.output,
                BITMASK_2BIT);
    MODIFY_BITS(handle->instance->CR, RTC_CR_POL_Pos,
                handle->init.output_polarity, BITMASK_1BIT);

    MODIFY_BITS(handle->instance->PRER, RTC_PRER_PREDIV_S_Pos,
                handle->init.sync_pre_divider, BITMASK_15BIT);
    MODIFY_BITS(handle->instance->PRER, RTC_PRER_PREDIV_A_Pos,
                handle->init.async_pre_divider, BITMASK_7BIT);

    err = hal_rtc_exit_init_mode(handle);
    if (err) {
        goto done;
    }

    MODIFY_BITS(handle->instance->OR, RTC_OR_ALARMOUTTYPE_Pos,
                handle->init.output_type, BITMASK_1BIT);
    MODIFY_BITS(handle->instance->OR, RTC_OR_OUT_RMP_Pos,
                handle->init.output_remap, BITMASK_1BIT);

    handle->state = RTC_STATE_READY;

done:
    hal_rtc_enable_write_protection(handle);
    return err;
}

// Needs rtc clock selection before calling
hal_err hal_rtc_init(rtc_handle_t *handle) {
    if (!handle || !handle->instance) {
        return ERR_RTC_INIT_BADARGS;
    }

    if (handle->state == RTC_STATE_RESET) {
        handle->lock = RTC_UNLOCKED;
    }

    handle->state = RTC_STATE_BUSY;

    hal_err err;

    err = hal_rtc_calendar_init(handle);
    if (err) {
        return err;
    }

    return OK;
}

void hal_rtc_bypass_shadow_enable(rtc_handle_t *handle) {

    handle->lock = RTC_LOCKED;
    handle->state = RTC_STATE_BUSY;

    hal_rtc_disable_write_protection(handle);

    SET_BIT(handle->instance->CR, RTC_CR_BYPSHAD);

    hal_rtc_enable_write_protection(handle);

    handle->state = RTC_STATE_READY;
    handle->lock = RTC_UNLOCKED;
}

void hal_rtc_bypass_shadow_disable(rtc_handle_t *handle) {

    handle->lock = RTC_LOCKED;
    handle->state = RTC_STATE_BUSY;

    hal_rtc_disable_write_protection(handle);

    CLEAR_BIT(handle->instance->CR, RTC_CR_BYPSHAD);

    hal_rtc_enable_write_protection(handle);

    handle->state = RTC_STATE_READY;
    handle->lock = RTC_UNLOCKED;
}
