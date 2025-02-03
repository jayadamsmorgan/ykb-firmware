#include "usb/usb_hid.h"

#include "hal/usb.h"
#include "stm32wbxx.h"
#include "usb/usb_descriptors.h"
#include "usb/usb_device.h"
#include "utils/utils.h"
#include <stdint.h>

#define DESCRIPTOR_TYPE_POS 1
#define VENDOR_LOW_POS 8
#define VENDOR_HIGH_POS 9
#define PRODUCT_LOW_POS 10
#define PRODUCT_HIGH_POS 11

#define LANG_ID_LOW_POS 2
#define LANG_ID_HIGH_POS 3

#define HID_EPIN_ADDR 0x81U
#define HID_EPIN_SIZE 0x04U

#define HID_HS_BINTERVAL 0x07U
#define HID_FS_BINTERVAL 0xAU

#define USBD_EP_TYPE_INTR 0x03U

#define USB_REQ_TYPE_STANDARD 0x00U
#define USB_REQ_TYPE_CLASS 0x20U
#define USB_REQ_TYPE_VENDOR 0x40U
#define USB_REQ_TYPE_MASK 0x60U

#define USBD_HID_REQ_SET_PROTOCOL 0x0BU
#define USBD_HID_REQ_GET_PROTOCOL 0x03U
#define USBD_HID_REQ_SET_IDLE 0x0AU
#define USBD_HID_REQ_GET_IDLE 0x02U
#define USBD_HID_REQ_SET_REPORT 0x09U
#define USBD_HID_REQ_GET_REPORT 0x01U

#define USB_REQ_GET_STATUS 0x00U
#define USB_REQ_CLEAR_FEATURE 0x01U
#define USB_REQ_SET_FEATURE 0x03U
#define USB_REQ_SET_ADDRESS 0x05U
#define USB_REQ_GET_DESCRIPTOR 0x06U
#define USB_REQ_SET_DESCRIPTOR 0x07U
#define USB_REQ_GET_CONFIGURATION 0x08U
#define USB_REQ_SET_CONFIGURATION 0x09U
#define USB_REQ_GET_INTERFACE 0x0AU
#define USB_REQ_SET_INTERFACE 0x0BU
#define USB_REQ_SYNCH_FRAME 0x0CU

#define USBD_STATE_DEFAULT 0x01U
#define USBD_STATE_ADDRESSED 0x02U
#define USBD_STATE_CONFIGURED 0x03U
#define USBD_STATE_SUSPENDED 0x04U

#define HID_REPORT_DESC 0x22U
#define HID_MOUSE_REPORT_DESC_SIZE 74U
#define HID_DESCRIPTOR_TYPE 0x21U
#define USB_HID_DESC_SIZ 9U
#define USB_HID_CONFIG_DESC_SIZ 34U

#define USBD_MAX_POWER 0x32U /* 100 mA */

#define USB_LEN_DEV_QUALIFIER_DESC 0x0AU
#define USB_LEN_DEV_DESC 0x12U
#define USB_LEN_CFG_DESC 0x09U
#define USB_LEN_IF_DESC 0x09U
#define USB_LEN_EP_DESC 0x07U
#define USB_LEN_OTG_DESC 0x03U
#define USB_LEN_LANGID_STR_DESC 0x04U
#define USB_LEN_OTHER_SPEED_DESC_SIZ 0x09U

extern uint8_t usb_device_desc[USB_DEVICE_DESC_LENGTH] ALIGNED;

extern uint8_t usb_lang_id_desc[USB_LANG_DESC_LENGTH] ALIGNED;

extern uint8_t usb_manufacturer_str_desc[USB_MANUFACTURER_DESC_LENGTH] ALIGNED;
extern uint16_t usb_manufacturer_str_desc_length;

extern uint8_t usb_product_str_desc[USB_PRODUCT_DESC_LENGTH] ALIGNED;
extern uint16_t usb_product_str_desc_length;

