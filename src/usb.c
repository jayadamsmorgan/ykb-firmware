#include "usb.h"

#include "logging.h"
#include "usb/usbd_hid.h"

USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef HID_Desc;

hal_err setup_usb() {

    LOG_INFO("USB: Setting up...");

    hal_err err;

    LOG_TRACE("USB: Starting init...");
    err = usb_device_init(&hUsbDeviceFS, &HID_Desc, DEVICE_FS);
    if (err) {
        LOG_CRITICAL("USB: Unable to init: Error %d", err);
        return err;
    }
    LOG_TRACE("USB: Init OK.");

    LOG_TRACE("USB: Registering HID device...");
    err = usb_device_register_class(&hUsbDeviceFS, &USBD_HID);
    if (err) {
        LOG_CRITICAL("USB: Unable to register HID device: Error %d", err);
        return err;
    }
    LOG_TRACE("USB: HID device registered.");

    LOG_TRACE("USB: Starting USB device...");
    err = usb_device_start(&hUsbDeviceFS);
    if (err) {
        LOG_CRITICAL("USB: Unable to start USB device: Error %d", err);
        return err;
    }
    LOG_TRACE("USB: USB device started.");

    LOG_INFO("USB: Setup complete.");

    return OK;
}
