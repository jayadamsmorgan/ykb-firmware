#include "hal/pcd.h"

#include "hal/pcd_ex.h"
#include "hal/usb.h"

#define PCD_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define PCD_MAX(a, b) (((a) > (b)) ? (a) : (b))
static HAL_StatusTypeDef PCD_EP_ISR_Handler(PCD_HandleTypeDef *hpcd);
static HAL_StatusTypeDef HAL_PCD_EP_DB_Transmit(PCD_HandleTypeDef *hpcd,
                                                PCD_EPTypeDef *ep,
                                                uint16_t wEPVal);
static uint16_t HAL_PCD_EP_DB_Receive(PCD_HandleTypeDef *hpcd,
                                      PCD_EPTypeDef *ep, uint16_t wEPVal);

HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd) {
    uint8_t i;

    /* Check the PCD handle allocation */
    if (hpcd == NULL) {
        return HAL_ERROR;
    }

    if (hpcd->State == HAL_PCD_STATE_RESET) {
        /* Allocate lock resource and initialize it */
        hpcd->Lock = HAL_UNLOCKED;

        /* Init the low level hardware : GPIO, CLOCK, NVIC... */
        HAL_PCD_MspInit(hpcd);
    }

    hpcd->State = HAL_PCD_STATE_BUSY;

    /* Disable the Interrupts */
    hal_usb_disable_interrupts();

    /* Init endpoints structures */
    for (i = 0U; i < hpcd->Init.dev_endpoints; i++) {
        /* Init ep structure */
        hpcd->IN_ep[i].is_in = 1U;
        hpcd->IN_ep[i].num = i;
        /* Control until ep is activated */
        hpcd->IN_ep[i].type = EP_TYPE_CTRL;
        hpcd->IN_ep[i].maxpacket = 0U;
        hpcd->IN_ep[i].xfer_buff = 0U;
        hpcd->IN_ep[i].xfer_len = 0U;
    }

    for (i = 0U; i < hpcd->Init.dev_endpoints; i++) {
        hpcd->OUT_ep[i].is_in = 0U;
        hpcd->OUT_ep[i].num = i;
        /* Control until ep is activated */
        hpcd->OUT_ep[i].type = EP_TYPE_CTRL;
        hpcd->OUT_ep[i].maxpacket = 0U;
        hpcd->OUT_ep[i].xfer_buff = 0U;
        hpcd->OUT_ep[i].xfer_len = 0U;
    }

    /* Init Device */
    hal_usb_device_init(hpcd->Init);

    hpcd->USB_Address = 0U;
    hpcd->State = HAL_PCD_STATE_READY;

    /* Activate LPM */
    if (hpcd->Init.lpm_enable == 1U) {
        (void)HAL_PCDEx_ActivateLPM(hpcd);
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_DeInit(PCD_HandleTypeDef *hpcd) {
    /* Check the PCD handle allocation */
    if (hpcd == NULL) {
        return HAL_ERROR;
    }

    hpcd->State = HAL_PCD_STATE_BUSY;

    /* Stop Device */
    if (USB_StopDevice(hpcd->Instance) != HAL_OK) {
        return HAL_ERROR;
    }

    /* DeInit the low level hardware: CLOCK, NVIC.*/
    HAL_PCD_MspDeInit(hpcd);

    hpcd->State = HAL_PCD_STATE_RESET;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd) {
    __HAL_LOCK(hpcd);
    hal_usb_disable_interrupts();
    (void)USB_DevConnect(hpcd->Instance);
    __HAL_UNLOCK(hpcd);

    return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd) {
    __HAL_LOCK(hpcd);
    hal_usb_disable_interrupts();
    (void)USB_DevDisconnect(hpcd->Instance);
    __HAL_UNLOCK(hpcd);

    return HAL_OK;
}

void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd) {
    uint32_t wIstr = USB_ReadInterrupts(hpcd->Instance);

    if ((wIstr & USB_ISTR_CTR) == USB_ISTR_CTR) {
        /* servicing of the endpoint correct transfer interrupt */
        /* clear of the CTR flag into the sub */
        (void)PCD_EP_ISR_Handler(hpcd);

        return;
    }

    if ((wIstr & USB_ISTR_RESET) == USB_ISTR_RESET) {
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_RESET);

        HAL_PCD_ResetCallback(hpcd);

        (void)HAL_PCD_SetAddress(hpcd, 0U);

        return;
    }

    if ((wIstr & USB_ISTR_PMAOVR) == USB_ISTR_PMAOVR) {
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_PMAOVR);

        return;
    }

    if ((wIstr & USB_ISTR_ERR) == USB_ISTR_ERR) {
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_ERR);

        return;
    }

    if ((wIstr & USB_ISTR_WKUP) == USB_ISTR_WKUP) {
        hpcd->Instance->CNTR &= (uint16_t)~(USB_CNTR_LPMODE);
        hpcd->Instance->CNTR &= (uint16_t)~(USB_CNTR_FSUSP);

        if (hpcd->LPM_State == LPM_L1) {
            hpcd->LPM_State = LPM_L0;
            HAL_PCDEx_LPM_Callback(hpcd, PCD_LPM_L0_ACTIVE);
        }

        HAL_PCD_ResumeCallback(hpcd);

        __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_WKUP);

        return;
    }

    if ((wIstr & USB_ISTR_SUSP) == USB_ISTR_SUSP) {
        /* Force low-power mode in the macrocell */
        hpcd->Instance->CNTR |= (uint16_t)USB_CNTR_FSUSP;

        /* clear of the ISTR bit must be done after setting of CNTR_FSUSP */
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_SUSP);

        hpcd->Instance->CNTR |= (uint16_t)USB_CNTR_LPMODE;

        HAL_PCD_SuspendCallback(hpcd);

        return;
    }

    /* Handle LPM Interrupt */
    if ((wIstr & USB_ISTR_L1REQ) == USB_ISTR_L1REQ) {
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_L1REQ);
        if (hpcd->LPM_State == LPM_L0) {
            /* Force suspend and low-power mode before going to L1 state*/
            hpcd->Instance->CNTR |= (uint16_t)USB_CNTR_LPMODE;
            hpcd->Instance->CNTR |= (uint16_t)USB_CNTR_FSUSP;

            hpcd->LPM_State = LPM_L1;
            hpcd->BESL =
                ((uint32_t)hpcd->Instance->LPMCSR & USB_LPMCSR_BESL) >> 2;
            HAL_PCDEx_LPM_Callback(hpcd, PCD_LPM_L1_ACTIVE);
        } else {
            HAL_PCD_SuspendCallback(hpcd);
        }

        return;
    }

    if ((wIstr & USB_ISTR_SOF) == USB_ISTR_SOF) {
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_SOF);

        HAL_PCD_SOFCallback(hpcd);

        return;
    }

    if ((wIstr & USB_ISTR_ESOF) == USB_ISTR_ESOF) {
        /* clear ESOF flag in ISTR */
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_ESOF);

        return;
    }
}

