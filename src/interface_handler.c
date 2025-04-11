#include "interface_handler.h"

#include "keyboard.h"
#include "logging.h"
#include "settings.h"
#include "usb/usbd_def.h"
#include "usb/usbd_hid.h"
#include "utils/utils.h"
#include "ykb_protocol.h"

#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#ifndef MAX_INCOMING_DATA_LENGTH
// 40KB just to be able to handle firmware update requests...
#define MAX_INCOMING_DATA_LENGTH 40960
#endif // MAX_INCOMING_DATA_LENGTH

static ykb_protocol_t previous;
static ykb_protocol_t *previous_ptr;

static uint8_t incoming_data[MAX_INCOMING_DATA_LENGTH] = {0};
static uint32_t incoming_data_length = 0;

static inline void cleanup() {
    previous_ptr = NULL;
    if (incoming_data_length != 0) {
        memset(incoming_data, 0, incoming_data_length);
        incoming_data_length = 0;
    }
}

extern USBD_HandleTypeDef hUsbDeviceFS;

static inline void send_packet_chunk(ykb_protocol_t *chunk) {
    uint8_t out_buff[sizeof(ykb_protocol_t) + 1] = {0};
    out_buff[0] = 1;
    memcpy(&out_buff[1], chunk, sizeof(ykb_protocol_t));
    USBD_HID_SendReport(&hUsbDeviceFS, VEND_HID_EPIN_ADDR, out_buff,
                        sizeof(out_buff));
}

void interface_send_error(uint8_t request_error, uint8_t error_description) {
    if (!IS_YKB_REQUEST_ERROR(request_error)) {
        LOG_CRITICAL("CORE: Packet handler: Send error: Not a request: %d",
                     request_error);
        return;
    }
    ykb_protocol_t result[1] = {0};
    uint8_t data[1] = {error_description};
    ykb_protocol_pack(result, request_error, data, 1);
    send_packet_chunk(result);
}

void interface_send_reply(uint8_t request, uint8_t *data,
                          uint32_t data_length) {
    uint32_t pack_size = ykb_protocol_get_pack_size(data_length);
    ykb_protocol_t result[pack_size];
    ykb_protocol_pack(result, request, data, data_length);
    for (uint32_t i = 0; i < pack_size; i++) {
        send_packet_chunk(&result[i]);
    }
}

static void handle_get_settings(uint8_t *data, uint32_t data_length) {
    UNUSED(data);
    UNUSED(data_length);

    uint8_t buff[sizeof(kb_settings_t)];

    kb_get_settings(buff);

    // Send OK
    interface_send_reply(YKB_REQUEST_GET_SETTINGS, buff, sizeof(buff));
}

static void handle_get_mappings(uint8_t *data, uint32_t data_length) {
    UNUSED(data);
    UNUSED(data_length);

    uint8_t buff[sizeof(uint8_t) * KB_KEY_COUNT];

    kb_get_mappings(buff);

    // Send OK
    interface_send_reply(YKB_REQUEST_GET_MAPPINGS, buff, sizeof(buff));
}

static void handle_get_values(uint8_t *data, uint32_t data_length) {
    UNUSED(data);
    UNUSED(data_length);

    uint8_t buff[sizeof(uint16_t) * KB_KEY_COUNT];

    kb_get_values(buff);

    // Send OK
    interface_send_reply(YKB_REQUEST_GET_VALUES, buff, sizeof(buff));
}

static void handle_get_thresholds(uint8_t *data, uint32_t data_length) {
    UNUSED(data);
    UNUSED(data_length);

    uint8_t buff[sizeof(uint8_t) * KB_KEY_COUNT];

    kb_get_thresholds(buff);

    // Send OK
    interface_send_reply(YKB_REQUEST_GET_THRESHOLDS, buff, sizeof(buff));
}

static void handle_set_settings(uint8_t *data, uint32_t data_length) {
    /* Sanity check */
    if (!data || data_length != sizeof(kb_settings_t)) {
        interface_send_error(YKB_REQUEST_ERROR_UNKNOWN, 1);
        return;
    }
    /* Sanity check */

    kb_settings_t settings;
    memcpy(&settings, data, sizeof(kb_settings_t));
    kb_set_settings(&settings);

    // Send OK
    interface_send_reply(YKB_REQUEST_SET_SETTINGS, NULL, 0);
}

