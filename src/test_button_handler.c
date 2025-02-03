#include "test_button_handler.h"

#include "error_handler.h"
#include "hal/bits.h"
#include "hal/gpio.h"
#include "hal/hal_err.h"
#include "hal/systick.h"
#include "hal/usb.h"

static gpio_pin_t pin = PA10;

hal_err setup_test_button_handler() {
    gpio_turn_on_port(pin.gpio);

    gpio_set_pupd(pin, GPIO_PULLUP);

    gpio_set_mode(pin, GPIO_MODE_OUTPUT);

    hal_err err;

    err = gpio_set_interrupt_falling(pin);
    if (err) {
        return err;
    }

    err = gpio_enable_interrupt(pin);
    if (err) {
        return err;
    }

    return OK;
}

void EXTI15_10_IRQHandler(void) {
    if (READ_BITS(EXTI->PR1, pin.num, BITMASK_1BIT)) {
        MODIFY_BITS(EXTI->PR1, pin.num, 1, BITMASK_1BIT);

        // Wakeup
        hal_usb_activate_remote_wakeup();
        systick_delay(10);
        hal_usb_deactivate_remote_wakeup();
    }
}
