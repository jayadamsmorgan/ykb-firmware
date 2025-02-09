#include "hal/usb.h"

#include "hal/hal_err.h"
#include "hal/pcd.h"
#include "stm32wbxx.h"
#include "utils/utils.h"

void hal_usb_enable_interrupts() {
    uint32_t winterruptmask;

    /* Clear pending interrupts */
    USB->ISTR = 0U;

    /* Set winterruptmask variable */
    winterruptmask = USB_CNTR_CTRM | USB_CNTR_WKUPM | USB_CNTR_SUSPM |
                     USB_CNTR_ERRM | USB_CNTR_SOFM | USB_CNTR_ESOFM |
                     USB_CNTR_RESETM | USB_CNTR_L1REQM;

    /* Set interrupt mask */
    USB->CNTR = (uint16_t)winterruptmask;
}

void hal_usb_disable_interrupts() {
    uint32_t winterruptmask;

    /* Set winterruptmask variable */
    winterruptmask = USB_CNTR_CTRM | USB_CNTR_WKUPM | USB_CNTR_SUSPM |
                     USB_CNTR_ERRM | USB_CNTR_SOFM | USB_CNTR_ESOFM |
                     USB_CNTR_RESETM | USB_CNTR_L1REQM;

    /* Clear interrupt mask */
    USB->CNTR &= (uint16_t)(~winterruptmask);
}

void hal_usb_device_init(USB_CfgTypeDef cfg) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(cfg);

    /* Init Device */
    /* CNTR_FRES = 1 */
    USB->CNTR = (uint16_t)USB_CNTR_FRES;

    /* CNTR_FRES = 0 */
    USB->CNTR = 0U;

    /* Clear pending interrupts */
    USB->ISTR = 0U;

    /*Set Btable Address*/
    USB->BTABLE = BTABLE_ADDRESS;
}

hal_err hal_usb_activate_endpoint(USB_EPTypeDef *ep) {
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
        return -909;
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
    } else {
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

hal_err USB_DeactivateEndpoint(USB_TypeDef *USBx, USB_EPTypeDef *ep) {
    if (ep->doublebuffer == 0U) {
        if (ep->is_in != 0U) {
            PCD_CLEAR_TX_DTOG(USBx, ep->num);

            /* Configure DISABLE status for the Endpoint */
            PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_DIS);
        }

        else {
            PCD_CLEAR_RX_DTOG(USBx, ep->num);

            /* Configure DISABLE status for the Endpoint */
            PCD_SET_EP_RX_STATUS(USBx, ep->num, USB_EP_RX_DIS);
        }
    }
    /* Double Buffer */
    else {
        if (ep->is_in == 0U) {
            /* Clear the data toggle bits for the endpoint IN/OUT*/
            PCD_CLEAR_RX_DTOG(USBx, ep->num);
            PCD_CLEAR_TX_DTOG(USBx, ep->num);

            /* Reset value of the data toggle bits for the endpoint out*/
            PCD_TX_DTOG(USBx, ep->num);

            PCD_SET_EP_RX_STATUS(USBx, ep->num, USB_EP_RX_DIS);
            PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_DIS);
        } else {
            /* Clear the data toggle bits for the endpoint IN/OUT*/
            PCD_CLEAR_RX_DTOG(USBx, ep->num);
            PCD_CLEAR_TX_DTOG(USBx, ep->num);
            PCD_RX_DTOG(USBx, ep->num);

            /* Configure DISABLE status for the Endpoint*/
            PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_DIS);
            PCD_SET_EP_RX_STATUS(USBx, ep->num, USB_EP_RX_DIS);
        }
    }

    return OK;
}

