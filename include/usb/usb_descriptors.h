#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

#include "utils/utils.h"
#include <stdint.h>

#define USB_DEVICE_DESC_LENGTH 18
#define USB_LANG_DESC_LENGTH 4
#define USB_MANUFACTURER_DESC_LENGTH 64
#define USB_PRODUCT_DESC_LENGTH 64
#define USB_SERIAL_DESC_LENGTH 26
#define USB_CONFIG_DESC_LENGTH 64
#define USB_INTERFACE_DESC_LENGTH 64

typedef enum {
    USB_DEVICE_SPEED_HIGH = 0U,
    USB_DEVICE_SPEED_FULL = 1U,
    USB_DEVICE_SPEED_LOW = 2U,
} usb_device_speed;

uint8_t *usb_device_get_device_descriptor(usb_device_speed speed,
                                          uint16_t *length);

uint8_t *usb_device_get_language_id_descriptor(usb_device_speed speed,
                                               uint16_t *length);

uint8_t *usb_device_get_manufacturer_str_descriptor(usb_device_speed speed,
                                                    uint16_t *length);

uint8_t *usb_device_get_product_str_descriptor(usb_device_speed speed,
                                               uint16_t *length);

uint8_t *usb_device_get_serial_str_descriptor(usb_device_speed speed,
                                              uint16_t *length);

uint8_t *usb_device_get_config_str_descriptor(usb_device_speed speed,
                                              uint16_t *length);

uint8_t *usb_device_get_interface_str_descriptor(usb_device_speed speed,
                                                 uint16_t *length);

typedef struct {

    uint8_t *(*usb_device_get_device_descriptor)(usb_device_speed speed,
                                                 uint16_t *length);

    uint8_t *(*usb_device_get_language_id_str_descriptor)(
        usb_device_speed speed, uint16_t *length);

    uint8_t *(*usb_device_get_manufacturer_str_descriptor)(
        usb_device_speed speed, uint16_t *length);

    uint8_t *(*usb_device_get_product_str_descriptor)(usb_device_speed speed,
                                                      uint16_t *length);

    uint8_t *(*usb_device_get_serial_str_descriptor)(usb_device_speed speed,
                                                     uint16_t *length);

    uint8_t *(*usb_device_get_config_str_descriptor)(usb_device_speed speed,
                                                     uint16_t *length);

    uint8_t *(*usb_device_get_interface_str_descriptor)(usb_device_speed speed,
                                                        uint16_t *length);

} usb_device_descriptors_t;

#endif // USB_DESCRIPTORS_H
