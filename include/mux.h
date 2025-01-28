#ifndef MUX_H
#define MUX_H

#include "hal/gpio.h"

typedef struct {
    gpio_pin_t ctrl[4];
    gpio_pin_t common;
} mux_t;

// Initialize multiplexors.
void mux_init(const mux_t *const mux);

// Select active channel on active multiplexor
void mux_select_chan(const mux_t *const mux, uint8_t channel);

// Read from active channel
int32_t mux_read(const mux_t *const mux);

#endif // MUX_H