hal_err USB_EPStartXfer(USB_TypeDef *USBx, USB_EPTypeDef *ep) {
    uint32_t len;
    uint16_t pmabuffer;
    uint16_t wEPVal;

    /* IN endpoint */
    if (ep->is_in == 1U) {
        /* Multi packet transfer */
        if (ep->xfer_len > ep->maxpacket) {
            len = ep->maxpacket;
        } else {
            len = ep->xfer_len;
        }

        /* configure and validate Tx endpoint */
        if (ep->doublebuffer == 0U) {
            USB_WritePMA(USBx, ep->xfer_buff, ep->pmaadress, (uint16_t)len);
            PCD_SET_EP_TX_CNT(USBx, ep->num, len);
        } else {
            /* double buffer bulk management */
            if (ep->type == EP_TYPE_BULK) {
                if (ep->xfer_len_db > ep->maxpacket) {
                    /* enable double buffer */
                    PCD_SET_BULK_EP_DBUF(USBx, ep->num);

                    /* each Time to write in PMA xfer_len_db will */
                    ep->xfer_len_db -= len;

                    /* Fill the two first buffer in the Buffer0 & Buffer1 */
                    if ((PCD_GET_ENDPOINT(USBx, ep->num) & USB_EP_DTOG_TX) !=
                        0U) {
                        /* Set the Double buffer counter for pmabuffer1 */
                        PCD_SET_EP_DBUF1_CNT(USBx, ep->num, ep->is_in, len);
                        pmabuffer = ep->pmaaddr1;

                        /* Write the user buffer to USB PMA */
                        USB_WritePMA(USBx, ep->xfer_buff, pmabuffer,
                                     (uint16_t)len);
                        ep->xfer_buff += len;

                        if (ep->xfer_len_db > ep->maxpacket) {
                            ep->xfer_len_db -= len;
                        } else {
                            len = ep->xfer_len_db;
                            ep->xfer_len_db = 0U;
                        }

                        /* Set the Double buffer counter for pmabuffer0 */
                        PCD_SET_EP_DBUF0_CNT(USBx, ep->num, ep->is_in, len);
                        pmabuffer = ep->pmaaddr0;

                        /* Write the user buffer to USB PMA */
                        USB_WritePMA(USBx, ep->xfer_buff, pmabuffer,
                                     (uint16_t)len);
                    } else {
                        /* Set the Double buffer counter for pmabuffer0 */
                        PCD_SET_EP_DBUF0_CNT(USBx, ep->num, ep->is_in, len);
                        pmabuffer = ep->pmaaddr0;

                        /* Write the user buffer to USB PMA */
                        USB_WritePMA(USBx, ep->xfer_buff, pmabuffer,
                                     (uint16_t)len);
                        ep->xfer_buff += len;

                        if (ep->xfer_len_db > ep->maxpacket) {
                            ep->xfer_len_db -= len;
                        } else {
                            len = ep->xfer_len_db;
                            ep->xfer_len_db = 0U;
                        }

                        /* Set the Double buffer counter for pmabuffer1 */
                        PCD_SET_EP_DBUF1_CNT(USBx, ep->num, ep->is_in, len);
                        pmabuffer = ep->pmaaddr1;

                        /* Write the user buffer to USB PMA */
                        USB_WritePMA(USBx, ep->xfer_buff, pmabuffer,
                                     (uint16_t)len);
                    }
                }
                /* auto Switch to single buffer mode when transfer <Mps no need
                   to manage in double buffer */
                else {
                    len = ep->xfer_len_db;

                    /* disable double buffer mode for Bulk endpoint */
                    PCD_CLEAR_BULK_EP_DBUF(USBx, ep->num);

                    /* Set Tx count with nbre of byte to be transmitted */
                    PCD_SET_EP_TX_CNT(USBx, ep->num, len);
                    pmabuffer = ep->pmaaddr0;

                    /* Write the user buffer to USB PMA */
                    USB_WritePMA(USBx, ep->xfer_buff, pmabuffer, (uint16_t)len);
                }
            } else /* Manage isochronous double buffer IN mode */
            {
                /* Each Time to write in PMA xfer_len_db will */
                ep->xfer_len_db -= len;

                /* Fill the data buffer */
                if ((PCD_GET_ENDPOINT(USBx, ep->num) & USB_EP_DTOG_TX) != 0U) {
                    /* Set the Double buffer counter for pmabuffer1 */
                    PCD_SET_EP_DBUF1_CNT(USBx, ep->num, ep->is_in, len);
                    pmabuffer = ep->pmaaddr1;

                    /* Write the user buffer to USB PMA */
                    USB_WritePMA(USBx, ep->xfer_buff, pmabuffer, (uint16_t)len);
                } else {
                    /* Set the Double buffer counter for pmabuffer0 */
                    PCD_SET_EP_DBUF0_CNT(USBx, ep->num, ep->is_in, len);
                    pmabuffer = ep->pmaaddr0;

                    /* Write the user buffer to USB PMA */
                    USB_WritePMA(USBx, ep->xfer_buff, pmabuffer, (uint16_t)len);
                }
            }
        }

        PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_VALID);
    } else /* OUT endpoint */
    {
        if (ep->doublebuffer == 0U) {
            if ((ep->xfer_len == 0U) && (ep->type == EP_TYPE_CTRL)) {
                /* This is a status out stage set the OUT_STATUS */
                PCD_SET_OUT_STATUS(USBx, ep->num);
            } else {
                PCD_CLEAR_OUT_STATUS(USBx, ep->num);
            }

            /* Multi packet transfer */
            if (ep->xfer_len > ep->maxpacket) {
                ep->xfer_len -= ep->maxpacket;
            } else {
                ep->xfer_len = 0U;
            }
        } else {
            /* First Transfer Coming From HAL_PCD_EP_Receive & From ISR */
            /* Set the Double buffer counter */
            if (ep->type == EP_TYPE_BULK) {
                /* Coming from ISR */
                if (ep->xfer_count != 0U) {
                    /* Update last value to check if there is blocking state */
                    wEPVal = PCD_GET_ENDPOINT(USBx, ep->num);

                    /* Blocking State */
                    if ((((wEPVal & USB_EP_DTOG_RX) != 0U) &&
                         ((wEPVal & USB_EP_DTOG_TX) != 0U)) ||
                        (((wEPVal & USB_EP_DTOG_RX) == 0U) &&
                         ((wEPVal & USB_EP_DTOG_TX) == 0U))) {
                        PCD_FREE_USER_BUFFER(USBx, ep->num, 0U);
                    }
                }
            }
            /* iso out double */
            else if (ep->type == EP_TYPE_ISOC) {
                /* Only single packet transfer supported in FS */
                ep->xfer_len = 0U;
            } else {
                return HAL_ERROR;
            }
        }

        PCD_SET_EP_RX_STATUS(USBx, ep->num, USB_EP_RX_VALID);
    }

    return OK;
}