HAL_StatusTypeDef HAL_PCD_DevConnect(PCD_HandleTypeDef *hpcd) {
    __HAL_LOCK(hpcd);
    (void)USB_DevConnect(hpcd->Instance);
    __HAL_UNLOCK(hpcd);

    return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_DevDisconnect(PCD_HandleTypeDef *hpcd) {
    __HAL_LOCK(hpcd);
    (void)USB_DevDisconnect(hpcd->Instance);
    __HAL_UNLOCK(hpcd);

    return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address) {
    __HAL_LOCK(hpcd);
    hpcd->USB_Address = address;
    (void)USB_SetDevAddress(hpcd->Instance, address);
    __HAL_UNLOCK(hpcd);

    return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                  uint16_t ep_mps, uint8_t ep_type) {
    HAL_StatusTypeDef ret = HAL_OK;
    PCD_EPTypeDef *ep;

    if ((ep_addr & 0x80U) == 0x80U) {
        ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
        ep->is_in = 1U;
    } else {
        ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
        ep->is_in = 0U;
    }

    ep->num = ep_addr & EP_ADDR_MSK;
    ep->maxpacket = (uint32_t)ep_mps & 0x7FFU;
    ep->type = ep_type;

    /* Set initial data PID. */
    if (ep_type == EP_TYPE_BULK) {
        ep->data_pid_start = 0U;
    }

    __HAL_LOCK(hpcd);
    hal_usb_activate_endpoint(ep);
    __HAL_UNLOCK(hpcd);

    return ret;
}

HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr) {
    PCD_EPTypeDef *ep;

    if ((ep_addr & 0x80U) == 0x80U) {
        ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
        ep->is_in = 1U;
    } else {
        ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
        ep->is_in = 0U;
    }
    ep->num = ep_addr & EP_ADDR_MSK;

    __HAL_LOCK(hpcd);
    (void)USB_DeactivateEndpoint(hpcd->Instance, ep);
    __HAL_UNLOCK(hpcd);
    return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                     uint8_t *pBuf, uint32_t len) {
    PCD_EPTypeDef *ep;

    ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];

    /*setup and start the Xfer */
    ep->xfer_buff = pBuf;
    ep->xfer_len = len;
    ep->xfer_count = 0U;
    ep->is_in = 0U;
    ep->num = ep_addr & EP_ADDR_MSK;

    (void)USB_EPStartXfer(hpcd->Instance, ep);

    return HAL_OK;
}

