#include "error_handler.h"

#include "hal_systick.h"

#include "logging.h"
#include "pinout.h"
#include "settings.h"
#include "utils/utils.h"

#ifdef PIN_ERROR_HANDLER_LED
static const gpio_pin_t led_dbg = PIN_ERROR_HANDLER_LED;
#endif // PIN_ERROR_HANDLER_LED

void setup_error_handler() {

#ifdef PIN_ERROR_HANDLER_LED
    LOG_INFO("Setting up...");

    gpio_turn_on_port(led_dbg.gpio);
    gpio_set_mode(led_dbg, GPIO_MODE_OUTPUT);

    LOG_INFO("Setup complete.");
#endif // PIN_ERROR_HANDLER_LED
}

void error_handler(hal_err error_code) {
    if (error_code == OK)
        return;

#ifdef PIN_ERROR_HANDLER_LED
    if (error_code > 0 || error_code > -100) {
        // Unknown error
        gpio_digital_write(led_dbg, HIGH);
        while (true) {
        }
    }

    int long_blinks = -error_code / 100;
    int short_blinks = -error_code % 100;

    while (true) {
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
    }
#else  // !PIN_ERROR_HANDLER_LED
    UNUSED(error_code);
    while (true) {
    }
#endif // PIN_ERROR_HANDLER_LED
}