hal_err USB_EPSetStall(USB_TypeDef *USBx, USB_EPTypeDef *ep) {
    if (ep->is_in != 0U) {
        PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_STALL);
    } else {
        PCD_SET_EP_RX_STATUS(USBx, ep->num, USB_EP_RX_STALL);
    }

    return OK;
}

hal_err USB_EPClearStall(USB_TypeDef *USBx, USB_EPTypeDef *ep) {
    if (ep->is_in != 0U) {
        PCD_CLEAR_TX_DTOG(USBx, ep->num);

        if (ep->type != EP_TYPE_ISOC) {
            /* Configure NAK status for the Endpoint */
            PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_NAK);
        }
    } else {
        PCD_CLEAR_RX_DTOG(USBx, ep->num);

        /* Configure VALID status for the Endpoint */
        PCD_SET_EP_RX_STATUS(USBx, ep->num, USB_EP_RX_VALID);
    }

    return OK;
}

hal_err USB_EPStopXfer(USB_TypeDef *USBx, USB_EPTypeDef *ep) {
    /* IN endpoint */
    if (ep->is_in == 1U) {
        if (ep->doublebuffer == 0U) {
            if (ep->type != EP_TYPE_ISOC) {
                /* Configure NAK status for the Endpoint */
                PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_NAK);
            } else {
                /* Configure TX Endpoint to disabled state */
                PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_DIS);
            }
        }
    } else /* OUT endpoint */
    {
        if (ep->doublebuffer == 0U) {
            if (ep->type != EP_TYPE_ISOC) {
                /* Configure NAK status for the Endpoint */
                PCD_SET_EP_RX_STATUS(USBx, ep->num, USB_EP_RX_NAK);
            } else {
                /* Configure RX Endpoint to disabled state */
                PCD_SET_EP_RX_STATUS(USBx, ep->num, USB_EP_RX_DIS);
            }
        }
    }

    return OK;
}

