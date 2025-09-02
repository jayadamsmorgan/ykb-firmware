#include "timerserver.h"

#include "rtc.h"

#include "hal_clock.h"
#include "hal_cortex.h"
#include "hal_exti.h"

static uint32_t max_wakeup_timer;

#define TS_RTC_HANDLER_MAX_DELAY (10U * (LSE_VALUE / 1000U))

typedef void (*timer_cb)(void);

typedef enum {
    TIMER_STATUS_CREATED = 0U,
    TIMER_STATUS_FREE = 1U,
    TIMER_STATUS_RUNNING = 2U,
} ts_timer_status;

typedef enum {
    TIMER_MODE_SINGLE_SHOT = 0U,
    TIMER_MODE_REPEATED = 1U,
} ts_timer_mode;

typedef struct {

    timer_cb callback;

    uint32_t counter_init;
    uint32_t count_left;

    ts_timer_status status;
    ts_timer_mode mode;

    uint32_t pid;

    uint8_t prev_id;
    uint8_t next_id;

} ts_timer_t;

static ts_timer_t timer_pool[255];

hal_err timerserver_init(timerserver_init_t init) {

    hal_rtc_bypass_shadow_enable(&rtc_handle);

    hal_rtc_disable_write_protection(&rtc_handle);

    uint32_t wakeup_timer_div =
        4U - hal_rtc_wakeup_clock_get_source(&rtc_handle);

    max_wakeup_timer = (rtc_handle.init.sync_pre_divider *
                            (rtc_handle.init.async_pre_divider + 1) -
                        TS_RTC_HANDLER_MAX_DELAY) >>
                       wakeup_timer_div;
    max_wakeup_timer = max_wakeup_timer > 0xFFFF ? 0xFFFF : max_wakeup_timer;

    hal_exti_enable_rising(EXTI_EVENT_19);
    hal_exti_enable_it(EXTI_EVENT_19);

    switch (init.mode) {
    case TS_INIT_MODE_FULL:
        for (uint8_t i = 0; i < init.timers_amount; i++) {
            timer_pool[i].status = TIMER_STATUS_FREE;
        }
        hal_rtc_wakeup_timer_disable(&rtc_handle);
        hal_rtc_wakeup_timer_clear_flag(&rtc_handle, RTC_FLAG_WUTF);
        hal_exti_clear_event(EXTI_EVENT_19);
        cortex_nvic_clear_pending(RTC_WKUP_IRQn);
        hal_rtc_wakeup_timer_enable_it(&rtc_handle);
        break;
    case TS_INIT_MODE_LIMITED:
        if (hal_rtc_wakeup_timer_get_flag(&rtc_handle, RTC_FLAG_WUTF))
            cortex_nvic_set_pending(RTC_WKUP_IRQn);
        break;
    default:
        hal_rtc_enable_write_protection(&rtc_handle);
        return ERR_TS_INIT_UNKNOWN_MODE;
    }

    hal_rtc_enable_write_protection(&rtc_handle);

    cortex_nvic_set_priority(RTC_WKUP_IRQn, 3, 0);
    cortex_nvic_enable(RTC_WKUP_IRQn);

    return OK;
}
