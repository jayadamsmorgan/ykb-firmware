#ifndef USB_HID_H
#define USB_HID_H

#include "usb/usb_descriptor.h"
#include <stdint.h>

typedef enum {
    USB_DESC_TYPE_DEVICE = 0x01U,
    USB_DESC_TYPE_CONFIGURATION = 0x02U,
    USB_DESC_TYPE_STRING = 0x03U,
    USB_DESC_TYPE_INTERFACE = 0x04U,
    USB_DESC_TYPE_ENDPOINT = 0x05U,
    USB_DESC_TYPE_DEVICE_QUALIFIER = 0x06U,
    USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION = 0x07U,
    USB_DESC_TYPE_IAD = 0x0BU,
    USB_DESC_TYPE_BOS = 0x0FU,
} usb_desc_type;

typedef struct {
    usb_desc_type type;
    uint16_t vendor_id;
    uint16_t product_id;
    uint16_t lang_id;

    const char *manufacturer_str;
    const char *product_str;
    const char *config_str;
    const char *interface_str;

} usb_hid_config_t;

void usb_hid_init(usb_hid_config_t config);

#endif // USB_HID_H
