#ifndef HAL_TIM_H
#define HAL_TIM_H

#include <stdint.h>

typedef enum {
    TIM_COUNTER_MODE_UP = 0U,
    TIM_COUNTER_MODE_DOWN = 1U,
    TIM_COUNTER_MODE_CENTER1 = 2U,
    TIM_COUNTER_MODE_CENTER2 = 3U,
    TIM_COUNTER_MODE_CENTER3 = 4U,
} tim_counter_mode;

typedef enum {
    TIM_CLOCK_DIVISION_DIV1 = 0U,
    TIM_CLOCK_DIVISION_DIV2 = 1U,
    TIM_CLOCK_DIVISION_DIV4 = 2U,
} tim_clock_division;

typedef enum {
    TIM_OC_MODE_TIMING = 0U,
    TIM_OC_MODE_ACTIVE = 1U,
    TIM_OC_MODE_INACTIVE = 2U,
    TIM_OC_MODE_TOGGLE = 3U,
    TIM_OC_MODE_PWM1 = 4U,
    TIM_OC_MODE_PWM2 = 5U,
    TIM_OC_MODE_FORCED_ACTIVE = 6U,
    TIM_OC_MODE_FORCED_INACTIVE = 7U,
    TIM_OC_MODE_RETRIGGERABLE_OPM1 = 8U,
    TIM_OC_MODE_RETRIGGERABLE_OPM2 = 9U,
    TIM_OC_MODE_COMBINED_PWM1 = 10U,
    TIM_OC_MODE_COMBINED_PWM2 = 11U,
    TIM_OC_MODE_ASYMMETRIC_PWM1 = 12U,
    TIM_OC_MODE_ASYMMETRIC_PWM2 = 13U,
} tim_oc_mode;

typedef enum {
    TIM_OC_POLARITY_HIGH = 0U,
    TIM_OC_POLARITY_LOW = 1U,
} tim_oc_polarity;

typedef enum {
    TIM_OC_IDLE_STATE_RESET = 0U,
    TIM_OC_IDLE_STATE_SET = 1U,
} tim_oc_idle_state;

typedef enum {
    TIM_IC_POLARITY_RISING = 0U,
    TIM_IC_POLARITY_FALLING = 1U,
    TIM_IC_POLARITY_BOTHEDGE = 2U,
} tim_ic_polarity;

typedef enum {
    TIM_IC_SELECTION_DIRECTTI = 0U,
    TIM_IC_SELECTION_INDIRECTTI = 1U,
    TIM_IC_SELECTION_TRC = 2U,
} tim_ic_selection;

typedef struct {

    uint16_t prescaler;

    uint16_t counter;

    tim_counter_mode counter_mode;

    uint16_t period;

    tim_clock_division clock_division;

    uint16_t repetition_counter;

    bool auto_reload_preload_enable;

} tim_init_t;

typedef struct {

    tim_oc_mode mode;

    uint16_t pulse;

    tim_oc_polarity polarity;

    tim_oc_polarity n_polarity;

    bool fast_mode;

    tim_oc_idle_state idle_state;

    tim_oc_idle_state n_idle_state;

} tim_oc_init_t;

typedef struct {

    tim_oc_mode mode;

    uint16_t pulse;

    tim_oc_polarity polarity;

    tim_oc_polarity n_polarity;

    tim_oc_idle_state idle_state;

    tim_oc_idle_state n_idle_state;

    tim_ic_polarity ic_polarity;

    tim_ic_selection ic_selection;

} tim_op_init_t;

#endif // HAL_TIM_H
