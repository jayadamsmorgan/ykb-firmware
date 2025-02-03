#include "usb/usb_device.h"

#include "hal/hal_err.h"
#include "hal/usb.h"
#include "stm32wb55xx.h"

extern usb_device_descriptors_t usb_device_descriptors;
usb_device_handle_t usb_device_handle;
extern usb_ll_handle_t usb_ll_handle;

hal_err usb_device_open_ep(usb_ll_handle_t *handle, uint8_t ep_addr,
                           uint8_t ep_type, uint16_t ep_mps) {
    usb_endpoint_t *ep;

    if ((ep_addr & 0x80U) == 0x80U) {
        ep = &handle->in_ep[ep_addr & EP_ADDR_MSK];
        ep->is_in = 1U;
    } else {
        ep = &handle->out_ep[ep_addr & EP_ADDR_MSK];
        ep->is_in = 0U;
    }

    ep->num = ep_addr & EP_ADDR_MSK;
    ep->maxpacket = (uint32_t)ep_mps & 0x7FFU;
    ep->type = ep_type;

    /* Set initial data PID. */
    if (ep_type == EP_TYPE_BULK) {
        ep->data_pid_start = 0U;
    }

    // __HAL_LOCK(handle); TODO
    return hal_usb_activate_endpoint(ep);
    // __HAL_UNLOCK(handle); TODO
}

void usb_device_close_ep(usb_ll_handle_t *handle, uint8_t ep_addr) {
    usb_endpoint_t *ep;

    if ((ep_addr & 0x80U) == 0x80U) {
        ep = &handle->in_ep[ep_addr & EP_ADDR_MSK];
        ep->is_in = 1U;
    } else {
        ep = &handle->out_ep[ep_addr & EP_ADDR_MSK];
        ep->is_in = 0U;
    }
    ep->num = ep_addr & EP_ADDR_MSK;

    // __HAL_LOCK(handle); TODO
    hal_usb_deactivate_endpoint(ep);
    // __HAL_UNLOCK(handle); TODO
}

void usb_device_transmit_ep(usb_ll_handle_t *handle, uint8_t ep_addr,
                            uint8_t *pBuf, uint32_t len) {
    usb_endpoint_t *ep;

    ep = &handle->in_ep[ep_addr & EP_ADDR_MSK];

    /*setup and start the Xfer */
    ep->xfer_buff = pBuf;
    ep->xfer_len = len;
    ep->xfer_fill_db = 1U;
    ep->xfer_len_db = len;
    ep->xfer_count = 0U;
    ep->is_in = 1U;
    ep->num = ep_addr & EP_ADDR_MSK;

    hal_usb_device_start_ep_xfer(ep);
}

void usb_device_ctrl_error(usb_device_handle_t *pdev) {
    hal_usb_stall_endpoint(pdev->p_data, 0x80U);
    hal_usb_stall_endpoint(pdev->p_data, 0U);
}

void usb_device_send_data(usb_device_handle_t *pdev, uint8_t *pbuf,
                          uint32_t len) {
    /* Set EP0 State */
    pdev->ep0_state = USBD_EP0_DATA_IN;
    pdev->ep_in[0].total_length = len;

    pdev->ep_in[0].rem_length = len;

    /* Start the transfer */
    usb_device_transmit_ep(pdev->p_data, 0x00U, pbuf, len);

    return;
}

hal_err usb_device_register_class(usb_device_handle_t *pdev,
                                  usb_device_class_t *pclass) {
    uint16_t len = 0U;

    if (!pdev) {
        return ERR_USBD_REG_CLASS_PDEV_NULL;
    }

    if (!pclass) {
        return ERR_USBD_REG_CLASS_PCLASS_NULL;
    }

    /* link the class to the USB Device handle */
    pdev->p_class[0] = pclass;

    /* Get Device Configuration Descriptor */
    if (pdev->p_class[pdev->class_id]->get_fs_config_descriptor) {
        pdev->p_conf_desc =
            (void *)pdev->p_class[pdev->class_id]->get_fs_config_descriptor(
                &len);
    }

    /* Increment the NumClasses */
    pdev->num_classes++;

    return OK;
}

hal_err usb_device_init() {

    usb_device_handle_t *handle = &usb_device_handle;

    handle->p_class[0] = NULL;
    handle->p_user_data[0] = NULL;
    handle->p_conf_desc = NULL;

    handle->p_desc = &usb_device_descriptors;

    handle->dev_state = USBD_STATE_DEFAULT;
    handle->id = 0;

    handle->p_data = &usb_ll_handle;

    hal_usb_enable_vdd();

    usb_ll_handle.instance = USB;
    usb_ll_handle.init.dev_endpoints = 8;
    usb_ll_handle.init.speed = 2;
    usb_ll_handle.init.phy_itface = 2;
    usb_ll_handle.init.sof_enable = false;
    usb_ll_handle.init.low_power_enable = false;
    usb_ll_handle.init.lpm_enable = false;
    usb_ll_handle.init.battery_charging_enable = false;

    hal_err err = hal_usb_init(&usb_ll_handle);
    if (err) {
        return err;
    }

    hal_usb_pma_config((usb_ll_handle_t *)handle->p_data, 0x00, PCD_SNG_BUF,
                       0x0C);
    hal_usb_pma_config((usb_ll_handle_t *)handle->p_data, 0x80, PCD_SNG_BUF,
                       0x4C);
    hal_usb_pma_config((usb_ll_handle_t *)handle->p_data, HID_EPIN_ADDR,
                       PCD_SNG_BUF, 0x8C);
    return OK;
}

void usb_device_start() {
    /* Start the low level driver  */
    hal_usb_start();
}
