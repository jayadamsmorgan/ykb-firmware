#ifndef BOOTLOADER

#include "settings.h"

#include "checks.h"

#include "logging.h"
#include "version.h"

#include "hal.h"

#include "adc.h"
#include "boot0_handler.h"
#include "clock.h"
#include "crs.h"
#include "eeprom.h"
#include "error_handler.h"
#include "fw_update_handler.h"
#include "hsem.h"
#include "ipcc.h"
#include "keyboard.h"
#include "rtc.h"
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
#if defined(BOOT0_HANDLER_ENABLED) && BOOT0_HANDLER_ENABLED == 1
    ERR_H(setup_boot0_handler());
#endif // BOOT0_HANDLER_ENABLED
    ERR_H(eeprom_init());
    ERR_H(setup_fw_update_handler());
    // ERR_H(setup_ipcc());
    // ERR_H(setup_rtc());
    // ERR_H(hsem_init());

    // Main
    ERR_H(setup_adc());
    ERR_H(kb_init());
#if defined(USB_ENABLED) && USB_ENABLED == 1
    ERR_H(setup_usb());
#endif // USB_ENABLED

    LOG_INFO("Successfully booted.");

    while (true) { // Main loop
        kb_handle();
        fw_update_handler();
    }
}

#endif // !BOOTLOADER
