#include "interface_handler.h"

#include "ykb_protocol.h"

#include "usb/usbd_def.h"
#include "usb/usbd_hid.h"

#include "fw_update_handler.h"
#include "keyboard.h"
#include "logging.h"
#include "settings.h"

#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#if defined(USB_ENABLED) && USB_ENABLED == 1
extern USBD_HandleTypeDef hUsbDeviceFS;
#endif // USB_ENABLED

static void interface_send_reply(communication_source source,
                                 ykb_protocol_t *packet, uint8_t *data,
                                 uint32_t data_length) {

    uint16_t size = data_length;
    if (data_length > YKB_PROTOCOL_DATA_LENGTH) {
        if ((packet->packet_number + 1) * YKB_PROTOCOL_DATA_LENGTH >
            data_length) {
            size = data_length % YKB_PROTOCOL_DATA_LENGTH;
        } else {
            size = YKB_PROTOCOL_DATA_LENGTH;
        }
    }

    uint8_t buff[sizeof(ykb_protocol_t) + 2];
    buff[0] = 1;

    memset(packet->data, 0, sizeof(packet->data));
    memcpy(packet->data,
           &data[packet->packet_number * YKB_PROTOCOL_DATA_LENGTH], size);

    packet->crc = ykb_crc16(packet->data, size);
    packet->packet_size = size;

    memcpy(&buff[1], packet, sizeof(ykb_protocol_t));

    switch (source) {
    case COMMUNICATION_SOURCE_USB:
#if defined(USB_ENABLED) && USB_ENABLED == 1
        USBD_HID_SendReport(&hUsbDeviceFS, VEND_HID_EPIN_ADDR, buff,
                            sizeof(buff));
#endif // USB_ENABLED
        break;
    case COMMUNICATION_SOURCE_BT:
#if defined(BLUETOOTH_ENABLED) && BLUETOOTH_ENABLED == 1
        // TODO
#endif // BLUETOOTH_ENABLED
        break;
    }
}

static void handle_get_settings(communication_source source,
                                ykb_protocol_t *packet) {

    LOG_DEBUG("New get settings request.");

    uint8_t buff[sizeof(kb_settings_t)];

    kb_get_settings(buff);

    // Send OK
    interface_send_reply(source, packet, buff, sizeof(buff));
}

static void handle_get_mappings(communication_source source,
                                ykb_protocol_t *packet) {

    LOG_DEBUG("New get mappings request.");

    uint8_t buff[KB_KEY_COUNT];

    kb_get_mappings(buff);

    // Send OK
    interface_send_reply(source, packet, buff, sizeof(buff));
}

static void handle_get_values(communication_source source,
                              ykb_protocol_t *packet) {

    LOG_DEBUG("New get values request, packet number: %d",
              packet->packet_number);

    kb_request_values(source, packet);
}

void interface_handle_get_values_response(communication_source source,
                                          ykb_protocol_t *packet,
                                          uint16_t *values) {

    LOG_DEBUG("Responding to get values request (packet number %d)...",
              packet->packet_number);

    interface_send_reply(source, packet, (uint8_t *)values,
                         sizeof(uint16_t) * KB_KEY_COUNT);
}

static void handle_get_thresholds(communication_source source,
                                  ykb_protocol_t *packet) {

    LOG_DEBUG("New get thresholds request, packet number: %d",
              packet->packet_number);

    uint8_t buff[sizeof(uint8_t) * KB_KEY_COUNT +
                 sizeof(uint16_t) * KB_KEY_COUNT * 2];

    kb_get_thresholds(buff);

    // Send OK
    interface_send_reply(source, packet, buff, sizeof(buff));
}

static void handle_set_settings(communication_source source,
                                ykb_protocol_t *packet) {

    LOG_DEBUG("New set settings request.");

    kb_settings_t new_settings = {0};
    memcpy(&new_settings, packet->data, sizeof(kb_settings_t));
    kb_set_settings(&new_settings);

    // Send OK
    interface_send_reply(source, packet, NULL, 0);
}

static void handle_set_mappings(communication_source source,
                                ykb_protocol_t *packet) {

    LOG_DEBUG("New set mappings request.");

    uint8_t mappings[KB_KEY_COUNT] = {0};
    memcpy(mappings, packet->data, sizeof(mappings));
    kb_set_mappings(mappings);

    // Send OK
    interface_send_reply(source, packet, NULL, 0);
}

