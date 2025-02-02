#include "usb/usb_descriptor.h"

#include "stm32wb55xx.h"
#include "utils/utils.h"

static usb_device_descriptors_t usb_device_descriptors = {
    usb_device_get_device_descriptor,           //
    usb_device_get_language_id_descriptor,      //
    usb_device_get_manufacturer_str_descriptor, //
    usb_device_get_product_str_descriptor,      //
    usb_device_get_language_id_descriptor,      //
    usb_device_get_serial_str_descriptor,       //
    usb_device_get_interface_str_descriptor     //
};

usb_device_descriptors_t *usb_device_get_descriptors() {
    return &usb_device_descriptors;
}

uint8_t *usb_device_get_device_descriptor(usb_device_speed speed,
                                          uint16_t *length) {
    UNUSED(speed);
    *length = USB_DEVICE_DESC_LENGTH;
    return usb_device_descriptor;
}

uint8_t *usb_device_get_language_id_descriptor(usb_device_speed speed,
                                               uint16_t *length) {
    UNUSED(speed);
    *length = USB_LANG_DESC_LENGTH;
    return usb_lang_id_descriptor;
}

uint8_t *usb_device_get_manufacturer_str_descriptor(usb_device_speed speed,
                                                    uint16_t *length) {
    UNUSED(speed);
    *length = usb_manufacturer_str_descriptor_length;
    return usb_manufacturer_str_descriptor;
}

uint8_t *usb_device_get_product_str_descriptor(usb_device_speed speed,
                                               uint16_t *length) {
    UNUSED(speed);
    *length = usb_product_str_descriptor_length;
    return usb_product_str_descriptor;
}

uint8_t *usb_device_get_serial_str_descriptor(usb_device_speed speed,
                                              uint16_t *length) {
    UNUSED(speed);
    *length = USB_SERIAL_DESC_LENGTH;
    return usb_serial_str_descriptor;
}

uint8_t *usb_device_get_config_str_descriptor(usb_device_speed speed,
                                              uint16_t *length) {
    UNUSED(speed);
    *length = usb_config_str_descriptor_length;
    return usb_config_str_descriptor;
}

uint8_t *usb_device_get_interface_str_descriptor(usb_device_speed speed,
                                                 uint16_t *length) {
    UNUSED(speed);
    *length = usb_interface_str_descriptor_length;
    return usb_interface_str_descriptor;
}
