#include "bl_version.h"
#include "boot_config.h"
#include "clock.h"
#include "config/memory_map.h"
#include "error_handler.h"
#include "hal/flash.h"
#include "logging.h"

#include <stdint.h>

typedef struct {
    uint32_t stack_address;
    void (*jump_ptr)(void);
} jump_t;

static void jump_to_app(uint32_t address) {

    LOG_INFO("Jumping to application at address %d", address);

    volatile jump_t *application = (volatile jump_t *)address;

    asm volatile("msr msp, %0; bx %1;"
                 :
                 : "r"(application->stack_address), "r"(application->jump_ptr));
}

hal_err flash_staging(size_t fw_size) {

    LOG_INFO("Firmware update ready, applying...");

    hal_err err;

    uint32_t page_amount = fw_size / FLASH_PAGE_SIZE + 1;

    if (APP_START_ADDRESS + page_amount * FLASH_PAGE_SIZE >=
        FW_STAGING_ADDRESS) {
        // Overflow will occur, cannot proceed...
        return -1;
    }

    if (FW_STAGING_ADDRESS + page_amount * FLASH_PAGE_SIZE >= FLASH_PAGE_231) {
        // Should not happen, but we should check anyways
        return -2;
    }

    // Start firmware update

    LOG_TRACE("Unlocking flash...");
    err = flash_unlock();
    if (err) {
        return err;
    }

    LOG_TRACE("Erasing old application...");
    err = flash_erase(APP_START_ADDRESS, page_amount, NULL);
    if (err) {
        flash_lock();
        return err;
    }

    LOG_TRACE("Flashing new application from staging...");
    size_t block_amount = fw_size / sizeof(uint64_t) + 1;
    size_t offset = 0;
    for (size_t i = 0; i < block_amount; i++) {
        uint64_t block = *(volatile uint64_t *)(FW_STAGING_ADDRESS + offset);
        err = flash_program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                            APP_START_ADDRESS + offset, block);
        if (err) {
            flash_lock();
            return err;
        }
        offset += sizeof(uint64_t);
    }

    LOG_TRACE("Erasing staging...");
    err = flash_erase(FW_STAGING_ADDRESS, page_amount, NULL);
    if (err) {
        flash_lock();
        return err;
    }

    LOG_TRACE("Clearing boot config...");
    err = boot_config_clear();
    if (err) {
        flash_lock();
        return err;
    }

    LOG_TRACE("Locking flash...");
    err = flash_lock();
    if (err) {
        return err;
    }

    LOG_INFO("Firmware update completed successfully.");

    return OK;
}

int main(void) {

    LOG_INFO("Start booting YarmanKB bootloader version " YKB_BL_FW_VERSION);
    setup_error_handler();
    ERR_H(setup_clock());
    ERR_H(setup_logging());

    LOG_INFO("Bootloader booted successfully.");

    size_t fw_size;
    if (boot_config_is_staged_ready(&fw_size)) {
        hal_err err = flash_staging(fw_size);
        if (err) {
            LOG_ERROR("Unable to flash staging: Error %d", err);
        }
    }

    jump_to_app(APP_START_ADDRESS);
}
