#include "usb.h"

#include "hal/hal_err.h"
#include "main.h"
#include "stm32wbxx_hal_pcd.h"
#include "stm32wbxx_it.h"
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_hid.h"

extern USBD_HandleTypeDef hUsbDeviceFS;
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
