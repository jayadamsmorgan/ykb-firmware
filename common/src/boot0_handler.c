#include "settings.h"

#if defined(BOOT0_HANDLER_ENABLED) && BOOT0_HANDLER_ENABLED == 1

#include "boot0_handler.h"

#include "hal_gpio.h"

#include "logging.h"

static volatile gpio_pin_t sw = PH3;

hal_err setup_boot0_handler() {

    LOG_INFO("Setting up...");

    gpio_turn_on_port(sw.gpio);

    gpio_set_pupd(sw, GPIO_PULLDOWN);

    gpio_set_mode(sw, GPIO_MODE_INPUT);

    hal_err err;

    LOG_TRACE("Setting interrupt...");
    err = gpio_set_interrupt_rising(sw);
    if (err) {
        LOG_CRITICAL("Unable to set interrupt: Error %d", err);
        return err;
    }
    LOG_TRACE("Interrupt is set.");

    LOG_TRACE("Enabling interrupt...");
    err = gpio_enable_interrupt(sw);
    if (err) {
        LOG_CRITICAL("Unable to enable interrupt: Error %d", err);
        return err;
    }
    LOG_TRACE("Interrupt enabled.");

    LOG_INFO("Setup complete.");

    return OK;
}

void exti_handler_3() {
    LOG_INFO("Triggered. Going to DFU...");
    NVIC_SystemReset();
}

#endif // BOOT0_HANDLER_ENABLED
