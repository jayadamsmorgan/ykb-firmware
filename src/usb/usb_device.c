#include "usb/usb_device.h"

#define USBD_STATE_DEFAULT 0x01U

extern usb_device_descriptors_t usb_device_descriptors;
usb_device_handle_t usb_device_handle;

hal_err usb_device_init() {

    usb_device_handle_t *handle = &usb_device_handle;

    handle->p_class[0] = NULL;
    handle->p_user_data[0] = NULL;
    handle->p_conf_desc = NULL;

    handle->p_desc = &usb_device_descriptors;

    handle->dev_state = USBD_STATE_DEFAULT;
    handle->id = 0;

    // TODO: ll init

    return OK;
}
