#include "adc.h"
#include "boot0_handler.h"
#include "clock.h"
#include "crs.h"
#include "error_handler.h"
#include "hal/gpio.h"
#include "hal/hal.h"
#include "keyboard.h"
#include "logging.h"
#include "test_button_handler.h"
#include "usb.h"
#include "version.h"

int main(void) {

    // Base
    system_init();
    setup_error_handler();
    ERR_H(setup_clock());
    ERR_H(setup_logging());
    ERR_H(setup_crs());
    ERR_H(hal_init());

    // Misc
    ERR_H(setup_boot0_handler());
    ERR_H(setup_test_button_handler());
    ERR_H(setup_tempsensor());

    // Main
    ERR_H(kb_init());
    ERR_H(setup_adc());
    ERR_H(setup_usb());

    LOG_INFO("MAIN: Successfully booted.");

    while (true) { // Main loop
        kb_handle(true);
    }
}
