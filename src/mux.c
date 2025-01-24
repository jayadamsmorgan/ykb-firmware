#include "mux.h"
#include "hal/gpio.h"
#include <errno.h>

void mux_init() {
    gpio_set_mode(PIN_MUX_0, GPIO_MODE_OUTPUT);
    gpio_set_mode(PIN_MUX_1, GPIO_MODE_OUTPUT);
    gpio_set_mode(PIN_MUX_2, GPIO_MODE_OUTPUT);

    gpio_set_mode(PIN_MUX_CHAN_A, GPIO_MODE_OUTPUT);
    gpio_set_mode(PIN_MUX_CHAN_B, GPIO_MODE_OUTPUT);
    gpio_set_mode(PIN_MUX_CHAN_C, GPIO_MODE_OUTPUT);
    gpio_set_mode(PIN_MUX_CHAN_D, GPIO_MODE_OUTPUT);

    gpio_set_mode(PIN_MUX_INPUT, GPIO_MODE_ANALOG);

    // gpio_adc_set_sampling_time(PIN_MUX_INPUT, GPIO_ADC_SMP_2_5_CYCLES);
    gpio_adc_set_resolution(GPIO_ADC_RES_10BIT);
    gpio_adc_calibrate(GPIO_CALIB_INPUT_SINGLE_ENDED, true, NULL);
}

void mux_select(uint8_t multiplexor) {
    gpio_digital_write(PIN_MUX_0, HIGH);
    gpio_digital_write(PIN_MUX_1, HIGH);
    gpio_digital_write(PIN_MUX_2, HIGH);

    switch (multiplexor) {
    case 0:
        gpio_digital_write(PIN_MUX_0, LOW);
        break;
    case 1:
        gpio_digital_write(PIN_MUX_1, LOW);
        break;
    default:
        gpio_digital_write(PIN_MUX_2, LOW);
        break;
    }
}

void mux_select_chan(uint8_t channel) {
    gpio_digital_write(PIN_MUX_CHAN_A, channel & 0b1);
    gpio_digital_write(PIN_MUX_CHAN_B, (channel >> 1) & 0b1);
    gpio_digital_write(PIN_MUX_CHAN_C, (channel >> 2) & 0b1);
    gpio_digital_write(PIN_MUX_CHAN_D, (channel >> 3) & 0b1);
}

inline uint32_t mux_read() {
    uint32_t data;
    error_t err = gpio_analog_read(PIN_MUX_INPUT, &data);
    if (err) {
        return err;
    }
    return data;
}
