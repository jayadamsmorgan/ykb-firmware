#include "usb.h"

#include "usb/usbd_hid.h"

USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef HID_Desc;

hal_err setup_usb() {

    hal_err err;

    err = usb_device_init(&hUsbDeviceFS, &HID_Desc, DEVICE_FS);
    if (err) {
        return err;
    }

    err = usb_device_register_class(&hUsbDeviceFS, &USBD_HID);
    if (err) {
        return err;
    }

    return usb_device_start(&hUsbDeviceFS);
}
