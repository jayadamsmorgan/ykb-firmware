#include "usb/usbd_hid.h"

#include "hal/systick.h"
#include "logging.h"
#include "usb/usbd_conf.h"
#include "usb/usbd_core.h"
#include "usb/usbd_ctlreq.h"
#include "usb/usbd_def.h"
#include "utils/utils.h"
#include <stdint.h>
#include <string.h>

static uint8_t USBD_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_HID_Setup(USBD_HandleTypeDef *pdev,
                              USBD_SetupReqTypedef *req);
static uint8_t USBD_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_HID_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t *USBD_HID_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_HID_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_HID_GetOtherSpeedCfgDesc(uint16_t *length);
static uint8_t *USBD_HID_GetDeviceQualifierDesc(uint16_t *length);

USBD_ClassTypeDef USBD_HID = {
    USBD_HID_Init,
    USBD_HID_DeInit,
    USBD_HID_Setup,
    NULL,             /* EP0_TxSent */
    NULL,             /* EP0_RxReady */
    USBD_HID_DataIn,  /* DataIn */
    USBD_HID_DataOut, /* DataOut */
    NULL,             /* SOF */
    NULL,
    NULL,
    USBD_HID_GetHSCfgDesc,
    USBD_HID_GetFSCfgDesc,
    USBD_HID_GetOtherSpeedCfgDesc,
    USBD_HID_GetDeviceQualifierDesc,
};

__ALIGN_BEGIN static uint8_t
    USBD_HID_CfgDesc[USB_HID_CONFIG_DESC_SIZ] __ALIGN_END = {
        /* --------------- configuration header ---------------- */
        0x09,
        USB_DESC_TYPE_CONFIGURATION, /* bLength, bDescriptorType        */
        LOBYTE(USB_HID_CONFIG_DESC_SIZ),
        HIBYTE(USB_HID_CONFIG_DESC_SIZ),
        0x02, /* bNumInterfaces                               */
        0x01,
        0x00,
        0xE0, /* bConfigurationValue, iConfig, bmAttributes   */
        USBD_MAX_POWER,

        /* -------- Interface 0 : Boot‑Keyboard (unchanged) ----- */
        0x09,
        USB_DESC_TYPE_INTERFACE,
        0x00,
        0x00, /* bInterfaceNumber =0, bAlternate =0   */
        0x01, /* bNumEndpoints = 1                    */
        0x03,
        0x01,
        0x01, /* class HID, sub‑class BOOT, proto KB  */
        0x00, /* iInterface                           */

        /* HID descriptor (9 bytes) */
        0x09,
        HID_DESCRIPTOR_TYPE,
        0x11,
        0x01, /* bcdHID = 1.11                        */
        0x00, /* bCountryCode                         */
        0x01, /* bNumDescriptors                      */
        0x22,
        LOBYTE(HID_KB_REPORT_DESC_SIZE),
        HIBYTE(HID_KB_REPORT_DESC_SIZE),

        /* Endpoint descriptor (7 bytes) – IN 0x81 */
        0x07,
        USB_DESC_TYPE_ENDPOINT,
        HID_EPIN_ADDR,
        0x03, /* Interrupt IN                         */
        HID_EPIN_SIZE,
        0x00,
        HID_FS_BINTERVAL,

        /* -------- Interface 1 : Vendor HID channel ------------ */
        /* Interface descriptor */
        0x09,
        USB_DESC_TYPE_INTERFACE,
        0x01,
        0x00, /* bInterfaceNumber, bAlternateSetting */
        0x02,
        0x03,
        0x00,
        0x00,
        0x00, /* 2 EPs, class HID, no proto          */

        /* HID descriptor */
        0x09,
        HID_DESCRIPTOR_TYPE,
        0x11,
        0x01,
        0x00,
        0x01,
        0x22, /* one report descriptor follows       */
        VEND_HID_REPORT_DESC_SIZE,
        0x00,

        /* IN endpoint 0x82 */
        0x07,
        USB_DESC_TYPE_ENDPOINT,
        VEND_HID_EPIN_ADDR,
        0x03,
        VEND_HID_EPSIZE,
        0x00,
        HID_FS_BINTERVAL,

        /* OUT endpoint 0x02 */
        0x07,
        USB_DESC_TYPE_ENDPOINT,
        VEND_HID_EPOUT_ADDR,
        0x03,
        VEND_HID_EPSIZE,
        0x00,
        HID_FS_BINTERVAL,
};

