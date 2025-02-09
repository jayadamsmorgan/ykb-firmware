#include "boot0_handler.h"

#include "error_handler.h"
#include "hal/bits.h"
#include "hal/gpio.h"
#include "hal/hal_err.h"

static volatile gpio_pin_t sw = PH3;

hal_err setup_boot0_handler() {

    gpio_turn_on_port(sw.gpio);

    gpio_set_pupd(sw, GPIO_PULLDOWN);

    gpio_set_mode(sw, GPIO_MODE_INPUT);

    hal_err err = gpio_set_interrupt_rising(sw);
    if (err) {
        return err;
    }

    return gpio_enable_interrupt(sw);
}

void EXTI3_IRQHandler(void) {
    if (READ_BITS(EXTI->PR1, sw.num, BITMASK_1BIT)) {
        MODIFY_BITS(EXTI->PR1, sw.num, 1, BITMASK_1BIT);
        NVIC_SystemReset();
    }
}
