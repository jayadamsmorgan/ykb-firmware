#include "fw_update_handler.h"

#include "hal/flash.h"
#include "logging.h"
#include "settings.h"
#include "stm32wb55xx.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static fw_update_source fw_source = FW_UPDATE_SOURCE_NONE;

static uint8_t firmware_update_buffer[MAX_FIRMWARE_UDPATE_SIZE] = {0};
static uint32_t firmware_update_buffer_size = 0U;
static bool firmware_update_ready = false;

void fw_update_cleanup() {
    LOG_TRACE("Cleaning up...");
    memset(firmware_update_buffer, 0, sizeof(firmware_update_buffer));
    firmware_update_buffer_size = 0U;
    firmware_update_ready = false;
    fw_source = FW_UPDATE_SOURCE_NONE;
}

void fw_update_ready() { firmware_update_ready = true; }

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

    if (firmware_update_buffer_size + length >= MAX_FIRMWARE_UDPATE_SIZE) {
        LOG_ERROR(
            "Failed to update firmware: exceeded MAX_FIRMWARE_UDPATE_SIZE.");
        fw_update_cleanup();
        return -2;
    }

    memcpy(&firmware_update_buffer[firmware_update_buffer_size], data, length);
    firmware_update_buffer_size += length;

    return OK;
}

fw_update_source fw_get_update_source() { return fw_source; }

void fw_update_handler() {

    if (!firmware_update_ready) {
        return;
    }

    LOG_INFO("Flashing new firmware of size %d from source %d, please DO NOT "
             "DISCONNECT...",
             firmware_update_buffer_size, fw_source);

    hal_err err;

    LOG_TRACE("Unlocking flash...");
    err = flash_unlock();
    if (err) {
        LOG_ERROR("Cannot unlock flash: %d", err);
        fw_update_cleanup();
        return;
    }
    LOG_DEBUG("Flash unlocked.");

    uint32_t end_page_index =
        flash_get_page(FLASH_BASE + firmware_update_buffer_size) + 1;
    uint32_t page_error;

    err = flash_erase(FLASH_PAGE_0, end_page_index, &page_error);
    if (err) {
        LOG_ERROR("Unable to erase flash: Error %d at page %d", err,
                  page_error);
        // We probably should let user know we fucked up...
        fw_update_cleanup();
        return;
    }

    size_t block_size = firmware_update_buffer_size / 8U;
    if (block_size % 8 != 0) {
        block_size++;
    }

    uint64_t data;

    LOG_TRACE("Flashing with block size of %d...", block_size);
    for (size_t i = 0; i < block_size; i++) {
        memcpy(&data, &firmware_update_buffer[i * 8], sizeof(uint64_t));
        uint32_t block = FLASH_BASE + (8 * i);
        err = flash_program(FLASH_TYPEPROGRAM_DOUBLEWORD, block, data);
        if (err) {
            LOG_ERROR("Unable to flash new firmware: Error %d");
            // We probably should let user know we fucked up...
            fw_update_cleanup();
            return;
        }
        LOG_TRACE("Flashed to block %d", block);
    }
    LOG_INFO("Successfully flashed new firmware.");

    // Locking and cleaning up is most likely redundant
    // But I would like to do it anyway...

    LOG_TRACE("Locking flash...");
    err = flash_lock();
    if (err) {
        LOG_ERROR("Cannot lock flash: %d", err);
        fw_update_cleanup();
        return;
    }
    LOG_DEBUG("Flash locked.");

    fw_update_cleanup();

    LOG_INFO("Restarting to the new firmware...");

    NVIC_SystemReset();
}
