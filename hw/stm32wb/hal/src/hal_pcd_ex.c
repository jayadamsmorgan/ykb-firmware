#include "hal_pcd_ex.h"

#include "hal_pcd.h"
#include "hal_systick.h"

HAL_StatusTypeDef HAL_PCDEx_PMAConfig(PCD_HandleTypeDef *hpcd, uint16_t ep_addr,
                                      uint16_t ep_kind, uint32_t pmaadress) {
    PCD_EPTypeDef *ep;

    /* initialize ep structure*/
    if ((0x80U & ep_addr) == 0x80U) {
        ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
    } else {
        ep = &hpcd->OUT_ep[ep_addr];
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

    return HAL_OK;
}

HAL_StatusTypeDef HAL_PCDEx_ActivateBCD(PCD_HandleTypeDef *hpcd) {
    USB_TypeDef *USBx = hpcd->Instance;
    hpcd->battery_charging_active = 1U;

    /* Enable BCD feature */
    USBx->BCDR |= USB_BCDR_BCDEN;

    /* Enable DCD : Data Contact Detect */
    USBx->BCDR &= ~(USB_BCDR_PDEN);
    USBx->BCDR &= ~(USB_BCDR_SDEN);
    USBx->BCDR |= USB_BCDR_DCDEN;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_PCDEx_DeActivateBCD(PCD_HandleTypeDef *hpcd) {
    USB_TypeDef *USBx = hpcd->Instance;
    hpcd->battery_charging_active = 0U;

    /* Disable BCD feature */
    USBx->BCDR &= ~(USB_BCDR_BCDEN);

    return HAL_OK;
}

void HAL_PCDEx_BCD_VBUSDetect(PCD_HandleTypeDef *hpcd) {
    USB_TypeDef *USBx = hpcd->Instance;
    uint32_t tickstart = systick_get_tick();

    /* Wait for Min DCD Timeout */
    systick_delay(300U);

    /* Data Pin Contact ? Check Detect flag */
    if ((USBx->BCDR & USB_BCDR_DCDET) == USB_BCDR_DCDET) {
        HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_CONTACT_DETECTION);
    }
    /* Primary detection: checks if connected to Standard Downstream Port
    (without charging capability) */
    USBx->BCDR &= ~(USB_BCDR_DCDEN);
    systick_delay(50U);
    USBx->BCDR |= (USB_BCDR_PDEN);
    systick_delay(50U);

    /* If Charger detect ? */
    if ((USBx->BCDR & USB_BCDR_PDET) == USB_BCDR_PDET) {
        /* Start secondary detection to check connection to Charging Downstream
        Port or Dedicated Charging Port */
        USBx->BCDR &= ~(USB_BCDR_PDEN);
        systick_delay(50U);
        USBx->BCDR |= (USB_BCDR_SDEN);
        systick_delay(50U);

        /* If CDP ? */
        if ((USBx->BCDR & USB_BCDR_SDET) == USB_BCDR_SDET) {
            /* Dedicated Downstream Port DCP */
            HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_DEDICATED_CHARGING_PORT);
        } else {
            /* Charging Downstream Port CDP */
            HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_CHARGING_DOWNSTREAM_PORT);
        }
    } else /* NO */
    {
        /* Standard Downstream Port */
        HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_STD_DOWNSTREAM_PORT);
    }

    /* Battery Charging capability discovery finished Start Enumeration */
    (void)HAL_PCDEx_DeActivateBCD(hpcd);

    /* Check for the Timeout, else start USB Device */
    if ((systick_get_tick() - tickstart) > 1000U) {
        HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_ERROR);
    } else {
        HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_DISCOVERY_COMPLETED);
    }
}

HAL_StatusTypeDef HAL_PCDEx_ActivateLPM(PCD_HandleTypeDef *hpcd) {

    USB_TypeDef *USBx = hpcd->Instance;
    hpcd->lpm_active = 1U;
    hpcd->LPM_State = LPM_L0;

    USBx->LPMCSR |= USB_LPMCSR_LMPEN;
    USBx->LPMCSR |= USB_LPMCSR_LPMACK;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_PCDEx_DeActivateLPM(PCD_HandleTypeDef *hpcd) {
    USB_TypeDef *USBx = hpcd->Instance;

    hpcd->lpm_active = 0U;

    USBx->LPMCSR &= ~(USB_LPMCSR_LMPEN);
    USBx->LPMCSR &= ~(USB_LPMCSR_LPMACK);

    return HAL_OK;
}

__weak void HAL_PCDEx_LPM_Callback(PCD_HandleTypeDef *hpcd,
                                   PCD_LPM_MsgTypeDef msg) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    UNUSED(msg);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_PCDEx_LPM_Callback could be implemented in the user file
     */
}

__weak void HAL_PCDEx_BCD_Callback(PCD_HandleTypeDef *hpcd,
                                   PCD_BCD_MsgTypeDef msg) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    UNUSED(msg);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_PCDEx_BCD_Callback could be implemented in the user file
     */
}