extern uint8_t usb_serial_str_desc[USB_SERIAL_DESC_LENGTH] ALIGNED;

extern uint8_t usb_config_str_desc[USB_CONFIG_DESC_LENGTH] ALIGNED;
extern uint16_t usb_config_str_desc_length;

extern uint8_t usb_interface_str_desc[USB_INTERFACE_DESC_LENGTH] ALIGNED;
extern uint16_t usb_interface_str_desc_length;

static uint8_t USBD_HID_Desc[USB_HID_DESC_SIZ] ALIGNED = {
    /* 18 */
    0x09,                /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
    0x11,                /* bcdHID: HID Class Spec release number */
    0x01,
    0x00, /* bCountryCode: Hardware target country */
    0x01, /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22, /* bDescriptorType */
    HID_MOUSE_REPORT_DESC_SIZE, /* wItemLength: Total length of Report
                                   descriptor */
    0x00,
};

static uint8_t HID_MOUSE_ReportDesc[HID_MOUSE_REPORT_DESC_SIZE] ALIGNED = {
    0x05, 0x01, /* Usage Page (Generic Desktop Ctrls)     */
    0x09, 0x02, /* Usage (Mouse)                          */
    0xA1, 0x01, /* Collection (Application)               */
    0x09, 0x01, /*   Usage (Pointer)                      */
    0xA1, 0x00, /*   Collection (Physical)                */
    0x05, 0x09, /*     Usage Page (Button)                */
    0x19, 0x01, /*     Usage Minimum (0x01)               */
    0x29, 0x03, /*     Usage Maximum (0x03)               */
    0x15, 0x00, /*     Logical Minimum (0)                */
    0x25, 0x01, /*     Logical Maximum (1)                */
    0x95, 0x03, /*     Report Count (3)                   */
    0x75, 0x01, /*     Report Size (1)                    */
    0x81, 0x02, /*     Input (Data,Var,Abs)               */
    0x95, 0x01, /*     Report Count (1)                   */
    0x75, 0x05, /*     Report Size (5)                    */
    0x81, 0x01, /*     Input (Const,Array,Abs)            */
    0x05, 0x01, /*     Usage Page (Generic Desktop Ctrls) */
    0x09, 0x30, /*     Usage (X)                          */
    0x09, 0x31, /*     Usage (Y)                          */
    0x09, 0x38, /*     Usage (Wheel)                      */
    0x15, 0x81, /*     Logical Minimum (-127)             */
    0x25, 0x7F, /*     Logical Maximum (127)              */
    0x75, 0x08, /*     Report Size (8)                    */
    0x95, 0x03, /*     Report Count (3)                   */
    0x81, 0x06, /*     Input (Data,Var,Rel)               */
    0xC0,       /*   End Collection                       */
    0x09, 0x3C, /*   Usage (Motion Wakeup)                */
    0x05, 0xFF, /*   Usage Page (Reserved 0xFF)           */
    0x09, 0x01, /*   Usage (0x01)                         */
    0x15, 0x00, /*   Logical Minimum (0)                  */
    0x25, 0x01, /*   Logical Maximum (1)                  */
    0x75, 0x01, /*   Report Size (1)                      */
    0x95, 0x02, /*   Report Count (2)                     */
    0xB1, 0x22, /*   Feature (Data,Var,Abs,NoWrp)         */
    0x75, 0x06, /*   Report Size (6)                      */
    0x95, 0x01, /*   Report Count (1)                     */
    0xB1, 0x01, /*   Feature (Const,Array,Abs,NoWrp)      */
    0xC0        /* End Collection                         */
};

typedef enum {
    USBD_HID_IDLE = 0,
    USBD_HID_BUSY,
} usb_hid_state;

typedef struct {
    uint32_t protocol;
    uint32_t idle_state;
    uint32_t alt_setting;
    usb_hid_state state;
} usb_hid_handle_t;

