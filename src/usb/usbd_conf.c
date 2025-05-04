#include "usb/usbd_conf.h"

#include "clock.h"
#include "crs.h"
#include "error_handler.h"
#include "hal/cortex.h"
#include "hal/gpio.h"
#include "hal/pcd.h"
#include "hal/pcd_ex.h"
#include "hal/power.h"
#include "hal/systick.h"
#include "usb/usbd_core.h"
#include "usb/usbd_hid.h"

PCD_HandleTypeDef hpcd_USB_FS;

void SystemClockConfig_Resume(void);

static USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status);

void HAL_PCD_MspInit(PCD_HandleTypeDef *pcdHandle) {
    if (pcdHandle->Instance == USB) {

        gpio_turn_on_port(GPIOA);

        gpio_pin_t usb_pins[2] = {PA11, PA12};
        for (uint8_t i = 0; i < 2; i++) {
            gpio_set_mode(usb_pins[i], GPIO_MODE_AF);
            gpio_set_af_mode(usb_pins[i], GPIO_AF_MODE_10);
            gpio_set_speed(usb_pins[i], GPIO_SPEED_HIGH);
            gpio_set_pupd(usb_pins[i], GPIO_PUPD_NONE);
        }

        /* Peripheral clock enable */
        clock_usb_enable();

        /* Peripheral interrupt init */
        cortex_nvic_set_priority(USB_LP_IRQn, 6, 0);
        cortex_nvic_enable(USB_LP_IRQn);
    }
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef *pcdHandle) {
    if (pcdHandle->Instance == USB) {
        clock_usb_disable();

        /* Peripheral interrupt Deinit*/
        cortex_nvic_disable(USB_LP_IRQn);
    }
}

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd) {
    USBD_LL_SetupStage((USBD_HandleTypeDef *)hpcd->pData,
                       (uint8_t *)hpcd->Setup);
}

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
    USBD_LL_DataOutStage((USBD_HandleTypeDef *)hpcd->pData, epnum,
                         hpcd->OUT_ep[epnum].xfer_buff);
}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
    USBD_LL_DataInStage((USBD_HandleTypeDef *)hpcd->pData, epnum,
                        hpcd->IN_ep[epnum].xfer_buff);
}

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd) {
    USBD_LL_SOF((USBD_HandleTypeDef *)hpcd->pData);
}

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd) {
    USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

    if (hpcd->Init.speed != PCD_SPEED_FULL) {
        // Error_Handler();
        return;
    }
    /* Set Speed. */
    USBD_LL_SetSpeed((USBD_HandleTypeDef *)hpcd->pData, speed);

    /* Reset Device. */
    USBD_LL_Reset((USBD_HandleTypeDef *)hpcd->pData);
}

void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd) {
    /* Inform USB library that core enters in suspend Mode. */
    USBD_LL_Suspend((USBD_HandleTypeDef *)hpcd->pData);
    /* Enter in STOP mode. */
    if (hpcd->Init.low_power_enable) {
        /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register.
         */
        SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk |
                                          SCB_SCR_SLEEPONEXIT_Msk));
    }
}

void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd) {
    if (hpcd->Init.low_power_enable) {
        /* Reset SLEEPDEEP bit of Cortex System Control Register. */
        SCB->SCR &= (uint32_t)~(
            (uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
        SystemClockConfig_Resume();
    }

    USBD_LL_Resume((USBD_HandleTypeDef *)hpcd->pData);
}

void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
    USBD_LL_IsoOUTIncomplete((USBD_HandleTypeDef *)hpcd->pData, epnum);
}

void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
    USBD_LL_IsoINIncomplete((USBD_HandleTypeDef *)hpcd->pData, epnum);
}

void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd) {
    USBD_LL_DevConnected((USBD_HandleTypeDef *)hpcd->pData);
}

void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd) {
    USBD_LL_DevDisconnected((USBD_HandleTypeDef *)hpcd->pData);
}

USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev) {
    /* Init USB Ip. */
    hpcd_USB_FS.pData = pdev;
    /* Link the driver to the stack. */
    pdev->pData = &hpcd_USB_FS;
    /* Enable USB power on Pwrctrl CR2 register. */
    hal_power_enable_vdd_usb();

    hpcd_USB_FS.Instance = USB;
    hpcd_USB_FS.Init.dev_endpoints = 8;
    hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
    hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
    hpcd_USB_FS.Init.Sof_enable = DISABLE;
    hpcd_USB_FS.Init.low_power_enable = DISABLE;
    hpcd_USB_FS.Init.lpm_enable = DISABLE;
    hpcd_USB_FS.Init.battery_charging_enable = DISABLE;

    if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK) {
        return 24; // TODO
    }

    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef *)pdev->pData, 0x00, PCD_SNG_BUF,
                        0x0C);
    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef *)pdev->pData, 0x80, PCD_SNG_BUF,
                        0x4C);
    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef *)pdev->pData, HID_EPIN_ADDR,
                        PCD_SNG_BUF, 0x8C);
    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef *)pdev->pData, VEND_HID_EPIN_ADDR,
                        PCD_SNG_BUF, 0xCC);
    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef *)pdev->pData, VEND_HID_EPOUT_ADDR,
                        PCD_SNG_BUF, 0x10C);
    return OK;
}

USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev) {
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_DeInit(pdev->pData);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev) {
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_Start(pdev->pData);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev) {
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_Stop(pdev->pData);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr,
                                  uint8_t ep_type, uint16_t ep_mps) {
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Close(pdev->pData, ep_addr);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Flush(pdev->pData, ep_addr);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_SetStall(pdev->pData, ep_addr);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev,
                                        uint8_t ep_addr) {
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
    PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef *)pdev->pData;

    if ((ep_addr & 0x80) == 0x80) {
        return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
    } else {
        return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
    }
}

USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev,
                                         uint8_t dev_addr) {
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_SetAddress(pdev->pData, dev_addr);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr,
                                    uint8_t *pbuf, uint32_t size) {
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev,
                                          uint8_t ep_addr, uint8_t *pbuf,
                                          uint32_t size) {
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
    return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef *)pdev->pData, ep_addr);
}

void USBD_LL_Delay(uint32_t Delay) { systick_delay(Delay); }

void *USBD_static_malloc(uint32_t size) {
    UNUSED(size);
    static uint32_t
        mem[(sizeof(USBD_HID_HandleTypeDef) / 4) + 1]; /* On 32-bit boundary */
    return mem;
}

void USBD_static_free(void *p) { UNUSED(p); }

void SystemClockConfig_Resume(void) {
    ERR_H(setup_clock());
    ERR_H(setup_crs());
}

USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status) {
    USBD_StatusTypeDef usb_status = USBD_OK;

    switch (hal_status) {
    case HAL_OK:
        usb_status = USBD_OK;
        break;
    case HAL_ERROR:
        usb_status = USBD_FAIL;
        break;
    case HAL_BUSY:
        usb_status = USBD_BUSY;
        break;
    case HAL_TIMEOUT:
        usb_status = USBD_FAIL;
        break;
    default:
        usb_status = USBD_FAIL;
        break;
    }
    return usb_status;
}
