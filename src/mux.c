#include "mux.h"

#include "hal/bits.h"

void mux_init(const mux_t *const mux) {
    for (uint8_t i = 0; i < 4; i++) {
        gpio_set_mode(mux->ctrl[i], GPIO_MODE_OUTPUT);
    }
    gpio_set_mode(mux->common, GPIO_MODE_ANALOG);
}

void mux_select_chan(const mux_t *const mux, uint8_t channel) {
    for (uint8_t i = 0; i < 4; i++) {
        gpio_digital_write(mux->ctrl[i], (channel >> i) & BITMASK_1BIT);
    }
}

inline int32_t mux_read(const mux_t *const mux) {
    uint32_t data;
    error_t err = gpio_analog_read(mux->common, &data);
    if (err) {
        return err;
    }
    return (uint16_t)data;
}
