#include "usb/usbd_hid.h"

#include "usb/usbd_ctlreq.h"
#include "utils/utils.h"

static uint8_t USBD_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_HID_Setup(USBD_HandleTypeDef *pdev,
                              USBD_SetupReqTypedef *req);
static uint8_t USBD_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t *USBD_HID_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_HID_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_HID_GetOtherSpeedCfgDesc(uint16_t *length);
static uint8_t *USBD_HID_GetDeviceQualifierDesc(uint16_t *length);

USBD_ClassTypeDef USBD_HID = {
    USBD_HID_Init,
    USBD_HID_DeInit,
    USBD_HID_Setup,
    NULL,            /* EP0_TxSent */
    NULL,            /* EP0_RxReady */
    USBD_HID_DataIn, /* DataIn */
    NULL,            /* DataOut */
    NULL,            /* SOF */
    NULL,
    NULL,
    USBD_HID_GetHSCfgDesc,
    USBD_HID_GetFSCfgDesc,
    USBD_HID_GetOtherSpeedCfgDesc,
    USBD_HID_GetDeviceQualifierDesc,
};

/* USB HID device FS Configuration Descriptor */
__ALIGN_BEGIN static uint8_t
    USBD_HID_CfgDesc[USB_HID_CONFIG_DESC_SIZ] __ALIGN_END = {
        0x09, /* bLength: Configuration Descriptor size */
        USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
        USB_HID_CONFIG_DESC_SIZ,     /* wTotalLength: Bytes returned */
        0x00, 0x01,                  /* bNumInterfaces: 1 interface */
        0x01, /* bConfigurationValue: Configuration value */
        0x00, /* iConfiguration: Index of string descriptor
                 describing the configuration */
        0xE0, /* bmAttributes: Bus Powered according to user configuration */
        USBD_MAX_POWER, /* MaxPower (mA) */

        /************** Descriptor of Joystick Mouse interface ****************/
        /* 09 */
        0x09,                    /* bLength: Interface Descriptor size */
        USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type
                                  */
        0x00,                    /* bInterfaceNumber: Number of Interface */
        0x00,                    /* bAlternateSetting: Alternate setting */
        0x01,                    /* bNumEndpoints */
        0x03,                    /* bInterfaceClass: HID */
        0x01,                    /* bInterfaceSubClass : 1=BOOT, 0=no boot */
        0x01, /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
        0,    /* iInterface: Index of string descriptor */
        /******************** Descriptor of Joystick Mouse HID
         ********************/
        /* 18 */
        0x09,                /* bLength: HID Descriptor size */
        HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
        0x11,                /* bcdHID: HID Class Spec release number */
        0x01, 0x00,          /* bCountryCode: Hardware target country */
        0x01, /* bNumDescriptors: Number of HID class descriptors to follow */
        0x22, /* bDescriptorType */
        HID_KB_REPORT_DESC_SIZE, /* wItemLength: Total length of Report
                                       descriptor */
        0x00,
        /******************** Descriptor of Mouse endpoint ********************/
        /* 27 */
        0x07,                   /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT, /* bDescriptorType:*/

        HID_EPIN_ADDR,          /* bEndpointAddress: Endpoint Address (IN) */
        0x03,                   /* bmAttributes: Interrupt endpoint */
        HID_EPIN_SIZE,          /* wMaxPacketSize: 4 Bytes max */
        0x00, HID_FS_BINTERVAL, /* bInterval: Polling Interval */
                                /* 34 */
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

static uint8_t HIDInEpAdd = HID_EPIN_ADDR;

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
        pdev->ep_in[HIDInEpAdd & 0xFU].bInterval = HID_HS_BINTERVAL;
    } else /* LOW and FULL-speed endpoints */
    {
        pdev->ep_in[HIDInEpAdd & 0xFU].bInterval = HID_FS_BINTERVAL;
    }

    /* Open EP IN */
    (void)USBD_LL_OpenEP(pdev, HIDInEpAdd, USBD_EP_TYPE_INTR, HID_EPIN_SIZE);
    pdev->ep_in[HIDInEpAdd & 0xFU].is_used = 1U;

    hhid->state = USBD_HID_IDLE;

    return (uint8_t)USBD_OK;
}

static uint8_t USBD_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    UNUSED(cfgidx);

    /* Close HID EPs */
    (void)USBD_LL_CloseEP(pdev, HIDInEpAdd);
    pdev->ep_in[HIDInEpAdd & 0xFU].is_used = 0U;
    pdev->ep_in[HIDInEpAdd & 0xFU].bInterval = 0U;

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
                len = MIN(HID_KB_REPORT_DESC_SIZE, req->wLength);
                pbuf = HID_KB_ReportDesc;
            } else if ((req->wValue >> 8) == HID_DESCRIPTOR_TYPE) {
                pbuf = USBD_HID_Desc;
                len = MIN(USB_HID_DESC_SIZ, req->wLength);
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

uint8_t USBD_HID_SendReport(USBD_HandleTypeDef *pdev, uint8_t *report,
                            uint16_t len) {
    USBD_HID_HandleTypeDef *hhid =
        (USBD_HID_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

    if (hhid == NULL) {
        return (uint8_t)USBD_FAIL;
    }

    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
        if (hhid->state == USBD_HID_IDLE) {
            hhid->state = USBD_HID_BUSY;
            (void)USBD_LL_Transmit(pdev, HIDInEpAdd, report, len);
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
    UNUSED(epnum);
    /* Ensure that the FIFO is empty before a new transfer, this condition could
    be caused by  a new transfer before the end of the previous transfer */
    ((USBD_HID_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId])->state =
        USBD_HID_IDLE;

    return (uint8_t)USBD_OK;
}

static uint8_t *USBD_HID_GetDeviceQualifierDesc(uint16_t *length) {
    *length = (uint16_t)sizeof(USBD_HID_DeviceQualifierDesc);

    return USBD_HID_DeviceQualifierDesc;
}
