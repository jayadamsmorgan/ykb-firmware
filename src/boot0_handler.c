#include "boot0_handler.h"

#include "error_handler.h"
#include "hal/bits.h"
#include "hal/gpio.h"
#include "hal/hal_err.h"
#include "logging.h"

static volatile gpio_pin_t sw = PH3;

hal_err setup_boot0_handler() {

    LOG_INFO("BOOT0_HANDLER: Setting up...");

    gpio_turn_on_port(sw.gpio);

    gpio_set_pupd(sw, GPIO_PULLDOWN);

    gpio_set_mode(sw, GPIO_MODE_INPUT);

    hal_err err;

    LOG_TRACE("BOOT0_HANDLER: Setting interrupt...");
    err = gpio_set_interrupt_rising(sw);
    if (err) {
        LOG_CRITICAL("BOOT0_HANDLER: Unable to set interrupt: Error %d", err);
        return err;
    }
    LOG_TRACE("BOOT0_HANDLER: Interrupt is set.");

    LOG_TRACE("BOOT0_HANDLER: Enabling interrupt...");
    err = gpio_enable_interrupt(sw);
    if (err) {
        LOG_CRITICAL("BOOT0_HANDLER: Unable to enable interrupt: Error %d",
                     err);
        return err;
    }
    LOG_TRACE("BOOT0_HANDLER: Interrupt enabled.");

    LOG_INFO("BOOT0_HANDLER: Setup complete.");

    return OK;
}

void exti_handler_3() {
    LOG_INFO("BOOT0_HANDLER: Triggered. Going to DFU...");
    NVIC_SystemReset();
}