/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_Desc[USB_HID_DESC_SIZ] __ALIGN_END = {
    /* 18 */
    0x09,                /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
    0x11,                /* bcdHID: HID Class Spec release number */
    0x01,
    0x00, /* bCountryCode: Hardware target country */
    0x01, /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22, /* bDescriptorType */
    HID_KB_REPORT_DESC_SIZE, /* wItemLength: Total length of Report
                                   descriptor */
    0x00,
};

/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t
    USBD_VEND_HID_Desc[USB_VEND_HID_DESC_SIZ] __ALIGN_END = {
        /* 18 */
        0x09,                /* bLength: HID Descriptor size */
        HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
        0x11,                /* bcdHID: HID Class Spec release number */
        0x01,
        0x00, /* bCountryCode: Hardware target country */
        0x01, /* bNumDescriptors: Number of HID class descriptors to follow */
        0x22, /* bDescriptorType */
        VEND_HID_REPORT_DESC_SIZE, /* wItemLength: Total length of Report
                                       descriptor */
        0x00,
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t
    USBD_HID_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
        USB_LEN_DEV_QUALIFIER_DESC,
        USB_DESC_TYPE_DEVICE_QUALIFIER,
        0x00,
        0x02,
        0x00,
        0x00,
        0x00,
        0x40,
        0x01,
        0x00,
};

__ALIGN_BEGIN static uint8_t
    HID_KB_ReportDesc[HID_KB_REPORT_DESC_SIZE] __ALIGN_END = {
        0x05, 0x01, /* Usage Page (Generic Desktop Ctrls)         */
        0x09, 0x06, /* Usage (Keyboard)                            */
        0xA1, 0x01, /* Collection (Application)                    */

        /* Modifier byte (8 bits) */
        0x05, 0x07, /*   Usage Page (Kbrd/Keypad)                  */
        0x19, 0xE0, /*   Usage Minimum (0xE0) [Left Ctrl]          */
        0x29, 0xE7, /*   Usage Maximum (0xE7) [Right GUI]          */
        0x15, 0x00, /*   Logical Minimum (0)                       */
        0x25, 0x01, /*   Logical Maximum (1)                       */
        0x75, 0x01, /*   Report Size (1)                           */
        0x95, 0x08, /*   Report Count (8)                          */
        0x81, 0x02, /*   Input (Data,Var,Abs) -- Modifier bits     */

        /* Reserved byte (1 byte) */
        0x95, 0x01, /*   Report Count (1)                          */
        0x75, 0x08, /*   Report Size (8)                           */
        0x81, 0x01, /*   Input (Const,Var,Abs) -- Reserved         */

        /* LED report (Num Lock, Caps Lock, etc.) */
        0x95, 0x05, /*   Report Count (5)                          */
        0x75, 0x01, /*   Report Size (1)                           */
        0x05, 0x08, /*   Usage Page (LEDs)                         */
        0x19, 0x01, /*   Usage Minimum (Num Lock)                  */
        0x29, 0x05, /*   Usage Maximum (Kana)                      */
        0x91, 0x02, /*   Output (Data,Var,Abs) -- LED states       */

        /* LED report padding */
        0x95, 0x01, /*   Report Count (1)                          */
        0x75, 0x03, /*   Report Size (3)                           */
        0x91, 0x01, /*   Output (Const,Var,Abs) -- LED padding     */

        /* Key array (6 keys) */
        0x95, 0x06, /*   Report Count (6)                          */
        0x75, 0x08, /*   Report Size (8)                           */
        0x15, 0x00, /*   Logical Minimum (0)                       */
        0x25, 0x65, /*   Logical Maximum (101)                     */
        0x05, 0x07, /*   Usage Page (Kbrd/Keypad)                  */
        0x19, 0x00, /*   Usage Minimum (0)                         */
        0x29, 0x65, /*   Usage Maximum (101)                       */
        0x81, 0x00, /*   Input (Data,Array,Abs) -- Keycodes        */

        0xC0 /* End Collection                              */
};

