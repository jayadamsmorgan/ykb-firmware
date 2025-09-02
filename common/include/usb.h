#ifndef USB_H
#define USB_H

#include "hal_err.h"

#include "usb/usbd_def.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

hal_err setup_usb();

#endif // USB_H
