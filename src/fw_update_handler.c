#include "fw_update_handler.h"

#include "boot_config.h"
#include "hal/flash.h"
#include "logging.h"
#include "memory_map.h"
#include "stm32wb55xx.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifndef MAX_FIRMWARE_UDPATE_SIZE
#define MAX_FIRMWARE_UDPATE_SIZE 50 * 1024
#endif // MAX_FIRMWARE_UDPATE_SIZE

#define MAX_BL_FIRMWARE_UPDATE_SIZE (BOOT_CONFIG_ADDRESS - FLASH_BASE)

#define FW_STAGING_PAGE_SIZE                                                   \
    ((SECURE_FLASH_ADDRESS - FW_STAGING_ADDRESS) / FLASH_PAGE_SIZE + 1)

static uint8_t fw_update_buffer[MAX_FIRMWARE_UDPATE_SIZE];
static fw_update_source fw_source = FW_UPDATE_SOURCE_NONE;
static bool firmware_update_ready = false;
static size_t fw_update_size = 0U;

static uint8_t bl_update_buffer[MAX_BL_FIRMWARE_UPDATE_SIZE];
static fw_update_source bl_source = FW_UPDATE_SOURCE_NONE;
static bool bootloader_update_ready = false;
static size_t bl_update_size = 0U;

static inline hal_err erase_staging() {
    return flash_erase(FW_STAGING_ADDRESS, FW_STAGING_PAGE_SIZE, NULL);
}

static inline void erase_staging_and_boot_config() {

    hal_err err;

    err = erase_staging();
    if (err) {
        LOG_ERROR("Unable to erase staging: Error %d", err);
    }

    err = boot_config_clear();
    if (err) {
        LOG_ERROR("Unable to clear boot config: Error %d", err);
    }
}

hal_err setup_fw_update_handler() {
    LOG_INFO("Setting up...");

    if (boot_config_is_staged_ready(NULL)) {
        LOG_DEBUG("Staging is not empty, emptying...");
        erase_staging_and_boot_config();
    }

    LOG_INFO("Setup complete.");

    return OK;
}

void fw_update_cleanup() {
    LOG_TRACE("Cleaning up...");

    erase_staging_and_boot_config();

    fw_source = FW_UPDATE_SOURCE_NONE;
    firmware_update_ready = false;
    fw_update_size = 0;

    LOG_TRACE("Cleaning up complete.");
}

void bl_update_cleanup() {
    LOG_TRACE("Cleaning up bootloader update...");

    bl_source = FW_UPDATE_SOURCE_NONE;
    bootloader_update_ready = false;
    bl_update_size = 0;

    LOG_TRACE("Cleaning up bootloader update complete.");
}

void fw_update_ready() { firmware_update_ready = true; }

void bl_update_ready() { bootloader_update_ready = true; }

hal_err fw_update_new_chunk(uint8_t *data, size_t length,
                            fw_update_source source) {
    if (fw_source != FW_UPDATE_SOURCE_NONE && source != fw_source) {
        LOG_ERROR(
            "Trying to update firmware while another update is in progress");
        return -1;
    }

    if (fw_source == FW_UPDATE_SOURCE_NONE) {
        fw_source = source;
    }

    if (fw_update_size + length >= MAX_FIRMWARE_UDPATE_SIZE) {
        LOG_ERROR(
            "Failed to update firmware: exceeded MAX_FIRMWARE_UDPATE_SIZE.");
        fw_update_cleanup();
        return -2;
    }

    memcpy(&fw_update_buffer[fw_update_size], data, length);
    fw_update_size += length;

    return OK;
}

hal_err bl_update_new_chunk(uint8_t *data, size_t length,
                            fw_update_source source) {
    if (bl_source != FW_UPDATE_SOURCE_NONE && source != bl_source) {
        LOG_ERROR(
            "Trying to update firmware while another update is in progress");
        return -1;
    }

    if (bl_source == FW_UPDATE_SOURCE_NONE) {
        bl_source = source;
    }

    if (bl_update_size + length >= MAX_BL_FIRMWARE_UPDATE_SIZE) {
        LOG_ERROR("Failed to update bootloader: exceeded "
                  "MAX_BL_FIRMWARE_UDPATE_SIZE.");
        bl_update_cleanup();
        return -2;
    }

    memcpy(&bl_update_buffer[bl_update_size], data, length);
    bl_update_size += length;

    return OK;
}