uint32_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef const *hpcd, uint8_t ep_addr) {
    return hpcd->OUT_ep[ep_addr & EP_ADDR_MSK].xfer_count;
}

HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                      uint8_t *pBuf, uint32_t len) {
    PCD_EPTypeDef *ep;

    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];

    /*setup and start the Xfer */
    ep->xfer_buff = pBuf;
    ep->xfer_len = len;
    ep->xfer_fill_db = 1U;
    ep->xfer_len_db = len;
    ep->xfer_count = 0U;
    ep->is_in = 1U;
    ep->num = ep_addr & EP_ADDR_MSK;

    (void)USB_EPStartXfer(hpcd->Instance, ep);

    return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd,
                                      uint8_t ep_addr) {
    PCD_EPTypeDef *ep;

    if (((uint32_t)ep_addr & EP_ADDR_MSK) > hpcd->Init.dev_endpoints) {
        return HAL_ERROR;
    }

    if ((0x80U & ep_addr) == 0x80U) {
        ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
        ep->is_in = 1U;
    } else {
        ep = &hpcd->OUT_ep[ep_addr];
        ep->is_in = 0U;
    }

    ep->is_stall = 1U;
    ep->num = ep_addr & EP_ADDR_MSK;

    __HAL_LOCK(hpcd);

    (void)USB_EPSetStall(hpcd->Instance, ep);

    __HAL_UNLOCK(hpcd);

    return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd,
                                      uint8_t ep_addr) {
    PCD_EPTypeDef *ep;

    if (((uint32_t)ep_addr & 0x0FU) > hpcd->Init.dev_endpoints) {
        return HAL_ERROR;
    }

    if ((0x80U & ep_addr) == 0x80U) {
        ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
        ep->is_in = 1U;
    } else {
        ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
        ep->is_in = 0U;
    }

    ep->is_stall = 0U;
    ep->num = ep_addr & EP_ADDR_MSK;

    __HAL_LOCK(hpcd);
    (void)USB_EPClearStall(hpcd->Instance, ep);
    __HAL_UNLOCK(hpcd);

    return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_EP_Abort(PCD_HandleTypeDef *hpcd, uint8_t ep_addr) {
    HAL_StatusTypeDef ret;
    PCD_EPTypeDef *ep;

    if ((0x80U & ep_addr) == 0x80U) {
        ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
    } else {
        ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
    }

    /* Stop Xfer */
    ret = USB_EPStopXfer(hpcd->Instance, ep);

    return ret;
}

HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint8_t ep_addr) {

    UNUSED(hpcd);
    UNUSED(ep_addr);

    return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd) {
    return (USB_ActivateRemoteWakeup(hpcd->Instance));
}

HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd) {
    return (USB_DeActivateRemoteWakeup(hpcd->Instance));
}

