#include "class/hid/hid_device.h"
#include "device/usbd.h"
#include "tusb.h"
#include <stdint.h>

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
    // TODO
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, const uint8_t *buffer,
                           uint16_t bufsize) {
    // TODO
}

const uint8_t *tud_hid_descriptor_report_cb(uint8_t instance) {
    // TODO
    return NULL;
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
    // TODO
    return NULL;
}

const uint8_t *tud_descriptor_device_cb() {
    // TODO
    return NULL;
}

const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    // TODO
    return NULL;
}

uint32_t tusb_time_millis_api() {
    // TODO
    return 0;
}
