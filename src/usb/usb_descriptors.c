#include "usb/usb_descriptors.h"

#include "stm32wb55xx.h"
#include "usb/usb_hid.h"
#include "utils/utils.h"

uint8_t usb_device_desc[USB_DEVICE_DESC_LENGTH] ALIGNED = {
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

uint8_t usb_lang_id_desc[USB_LANG_DESC_LENGTH] ALIGNED = {
    0x04, /* Language ID String Desc */
    0x03, /* USB Desc Type String */
    0x00, /* USB Lang ID LOW */
    0x00  /* USB Lang ID HIGH */
};

uint8_t usb_manufacturer_str_desc[USB_MANUFACTURER_DESC_LENGTH] ALIGNED;
uint16_t usb_manufacturer_str_desc_length;

uint8_t usb_product_str_desc[USB_PRODUCT_DESC_LENGTH] ALIGNED;
uint16_t usb_product_str_desc_length;

uint8_t usb_serial_str_desc[USB_SERIAL_DESC_LENGTH] ALIGNED = {
    USB_SERIAL_DESC_LENGTH,
    USB_DESC_TYPE_STRING,
};

uint8_t usb_config_str_desc[USB_CONFIG_DESC_LENGTH] ALIGNED;
uint16_t usb_config_str_desc_length;

uint8_t usb_interface_str_desc[USB_INTERFACE_DESC_LENGTH] ALIGNED;
uint16_t usb_interface_str_desc_length;

usb_device_descriptors_t usb_device_descriptors = {
    usb_device_get_device_descriptor,           //
    usb_device_get_language_id_descriptor,      //
    usb_device_get_manufacturer_str_descriptor, //
    usb_device_get_product_str_descriptor,      //
    usb_device_get_language_id_descriptor,      //
    usb_device_get_serial_str_descriptor,       //
    usb_device_get_interface_str_descriptor     //
};

uint8_t *usb_device_get_device_descriptor(usb_device_speed speed,
                                          uint16_t *length) {
    UNUSED(speed);
    *length = USB_DEVICE_DESC_LENGTH;
    return usb_device_desc;
}

uint8_t *usb_device_get_language_id_descriptor(usb_device_speed speed,
                                               uint16_t *length) {
    UNUSED(speed);
    *length = USB_LANG_DESC_LENGTH;
    return usb_lang_id_desc;
}

uint8_t *usb_device_get_manufacturer_str_descriptor(usb_device_speed speed,
                                                    uint16_t *length) {
    UNUSED(speed);
    *length = usb_manufacturer_str_desc_length;
    return usb_manufacturer_str_desc;
}

uint8_t *usb_device_get_product_str_descriptor(usb_device_speed speed,
                                               uint16_t *length) {
    UNUSED(speed);
    *length = usb_product_str_desc_length;
    return usb_product_str_desc;
}

uint8_t *usb_device_get_serial_str_descriptor(usb_device_speed speed,
                                              uint16_t *length) {
    UNUSED(speed);
    *length = USB_SERIAL_DESC_LENGTH;
    return usb_serial_str_desc;
}

uint8_t *usb_device_get_config_str_descriptor(usb_device_speed speed,
                                              uint16_t *length) {
    UNUSED(speed);
    *length = usb_config_str_desc_length;
    return usb_config_str_desc;
}

uint8_t *usb_device_get_interface_str_descriptor(usb_device_speed speed,
                                                 uint16_t *length) {
    UNUSED(speed);
    *length = usb_interface_str_desc_length;
    return usb_interface_str_desc;
}
