#include "interface_handler.h"

#include "hal/systick.h"
#include "keyboard.h"
#include "logging.h"
#include "settings.h"
#include "usb/usbd_def.h"
#include "usb/usbd_hid.h"
#include "utils/utils.h"
#include "ykb_protocol.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

extern USBD_HandleTypeDef hUsbDeviceFS;

static void interface_send_reply(ykb_protocol_t *packet, uint8_t *data,
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

    LOG_INFO("Packet number: %d", packet->packet_number);
    for (uint8_t i = 0; i < size; i++) {
        printf("%d ", packet->data[i]);
    }
    printf("\r\n");

    packet->crc = ykb_crc16(packet->data, size);
    packet->packet_size = size;

    memcpy(&buff[1], packet, sizeof(ykb_protocol_t));

    USBD_HID_SendReport(&hUsbDeviceFS, VEND_HID_EPIN_ADDR, buff, sizeof(buff));
}

static void handle_get_settings(ykb_protocol_t *packet) {

    LOG_DEBUG("CORE: Packet handler: New get settings request.");

    uint8_t buff[sizeof(kb_settings_t)];

    kb_get_settings(buff);

    // Send OK
    interface_send_reply(packet, buff, sizeof(buff));
}

static void handle_get_mappings(ykb_protocol_t *packet) {

    LOG_DEBUG("CORE: Packet handler: New get mappings request.");

    uint8_t buff[KB_KEY_COUNT];

    kb_get_mappings(buff);

    // Send OK
    interface_send_reply(packet, buff, sizeof(buff));
}

static void handle_get_values(ykb_protocol_t *packet) {

    LOG_DEBUG("CORE: Packet handler: New get values request.");

    kb_request_values(packet);
}

void interface_handle_get_values_response(ykb_protocol_t *packet,
                                          uint16_t *values) {

    LOG_DEBUG("CORE: Packet handler: Responding to get values request...");

    interface_send_reply(packet, (uint8_t *)values,
                         sizeof(uint16_t) * KB_KEY_COUNT);
}

static void handle_get_thresholds(ykb_protocol_t *packet) {

    LOG_DEBUG("CORE: Packet handler: New get thresholds request.");

    uint8_t buff[sizeof(uint8_t) * KB_KEY_COUNT +
                 sizeof(uint16_t) * KB_KEY_COUNT * 2];

    kb_get_thresholds(buff);

    // Send OK
    interface_send_reply(packet, buff, sizeof(buff));
}

typedef void (*fp)(ykb_protocol_t *packet);

static fp request_fp_map[9] = {
    handle_get_settings,   //
    handle_get_mappings,   //
    handle_get_values,     //
    handle_get_thresholds, //
};

void interface_handle_new_packet(uint8_t *packet, uint8_t packet_length) {

    ykb_protocol_t result;
    memcpy(&result, packet, packet_length);

    uint8_t version = result.request_and_version & 0x0F;
    uint8_t request = result.request_and_version & 0xF0;

    if (version != YKB_PROTOCOL_VERSION) {
        LOG_ERROR("VERSION MISMATCH");
        return;
    }

    if (IS_YKB_GET_REQUEST(request)) {
        request_fp_map[(request >> 4) - 1](&result);
    }
}