PCD_StateTypeDef HAL_PCD_GetState(PCD_HandleTypeDef const *hpcd) {
    return hpcd->State;
}

static HAL_StatusTypeDef PCD_EP_ISR_Handler(PCD_HandleTypeDef *hpcd) {
    PCD_EPTypeDef *ep;
    uint16_t count;
    uint16_t wIstr;
    uint16_t wEPVal;
    uint16_t TxPctSize;
    uint8_t epindex;

    /* stay in loop while pending interrupts */
    while ((hpcd->Instance->ISTR & USB_ISTR_CTR) != 0U) {
        wIstr = hpcd->Instance->ISTR;

        /* extract highest priority endpoint number */
        epindex = (uint8_t)(wIstr & USB_ISTR_EP_ID);

        if (epindex == 0U) {
            /* Decode and service control endpoint interrupt */

            /* DIR bit = origin of the interrupt */
            if ((wIstr & USB_ISTR_DIR) == 0U) {
                /* DIR = 0 */

                /* DIR = 0 => IN  int */
                /* DIR = 0 implies that (EP_CTR_TX = 1) always */
                PCD_CLEAR_TX_EP_CTR(hpcd->Instance, PCD_ENDP0);
                ep = &hpcd->IN_ep[0];

                ep->xfer_count = PCD_GET_EP_TX_CNT(hpcd->Instance, ep->num);
                ep->xfer_buff += ep->xfer_count;

                /* TX COMPLETE */
                HAL_PCD_DataInStageCallback(hpcd, 0U);

                if ((hpcd->USB_Address > 0U) && (ep->xfer_len == 0U)) {
                    hpcd->Instance->DADDR =
                        ((uint16_t)hpcd->USB_Address | USB_DADDR_EF);
                    hpcd->USB_Address = 0U;
                }
            } else {
                /* DIR = 1 */

                /* DIR = 1 & CTR_RX => SETUP or OUT int */
                /* DIR = 1 & (CTR_TX | CTR_RX) => 2 int pending */
                ep = &hpcd->OUT_ep[0];
                wEPVal = PCD_GET_ENDPOINT(hpcd->Instance, PCD_ENDP0);

                if ((wEPVal & USB_EP_SETUP) != 0U) {
                    /* Get SETUP Packet */
                    ep->xfer_count = PCD_GET_EP_RX_CNT(hpcd->Instance, ep->num);

                    USB_ReadPMA(hpcd->Instance, (uint8_t *)hpcd->Setup,
                                ep->pmaadress, (uint16_t)ep->xfer_count);

                    /* SETUP bit kept frozen while CTR_RX = 1 */
                    PCD_CLEAR_RX_EP_CTR(hpcd->Instance, PCD_ENDP0);

                    /* Process SETUP Packet*/
                    HAL_PCD_SetupStageCallback(hpcd);
                } else if ((wEPVal & USB_EP_CTR_RX) != 0U) {
                    PCD_CLEAR_RX_EP_CTR(hpcd->Instance, PCD_ENDP0);

                    /* Get Control Data OUT Packet */
                    ep->xfer_count = PCD_GET_EP_RX_CNT(hpcd->Instance, ep->num);

                    if ((ep->xfer_count != 0U) && (ep->xfer_buff != 0U)) {
                        USB_ReadPMA(hpcd->Instance, ep->xfer_buff,
                                    ep->pmaadress, (uint16_t)ep->xfer_count);

                        ep->xfer_buff += ep->xfer_count;

                        /* Process Control Data OUT Packet */
                        HAL_PCD_DataOutStageCallback(hpcd, 0U);
                    }

                    wEPVal =
                        (uint16_t)PCD_GET_ENDPOINT(hpcd->Instance, PCD_ENDP0);

                    if (((wEPVal & USB_EP_SETUP) == 0U) &&
                        ((wEPVal & USB_EP_RX_STRX) != USB_EP_RX_VALID)) {
                        PCD_SET_EP_RX_STATUS(hpcd->Instance, PCD_ENDP0,
                                             USB_EP_RX_VALID);
                    }
                }
            }
        } else {
            /* Decode and service non control endpoints interrupt */
            /* process related endpoint register */
            wEPVal = PCD_GET_ENDPOINT(hpcd->Instance, epindex);

            if ((wEPVal & USB_EP_CTR_RX) != 0U) {
                /* clear int flag */
                PCD_CLEAR_RX_EP_CTR(hpcd->Instance, epindex);
                ep = &hpcd->OUT_ep[epindex];

                /* OUT Single Buffering */
                if (ep->doublebuffer == 0U) {
                    count =
                        (uint16_t)PCD_GET_EP_RX_CNT(hpcd->Instance, ep->num);

                    if (count != 0U) {
                        USB_ReadPMA(hpcd->Instance, ep->xfer_buff,
                                    ep->pmaadress, count);
                    }
                } else {
                    /* manage double buffer bulk out */
                    if (ep->type == EP_TYPE_BULK) {
                        count = HAL_PCD_EP_DB_Receive(hpcd, ep, wEPVal);
                    } else /* manage double buffer iso out */
                    {
                        /* free EP OUT Buffer */
                        PCD_FREE_USER_BUFFER(hpcd->Instance, ep->num, 0U);

                        if ((PCD_GET_ENDPOINT(hpcd->Instance, ep->num) &
                             USB_EP_DTOG_RX) != 0U) {
                            /* read from endpoint BUF0Addr buffer */
                            count = (uint16_t)PCD_GET_EP_DBUF0_CNT(
                                hpcd->Instance, ep->num);

                            if (count != 0U) {
                                USB_ReadPMA(hpcd->Instance, ep->xfer_buff,
                                            ep->pmaaddr0, count);
                            }
                        } else {
                            /* read from endpoint BUF1Addr buffer */
                            count = (uint16_t)PCD_GET_EP_DBUF1_CNT(
                                hpcd->Instance, ep->num);

                            if (count != 0U) {
                                USB_ReadPMA(hpcd->Instance, ep->xfer_buff,
                                            ep->pmaaddr1, count);
                            }
                        }
                    }
                }

                /* multi-packet on the NON control OUT endpoint */
                ep->xfer_count += count;
                ep->xfer_buff += count;

                if ((ep->xfer_len == 0U) || (count < ep->maxpacket)) {
                    /* RX COMPLETE */
                    HAL_PCD_DataOutStageCallback(hpcd, ep->num);
                } else {
                    (void)USB_EPStartXfer(hpcd->Instance, ep);
                }
            }

            if ((wEPVal & USB_EP_CTR_TX) != 0U) {
                ep = &hpcd->IN_ep[epindex];

                /* clear int flag */
                PCD_CLEAR_TX_EP_CTR(hpcd->Instance, epindex);

                if (ep->type == EP_TYPE_ISOC) {
                    ep->xfer_len = 0U;

                    if (ep->doublebuffer != 0U) {
                        if ((wEPVal & USB_EP_DTOG_TX) != 0U) {
                            PCD_SET_EP_DBUF0_CNT(hpcd->Instance, ep->num,
                                                 ep->is_in, 0U);
                        } else {
                            PCD_SET_EP_DBUF1_CNT(hpcd->Instance, ep->num,
                                                 ep->is_in, 0U);
                        }
                    }

                    /* TX COMPLETE */
                    HAL_PCD_DataInStageCallback(hpcd, ep->num);
                } else {
                    /* Manage Single Buffer Transaction */
                    if ((wEPVal & USB_EP_KIND) == 0U) {
                        /* Multi-packet on the NON control IN endpoint */
                        TxPctSize = (uint16_t)PCD_GET_EP_TX_CNT(hpcd->Instance,
                                                                ep->num);

                        if (ep->xfer_len > TxPctSize) {
                            ep->xfer_len -= TxPctSize;
                        } else {
                            ep->xfer_len = 0U;
                        }

                        /* Zero Length Packet? */
                        if (ep->xfer_len == 0U) {
                            /* TX COMPLETE */
                            HAL_PCD_DataInStageCallback(hpcd, ep->num);
                        } else {
                            /* Transfer is not yet Done */
                            ep->xfer_buff += TxPctSize;
                            ep->xfer_count += TxPctSize;
                            (void)USB_EPStartXfer(hpcd->Instance, ep);
                        }
                    }
                    /* Double Buffer bulk IN (bulk transfer Len > Ep_Mps) */
                    else {
                        (void)HAL_PCD_EP_DB_Transmit(hpcd, ep, wEPVal);
                    }
                }
            }
        }
    }

    return HAL_OK;
}

