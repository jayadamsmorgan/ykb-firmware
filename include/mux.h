#ifndef MUX_H
#define MUX_H

#include "hal/gpio.h"
#include <stdint.h>

#define ERR_MUX_INIT_CTRLS_NULL -1001
#define ERR_MUX_INIT_CTRLS_ZERO -1002
#define ERR_MUX_SELECT_CHAN_INV -1003
#define ERR_MUX_INV_CHAN_AMNT -1004

typedef struct {
    const gpio_pin_t *ctrls;
    const uint8_t ctrls_amount;

    const uint8_t channel_amount;

    const gpio_pin_t common;
} mux_t;

hal_err mux_init(const mux_t *const mux);

hal_err mux_select_channel(const mux_t *const mux, uint8_t chan);

#endif // MUX_H