hal_err USB_StopDevice(USB_TypeDef *USBx) {
    /* disable all interrupts and force USB reset */
    USBx->CNTR = (uint16_t)USB_CNTR_FRES;

    /* clear interrupt status register */
    USBx->ISTR = 0U;

    /* switch-off device */
    USBx->CNTR = (uint16_t)(USB_CNTR_FRES | USB_CNTR_PDWN);

    return OK;
}

hal_err USB_SetDevAddress(USB_TypeDef *USBx, uint8_t address) {
    if (address == 0U) {
        /* set device address and enable function */
        USBx->DADDR = (uint16_t)USB_DADDR_EF;
    }

    return OK;
}

hal_err USB_DevConnect(USB_TypeDef *USBx) {
    /* Enabling DP Pull-UP bit to Connect internal PU resistor on USB DP line */
    USBx->BCDR |= (uint16_t)USB_BCDR_DPPU;

    return OK;
}

hal_err USB_DevDisconnect(USB_TypeDef *USBx) {
    /* Disable DP Pull-Up bit to disconnect the Internal PU resistor on USB DP
     * line */
    USBx->BCDR &= (uint16_t)(~(USB_BCDR_DPPU));

    return OK;
}

uint32_t USB_ReadInterrupts(USB_TypeDef const *USBx) {
    uint32_t tmpreg;

    tmpreg = USBx->ISTR;
    return tmpreg;
}

hal_err USB_ActivateRemoteWakeup(USB_TypeDef *USBx) {
    USBx->CNTR |= (uint16_t)USB_CNTR_RESUME;

    return OK;
}

hal_err USB_DeActivateRemoteWakeup(USB_TypeDef *USBx) {
    USBx->CNTR &= (uint16_t)(~USB_CNTR_RESUME);

    return OK;
}

void USB_WritePMA(USB_TypeDef const *USBx, uint8_t *pbUsrBuf,
                  uint16_t wPMABufAddr, uint16_t wNBytes) {
    uint32_t n = ((uint32_t)wNBytes + 1U) >> 1;
    uint32_t BaseAddr = (uint32_t)USBx;
    uint32_t count;
    uint16_t WrVal;
    __IO uint16_t *pdwVal;
    uint8_t *pBuf = pbUsrBuf;

    pdwVal = (__IO uint16_t *)(BaseAddr + 0x400U +
                               ((uint32_t)wPMABufAddr * PMA_ACCESS));

    for (count = n; count != 0U; count--) {
        WrVal = pBuf[0];
        WrVal |= (uint16_t)pBuf[1] << 8;
        *pdwVal = (WrVal & 0xFFFFU);
        pdwVal++;

        pBuf++;
        pBuf++;
    }
}

void USB_ReadPMA(USB_TypeDef const *USBx, uint8_t *pbUsrBuf,
                 uint16_t wPMABufAddr, uint16_t wNBytes) {
    uint32_t n = (uint32_t)wNBytes >> 1;
    uint32_t BaseAddr = (uint32_t)USBx;
    uint32_t count;
    uint32_t RdVal;
    __IO uint16_t *pdwVal;
    uint8_t *pBuf = pbUsrBuf;

    pdwVal = (__IO uint16_t *)(BaseAddr + 0x400U +
                               ((uint32_t)wPMABufAddr * PMA_ACCESS));

    for (count = n; count != 0U; count--) {
        RdVal = *(__IO uint16_t *)pdwVal;
        pdwVal++;
        *pBuf = (uint8_t)((RdVal >> 0) & 0xFFU);
        pBuf++;
        *pBuf = (uint8_t)((RdVal >> 8) & 0xFFU);
        pBuf++;
    }

    if ((wNBytes % 2U) != 0U) {
        RdVal = *pdwVal;
        *pBuf = (uint8_t)((RdVal >> 0) & 0xFFU);
    }
}
