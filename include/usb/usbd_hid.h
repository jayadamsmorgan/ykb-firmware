#ifndef __USB_HID_H
#define __USB_HID_H

#include "usbd_ioreq.h"

#ifndef HID_EPIN_ADDR
#define HID_EPIN_ADDR 0x81U
#endif /* HID_EPIN_ADDR */
#define HID_EPIN_SIZE 0x04U

#define USB_HID_CONFIG_DESC_SIZ 34U
#define USB_HID_DESC_SIZ 9U
#define HID_KB_REPORT_DESC_SIZE 63U

#define HID_DESCRIPTOR_TYPE 0x21U
#define HID_REPORT_DESC 0x22U

#ifndef HID_HS_BINTERVAL
#define HID_HS_BINTERVAL 0x07U
#endif /* HID_HS_BINTERVAL */

#define USBD_HID_REQ_SET_PROTOCOL 0x0BU
#define USBD_HID_REQ_GET_PROTOCOL 0x03U

#define USBD_HID_REQ_SET_IDLE 0x0AU
#define USBD_HID_REQ_GET_IDLE 0x02U

#define USBD_HID_REQ_SET_REPORT 0x09U
#define USBD_HID_REQ_GET_REPORT 0x01U

typedef enum {
    USBD_HID_IDLE = 0,
    USBD_HID_BUSY,
} USBD_HID_StateTypeDef;

typedef struct {
    uint32_t Protocol;
    uint32_t IdleState;
    uint32_t AltSetting;
    USBD_HID_StateTypeDef state;
} USBD_HID_HandleTypeDef;

/*
 * HID Class specification version 1.1
 * 6.2.1 HID Descriptor
 */

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdHID;
    uint8_t bCountryCode;
    uint8_t bNumDescriptors;
    uint8_t bHIDDescriptorType;
    uint16_t wItemLength;
} __PACKED USBD_HIDDescTypeDef;

extern USBD_ClassTypeDef USBD_HID;
#define USBD_HID_CLASS &USBD_HID

uint8_t USBD_HID_SendReport(USBD_HandleTypeDef *pdev, uint8_t *report,
                            uint16_t len);
uint32_t USBD_HID_GetPollingInterval(USBD_HandleTypeDef *pdev);

#endif /* __USB_HID_H */
