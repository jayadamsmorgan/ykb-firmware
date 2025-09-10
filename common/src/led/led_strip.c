#include "led/led_strip.h"

#include "hal_dma.h"
#include "hal_tim.h"

hal_err led_strip_init(led_strip_t *ls) {
    return OK;
}

hal_err led_strip_set_static(led_strip_t *ls, color_t *color) {}

hal_err led_strip_set_i(led_strip_t *ls, color_t *color, size_t index) {}

hal_err led_strip_clear(led_strip_t *ls) {}
