#include "class/hid/hid_device.h"
#include "device/usbd.h"
#include "tusb.h"
#include <stdint.h>

uint16_t tud_hid_get_report_cb(uint8_t, uint8_t, hid_report_type_t, uint8_t *,
                               uint16_t) {
    // TODO
    return 0;
}

void tud_hid_set_report_cb(uint8_t, uint8_t, hid_report_type_t, const uint8_t *,
                           uint16_t) {
    // TODO
}

const uint8_t *tud_hid_descriptor_report_cb(uint8_t) {
    // TODO
    return NULL;
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t) {
    // TODO
    return NULL;
}

const uint8_t *tud_descriptor_device_cb() {
    // TODO
    return NULL;
}

const uint16_t *tud_descriptor_string_cb(uint8_t, uint16_t) {
    // TODO
    return NULL;
}

uint32_t tusb_time_millis_api() {
    // TODO
    return 0;
}
