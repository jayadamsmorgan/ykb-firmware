#include "settings.h"

#if defined(USB_ENABLED) && USB_ENABLED == 1

#include "usb.h"

#include "hal_periph_clock.h"

#include "logging.h"
#include "usb/usbd_core.h"
#include "usb/usbd_hid.h"

USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef HID_Desc;

hal_err setup_usb() {

    LOG_INFO("Setting up...");

    hal_err err;

    LOG_TRACE("Selecting USB clock source...");
    periph_clock_select_usb_source(PERIPHCLK_USB_PLLQ);

    LOG_TRACE("Starting init...");
    err = usb_device_init(&hUsbDeviceFS, &HID_Desc, DEVICE_FS);
    if (err) {
        LOG_CRITICAL("Unable to init: Error %d", err);
        return err;
    }
    LOG_TRACE("Init OK.");

    LOG_TRACE("Registering HID device...");
    err = usb_device_register_class(&hUsbDeviceFS, &USBD_HID);
    if (err) {
        LOG_CRITICAL("Unable to register HID device: Error %d", err);
        return err;
    }
    LOG_TRACE("HID device registered.");

    LOG_TRACE("Starting USB device...");
    err = usb_device_start(&hUsbDeviceFS);
    if (err) {
        LOG_CRITICAL("Unable to start USB device: Error %d", err);
        return err;
    }
    LOG_TRACE("USB device started.");

    LOG_INFO("Setup complete.");

    return OK;
}

#endif // USB_ENABLED
