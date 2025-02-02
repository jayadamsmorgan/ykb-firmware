#include "usb/usb_hid.h"

#include "stm32wbxx.h"
#include "usb/usb_descriptor.h"
#include "utils/utils.h"

#define DESCRIPTOR_TYPE_POS 1
#define VENDOR_LOW_POS 8
#define VENDOR_HIGH_POS 9
#define PRODUCT_LOW_POS 10
#define PRODUCT_HIGH_POS 11

#define LANG_ID_LOW_POS 2
#define LANG_ID_HIGH_POS 3

void usb_hid_init_serial_str() {
    // TODO: split this implementation with "hal/flash.h"
    //
    // This code is from STM32 code
    //
    // But I have no idea what UID_BASE is,
    // Cannot find this address in reference manual.
    // UID64 address is different there.
    uint32_t deviceserial0;
    uint32_t deviceserial1;
    uint32_t deviceserial2;

    deviceserial0 = *(uint32_t *)UID_BASE;
    deviceserial1 = *(uint32_t *)(UID_BASE + 0x4);
    deviceserial2 = *(uint32_t *)(UID_BASE + 0x8);

    deviceserial0 += deviceserial2;

    if (deviceserial0 != 0) {
        int_to_unicode(deviceserial0, &usb_serial_str_descriptor[2], 8);
        int_to_unicode(deviceserial1, &usb_serial_str_descriptor[18], 4);
    }
}

void usb_hid_init(usb_hid_config_t config) {

    usb_device_descriptor[DESCRIPTOR_TYPE_POS] = config.type;

    usb_device_descriptor[VENDOR_LOW_POS] = LOBYTE(config.vendor_id);
    usb_device_descriptor[VENDOR_HIGH_POS] = HIBYTE(config.vendor_id);

    usb_device_descriptor[PRODUCT_LOW_POS] = LOBYTE(config.vendor_id);
    usb_device_descriptor[PRODUCT_HIGH_POS] = HIBYTE(config.product_id);

    usb_lang_id_descriptor[LANG_ID_LOW_POS] = LOBYTE(config.lang_id);
    usb_lang_id_descriptor[LANG_ID_HIGH_POS] = HIBYTE(config.lang_id);

    ascii_to_unicode((uint8_t *)config.manufacturer_str,
                     usb_manufacturer_str_descriptor,
                     &usb_manufacturer_str_descriptor_length,
                     USB_DESC_TYPE_STRING, USB_MANUFACTURER_DESC_LENGTH);

    ascii_to_unicode((uint8_t *)config.product_str, usb_product_str_descriptor,
                     &usb_product_str_descriptor_length, USB_DESC_TYPE_STRING,
                     USB_PRODUCT_DESC_LENGTH);

    usb_hid_init_serial_str();

    ascii_to_unicode((uint8_t *)config.config_str, usb_config_str_descriptor,
                     &usb_config_str_descriptor_length, USB_DESC_TYPE_STRING,
                     USB_CONFIG_DESC_LENGTH);

    ascii_to_unicode((uint8_t *)config.interface_str,
                     usb_interface_str_descriptor,
                     &usb_interface_str_descriptor_length, USB_DESC_TYPE_STRING,
                     USB_INTERFACE_DESC_LENGTH);
}
