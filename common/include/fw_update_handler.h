#ifndef FW_UPDATE_HANDLER_H
#define FW_UPDATE_HANDLER_H

#include "hal_err.h"

#include <stddef.h>
#include <stdint.h>

typedef enum {
    FW_UPDATE_SOURCE_NONE = 0U,
    FW_UPDATE_SOURCE_USB = 1U,
    FW_UPDATE_SOURCE_BT = 2U,
} fw_update_source;

hal_err setup_fw_update_handler();

void fw_update_cleanup();
void bl_update_cleanup();

hal_err fw_update_new_chunk(uint8_t *data, size_t length,
                            fw_update_source source);
hal_err bl_update_new_chunk(uint8_t *data, size_t length,
                            fw_update_source source);

void fw_update_ready();
void bl_update_ready();

fw_update_source fw_get_update_source();
fw_update_source bl_get_update_source();

void fw_update_handler();

#endif // FW_UPDATE_HANDLER_H