static uint16_t HAL_PCD_EP_DB_Receive(PCD_HandleTypeDef *hpcd,
                                      PCD_EPTypeDef *ep, uint16_t wEPVal) {
    uint16_t count;

    /* Manage Buffer0 OUT */
    if ((wEPVal & USB_EP_DTOG_RX) != 0U) {
        /* Get count of received Data on buffer0 */
        count = (uint16_t)PCD_GET_EP_DBUF0_CNT(hpcd->Instance, ep->num);

        if (ep->xfer_len >= count) {
            ep->xfer_len -= count;
        } else {
            ep->xfer_len = 0U;
        }

        if (ep->xfer_len == 0U) {
            /* Set NAK to OUT endpoint since double buffer is enabled */
            PCD_SET_EP_RX_STATUS(hpcd->Instance, ep->num, USB_EP_RX_NAK);
        }

        /* Check if Buffer1 is in blocked state which requires to toggle */
        if ((wEPVal & USB_EP_DTOG_TX) != 0U) {
            PCD_FREE_USER_BUFFER(hpcd->Instance, ep->num, 0U);
        }

        if (count != 0U) {
            USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr0, count);
        }
    }
    /* Manage Buffer 1 DTOG_RX=0 */
    else {
        /* Get count of received data */
        count = (uint16_t)PCD_GET_EP_DBUF1_CNT(hpcd->Instance, ep->num);

        if (ep->xfer_len >= count) {
            ep->xfer_len -= count;
        } else {
            ep->xfer_len = 0U;
        }

        if (ep->xfer_len == 0U) {
            /* Set NAK on the current endpoint */
            PCD_SET_EP_RX_STATUS(hpcd->Instance, ep->num, USB_EP_RX_NAK);
        }

        /* Need to FreeUser Buffer */
        if ((wEPVal & USB_EP_DTOG_TX) == 0U) {
            PCD_FREE_USER_BUFFER(hpcd->Instance, ep->num, 0U);
        }

        if (count != 0U) {
            USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr1, count);
        }
    }

    return count;
}