static void handle_set_mappings(uint8_t *data, uint32_t data_length) {
    /* Sanity check */
    if (!data || data_length != sizeof(uint8_t) * KB_KEY_COUNT) {
        interface_send_error(YKB_REQUEST_ERROR_UNKNOWN, 1);
        return;
    }
    for (uint8_t i = 0; i < data_length; i++) {
        if ((data[i] > KEY_NOKEY && data[i] < KEY_A) || data[i] > KEY_LAYER) {
            interface_send_error(YKB_REQUEST_ERROR_UNKNOWN, 1);
            return;
        }
    }
    /* Sanity check */

    kb_set_mappings(data);

    // Send OK
    interface_send_reply(YKB_REQUEST_SET_MAPPINGS, NULL, 0);
}

static void handle_set_thresholds(uint8_t *data, uint32_t data_length) {
    /* Sanity check */
    if (!data || data_length != sizeof(uint8_t) * KB_KEY_COUNT) {
        interface_send_error(YKB_REQUEST_ERROR_UNKNOWN, 1);
        return;
    }
    for (uint8_t i = 0; i < data_length; i++) {
        if (data[i] == 0 || data[i] > 100) {
            interface_send_error(YKB_REQUEST_ERROR_UNKNOWN, 1);
            return;
        }
    }
    /* Sanity check */

    kb_set_thresholds(data);

    // Send OK
    interface_send_reply(YKB_REQUEST_SET_THRESHOLDS, NULL, 0);
}

static void handle_firmware_update(uint8_t *data, uint32_t data_length) {
    // TODO
    UNUSED(data);
    UNUSED(data_length);
    interface_send_error(YKB_REQUEST_ERROR_UNKNOWN, 1);
}

static void handle_calibrate(uint8_t *data, uint32_t data_length) {
    /* Sanity check */
    // Data should be 2 arrays of minimal and maximal thresholds
    if (!data || data_length != sizeof(uint16_t) * KB_KEY_COUNT * 2) {
        interface_send_error(YKB_REQUEST_ERROR_UNKNOWN, 1);
        return;
    }
    /* Sanity check */

    uint16_t min_thresholds[KB_KEY_COUNT];
    uint16_t max_thresholds[KB_KEY_COUNT];
    kb_calibrate(min_thresholds, max_thresholds);

    // Send OK
    interface_send_reply(YKB_REQUEST_CALIBRATE, NULL, 0);
}

typedef void (*fp)(uint8_t *data, uint32_t data_length);

static fp request_fp_map[9] = {
    handle_get_settings,    //
    handle_get_mappings,    //
    handle_get_values,      //
    handle_get_thresholds,  //
    handle_set_settings,    //
    handle_set_mappings,    //
    handle_set_thresholds,  //
    handle_firmware_update, //
    handle_calibrate,       //
};

void interface_handle_new_packet(uint8_t *packet, uint8_t packet_length) {
    LOG_TRACE("CORE: Packet handler: received packet, parsing...");

    if (!packet || packet_length == 0) {
        LOG_CRITICAL("CORE: Packet handler: Packet is NULL or zero-length");
        interface_send_error(YKB_REQUEST_ERROR_UNKNOWN, 0);
        cleanup();
        return;
    }

    ykb_protocol_t next;
    int res = ykb_protocol_parse(previous_ptr, &next, packet);

    if (res < 0) {
        LOG_ERROR("CORE: Packet handler: Error parsing: %d", res);
        interface_send_error(YKB_REQUEST_ERROR_PARSING, abs(res));
        cleanup();
        return;
    }

    LOG_DEBUG("CORE: Packet handler: New packet parsed, packet number: %d",
              next.packet_number);
    if (incoming_data_length + next.packet_size) {
        LOG_ERROR("CORE: Packet handler: Out of memory...");
        interface_send_error(YKB_REQUEST_ERROR_OUT_OF_MEM, 0);
        cleanup();
        return;
    }

    memcpy(&incoming_data[incoming_data_length], next.data, next.packet_size);
    incoming_data_length += next.packet_size;

    previous = next;
    previous_ptr = &previous;

    if (res == 1) {
        uint8_t request = previous.request_and_version & 0xF0;
        if (!IS_YKB_GET_REQUEST(request) || !IS_YKB_SET_REQUEST(request)) {
            LOG_ERROR("CORE: Packet handler: Cannot process request %d",
                      request);
            return;
        }
        request_fp_map[(request >> 4)](incoming_data, incoming_data_length);
        cleanup();
        return;
    }

    return;
}
