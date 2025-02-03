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

typedef enum {
    USBD_HID_IDLE = 0,
    USBD_HID_BUSY,
} usb_hid_state;

typedef struct {
    uint32_t Protocol;
    uint32_t IdleState;
    uint32_t AltSetting;
    usb_hid_state state;
} usb_hid_handle_t;

static inline void *usb_hid_static_malloc() {
    static uint32_t
        mem[(sizeof(usb_hid_handle_t) / 4) + 1]; /* On 32-bit boundary */
    return mem;
}

typedef enum {
    USBD_OK = 0U,
    USBD_BUSY,
    USBD_EMEM,
    USBD_FAIL,
} USBD_StatusTypeDef;

uint8_t usb_hid_class_init(usb_device_handle_t *pdev, uint8_t cfgidx) {
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

// usb_device_class_t USBD_HID = {
//     usb_hid_class_init,
//     USBD_HID_DeInit,
//     USBD_HID_Setup,
//     NULL,            /* EP0_TxSent */
//     NULL,            /* EP0_RxReady */
//     USBD_HID_DataIn, /* DataIn */
//     NULL,            /* DataOut */
//     NULL,            /* SOF */
//     NULL,
//     NULL,
//     USBD_HID_GetHSCfgDesc,
//     USBD_HID_GetFSCfgDesc,
//     USBD_HID_GetOtherSpeedCfgDesc,
//     USBD_HID_GetDeviceQualifierDesc,
// };

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