__ALIGN_BEGIN static uint8_t
    VEND_HID_ReportDesc[VEND_HID_REPORT_DESC_SIZE] __ALIGN_END = {
        0x06, 0x00, 0xFF, // Usage Page (Vendor-defined 0xFF00)
        0x09, 0x01,       // Usage (Vendor-defined)
        0xA1, 0x01,       // Collection (Application)
        0x85, 0x01,       //   REPORT_ID (1)
        0x15, 0x00, 0x26, 0xFF, 0x00, 0x75, 0x08, 0x95, 0x40, //   64-byte INPUT
        0x09, 0x01, 0x81, 0x02, //   Input (Data,Var,Abs)

        0x85, 0x01,             //   REPORT_ID (1) again
        0x95, 0x40, 0x75, 0x08, //   64-byte OUTPUT
        0x09, 0x01, 0x91, 0x02, //   Output (Data,Var,Abs)
        0xC0                    // End Collection
};

uint8_t vendRxBuf[64];

static uint8_t USBD_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    UNUSED(cfgidx);

    USBD_HID_HandleTypeDef *hhid;

    hhid =
        (USBD_HID_HandleTypeDef *)USBD_malloc(sizeof(USBD_HID_HandleTypeDef));

    if (hhid == NULL) {
        pdev->pClassDataCmsit[pdev->classId] = NULL;
        return (uint8_t)USBD_EMEM;
    }

    pdev->pClassDataCmsit[pdev->classId] = (void *)hhid;
    pdev->pClassData = pdev->pClassDataCmsit[pdev->classId];

    if (pdev->dev_speed == USBD_SPEED_HIGH) {
        pdev->ep_in[HID_EPIN_ADDR & 0xFU].bInterval = HID_HS_BINTERVAL;
    } else /* LOW and FULL-speed endpoints */
    {
        pdev->ep_in[HID_EPIN_ADDR & 0xFU].bInterval = HID_FS_BINTERVAL;
    }

    /* Open EP IN */
    (void)USBD_LL_OpenEP(pdev, HID_EPIN_ADDR, USBD_EP_TYPE_INTR, HID_EPIN_SIZE);
    pdev->ep_in[HID_EPIN_ADDR & 0xFU].is_used = 1U;

    pdev->ep_in[VEND_HID_EPIN_ADDR & 0xFU].bInterval = HID_FS_BINTERVAL;
    pdev->ep_out[VEND_HID_EPOUT_ADDR & 0xFU].bInterval = HID_FS_BINTERVAL;
    USBD_LL_OpenEP(pdev, VEND_HID_EPIN_ADDR, USBD_EP_TYPE_BULK,
                   VEND_HID_EPSIZE);
    USBD_LL_OpenEP(pdev, VEND_HID_EPOUT_ADDR, USBD_EP_TYPE_BULK,
                   VEND_HID_EPSIZE);

    pdev->ep_in[VEND_HID_EPIN_ADDR & 0xFU].is_used = 1U;
    pdev->ep_out[VEND_HID_EPOUT_ADDR & 0xFU].is_used = 1U;

    USBD_LL_PrepareReceive(pdev, VEND_HID_EPOUT_ADDR, vendRxBuf,
                           VEND_HID_EPSIZE);

    hhid->kb_state = USBD_HID_IDLE;
    hhid->vend_state = USBD_HID_IDLE;

    return (uint8_t)USBD_OK;
}

static uint8_t USBD_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    UNUSED(cfgidx);

    /* Close HID EPs */
    (void)USBD_LL_CloseEP(pdev, HID_EPIN_ADDR);
    pdev->ep_in[HID_EPIN_ADDR & 0xFU].is_used = 0U;
    pdev->ep_in[HID_EPIN_ADDR & 0xFU].bInterval = 0U;

    USBD_LL_CloseEP(pdev, VEND_HID_EPIN_ADDR);
    pdev->ep_in[VEND_HID_EPIN_ADDR & 0xFU].is_used = 0U;
    pdev->ep_in[VEND_HID_EPIN_ADDR & 0xFU].bInterval = 0U;

    USBD_LL_CloseEP(pdev, VEND_HID_EPOUT_ADDR);
    pdev->ep_out[VEND_HID_EPOUT_ADDR & 0xFU].is_used = 0U;
    pdev->ep_out[VEND_HID_EPOUT_ADDR & 0xFU].bInterval = 0U;

    /* Free allocated memory */
    if (pdev->pClassDataCmsit[pdev->classId] != NULL) {
        (void)USBD_free(pdev->pClassDataCmsit[pdev->classId]);
        pdev->pClassDataCmsit[pdev->classId] = NULL;
    }

    return (uint8_t)USBD_OK;
}

