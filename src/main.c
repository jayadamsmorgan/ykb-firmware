#include "adc.h"
#include "boot0_handler.h"
#include "clock.h"
#include "crs.h"
#include "error_handler.h"
#include "hal/adc.h"
#include "hal/gpio.h"
#include "hal/hal.h"
#include "hal/systick.h"
#include "keyboard.h"
#include "logging.h"
#include "test_button_handler.h"
#include "usb.h"
#include "version.h"
#include <stdint.h>

int main(void) {

    // Base
    LOG_INFO("Start booting...");
    system_init();
    LOG_TRACE("CORE: System init OK.");
    setup_error_handler();
    ERR_H(setup_clock());
    ERR_H(setup_crs());
    ERR_H(hal_init());
    LOG_TRACE("CORE: HAL init OK.");
    ERR_H(setup_logging());

    // Misc
    ERR_H(setup_boot0_handler());
    ERR_H(setup_test_button_handler());

    // Main
    ERR_H(kb_init());
    ERR_H(setup_adc());
    ERR_H(setup_usb());

    LOG_INFO("MAIN: Successfully booted.");

    while (true) { // Main loop
        kb_handle(true);
    }
}
