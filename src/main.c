#include "clock.h"
#include "error_handler.h"
#include "hal/bits.h"
#include "hal/gpio.h"
#include "hal/hal.h"
#include "hal/systick.h"
#include "stm32wb55xx.h"
#include "usb.h"

static volatile bool isOn = LOW;

gpio_pin_t red = PB1;
gpio_pin_t sw = PA10;

int main(void) {

    system_init();

    setup_error_handler();

    ERR_H(setup_clock());

    ERR_H(systick_init());

    ERR_H(hal_init());

    gpio_turn_on_port(red.gpio);
    gpio_turn_on_port(sw.gpio);

    gpio_set_pupd(sw, GPIO_PULLUP);

    gpio_set_mode(red, GPIO_MODE_OUTPUT);
    gpio_set_mode(sw, GPIO_MODE_INPUT);

    ERR_H(gpio_set_interrupt_falling(sw));
    ERR_H(gpio_enable_interrupt(sw));

    while (true) {
    }

    return 0;
}

void EXTI15_10_IRQHandler(void) {
    if (READ_BITS(EXTI->PR1, sw.num, BITMASK_1BIT)) {
        MODIFY_BITS(EXTI->PR1, sw.num, 1, BITMASK_1BIT);

        gpio_digital_write(red, isOn);
        isOn = !isOn;
    }
}
