#ifndef HAL_RTC_H
#define HAL_RTC_H

#include "hal_bits.h"
#include "hal_err.h"

#include "utils/utils.h"

#include "stm32wbxx.h"

typedef RTC_TypeDef rtc_t;

typedef enum {
    RTC_HOUR_FORMAT_24 = 0U,
    RTC_HOUR_FORMAT_12 = 1U,
} rtc_hour_format;

typedef enum {
    RTC_OUTPUT_DISABLE = 0U,
    RTC_OUTPUT_ALARMA = 1U,
    RTC_OUTPUT_ALARMB = 2U,
    RTC_OUTPUT_WAKEUP = 3U,
} rtc_output;

typedef enum {
    RTC_OUTPUT_REMAP_DISABLE = 0U,
    RTC_OUTPUT_REMAP_ENABLE = 1U,
} rtc_output_remap;

typedef enum {
    RTC_OUTPUT_POLARITY_HIGH = 0U,
    RTC_OUTPUT_POLARITY_LOW = 1U,
} rtc_output_polarity;

typedef enum {
    RTC_OUTPUT_TYPE_OPENDRAIN = 0U,
    RTC_OUTPUT_TYPE_PUSHPULL = 1U,
} rtc_output_type;

typedef enum {
    RTC_STATE_RESET = 0U,
    RTC_STATE_READY = 1U,
    RTC_STATE_BUSY = 2U,
    RTC_STATE_TIMEOUT = 3U,
    RTC_STATE_ERROR = 4U,
} rtc_state;

typedef struct {

    rtc_hour_format hour_format;

    uint8_t async_pre_divider : 7;

    uint16_t sync_pre_divider : 15;

    rtc_output output;

    rtc_output_remap output_remap;

    rtc_output_polarity output_polarity;

    rtc_output_type output_type;

} rtc_init_t;

typedef enum {
    RTC_UNLOCKED = 0U,
    RTC_LOCKED = 1U,
} rtc_lock;

typedef struct {

    rtc_t *instance;

    rtc_init_t init;

    rtc_lock lock;

    __IO rtc_state state;

} rtc_handle_t;

typedef enum {
    RTC_TIME_AM = 0U,
    RTC_TIME_PM = 1U,
} rtc_time_ampm;

typedef struct __PACKED {
    uint8_t hours : 5;
    rtc_time_ampm am_pm : 1;
    uint8_t minutes : 6;
    uint8_t seconds : 6;
    uint32_t sub_seconds : 32;
} rtc_time_t;

typedef enum {
    RTC_WEEK_DAY_MONDAY = 0U,
    RTC_WEEK_DAY_TUESDAY = 1U,
    RTC_WEEK_DAY_WEDNESDAY = 2U,
    RTC_WEEK_DAY_THURSDAY = 3U,
    RTC_WEEK_DAY_FRIDAY = 4U,
    RTC_WEEK_DAY_SATURDAY = 5U,
    RTC_WEEK_DAY_SUNDAY = 6U,
} rtc_week_day;

typedef enum {
    RTC_MONTH_JANUARY = 0U,
    RTC_MONTH_FEBRUARY = 1U,
    RTC_MONTH_MARCH = 2U,
    RTC_MONTH_APRIL = 3U,
    RTC_MONTH_MAY = 4U,
    RTC_MONTH_JUNE = 5U,
    RTC_MONTH_JULY = 6U,
    RTC_MONTH_AUGUST = 7U,
    RTC_MONTH_SEPTEMBER = 8U,
    RTC_MONTH_OCTOBER = 9U,
    RTC_MONTH_NOVEMBER = 10U,
    RTC_MONTH_DECEMBER = 11U,
} rtc_month;

typedef struct __PACKED {
    rtc_week_day week_day : 3;
    rtc_month month : 4;
    uint8_t day : 5;
    uint8_t year : 7;
} rtc_date_t;

typedef enum {
    RTC_ALARM_MODE_DATE = 0U,
    RTC_ALARM_MODE_WEEK_DAY = 1U,
} rtc_alarm_mode;

typedef enum {
    RTC_ALARM_TYPE_A = 0U,
    RTC_ALARM_TYPE_B = 1U,
} rtc_alarm_type;

typedef enum {
    RTC_ALARM_PRECISION_NO_DATE = 0U,
    RTC_ALARM_PRECISION_NO_HOURS = 1U,
    RTC_ALARM_PRECISION_NO_MINUTES = 2U,
    RTC_ALARM_PRECISION_NO_SECONDS = 3U,
} rtc_alarm_precision;

typedef enum {
    RTC_ALARM_SUB_SECONDS_PRECISION_NONE = 0U,
    RTC_ALARM_SUB_SECONDS_PRECISION_1 = 1U,
    RTC_ALARM_SUB_SECONDS_PRECISION_2 = 2U,
    RTC_ALARM_SUB_SECONDS_PRECISION_3 = 3U,
    RTC_ALARM_SUB_SECONDS_PRECISION_4 = 4U,
    RTC_ALARM_SUB_SECONDS_PRECISION_5 = 5U,
    RTC_ALARM_SUB_SECONDS_PRECISION_6 = 6U,
    RTC_ALARM_SUB_SECONDS_PRECISION_7 = 7U,
    RTC_ALARM_SUB_SECONDS_PRECISION_8 = 8U,
    RTC_ALARM_SUB_SECONDS_PRECISION_9 = 9U,
    RTC_ALARM_SUB_SECONDS_PRECISION_10 = 10U,
    RTC_ALARM_SUB_SECONDS_PRECISION_11 = 11U,
    RTC_ALARM_SUB_SECONDS_PRECISION_12 = 12U,
    RTC_ALARM_SUB_SECONDS_PRECISION_13 = 13U,
    RTC_ALARM_SUB_SECONDS_PRECISION_14 = 14U,
    RTC_ALARM_SUB_SECONDS_PRECISION_15 = 15U,
    RTC_ALARM_SUB_SECONDS_PRECISION_MAX = RTC_ALARM_SUB_SECONDS_PRECISION_15,
} rtc_alarm_sub_seconds_precision;