static inline void *usb_hid_static_malloc() {
    static uint32_t
        mem[(sizeof(usb_hid_handle_t) / 4) + 1]; /* On 32-bit boundary */
    return mem;
}

static inline void usb_hid_static_free(void *ptr) { UNUSED(ptr); }

static uint8_t usb_hid_class_init(usb_device_handle_t *pdev, uint8_t cfgidx) {
    UNUSED(cfgidx);

    usb_hid_handle_t *hhid;

    hhid = (usb_hid_handle_t *)usb_hid_static_malloc();

    if (!hhid) {
        pdev->p_class_data_cmsit[pdev->class_id] = NULL;
        return USBD_EMEM;
    }

    pdev->p_class_data_cmsit[pdev->class_id] = (void *)hhid;
    pdev->p_class_data = pdev->p_class_data_cmsit[pdev->class_id];

    if (pdev->dev_speed == USB_DEVICE_SPEED_HIGH) {
        pdev->ep_in[HID_EPIN_ADDR & 0xFU].b_interval = HID_HS_BINTERVAL;
    } else /* LOW and FULL-speed endpoints */
    {
        pdev->ep_in[HID_EPIN_ADDR & 0xFU].b_interval = HID_FS_BINTERVAL;
    }

    /* Open EP IN */
    if (usb_device_open_ep(pdev->p_data, HID_EPIN_ADDR, USBD_EP_TYPE_INTR,
                           HID_EPIN_SIZE)) {
        return USBD_FAIL;
    }
    pdev->ep_in[HID_EPIN_ADDR & 0xFU].is_used = 1U;

    hhid->state = USBD_HID_IDLE;

    return USBD_OK;
}

static uint8_t usb_hid_class_deinit(usb_device_handle_t *pdev, uint8_t cfgidx) {
    UNUSED(cfgidx);

    usb_device_close_ep(pdev->p_data, HID_EPIN_ADDR);

    pdev->ep_in[HID_EPIN_ADDR & 0xFU].is_used = 0U;
    pdev->ep_in[HID_EPIN_ADDR & 0xFU].b_interval = 0U;

    /* Free allocated memory */
    if (pdev->p_class_data_cmsit[pdev->class_id] != NULL) {
        usb_hid_static_free(pdev->p_class_data_cmsit[pdev->class_id]);
        pdev->p_class_data_cmsit[pdev->class_id] = NULL;
    }

    return USBD_OK;
}

