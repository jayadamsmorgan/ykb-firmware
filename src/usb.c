#include "usb.h"

#include "crs.h"
#include "usb/usb_hid.h"
#include "utils/utils.h"

#define USBD_VID 0x483
#define USBD_LANGID_STRING 1033
#define USBD_MANUFACTURER_STRING "STMicroelectronics"
#define USBD_PID 0x5710
#define USBD_PRODUCT_STRING "STM32 Human interface"
#define USBD_CONFIGURATION_STRING "HID Config"
#define USBD_INTERFACE_STRING "HID Interface"

hal_err usb_init() {

    hal_err err;

    err = crs_setup();
    if (err) {
        return err;
    }

    usb_hid_config_t hid_config = {
        .type = USB_DESC_TYPE_DEVICE,

        .vendor_id = USBD_VID,
        .product_id = USBD_PID,
        .lang_id = USBD_LANGID_STRING,

        .manufacturer_str = USBD_MANUFACTURER_STRING,
        .product_str = USBD_PRODUCT_STRING,
        .config_str = USBD_CONFIGURATION_STRING,
        .interface_str = USBD_INTERFACE_STRING,
    };

    usb_hid_init(hid_config);

    return OK;
}
