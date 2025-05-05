#ifndef BOOT_CONFIG_H
#define BOOT_CONFIG_H

#include "hal_flash.h"
#include "memory_map.h"

#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#ifndef BOOT_CONFIG_STAGED_READY_FLAG
#define BOOT_CONFIG_STAGED_READY_FLAG "STAGED_FW_READY"
#endif // BOOT_CONFIG_STAGED_READY_FLAG

typedef struct {

    char ready_flag[sizeof(BOOT_CONFIG_STAGED_READY_FLAG)];
    size_t staged_fw_size;

} boot_config_t;

static inline hal_err boot_config_clear() {
    return flash_erase(BOOT_CONFIG_ADDRESS, 1, NULL);
}

static inline hal_err boot_config_set_staged_ready(size_t fw_size) {

    hal_err err;

    err = flash_unlock();
    if (err) {
        return err;
    }

    err = boot_config_clear();
    if (err) {
        flash_lock();
        return err;
    }

    boot_config_t config = {.ready_flag = BOOT_CONFIG_STAGED_READY_FLAG,
                            .staged_fw_size = fw_size};

    const size_t temp_size = sizeof(boot_config_t) / 8 + 1;
    uint64_t temp[temp_size];
    temp[temp_size - 1] = 0;
    memcpy(temp, &config, sizeof(boot_config_t));

    for (size_t i = 0; i < temp_size; i++) {
        err =
            flash_program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                          BOOT_CONFIG_ADDRESS + i * sizeof(uint64_t), temp[i]);
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

static inline bool boot_config_is_staged_ready(size_t *fw_size) {

    volatile boot_config_t *config =
        (volatile boot_config_t *)BOOT_CONFIG_ADDRESS;

    if (strncmp((char *)config->ready_flag, BOOT_CONFIG_STAGED_READY_FLAG,
                sizeof(BOOT_CONFIG_STAGED_READY_FLAG)) != 0) {
        return false;
    }

    if (fw_size) {
        *fw_size = config->staged_fw_size;
    }

    return true;
}

#endif // BOOT_CONFIG_H