typedef struct {
    rtc_alarm_mode mode;
    rtc_time_t time;
    rtc_alarm_precision precision[4];
    rtc_alarm_sub_seconds_precision sub_seconds_precision;
    uint8_t date_week_day; // Depends on `mode`
    rtc_alarm_type type;
} rtc_alarm_t;

typedef enum {
    RTC_WAKEUP_CLOCK_SOURCE_DIV16 = 0U,
    RTC_WAKEUP_CLOCK_SOURCE_DIV8 = 1U,
    RTC_WAKEUP_CLOCK_SOURCE_DIV4 = 2U,
    RTC_WAKEUP_CLOCK_SOURCE_DIV2 = 3U,
    RTC_WAKEUP_CLOCK_SOURCE_CKSPRE16BIT = 4U,
    RTC_WAKEUP_CLOCK_SOURCE_CKSPRE17BIT = 6U,
} rtc_wakeup_clock_source;

#define RTC_TIMEOUT_VALUE 1000U

hal_err hal_rtc_init(rtc_handle_t *handle);

void hal_rtc_bypass_shadow_enable(rtc_handle_t *handle);

void hal_rtc_bypass_shadow_disable(rtc_handle_t *handle);

static inline void
hal_rtc_wakeup_clock_select_source(rtc_handle_t *handle,
                                   rtc_wakeup_clock_source source) {
    MODIFY_BITS(handle->instance->CR, RTC_CR_WUCKSEL_Pos, source, BITMASK_3BIT);
}

static inline rtc_wakeup_clock_source
hal_rtc_wakeup_clock_get_source(rtc_handle_t *handle) {
    return READ_BITS(handle->instance->CR, RTC_CR_WUCKSEL_Pos, BITMASK_3BIT);
}

static inline void hal_rtc_wakeup_timer_enable(rtc_handle_t *handle) {
    SET_BIT(handle->instance->CR, RTC_CR_WUTE);
}

static inline void hal_rtc_wakeup_timer_disable(rtc_handle_t *handle) {
    CLEAR_BIT(handle->instance->CR, RTC_CR_WUTE);
}

static inline bool hal_rtc_wakeup_timer_enabled(rtc_handle_t *handle) {
    return READ_BIT(handle->instance->CR, RTC_CR_WUTE);
}

typedef enum {
    RTC_FLAG_RECALPF = RTC_ISR_RECALPF,
    RTC_FLAG_TAMP3F = RTC_ISR_TAMP3F,
    RTC_FLAG_TAMP2F = RTC_ISR_TAMP2F,
    RTC_FLAG_TAMP1F = RTC_ISR_TAMP1F,
    RTC_FLAG_TSOVF = RTC_ISR_TSOVF,
    RTC_FLAG_TSF = RTC_ISR_TSF,
    RTC_FLAG_ITSF = RTC_ISR_ITSF,
    RTC_FLAG_WUTF = RTC_ISR_WUTF,
    RTC_FLAG_ALRBF = RTC_ISR_ALRBF,
    RTC_FLAG_ALRAF = RTC_ISR_ALRAF,
    RTC_FLAG_INITF = RTC_ISR_INITF,
    RTC_FLAG_RSF = RTC_ISR_RSF,
    RTC_FLAG_INITS = RTC_ISR_INITS,
    RTC_FLAG_SHPF = RTC_ISR_SHPF,
    RTC_FLAG_WUTWF = RTC_ISR_WUTWF,
    RTC_FLAG_ALRBWF = RTC_ISR_ALRBWF,
    RTC_FLAG_ALRAWF = RTC_ISR_ALRAWF,
} rtc_flag;

static inline void hal_rtc_wakeup_timer_clear_flag(rtc_handle_t *handle,
                                                   rtc_flag flag) {
    CLEAR_BIT(handle->instance->ISR, flag);
}

static inline bool hal_rtc_wakeup_timer_get_flag(rtc_handle_t *handle,
                                                 rtc_flag flag) {
    return READ_BIT(handle->instance->ISR, flag);
}

static inline void hal_rtc_wakeup_timer_enable_it(rtc_handle_t *handle) {
    SET_BIT(handle->instance->CR, RTC_CR_WUTIE);
}

static inline void hal_rtc_disable_write_protection(rtc_handle_t *handle) {
    WRITE_REG(handle->instance->WPR, 0xCAU);
    WRITE_REG(handle->instance->WPR, 0x53U);
}

static inline void hal_rtc_enable_write_protection(rtc_handle_t *handle) {
    WRITE_REG(handle->instance->WPR, 0xFFU);
}

#endif // HAL_RTC_H
