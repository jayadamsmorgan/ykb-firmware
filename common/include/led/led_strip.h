#ifndef LED_STRIP_H
#define LED_STRIP_H

#include "hal_err.h"

#include "hal_gpio.h"

#include "led/color.h"

#include <stddef.h>

typedef enum {
    LED_STRIP_FREQ_800KHZ = 0U,
    LED_STRIP_FREQ_400KHZ = 1U,
} led_strip_frequency;

typedef struct {
    size_t led_count;
    bool rgbw;
    gpio_pin_t *do_pin;
    led_strip_frequency frequency;
} led_strip_t;

hal_err led_strip_init(led_strip_t *ls);

hal_err led_strip_set_static(led_strip_t *ls, color_t *color);
hal_err led_strip_set_i(led_strip_t *ls, color_t *color, size_t index);

hal_err led_strip_clear(led_strip_t *ls);

#endif // LED_STRIP_H
