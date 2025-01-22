#include "mux.h"

void mux_init() {
    // gpio_set_mode(PIN_MUX_0, GPIO_MODE_OUTPUT);
    // gpio_set_mode(PIN_MUX_1, GPIO_MODE_OUTPUT);
    // gpio_set_mode(PIN_MUX_2, GPIO_MODE_OUTPUT);
    //
    // gpio_set_mode(PIN_MUX_CHAN_A, GPIO_MODE_OUTPUT);
    // gpio_set_mode(PIN_MUX_CHAN_B, GPIO_MODE_OUTPUT);
    // gpio_set_mode(PIN_MUX_CHAN_C, GPIO_MODE_OUTPUT);
    // gpio_set_mode(PIN_MUX_CHAN_D, GPIO_MODE_OUTPUT);
    //
    // gpio_set_mode(PIN_MUX_INPUT, GPIO_MODE_ANALOG);
}

void mux_select(uint8_t multiplexor) {
    // gpio_write(PIN_MUX_0, HIGH);
    // gpio_write(PIN_MUX_1, HIGH);
    // gpio_write(PIN_MUX_2, HIGH);

    switch (multiplexor) {
    case 0:
        // gpio_write(PIN_MUX_0, LOW);
        break;
    case 1:
        // gpio_write(PIN_MUX_1, LOW);
        break;
    default:
        // gpio_write(PIN_MUX_2, LOW);
        break;
    }
}

void mux_select_chan(uint8_t channel) {
    // gpio_write(PIN_MUX_CHAN_A, channel & 0b1);
    // gpio_write(PIN_MUX_CHAN_B, (channel >> 1) & 0b1);
    // gpio_write(PIN_MUX_CHAN_C, (channel >> 2) & 0b1);
    // gpio_write(PIN_MUX_CHAN_D, (channel >> 3) & 0b1);
}

uint8_t mux_read() {
    // TODO: analog read impl in gpio.h
    return 0;
}