static uint8_t USBD_HID_Setup(USBD_HandleTypeDef *pdev,
                              USBD_SetupReqTypedef *req) {
    USBD_HID_HandleTypeDef *hhid =
        (USBD_HID_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];
    USBD_StatusTypeDef ret = USBD_OK;
    uint16_t len;
    uint8_t *pbuf;
    uint16_t status_info = 0U;

    if (hhid == NULL) {
        return (uint8_t)USBD_FAIL;
    }

    uint8_t if_num = (uint8_t)req->wIndex;

    switch (req->bmRequest & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_CLASS:
        switch (req->bRequest) {
        case USBD_HID_REQ_SET_PROTOCOL:
            hhid->Protocol = (uint8_t)(req->wValue);
            break;

        case USBD_HID_REQ_GET_PROTOCOL:
            (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->Protocol, 1U);
            break;

        case USBD_HID_REQ_SET_IDLE:
            hhid->IdleState = (uint8_t)(req->wValue >> 8);
            break;

        case USBD_HID_REQ_GET_IDLE:
            (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->IdleState, 1U);
            break;

        default:
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
            break;
        }
        break;
    case USB_REQ_TYPE_STANDARD:
        switch (req->bRequest) {
        case USB_REQ_GET_STATUS:
            if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                (void)USBD_CtlSendData(pdev, (uint8_t *)&status_info, 2U);
            } else {
                USBD_CtlError(pdev, req);
                ret = USBD_FAIL;
            }
            break;

        case USB_REQ_GET_DESCRIPTOR:
            if ((req->wValue >> 8) == HID_REPORT_DESC) {
                if (if_num == 0) {
                    len = MIN(HID_KB_REPORT_DESC_SIZE, req->wLength);
                    pbuf = HID_KB_ReportDesc;
                } else if (if_num == 1) {
                    len = MIN(VEND_HID_REPORT_DESC_SIZE, req->wLength);
                    pbuf = VEND_HID_ReportDesc;
                } else {
                    USBD_CtlError(pdev, req);
                    ret = USBD_FAIL;
                    break;
                }
            } else if ((req->wValue >> 8) == HID_DESCRIPTOR_TYPE) {
                if (if_num == 0) {
                    len = MIN(USB_HID_DESC_SIZ, req->wLength);
                    pbuf = USBD_HID_Desc;
                } else if (if_num == 1) {
                    len = MIN(USB_VEND_HID_DESC_SIZ, req->wLength);
                    pbuf = USBD_VEND_HID_Desc;
                } else {
                    USBD_CtlError(pdev, req);
                    ret = USBD_FAIL;
                    break;
                }
            } else {
                USBD_CtlError(pdev, req);
                ret = USBD_FAIL;
                break;
            }
            (void)USBD_CtlSendData(pdev, pbuf, len);
            break;

        case USB_REQ_GET_INTERFACE:
            if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->AltSetting, 1U);
            } else {
                USBD_CtlError(pdev, req);
                ret = USBD_FAIL;
            }
            break;

        case USB_REQ_SET_INTERFACE:
            if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                hhid->AltSetting = (uint8_t)(req->wValue);
            } else {
                USBD_CtlError(pdev, req);
                ret = USBD_FAIL;
            }
            break;

        case USB_REQ_CLEAR_FEATURE:
            break;

        default:
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
            break;
        }
        break;

    default:
        USBD_CtlError(pdev, req);
        ret = USBD_FAIL;
        break;
    }

    return (uint8_t)ret;
}

