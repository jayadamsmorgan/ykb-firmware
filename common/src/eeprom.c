#include "eeprom.h"

#include "hal_flash.h"

#include "memory_map.h"

#include <stdint.h>
#include <string.h>

// Returns EEPROM size in bytes
size_t eeprom_get_size() { return APP_START_ADDRESS - EEPROM_START_ADDRESS; }

hal_err eeprom_init() {

    if (EEPROM_START_ADDRESS >= APP_START_ADDRESS) {
        return ERR_EEPROM_INIT_BADPAGERANGE;
    }

    return OK;
}

// Get value from EEPROM
//
// start_page <= `address`< end_page
hal_err eeprom_get(uint32_t address, void *value, size_t size) {

    if (!value || size == 0U) {
        return ERR_EEPROM_GET_BADARGS;
    }

    if (address < EEPROM_START_ADDRESS ||
        address + size - 1U >= APP_START_ADDRESS) {
        return ERR_EEPROM_GET_OUTOFBOUNDS;
    }

    const size_t buff_size = (size / 4U) + 1U;

    uint32_t buffer[buff_size];

    for (size_t i = 0; i < buff_size; i++) {
        buffer[i] = *(volatile uint32_t *)address;
        address += 4U;
    }

    if (value) {
        memcpy(value, buffer, size);
    }

    return OK;
}

hal_err eeprom_clear() {

    hal_err err;

    err = flash_unlock();
    if (err) {
        return err;
    }

    err = flash_erase(EEPROM_START_ADDRESS, eeprom_get_size() / 4096U, NULL);
    if (err) {
        flash_lock();
        return err;
    }

    err = flash_lock();
    if (err) {
        return err;
    }

    return OK;
}

// Save value to EEPROM
//
// start_page <= `address` < end_page
hal_err eeprom_save(uint32_t address, void *value, size_t size) {

    if (!value || size == 0U) {
        return ERR_EEPROM_SAVE_BADARGS;
    }

    if (address < EEPROM_START_ADDRESS ||
        address + size - 1U >= APP_START_ADDRESS) {
        return ERR_EEPROM_SAVE_OUTOFBOUNDS;
    }

    size_t buff_size = size / 8U;
    if (size % 8 != 0) {
        buff_size++;
    }

    uint64_t buffer[buff_size];
    buffer[buff_size - 1] = 0U;

    memcpy(buffer, value, size);

    hal_err err;

    err = flash_unlock();
    if (err) {
        return err;
    }

    for (size_t i = 0; i < buff_size; i++) {
        err = flash_program(FLASH_TYPEPROGRAM_DOUBLEWORD, address + (8 * i),
                            buffer[i]);
        if (err) {
            flash_lock();
            return err;
        }
    }

    err = flash_lock();
    if (err) {
        return err;
    }

    return OK;
}