static uint8_t usb_hid_class_setup(usb_device_handle_t *pdev,
                                   usb_device_setup_request_t *req) {
    usb_hid_handle_t *hhid =
        (usb_hid_handle_t *)pdev->p_class_data_cmsit[pdev->class_id];

    if (!hhid) {
        return USBD_FAIL;
    }

    uint16_t len;
    uint8_t *pbuf;
    uint16_t status_info = 0U;

    switch (req->bm_request & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_CLASS:
        switch (req->b_request) {
        case USBD_HID_REQ_SET_PROTOCOL:
            hhid->protocol = (uint8_t)(req->w_value);
            break;

        case USBD_HID_REQ_GET_PROTOCOL:
            usb_device_send_data(pdev, (uint8_t *)&hhid->protocol, 1U);
            break;

        case USBD_HID_REQ_SET_IDLE:
            hhid->idle_state = (uint8_t)(req->w_value >> 8);
            break;

        case USBD_HID_REQ_GET_IDLE:
            usb_device_send_data(pdev, (uint8_t *)&hhid->idle_state, 1U);
            break;

        default:
            usb_device_ctrl_error(pdev);
            return USBD_FAIL;
        }
        break;
    case USB_REQ_TYPE_STANDARD:
        switch (req->b_request) {
        case USB_REQ_GET_STATUS:
            if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                (void)usb_device_send_data(pdev, (uint8_t *)&status_info, 2U);
            } else {
                usb_device_ctrl_error(pdev);
                return USBD_FAIL;
            }
            break;

        case USB_REQ_GET_DESCRIPTOR:
            if ((req->w_value >> 8) == HID_REPORT_DESC) {
                len = MIN(HID_MOUSE_REPORT_DESC_SIZE, req->w_length);
                pbuf = HID_MOUSE_ReportDesc;
            } else if ((req->w_value >> 8) == HID_DESCRIPTOR_TYPE) {
                pbuf = USBD_HID_Desc;
                len = MIN(USB_HID_DESC_SIZ, req->w_length);
            } else {
                usb_device_ctrl_error(pdev);
                return USBD_FAIL;
                break;
            }
            (void)usb_device_send_data(pdev, pbuf, len);
            break;

        case USB_REQ_GET_INTERFACE:
            if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                (void)usb_device_send_data(pdev, (uint8_t *)&hhid->alt_setting,
                                           1U);
            } else {
                usb_device_ctrl_error(pdev);
                return USBD_FAIL;
            }
            break;

        case USB_REQ_SET_INTERFACE:
            if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                hhid->alt_setting = (uint8_t)(req->w_value);
            } else {
                usb_device_ctrl_error(pdev);
                return USBD_FAIL;
            }
            break;

        case USB_REQ_CLEAR_FEATURE:
            break;

        default:
            usb_device_ctrl_error(pdev);
            return USBD_FAIL;
            break;
        }
        break;

    default:
        usb_device_ctrl_error(pdev);
        return USBD_FAIL;
        break;
    }

    return USBD_OK;
}

static uint8_t usb_hid_class_data_in(usb_device_handle_t *pdev, uint8_t epnum) {
    UNUSED(epnum);
    /* Ensure that the FIFO is empty before a new transfer, this condition could
    be caused by  a new transfer before the end of the previous transfer */
    ((usb_hid_handle_t *)pdev->p_class_data_cmsit[pdev->class_id])->state =
        USBD_HID_IDLE;

    return (uint8_t)USBD_OK;
}

