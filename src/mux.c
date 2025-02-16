#include "mux.h"
#include "hal/bits.h"
#include "hal/gpio.h"
#include "hal/hal_err.h"
#include <stdint.h>

hal_err mux_init(const mux_t *const mux) {

    if (mux->ctrls == NULL) {
        return ERR_MUX_INIT_CTRLS_NULL;
    }
    if (mux->ctrls_amount == 0) {
        return ERR_MUX_INIT_CTRLS_ZERO;
    }
    if (mux->channel_amount > mux->ctrls_amount * mux->ctrls_amount) {
        return ERR_MUX_INV_CHAN_AMNT;
    }

    for (uint8_t i = 0; i < mux->ctrls_amount; i++) {
        gpio_pin_t pin = mux->ctrls[i];
        gpio_turn_on_port(pin.gpio);
        gpio_set_mode(pin, GPIO_MODE_OUTPUT);
        gpio_set_speed(pin, GPIO_SPEED_HIGH);
    }

    return OK;
}

hal_err mux_select_channel(const mux_t *const mux, uint8_t channel) {

#ifdef DEBUG
    if (channel >= mux->channel_amount) {
        return ERR_MUX_SELECT_CHAN_INV;
    }
#endif // DEBUG

    for (uint8_t i = 0; i < mux->ctrls_amount; i++) {
        gpio_digital_write(mux->ctrls[i], (channel >> i) & BITMASK_1BIT);
    }

    return OK;
}