static HAL_StatusTypeDef HAL_PCD_EP_DB_Transmit(PCD_HandleTypeDef *hpcd,
                                                PCD_EPTypeDef *ep,
                                                uint16_t wEPVal) {
    uint32_t len;
    uint16_t TxPctSize;

    /* Data Buffer0 ACK received */
    if ((wEPVal & USB_EP_DTOG_TX) != 0U) {
        /* multi-packet on the NON control IN endpoint */
        TxPctSize = (uint16_t)PCD_GET_EP_DBUF0_CNT(hpcd->Instance, ep->num);

        if (ep->xfer_len > TxPctSize) {
            ep->xfer_len -= TxPctSize;
        } else {
            ep->xfer_len = 0U;
        }

        /* Transfer is completed */
        if (ep->xfer_len == 0U) {
            PCD_SET_EP_DBUF0_CNT(hpcd->Instance, ep->num, ep->is_in, 0U);
            PCD_SET_EP_DBUF1_CNT(hpcd->Instance, ep->num, ep->is_in, 0U);

            if (ep->type == EP_TYPE_BULK) {
                /* Set Bulk endpoint in NAK state */
                PCD_SET_EP_TX_STATUS(hpcd->Instance, ep->num, USB_EP_TX_NAK);
            }

            /* TX COMPLETE */
            HAL_PCD_DataInStageCallback(hpcd, ep->num);

            if ((wEPVal & USB_EP_DTOG_RX) != 0U) {
                PCD_FREE_USER_BUFFER(hpcd->Instance, ep->num, 1U);
            }

            return HAL_OK;
        } else /* Transfer is not yet Done */
        {
            /* Need to Free USB Buffer */
            if ((wEPVal & USB_EP_DTOG_RX) != 0U) {
                PCD_FREE_USER_BUFFER(hpcd->Instance, ep->num, 1U);
            }

            /* Still there is data to Fill in the next Buffer */
            if (ep->xfer_fill_db == 1U) {
                ep->xfer_buff += TxPctSize;
                ep->xfer_count += TxPctSize;

                /* Calculate the len of the new buffer to fill */
                if (ep->xfer_len_db >= ep->maxpacket) {
                    len = ep->maxpacket;
                    ep->xfer_len_db -= len;
                } else if (ep->xfer_len_db == 0U) {
                    len = TxPctSize;
                    ep->xfer_fill_db = 0U;
                } else {
                    ep->xfer_fill_db = 0U;
                    len = ep->xfer_len_db;
                    ep->xfer_len_db = 0U;
                }

                /* Write remaining Data to Buffer */
                /* Set the Double buffer counter for pma buffer0 */
                PCD_SET_EP_DBUF0_CNT(hpcd->Instance, ep->num, ep->is_in, len);

                /* Copy user buffer to USB PMA */
                USB_WritePMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr0,
                             (uint16_t)len);
            }
        }
    } else /* Data Buffer1 ACK received */
    {
        /* multi-packet on the NON control IN endpoint */
        TxPctSize = (uint16_t)PCD_GET_EP_DBUF1_CNT(hpcd->Instance, ep->num);

        if (ep->xfer_len >= TxPctSize) {
            ep->xfer_len -= TxPctSize;
        } else {
            ep->xfer_len = 0U;
        }

        /* Transfer is completed */
        if (ep->xfer_len == 0U) {
            PCD_SET_EP_DBUF0_CNT(hpcd->Instance, ep->num, ep->is_in, 0U);
            PCD_SET_EP_DBUF1_CNT(hpcd->Instance, ep->num, ep->is_in, 0U);

            if (ep->type == EP_TYPE_BULK) {
                /* Set Bulk endpoint in NAK state */
                PCD_SET_EP_TX_STATUS(hpcd->Instance, ep->num, USB_EP_TX_NAK);
            }

            /* TX COMPLETE */
            HAL_PCD_DataInStageCallback(hpcd, ep->num);

            /* need to Free USB Buff */
            if ((wEPVal & USB_EP_DTOG_RX) == 0U) {
                PCD_FREE_USER_BUFFER(hpcd->Instance, ep->num, 1U);
            }

            return HAL_OK;
        } else /* Transfer is not yet Done */
        {
            /* Need to Free USB Buffer */
            if ((wEPVal & USB_EP_DTOG_RX) == 0U) {
                PCD_FREE_USER_BUFFER(hpcd->Instance, ep->num, 1U);
            }

            /* Still there is data to Fill in the next Buffer */
            if (ep->xfer_fill_db == 1U) {
                ep->xfer_buff += TxPctSize;
                ep->xfer_count += TxPctSize;

                /* Calculate the len of the new buffer to fill */
                if (ep->xfer_len_db >= ep->maxpacket) {
                    len = ep->maxpacket;
                    ep->xfer_len_db -= len;
                } else if (ep->xfer_len_db == 0U) {
                    len = TxPctSize;
                    ep->xfer_fill_db = 0U;
                } else {
                    len = ep->xfer_len_db;
                    ep->xfer_len_db = 0U;
                    ep->xfer_fill_db = 0;
                }

                /* Set the Double buffer counter for pma buffer1 */
                PCD_SET_EP_DBUF1_CNT(hpcd->Instance, ep->num, ep->is_in, len);

                /* Copy the user buffer to USB PMA */
                USB_WritePMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr1,
                             (uint16_t)len);
            }
        }
    }

    /* Enable endpoint IN */
    PCD_SET_EP_TX_STATUS(hpcd->Instance, ep->num, USB_EP_TX_VALID);

    return HAL_OK;
}
