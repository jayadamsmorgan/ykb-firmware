#include "usb/usb_device.h"

#include "hal/hal_err.h"
#include "hal/usb.h"
#include "stm32wb55xx.h"

#define USBD_STATE_DEFAULT 0x01U
#define HID_EPIN_ADDR 0x81U

#define EP_TYPE_CTRL 0U
#define EP_TYPE_ISOC 1U
#define EP_TYPE_BULK 2U
#define EP_TYPE_INTR 3U
#define EP_TYPE_MSK 3U

extern usb_device_descriptors_t usb_device_descriptors;
usb_device_handle_t usb_device_handle;
extern usb_ll_handle_t usb_ll_handle;

hal_err usb_activate_endpoint(usb_endpoint_t *ep) {
    uint16_t wEpRegVal;

    wEpRegVal = PCD_GET_ENDPOINT(USB, ep->num) & USB_EP_T_MASK;

    /* initialize Endpoint */
    switch (ep->type) {
    case EP_TYPE_CTRL:
        wEpRegVal |= USB_EP_CONTROL;
        break;

    case EP_TYPE_BULK:
        wEpRegVal |= USB_EP_BULK;
        break;

    case EP_TYPE_INTR:
        wEpRegVal |= USB_EP_INTERRUPT;
        break;

    case EP_TYPE_ISOC:
        wEpRegVal |= USB_EP_ISOCHRONOUS;
        break;

    default:
        return ERR_USBD_ACT_END_INV_EPTYPE;
    }

    PCD_SET_ENDPOINT(USB, ep->num, (wEpRegVal | USB_EP_CTR_RX | USB_EP_CTR_TX));

    PCD_SET_EP_ADDRESS(USB, ep->num, ep->num);

    if (ep->doublebuffer == 0U) {
        if (ep->is_in != 0U) {
            /*Set the endpoint Transmit buffer address */
            PCD_SET_EP_TX_ADDRESS(USB, ep->num, ep->pmaadress);
            PCD_CLEAR_TX_DTOG(USB, ep->num);

            if (ep->type != EP_TYPE_ISOC) {
                /* Configure NAK status for the Endpoint */
                PCD_SET_EP_TX_STATUS(USB, ep->num, USB_EP_TX_NAK);
            } else {
                /* Configure TX Endpoint to disabled state */
                PCD_SET_EP_TX_STATUS(USB, ep->num, USB_EP_TX_DIS);
            }
        } else {
            /* Set the endpoint Receive buffer address */
            PCD_SET_EP_RX_ADDRESS(USB, ep->num, ep->pmaadress);

            /* Set the endpoint Receive buffer counter */
            PCD_SET_EP_RX_CNT(USB, ep->num, ep->maxpacket);
            PCD_CLEAR_RX_DTOG(USB, ep->num);

            if (ep->num == 0U) {
                /* Configure VALID status for EP0 */
                PCD_SET_EP_RX_STATUS(USB, ep->num, USB_EP_RX_VALID);
            } else {
                /* Configure NAK status for OUT Endpoint */
                PCD_SET_EP_RX_STATUS(USB, ep->num, USB_EP_RX_NAK);
            }
        }
    }
    /* Double Buffer */
    else {
        if (ep->type == EP_TYPE_BULK) {
            /* Set bulk endpoint as double buffered */
            PCD_SET_BULK_EP_DBUF(USB, ep->num);
        } else {
            /* Set the ISOC endpoint in double buffer mode */
            PCD_CLEAR_EP_KIND(USB, ep->num);
        }

        /* Set buffer address for double buffered mode */
        PCD_SET_EP_DBUF_ADDR(USB, ep->num, ep->pmaaddr0, ep->pmaaddr1);

        if (ep->is_in == 0U) {
            /* Clear the data toggle bits for the endpoint IN/OUT */
            PCD_CLEAR_RX_DTOG(USB, ep->num);
            PCD_CLEAR_TX_DTOG(USB, ep->num);

            /* Set endpoint RX count */
            PCD_SET_EP_DBUF_CNT(USB, ep->num, ep->is_in, ep->maxpacket);

            /* Set endpoint RX to valid state */
            PCD_SET_EP_RX_STATUS(USB, ep->num, USB_EP_RX_VALID);
            PCD_SET_EP_TX_STATUS(USB, ep->num, USB_EP_TX_DIS);
        } else {
            /* Clear the data toggle bits for the endpoint IN/OUT */
            PCD_CLEAR_RX_DTOG(USB, ep->num);
            PCD_CLEAR_TX_DTOG(USB, ep->num);

            if (ep->type != EP_TYPE_ISOC) {
                /* Configure NAK status for the Endpoint */
                PCD_SET_EP_TX_STATUS(USB, ep->num, USB_EP_TX_NAK);
            } else {
                /* Configure TX Endpoint to disabled state */
                PCD_SET_EP_TX_STATUS(USB, ep->num, USB_EP_TX_DIS);
            }

            PCD_SET_EP_RX_STATUS(USB, ep->num, USB_EP_RX_DIS);
        }
    }

    return OK;
}

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
    return usb_activate_endpoint(ep);
    // __HAL_UNLOCK(handle); TODO
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