static uint8_t usb_hid_cfg_desc[USB_HID_CONFIG_DESC_SIZ] ALIGNED = {
    0x09,                        /* bLength: Configuration Descriptor size */
    USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
    USB_HID_CONFIG_DESC_SIZ,     /* wTotalLength: Bytes returned */
    0x00, 0x01,                  /* bNumInterfaces: 1 interface */
    0x01,                        /* bConfigurationValue: Configuration value */
    0x00,                        /* iConfiguration: Index of string descriptor
                                    describing the configuration */
    0xE0, /* bmAttributes: Bus Powered according to user configuration */
    USBD_MAX_POWER, /* MaxPower (mA) */

    /************** Descriptor of Joystick Mouse interface ****************/
    /* 09 */
    0x09,                    /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type */
    0x00,                    /* bInterfaceNumber: Number of Interface */
    0x00,                    /* bAlternateSetting: Alternate setting */
    0x01,                    /* bNumEndpoints */
    0x03,                    /* bInterfaceClass: HID */
    0x01,                    /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x02, /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,    /* iInterface: Index of string descriptor */
    /******************** Descriptor of Joystick Mouse HID ********************/
    /* 18 */
    0x09,                /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
    0x11,                /* bcdHID: HID Class Spec release number */
    0x01, 0x00,          /* bCountryCode: Hardware target country */
    0x01, /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22, /* bDescriptorType */
    HID_MOUSE_REPORT_DESC_SIZE, /* wItemLength: Total length of Report
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

typedef struct {
    uint8_t b_length;
    uint8_t b_descriptor_type;
    uint8_t b_descriptor_sub_type;
} usb_device_desc_header_t;

typedef struct {
    uint8_t b_length;
    uint8_t b_descriptor_type;
    uint16_t w_total_length;
    uint8_t b_num_interfaces;
    uint8_t b_configuration_value;
    uint8_t i_configuration;
    uint8_t bm_attributes;
    uint8_t b_max_power;
} __PACKED usb_device_config_desc_t;

typedef struct {
    uint8_t b_length;
    uint8_t b_descriptor_type;
    uint8_t b_endpoint_address;
    uint8_t bm_attributes;
    uint16_t w_max_packet_size;
    uint8_t b_interval;
} __PACKED usb_device_ep_desc_t;

usb_device_desc_header_t *USBD_GetNextDesc(uint8_t *pbuf, uint16_t *ptr) {
    usb_device_desc_header_t *pnext = (usb_device_desc_header_t *)(void *)pbuf;

    *ptr += pnext->b_length;
    pnext = (usb_device_desc_header_t *)(void *)(pbuf + pnext->b_length);

    return pnext;
}

void *usb_device_get_endpoint_desc(uint8_t *pConfDesc, uint8_t EpAddr) {
    usb_device_desc_header_t *pdesc =
        (usb_device_desc_header_t *)(void *)pConfDesc;
    usb_device_config_desc_t *desc =
        (usb_device_config_desc_t *)(void *)pConfDesc;
    usb_device_ep_desc_t *pEpDesc = NULL;
    uint16_t ptr;

    if (desc->w_total_length > desc->b_length) {
        ptr = desc->b_length;

        while (ptr < desc->w_total_length) {
            pdesc = USBD_GetNextDesc((uint8_t *)pdesc, &ptr);

            if (pdesc->b_descriptor_type == USB_DESC_TYPE_ENDPOINT) {
                pEpDesc = (usb_device_ep_desc_t *)(void *)pdesc;

                if (pEpDesc->b_endpoint_address == EpAddr) {
                    break;
                } else {
                    pEpDesc = NULL;
                }
            }
        }
    }
    return (void *)pEpDesc;
}

static uint8_t *usb_hid_class_get_hsc_cfg_desc(uint16_t *length) {
    usb_device_endpoint_t *pEpDesc =
        usb_device_get_endpoint_desc(usb_hid_cfg_desc, HID_EPIN_ADDR);

    if (pEpDesc != NULL) {
        pEpDesc->b_interval = HID_HS_BINTERVAL;
    }

    *length = (uint16_t)sizeof(usb_hid_cfg_desc);
    return usb_hid_cfg_desc;
}

static uint8_t *usb_hid_class_get_fs_cfg_desc(uint16_t *length) {
    usb_device_endpoint_t *pEpDesc =
        usb_device_get_endpoint_desc(usb_hid_cfg_desc, HID_EPIN_ADDR);

    if (pEpDesc != NULL) {
        pEpDesc->b_interval = HID_FS_BINTERVAL;
    }

    *length = (uint16_t)sizeof(usb_hid_cfg_desc);
    return usb_hid_cfg_desc;
}

static uint8_t *usb_hid_class_get_other_speed_cfg_desc(uint16_t *length) {
    usb_device_endpoint_t *pEpDesc =
        usb_device_get_endpoint_desc(usb_hid_cfg_desc, HID_EPIN_ADDR);

    if (pEpDesc != NULL) {
        pEpDesc->b_interval = HID_FS_BINTERVAL;
    }

    *length = (uint16_t)sizeof(usb_hid_cfg_desc);
    return usb_hid_cfg_desc;
}

static uint8_t
    usb_hid_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] ALIGNED = {
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

static uint8_t *usb_device_hid_get_device_qualifier_desc(uint16_t *length) {
    *length = (uint16_t)sizeof(usb_hid_device_qualifier_desc);

    return usb_hid_device_qualifier_desc;
}

usb_device_class_t usb_hid_class = {
    usb_hid_class_init,
    usb_hid_class_deinit,
    usb_hid_class_setup,
    NULL,                  /* EP0_TxSent */
    NULL,                  /* EP0_RxReady */
    usb_hid_class_data_in, /* DataIn */
    NULL,                  /* DataOut */
    NULL,                  /* SOF */
    NULL,
    NULL,
    usb_hid_class_get_hsc_cfg_desc,
    usb_hid_class_get_fs_cfg_desc,
    usb_hid_class_get_other_speed_cfg_desc,
    usb_device_hid_get_device_qualifier_desc,
};