fw_update_source fw_get_update_source() { return fw_source; }

fw_update_source bl_get_update_source() { return bl_source; }

static inline void fw_update() {
    LOG_INFO("Putting firmware update in staging...", fw_update_size,
             fw_source);

    hal_err err;

    LOG_TRACE("Unlocking flash...");
    err = flash_unlock();
    if (err) {
        LOG_ERROR("Unable to unlock flash: Error %d", err);
        fw_update_cleanup();
        return;
    }
    LOG_DEBUG("Flash unlocked.");

    LOG_TRACE("Erasing staging...");
    err = erase_staging();
    if (err) {
        LOG_ERROR("Unable to erase staging: Error %d", err);
    }
    LOG_DEBUG("Staging erase complete.");

    size_t block_size = fw_update_size / 8U;
    if (block_size % 8 != 0) {
        block_size++;
    }

    uint64_t data;

    LOG_TRACE("Start flashing staging (%d blocks)...", block_size);
    for (size_t i = 0; i < block_size; i++) {
        memcpy(&data, &fw_update_buffer[i * 8], sizeof(uint64_t));
        uint32_t block = FW_STAGING_ADDRESS + (sizeof(uint64_t) * i);
        err = flash_program(FLASH_TYPEPROGRAM_DOUBLEWORD, block, data);
        if (err) {
            LOG_ERROR("Unable to write block %d to staging: Error %d", block,
                      err);
            fw_update_cleanup();
            return;
        }
    }
    LOG_TRACE("Staging flashed successfully.");

    LOG_TRACE("Setting boot config...");
    err = boot_config_set_staged_ready(fw_update_size);
    if (err) {
        LOG_ERROR("Unable to set boot config: Error %d", err);
        fw_update_cleanup();
        return;
    }
    LOG_TRACE("Boot config is set.");

    LOG_TRACE("Locking the flash...");
    err = flash_lock();
    if (err) {
        LOG_ERROR("Unable to lock the flash: Error %d", err);
        fw_update_cleanup();
        return;
    }
    LOG_TRACE("Flash locked.");

    LOG_INFO("Rebooting to bootloader...");

    NVIC_SystemReset();
}

static inline void bl_update() {
    LOG_INFO("Updating bootloader...");

    hal_err err;

    LOG_TRACE("Unlocking flash...");
    err = flash_unlock();
    if (err) {
        LOG_ERROR("Unable to unlock flash: Error %d", err);
        bl_update_cleanup();
        return;
    }

    LOG_TRACE("Erasing old bootloader...");
    uint32_t page_amount = bl_update_size / FLASH_PAGE_SIZE + 1;
    err = flash_erase(FLASH_BASE, page_amount, NULL);
    if (err) {
        LOG_ERROR("Unable tp erase old bootloader: Error %d", err);
        bl_update_cleanup();
        return;
    }

    size_t block_size = bl_update_size / 8U;
    if (block_size % 8 != 0) {
        block_size++;
    }

    uint64_t data;

    LOG_TRACE("Flashing new bootloader (%d blocks)...", block_size);
    for (size_t i = 0; i < block_size; i++) {
        memcpy(&data, &bl_update_buffer[i * 8], sizeof(uint64_t));
        uint32_t block = FLASH_BASE + (sizeof(uint64_t) * i);
        err = flash_program(FLASH_TYPEPROGRAM_DOUBLEWORD, block, data);
        if (err) {
            LOG_ERROR("Unable to write block %d to bootloader: Error %d", block,
                      err);
            bl_update_cleanup();
            return;
        }
    }
    LOG_TRACE("Bootloader flashed successfully.");

    LOG_TRACE("Locking the flash...");
    err = flash_lock();
    if (err) {
        LOG_ERROR("Unable to lock the flash: Error %d", err);
        bl_update_cleanup();
        return;
    }
    LOG_TRACE("Flash locked.");

    LOG_INFO("Rebooting to bootloader...");

    NVIC_SystemReset();
}

void fw_update_handler() {

    if (bootloader_update_ready) {
        bl_update();
    }

    if (firmware_update_ready) {
        fw_update();
    }
}