static uint8_t thresholds_buffer[KB_KEY_COUNT * sizeof(uint16_t) * 2 +
                                 KB_KEY_COUNT * sizeof(uint8_t)];
static uint8_t thresholds_buffer_length = 0U;

static void thresholds_buffer_cleanup() {
    memset(thresholds_buffer, 0, sizeof(thresholds_buffer));
    thresholds_buffer_length = 0U;
}

static void handle_set_thresholds(communication_source source,
                                  ykb_protocol_t *packet) {

    LOG_DEBUG("New set thresholds request, packet number: %d",
              packet->packet_number);

    if (packet->packet_number == 0 && thresholds_buffer_length != 0) {
        LOG_DEBUG("Clearing old set thresholds try...");
        thresholds_buffer_cleanup();
    }

    if (thresholds_buffer_length + packet->packet_size >=
        YKB_PROTOCOL_DATA_LENGTH) {
        LOG_ERROR("Error setting thresholds: buffer overflow.");
        thresholds_buffer_cleanup();
        return;
    }

    memcpy(&thresholds_buffer[thresholds_buffer_length], packet->data,
           packet->packet_size);

    if (packet->packet_size < YKB_PROTOCOL_DATA_LENGTH) {
        kb_set_thresholds(thresholds_buffer);
        thresholds_buffer_cleanup();
    }

    interface_send_reply(source, packet, NULL, 0);
}

static void handle_firmware_update(communication_source source,
                                   ykb_protocol_t *packet) {
    LOG_DEBUG("New firmware update packet, packet number: %d",
              packet->packet_number);

    if (packet->packet_number == 0 &&
        fw_get_update_source() == FW_UPDATE_SOURCE_USB) {
        LOG_DEBUG("Clearing old firmware update try...");
        fw_update_cleanup();
    }

    hal_err err = fw_update_new_chunk(packet->data, packet->packet_size,
                                      FW_UPDATE_SOURCE_USB);
    if (err) {
        LOG_ERROR("FW update error: %d", err);
        return;
    }

    if (packet->packet_size < YKB_PROTOCOL_DATA_LENGTH) {
        fw_update_ready();
    }

    // Send OK
    interface_send_reply(source, packet, NULL, 0);
}

static void handle_bootloader_update(communication_source source,
                                     ykb_protocol_t *packet) {
    LOG_DEBUG("New bootloader update packet, packet number: %d",
              packet->packet_number);

    if (packet->packet_number == 0 &&
        bl_get_update_source() == FW_UPDATE_SOURCE_USB) {
        LOG_DEBUG("Clearing old bootloader update try...");
        bl_update_cleanup();
    }

    hal_err err = bl_update_new_chunk(packet->data, packet->packet_size,
                                      FW_UPDATE_SOURCE_USB);

    if (err) {
        LOG_ERROR("BL update error: %d", err);
        return;
    }

    if (packet->packet_size < YKB_PROTOCOL_DATA_LENGTH) {
        bl_update_ready();
    }

    // Send OK
    interface_send_reply(source, packet, NULL, 0);
}

typedef void (*fp)(communication_source source, ykb_protocol_t *packet);

static fp request_fp_map[9] = {
    handle_get_settings,      //
    handle_get_mappings,      //
    handle_get_values,        //
    handle_get_thresholds,    //
    handle_set_settings,      //
    handle_set_mappings,      //
    handle_set_thresholds,    //
    handle_firmware_update,   //
    handle_bootloader_update, //
};

void interface_handle_new_packet(communication_source source, uint8_t *packet,
                                 uint8_t packet_length) {

    ykb_protocol_t result;
    memcpy(&result, packet, packet_length);

    uint8_t version = result.request_and_version & 0x0F;
    uint8_t request = result.request_and_version & 0xF0;

    if (version != YKB_PROTOCOL_VERSION) {
        LOG_ERROR("VERSION MISMATCH ver %d req %d", version, request);
        return;
    }

    if (IS_YKB_GET_REQUEST(request) || IS_YKB_SET_REQUEST(request)) {
        request_fp_map[(request >> 4) - 1](source, &result);
    }
}