extern usb_device_handle_t usb_device_handle;

usb_device_status usb_hid_register_class() {

    usb_device_handle_t *pdev = &usb_device_handle;
    usb_device_class_t *pclass = &usb_hid_class;
    uint16_t len = 0U;

    if (pclass == NULL) {
        return USBD_FAIL;
    }

    /* link the class to the USB Device handle */
    pdev->p_class[0] = pclass;

    /* Get Device Configuration Descriptor */
    if (pdev->p_class[pdev->class_id]->get_fs_config_descriptor != NULL) {
        pdev->p_conf_desc =
            (void *)pdev->p_class[pdev->class_id]->get_fs_config_descriptor(
                &len);
    }

    /* Increment the NumClasses */
    pdev->num_classes++;

    return USBD_OK;
}

void usb_hid_init_serial_str() {
    // TODO: split this implementation with "hal/flash.h"
    //
    // This code is from STM32 code
    //
    // But I have no idea what UID_BASE is,
    // Cannot find this address in reference manual.
    // UID64 address is different there.
    uint32_t deviceserial0;
    uint32_t deviceserial1;
    uint32_t deviceserial2;

    deviceserial0 = *(uint32_t *)UID_BASE;
    deviceserial1 = *(uint32_t *)(UID_BASE + 0x4);
    deviceserial2 = *(uint32_t *)(UID_BASE + 0x8);

    deviceserial0 += deviceserial2;

    if (deviceserial0 != 0) {
        int_to_unicode(deviceserial0, &usb_serial_str_desc[2], 8);
        int_to_unicode(deviceserial1, &usb_serial_str_desc[18], 4);
    }
}

void usb_hid_init(usb_hid_config_t config) {

    usb_device_desc[DESCRIPTOR_TYPE_POS] = USB_DESC_TYPE_DEVICE;

    usb_device_desc[VENDOR_LOW_POS] = LOBYTE(config.vendor_id);
    usb_device_desc[VENDOR_HIGH_POS] = HIBYTE(config.vendor_id);

    usb_device_desc[PRODUCT_LOW_POS] = LOBYTE(config.vendor_id);
    usb_device_desc[PRODUCT_HIGH_POS] = HIBYTE(config.product_id);

    usb_lang_id_desc[LANG_ID_LOW_POS] = LOBYTE(config.lang_id);
    usb_lang_id_desc[LANG_ID_HIGH_POS] = HIBYTE(config.lang_id);

    ascii_to_unicode((uint8_t *)config.manufacturer_str,
                     usb_manufacturer_str_desc,
                     &usb_manufacturer_str_desc_length, USB_DESC_TYPE_STRING,
                     USB_MANUFACTURER_DESC_LENGTH);

    ascii_to_unicode((uint8_t *)config.product_str, usb_product_str_desc,
                     &usb_product_str_desc_length, USB_DESC_TYPE_STRING,
                     USB_PRODUCT_DESC_LENGTH);

    usb_hid_init_serial_str();

    ascii_to_unicode((uint8_t *)config.config_str, usb_config_str_desc,
                     &usb_config_str_desc_length, USB_DESC_TYPE_STRING,
                     USB_CONFIG_DESC_LENGTH);

    ascii_to_unicode((uint8_t *)config.interface_str, usb_interface_str_desc,
                     &usb_interface_str_desc_length, USB_DESC_TYPE_STRING,
                     USB_INTERFACE_DESC_LENGTH);
}
