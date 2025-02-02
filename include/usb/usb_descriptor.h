#ifndef USB_DESCRIPTOR_H
#define USB_DESCRIPTOR_H

#include "usb/usb_hid.h"
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

uint8_t usb_device_descriptor[USB_DEVICE_DESC_LENGTH] ALIGNED = {
    0x12, /*bLength */
    0x00, /*bDescriptorType*/
    0x00, /*bcdUSB */
    0x02, /* ? */
    0x00, /*bDeviceClass*/
    0x00, /*bDeviceSubClass*/
    0x00, /*bDeviceProtocol*/
    0x40, /*bMaxPacketSize*/
    0x00, /*idVendor*/
    0x00, /*idVendor*/
    0x00, /*idProduct*/
    0x00, /*idProduct*/
    0x00, /*bcdDevice rel. 2.00*/
    0x02, /* ? */
    0x01, /*Index of manufacturer  string*/
    0x02, /*Index of product string*/
    0x03, /*Index of serial number string*/
    0x01  /*bNumConfigurations*/
};

uint8_t usb_lang_id_descriptor[USB_LANG_DESC_LENGTH] ALIGNED = {
    0x04, /* Language ID String Desc */
    0x03, /* USB Desc Type String */
    0x00, /* USB Lang ID LOW */
    0x00  /* USB Lang ID HIGH */
};

uint8_t usb_manufacturer_str_descriptor[USB_MANUFACTURER_DESC_LENGTH] ALIGNED;
uint16_t usb_manufacturer_str_descriptor_length;

uint8_t usb_product_str_descriptor[USB_PRODUCT_DESC_LENGTH] ALIGNED;
uint16_t usb_product_str_descriptor_length;

uint8_t usb_serial_str_descriptor[USB_SERIAL_DESC_LENGTH] ALIGNED;
uint16_t usb_serial_str_descriptor_length;

uint8_t usb_config_str_descriptor[USB_CONFIG_DESC_LENGTH] ALIGNED;
uint16_t usb_config_str_descriptor_length;

uint8_t usb_interface_str_descriptor[USB_INTERFACE_DESC_LENGTH] ALIGNED;
uint16_t usb_interface_str_descriptor_length;

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

#endif // USB_DESCRIPTOR_H
