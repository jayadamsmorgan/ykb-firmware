#include "error_handler.h"

#include "hal_systick.h"

#include "logging.h"
#include "pinout.h"
#include "settings.h"

static const gpio_pin_t led_dbg = PIN_LED_DBG1;

void setup_error_handler() {

    LOG_INFO("Setting up...");

    gpio_turn_on_port(led_dbg.gpio);
    gpio_set_mode(led_dbg, GPIO_MODE_OUTPUT);

    LOG_INFO("Setup complete.");
}

void error_handler(hal_err error_code) {

    if (error_code == OK)
        return;

    if (error_code > 0 || error_code > -100) {
        // Unknown error
#ifdef DEBUG
        gpio_digital_write(led_dbg, HIGH);
#endif // DEBUG
        while (true) {
        }
    }

#ifdef DEBUG
    int long_blinks = -error_code / 100;
    int short_blinks = -error_code % 100;
#endif // DEBUG

    while (true) {
#if DEBUG
        for (int i = 0; i < long_blinks; i++) {
            systick_delay(DBG_LONG_BLINK_DELAY);
            gpio_digital_write(led_dbg, HIGH);
            systick_delay(DBG_LONG_BLINK_DELAY);
            gpio_digital_write(led_dbg, LOW);
        }
        for (int i = 0; i < short_blinks; i++) {
            systick_delay(DBG_SHORT_BLINK_DELAY);
            gpio_digital_write(led_dbg, HIGH);
            systick_delay(DBG_SHORT_BLINK_DELAY);
            gpio_digital_write(led_dbg, LOW);
        }
#endif // DEBUG
    }
}

// void HardFault_Handler(void) { ERR_H(ERR_HARDFAULT); }
//
// void BusFault_Handler(void) { ERR_H(ERR_BUSFAULT); }
//
// void UsageFault_Handler(void) { ERR_H(ERR_USAGEFAULT); }