uint8_t USBD_HID_SendReport(USBD_HandleTypeDef *pdev, uint8_t ep_addr,
                            uint8_t *report, uint16_t len) {
    USBD_HID_HandleTypeDef *hhid =
        (USBD_HID_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

    if (hhid == NULL) {
        return (uint8_t)USBD_FAIL;
    }

    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
        if (ep_addr == HID_EPIN_ADDR) {
            if (hhid->kb_state == USBD_HID_IDLE) {
                hhid->kb_state = USBD_HID_BUSY;
                (void)USBD_LL_Transmit(pdev, ep_addr, report, len);
            }

        } else if (ep_addr == VEND_HID_EPIN_ADDR) {
            if (hhid->vend_state == USBD_HID_IDLE) {
                hhid->vend_state = USBD_HID_BUSY;
                (void)USBD_LL_Transmit(pdev, ep_addr, report, len);
            }
        }
    }

    return (uint8_t)USBD_OK;
}

uint32_t USBD_HID_GetPollingInterval(USBD_HandleTypeDef *pdev) {
    uint32_t polling_interval;

    /* HIGH-speed endpoints */
    if (pdev->dev_speed == USBD_SPEED_HIGH) {
        /* Sets the data transfer polling interval for high speed transfers.
         Values between 1..16 are allowed. Values correspond to interval
         of 2 ^ (bInterval-1). This option (8 ms, corresponds to
         HID_HS_BINTERVAL */
        polling_interval = (((1U << (HID_HS_BINTERVAL - 1U))) / 8U);
    } else /* LOW and FULL-speed endpoints */
    {
        /* Sets the data transfer polling interval for low and full
        speed transfers */
        polling_interval = HID_FS_BINTERVAL;
    }

    return ((uint32_t)(polling_interval));
}

static uint8_t *USBD_HID_GetFSCfgDesc(uint16_t *length) {
    USBD_EpDescTypeDef *pEpDesc =
        USBD_GetEpDesc(USBD_HID_CfgDesc, HID_EPIN_ADDR);

    if (pEpDesc != NULL) {
        pEpDesc->bInterval = HID_FS_BINTERVAL;
    }

    *length = (uint16_t)sizeof(USBD_HID_CfgDesc);
    return USBD_HID_CfgDesc;
}

static uint8_t *USBD_HID_GetHSCfgDesc(uint16_t *length) {
    USBD_EpDescTypeDef *pEpDesc =
        USBD_GetEpDesc(USBD_HID_CfgDesc, HID_EPIN_ADDR);

    if (pEpDesc != NULL) {
        pEpDesc->bInterval = HID_HS_BINTERVAL;
    }

    *length = (uint16_t)sizeof(USBD_HID_CfgDesc);
    return USBD_HID_CfgDesc;
}

static uint8_t *USBD_HID_GetOtherSpeedCfgDesc(uint16_t *length) {
    USBD_EpDescTypeDef *pEpDesc =
        USBD_GetEpDesc(USBD_HID_CfgDesc, HID_EPIN_ADDR);

    if (pEpDesc != NULL) {
        pEpDesc->bInterval = HID_FS_BINTERVAL;
    }

    *length = (uint16_t)sizeof(USBD_HID_CfgDesc);
    return USBD_HID_CfgDesc;
}

static uint8_t USBD_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum) {
    if (epnum == (HID_EPIN_ADDR & 0x7F)) {
        ((USBD_HID_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId])
            ->kb_state = USBD_HID_IDLE;
    } else if (epnum == (VEND_HID_EPIN_ADDR & 0x7F)) {
        ((USBD_HID_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId])
            ->vend_state = USBD_HID_IDLE;
    }

    return (uint8_t)USBD_OK;
}

static uint8_t USBD_HID_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum) {

    if (epnum == (VEND_HID_EPOUT_ADDR & 0x7F)) {
        /* vendRxBuf now holds the OUT report – process it here          */
        USBD_LL_PrepareReceive(pdev, VEND_HID_EPOUT_ADDR, vendRxBuf,
                               VEND_HID_EPSIZE);
    }

    return (uint8_t)USBD_OK;
}

static uint8_t *USBD_HID_GetDeviceQualifierDesc(uint16_t *length) {
    *length = (uint16_t)sizeof(USBD_HID_DeviceQualifierDesc);

    return USBD_HID_DeviceQualifierDesc;
}
