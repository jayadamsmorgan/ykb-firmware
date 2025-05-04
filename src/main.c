#include "logging.h"
#include "version.h"

#include "hal/hal.h"

#include "adc.h"
#include "boot0_handler.h"
#include "clock.h"
#include "crs.h"
#include "error_handler.h"
#include "fw_update_handler.h"
#include "keyboard.h"
#include "usb.h"

int main(void) {

    // Base
    LOG_INFO("Start booting YarmanKB Dactyl firmware "
             "version " YKB_FW_VERSION);
    setup_error_handler();
    ERR_H(setup_clock());
    ERR_H(setup_crs());
    ERR_H(hal_init());
    LOG_TRACE("HAL init OK.");
    ERR_H(setup_logging());

    // Misc
    ERR_H(setup_boot0_handler());
    ERR_H(setup_fw_update_handler());

    // Main
    ERR_H(setup_adc());
    ERR_H(kb_init());
    ERR_H(setup_usb());

    LOG_INFO("Successfully booted.");

    while (true) { // Main loop
        kb_handle();
        fw_update_handler();
    }
}
