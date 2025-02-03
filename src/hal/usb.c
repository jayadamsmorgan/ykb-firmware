#include "hal/usb.h"

#include "hal/hal_err.h"
#include "stm32wbxx.h"
#include "usb/usb_device.h"

#define BTABLE_ADDRESS 0x000U

usb_ll_handle_t usb_ll_handle;

void hal_usb_enable_vdd() { SET_BIT(PWR->CR2, PWR_CR2_USV); }

void hal_usb_disable_interrupts() {
    uint32_t winterruptmask = USB_CNTR_CTRM | USB_CNTR_WKUPM | USB_CNTR_SUSPM |
                              USB_CNTR_ERRM | USB_CNTR_SOFM | USB_CNTR_ESOFM |
                              USB_CNTR_RESETM | USB_CNTR_L1REQM;
    USB->CNTR &= (uint16_t)(~winterruptmask);
    return;
}

hal_err hal_usb_init(usb_ll_handle_t *handle) {

    if (!handle) {
        return ERR_USB_INIT_HANDLE_NULL;
    }

    if (!handle->instance) {
        return ERR_USB_INIT_INSTANCE_NULL;
    }

    uint8_t i;

    if (handle->state == USB_COMM_STATE_RESET) {
        /* Allocate lock resource and initialize it */
        handle->lock = USB_LOCK_UNLOCKED;
    }

    handle->state = USB_COMM_STATE_BUSY;

    /* Disable the Interrupts */
    hal_usb_disable_interrupts();

    /* Init endpoints structures */
    for (i = 0U; i < handle->init.dev_endpoints; i++) {
        /* Init ep structure */
        handle->in_ep[i].is_in = 1U;
        handle->in_ep[i].num = i;
        /* Control until ep is activated */
        handle->in_ep[i].type = 0U;
        handle->in_ep[i].maxpacket = 0U;
        handle->in_ep[i].xfer_buff = 0U;
        handle->in_ep[i].xfer_len = 0U;
    }

    for (i = 0U; i < handle->init.dev_endpoints; i++) {
        handle->out_ep[i].is_in = 0U;
        handle->out_ep[i].num = i;
        /* Control until ep is activated */
        handle->out_ep[i].type = 0U;
        handle->out_ep[i].maxpacket = 0U;
        handle->out_ep[i].xfer_buff = 0U;
        handle->out_ep[i].xfer_len = 0U;
    }

    /* Init Device */
    WRITE_REG(USB->CNTR, USB_CNTR_FRES);

    /* CNTR_FRES = 0 */
    WRITE_REG(USB->CNTR, 0U);

    /* Clear pending interrupts */
    WRITE_REG(USB->ISTR, 0U);

    /*Set Btable Address*/
    WRITE_REG(USB->BTABLE, BTABLE_ADDRESS);

    handle->usb_address = 0U;
    handle->state = USB_COMM_STATE_READY;

    /* Activate LPM */
    if (handle->init.lpm_enable == 1U) {
        // TODO
    }

    return OK;
}

void hal_usb_pma_config(usb_ll_handle_t *handle, uint16_t ep_addr,
                        uint16_t ep_kind, uint32_t pmaadress) {
    usb_endpoint_t *ep;

    /* initialize ep structure*/
    if ((0x80U & ep_addr) == 0x80U) {
        ep = &handle->in_ep[ep_addr & EP_ADDR_MSK];
    } else {
        ep = &handle->out_ep[ep_addr];
    }

    /* Here we check if the endpoint is single or double Buffer*/
    if (ep_kind == PCD_SNG_BUF) {
        /* Single Buffer */
        ep->doublebuffer = 0U;
        /* Configure the PMA */
        ep->pmaadress = (uint16_t)pmaadress;
    } else /* USB_DBL_BUF */
    {
        /* Double Buffer Endpoint */
        ep->doublebuffer = 1U;
        /* Configure the PMA */
        ep->pmaaddr0 = (uint16_t)(pmaadress & 0xFFFFU);
        ep->pmaaddr1 = (uint16_t)((pmaadress & 0xFFFF0000U) >> 16);
    }

    return;
}
