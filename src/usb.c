#include "usb.h"

#include "crs.h"
#include "hal/gpio.h"
#include "hal/usb.h"
#include "usb/usb_descriptors.h"
#include "usb/usb_device.h"
#include "usb/usb_hid.h"
#include "utils/utils.h"
#include <stdint.h>

#define USBD_VID 0x483
#define USBD_PID 0x5710
#define USBD_LANGID_STRING 1033

#define USBD_MANUFACTURER_STRING "STMicroelectronics"
#define USBD_PRODUCT_STRING "STM32 Human interface"
#define USBD_CONFIGURATION_STRING "HID Config"
#define USBD_INTERFACE_STRING "HID Interface"

hal_err usb_init() {

    hal_err err;

    err = crs_setup();
    if (err) {
        return err;
    }

    usb_hid_config_t hid_config = {
        .vendor_id = USBD_VID,
        .product_id = USBD_PID,
        .lang_id = USBD_LANGID_STRING,

        .manufacturer_str = USBD_MANUFACTURER_STRING,
        .product_str = USBD_PRODUCT_STRING,
        .config_str = USBD_CONFIGURATION_STRING,
        .interface_str = USBD_INTERFACE_STRING,
    };

    usb_hid_init(hid_config);

    err = usb_device_init();
    if (err) {
        return err;
    }

    err = usb_hid_register_class();
    if (err) {
        return err;
    }

    usb_device_start();

    return OK;
}

#define PCD_GET_EP_TX_CNT(USBx, bEpNum)                                        \
    ((uint32_t)(*PCD_EP_TX_CNT((USBx), (bEpNum))) & 0x3ffU)
#define PCD_GET_EP_RX_CNT(USBx, bEpNum)                                        \
    ((uint32_t)(*PCD_EP_RX_CNT((USBx), (bEpNum))) & 0x3ffU)
#define PCD_EP_TX_CNT(USBx, bEpNum)                                            \
    ((uint16_t *)((((uint32_t)(USBx)->BTABLE + ((uint32_t)(bEpNum) * 8U) +     \
                    2U) *                                                      \
                   1U) +                                                       \
                  ((uint32_t)(USBx) + 0x400U)))

#define PCD_EP_RX_CNT(USBx, bEpNum)                                            \
    ((uint16_t *)((((uint32_t)(USBx)->BTABLE + ((uint32_t)(bEpNum) * 8U) +     \
                    6U) *                                                      \
                   1U) +                                                       \
                  ((uint32_t)(USBx) + 0x400U)))

#define PCD_CLEAR_TX_EP_CTR(USBx, bEpNum)                                      \
    do {                                                                       \
        uint16_t _wRegVal;                                                     \
                                                                               \
        _wRegVal =                                                             \
            PCD_GET_ENDPOINT((USBx), (bEpNum)) & (0xFF7FU & USB_EPREG_MASK);   \
                                                                               \
        PCD_SET_ENDPOINT((USBx), (bEpNum), (_wRegVal | USB_EP_CTR_RX));        \
    } while (0) /* PCD_CLEAR_TX_EP_CTR */

#define PCD_CLEAR_RX_EP_CTR(USBx, bEpNum)                                      \
    do {                                                                       \
        uint16_t _wRegVal;                                                     \
                                                                               \
        _wRegVal =                                                             \
            PCD_GET_ENDPOINT((USBx), (bEpNum)) & (0x7FFFU & USB_EPREG_MASK);   \
                                                                               \
        PCD_SET_ENDPOINT((USBx), (bEpNum), (_wRegVal | USB_EP_CTR_TX));        \
    } while (0) /* PCD_CLEAR_RX_EP_CTR */

#define USB_REQ_RECIPIENT_DEVICE 0x00U
#define USB_REQ_RECIPIENT_INTERFACE 0x01U
#define USB_REQ_RECIPIENT_ENDPOINT 0x02U
#define USB_REQ_RECIPIENT_MASK 0x03U

#define USB_EP_RX_STRX (0x3U << 12)

extern usb_ll_handle_t usb_ll_handle;

void USBD_ParseSetupRequest(usb_device_setup_request_t *req, uint8_t *pdata) {
    uint8_t *pbuff = pdata;

    req->bm_request = *(uint8_t *)(pbuff);

    pbuff++;
    req->b_request = *(uint8_t *)(pbuff);

    pbuff++;
    req->w_value = SWAPBYTE(pbuff);

    pbuff++;
    pbuff++;
    req->w_index = SWAPBYTE(pbuff);

    pbuff++;
    pbuff++;
    req->w_length = SWAPBYTE(pbuff);
}

#define USB_REQ_TYPE_STANDARD 0x00U
#define USB_REQ_TYPE_CLASS 0x20U
#define USB_REQ_TYPE_VENDOR 0x40U
#define USB_REQ_TYPE_MASK 0x60U
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

#define USBD_IDX_LANGID_STR 0x00U
#define USBD_IDX_MFC_STR 0x01U
#define USBD_IDX_PRODUCT_STR 0x02U
#define USBD_IDX_SERIAL_STR 0x03U
#define USBD_IDX_CONFIG_STR 0x04U
#define USBD_IDX_INTERFACE_STR 0x05U

static void USBD_GetDescriptor(usb_device_handle_t *pdev,
                               usb_device_setup_request_t *req) {
    uint16_t len = 0U;
    uint8_t *pbuf = NULL;
    uint8_t err = 0U;

    switch (req->w_value >> 8) {
    case USB_DESC_TYPE_DEVICE:
        pbuf = pdev->p_desc->usb_device_get_device_descriptor(pdev->dev_speed,
                                                              &len);
        break;

    case USB_DESC_TYPE_CONFIGURATION:
        if (pdev->dev_speed == USB_DEVICE_SPEED_HIGH) {
            {
                pbuf =
                    (uint8_t *)pdev->p_class[0]->get_hs_config_descriptor(&len);
            }
            pbuf[1] = USB_DESC_TYPE_CONFIGURATION;
        } else {
            {
                pbuf =
                    (uint8_t *)pdev->p_class[0]->get_fs_config_descriptor(&len);
            }
            pbuf[1] = USB_DESC_TYPE_CONFIGURATION;
        }
        break;

    case USB_DESC_TYPE_STRING:
        switch ((uint8_t)(req->w_value)) {
        case USBD_IDX_LANGID_STR:
            if (pdev->p_desc->usb_device_get_language_id_str_descriptor !=
                NULL) {
                pbuf = pdev->p_desc->usb_device_get_language_id_str_descriptor(
                    pdev->dev_speed, &len);
            } else {
                usb_device_ctrl_error(pdev);
                err++;
            }
            break;

        case USBD_IDX_MFC_STR:
            if (pdev->p_desc->usb_device_get_manufacturer_str_descriptor !=
                NULL) {
                pbuf = pdev->p_desc->usb_device_get_manufacturer_str_descriptor(
                    pdev->dev_speed, &len);
            } else {
                usb_device_ctrl_error(pdev);
                err++;
            }
            break;

        case USBD_IDX_PRODUCT_STR:
            if (pdev->p_desc->usb_device_get_product_str_descriptor != NULL) {
                pbuf = pdev->p_desc->usb_device_get_product_str_descriptor(
                    pdev->dev_speed, &len);
            } else {
                usb_device_ctrl_error(pdev);
                err++;
            }
            break;

        case USBD_IDX_SERIAL_STR:
            if (pdev->p_desc->usb_device_get_serial_str_descriptor != NULL) {
                pbuf = pdev->p_desc->usb_device_get_serial_str_descriptor(
                    pdev->dev_speed, &len);
            } else {
                usb_device_ctrl_error(pdev);
                err++;
            }
            break;

        case USBD_IDX_CONFIG_STR:
            if (pdev->p_desc->usb_device_get_config_str_descriptor != NULL) {
                pbuf = pdev->p_desc->usb_device_get_config_str_descriptor(
                    pdev->dev_speed, &len);
            } else {
                usb_device_ctrl_error(pdev);
                err++;
            }
            break;

        case USBD_IDX_INTERFACE_STR:
            if (pdev->p_desc->usb_device_get_interface_str_descriptor != NULL) {
                pbuf = pdev->p_desc->usb_device_get_interface_str_descriptor(
                    pdev->dev_speed, &len);
            } else {
                usb_device_ctrl_error(pdev);
                err++;
            }
            break;

        default:

            usb_device_ctrl_error(pdev);
            err++;
            break;
        }
        break;

    case USB_DESC_TYPE_DEVICE_QUALIFIER:

        if (pdev->dev_speed == USB_DEVICE_SPEED_HIGH) {
            {
                pbuf = (uint8_t *)pdev->p_class[0]
                           ->get_device_qualifier_descriptor(&len);
            }
        } else {
            usb_device_ctrl_error(pdev);
            err++;
        }
        break;

    case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
        if (pdev->dev_speed == USB_DEVICE_SPEED_HIGH) {
            {
                pbuf = (uint8_t *)pdev->p_class[0]
                           ->get_other_speed_config_descriptor(&len);
            }
            pbuf[1] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
        } else {
            usb_device_ctrl_error(pdev);
            err++;
        }
        break;

    default:
        usb_device_ctrl_error(pdev);
        err++;
        break;
    }

    if (err != 0U) {
        return;
    }

    if (req->w_length != 0U) {
        if (len != 0U) {
            len = MIN(len, req->w_length);
            usb_device_ctrl_send_data(pdev, pbuf, len);
        } else {
            usb_device_ctrl_error(pdev);
        }
    } else {
        usb_device_ctrl_send_status(pdev);
    }
}

static void USBD_SetAddress(usb_device_handle_t *pdev,
                            usb_device_setup_request_t *req) {
    uint8_t dev_addr;

    if ((req->w_index == 0U) && (req->w_length == 0U) &&
        (req->w_value < 128U)) {
        dev_addr = (uint8_t)(req->w_value) & 0x7FU;

        if (pdev->dev_state == USBD_STATE_CONFIGURED) {
            usb_device_ctrl_error(pdev);
        } else {
            pdev->dev_address = dev_addr;
            hal_usb_set_address(pdev->p_data, dev_addr);
            usb_device_ctrl_send_status(pdev);

            if (dev_addr != 0U) {
                pdev->dev_state = USBD_STATE_ADDRESSED;
            } else {
                pdev->dev_state = USBD_STATE_DEFAULT;
            }
        }
    } else {
        usb_device_ctrl_error(pdev);
    }
}

void USBD_ClrClassConfig(usb_device_handle_t *pdev, uint8_t cfgidx) {
    /* Clear configuration  and De-initialize the Class process */
    pdev->p_class[0]->deinit(pdev, cfgidx);
}

usb_device_status USBD_SetClassConfig(usb_device_handle_t *pdev,
                                      uint8_t cfgidx) {
    usb_device_status ret = USBD_OK;

    if (pdev->p_class[0] != NULL) {
        /* Set configuration and Start the Class */
        ret = (usb_device_status)pdev->p_class[0]->init(pdev, cfgidx);
    }

    return ret;
}

#define USBD_MAX_NUM_CONFIGURATION 1U

static void USBD_SetConfig(usb_device_handle_t *pdev,
                           usb_device_setup_request_t *req) {
    static uint8_t cfgidx;

    cfgidx = (uint8_t)(req->w_value);

    if (cfgidx > USBD_MAX_NUM_CONFIGURATION) {
        usb_device_ctrl_error(pdev);
        return;
    }

    switch (pdev->dev_state) {
    case USBD_STATE_ADDRESSED:
        if (cfgidx != 0U) {
            pdev->dev_config = cfgidx;

            uint8_t ret = USBD_SetClassConfig(pdev, cfgidx);

            if (ret != USBD_OK) {
                usb_device_ctrl_error(pdev);
                pdev->dev_state = USBD_STATE_ADDRESSED;
            } else {
                usb_device_ctrl_send_status(pdev);
                pdev->dev_state = USBD_STATE_CONFIGURED;
            }
        } else {
            usb_device_ctrl_send_status(pdev);
        }
        break;

    case USBD_STATE_CONFIGURED:
        if (cfgidx == 0U) {
            pdev->dev_state = USBD_STATE_ADDRESSED;
            pdev->dev_config = cfgidx;
            (void)USBD_ClrClassConfig(pdev, cfgidx);
            usb_device_ctrl_send_status(pdev);
        } else if (cfgidx != pdev->dev_config) {
            /* Clear old configuration */
            (void)USBD_ClrClassConfig(pdev, (uint8_t)pdev->dev_config);

            /* set new configuration */
            pdev->dev_config = cfgidx;

            uint8_t ret = USBD_SetClassConfig(pdev, cfgidx);

            if (ret != USBD_OK) {
                usb_device_ctrl_error(pdev);
                (void)USBD_ClrClassConfig(pdev, (uint8_t)pdev->dev_config);
                pdev->dev_state = USBD_STATE_ADDRESSED;
            } else {
                usb_device_ctrl_send_status(pdev);
            }
        } else {
            usb_device_ctrl_send_status(pdev);
        }
        break;

    default:
        usb_device_ctrl_error(pdev);
        (void)USBD_ClrClassConfig(pdev, cfgidx);
        break;
    }
}

static void USBD_GetConfig(usb_device_handle_t *pdev,
                           usb_device_setup_request_t *req) {
    if (req->w_length != 1U) {
        usb_device_ctrl_error(pdev);
    } else {
        switch (pdev->dev_state) {
        case USBD_STATE_DEFAULT:
        case USBD_STATE_ADDRESSED:
            pdev->dev_default_config = 0U;
            usb_device_ctrl_send_data(pdev,
                                      (uint8_t *)&pdev->dev_default_config, 1U);
            break;

        case USBD_STATE_CONFIGURED:
            usb_device_ctrl_send_data(pdev, (uint8_t *)&pdev->dev_config, 1U);
            break;

        default:
            usb_device_ctrl_error(pdev);
            break;
        }
    }
}

#define USB_CONFIG_REMOTE_WAKEUP 0x02U
#define USB_CONFIG_SELF_POWERED 0x01U

static void USBD_GetStatus(usb_device_handle_t *pdev,
                           usb_device_setup_request_t *req) {
    switch (pdev->dev_state) {
    case USBD_STATE_DEFAULT:
    case USBD_STATE_ADDRESSED:
    case USBD_STATE_CONFIGURED:
        if (req->w_length != 0x2U) {
            usb_device_ctrl_error(pdev);
            break;
        }

        pdev->dev_config_status = USB_CONFIG_SELF_POWERED;

        if (pdev->dev_remote_wakeup != 0U) {
            pdev->dev_config_status |= USB_CONFIG_REMOTE_WAKEUP;
        }

        usb_device_ctrl_send_data(pdev, (uint8_t *)&pdev->dev_config_status,
                                  2U);
        break;

    default:
        usb_device_ctrl_error(pdev);
        break;
    }
}

#define USB_FEATURE_REMOTE_WAKEUP 0x01U
#define USB_FEATURE_TEST_MODE 0x02U

static void USBD_SetFeature(usb_device_handle_t *pdev,
                            usb_device_setup_request_t *req) {
    if (req->w_value == USB_FEATURE_REMOTE_WAKEUP) {
        pdev->dev_remote_wakeup = 1U;
        usb_device_ctrl_send_status(pdev);
    } else if (req->w_value == USB_FEATURE_TEST_MODE) {
        pdev->dev_test_mode = (uint8_t)(req->w_index >> 8);
        usb_device_ctrl_send_status(pdev);
    } else {
        usb_device_ctrl_error(pdev);
    }
}

static void USBD_ClrFeature(usb_device_handle_t *pdev,
                            usb_device_setup_request_t *req) {
    switch (pdev->dev_state) {
    case USBD_STATE_DEFAULT:
    case USBD_STATE_ADDRESSED:
    case USBD_STATE_CONFIGURED:
        if (req->w_value == USB_FEATURE_REMOTE_WAKEUP) {
            pdev->dev_remote_wakeup = 0U;
            usb_device_ctrl_send_status(pdev);
        }
        break;

    default:
        usb_device_ctrl_error(pdev);
        break;
    }
}

void USBD_StdDevReq(usb_device_handle_t *pdev,
                    usb_device_setup_request_t *req) {

    switch (req->bm_request & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_CLASS:
    case USB_REQ_TYPE_VENDOR:
        pdev->p_class[pdev->class_id]->setup(pdev, req);
        break;

    case USB_REQ_TYPE_STANDARD:
        switch (req->b_request) {
        case USB_REQ_GET_DESCRIPTOR:
            USBD_GetDescriptor(pdev, req);
            break;

        case USB_REQ_SET_ADDRESS:
            USBD_SetAddress(pdev, req);
            break;

        case USB_REQ_SET_CONFIGURATION:
            USBD_SetConfig(pdev, req);
            break;

        case USB_REQ_GET_CONFIGURATION:
            USBD_GetConfig(pdev, req);
            break;

        case USB_REQ_GET_STATUS:
            USBD_GetStatus(pdev, req);
            break;

        case USB_REQ_SET_FEATURE:
            USBD_SetFeature(pdev, req);
            break;

        case USB_REQ_CLEAR_FEATURE:
            USBD_ClrFeature(pdev, req);
            break;

        default:
            usb_device_ctrl_error(pdev);
            break;
        }
        break;

    default:
        usb_device_ctrl_error(pdev);
        break;
    }
}

#define USBD_MAX_NUM_INTERFACES 1U

void USBD_StdItfReq(usb_device_handle_t *pdev,
                    usb_device_setup_request_t *req) {
    uint8_t idx;

    switch (req->bm_request & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_CLASS:
    case USB_REQ_TYPE_VENDOR:
    case USB_REQ_TYPE_STANDARD:
        switch (pdev->dev_state) {
        case USBD_STATE_DEFAULT:
        case USBD_STATE_ADDRESSED:
        case USBD_STATE_CONFIGURED:

            if (LOBYTE(req->w_index) <= USBD_MAX_NUM_INTERFACES) {
                /* Get the class index relative to this interface */
                idx = 0x00U;
                usb_device_status ret = USBD_OK;
                if (((uint8_t)idx != 0xFFU) &&
                    (idx < USBD_MAX_SUPPORTED_CLASS)) {
                    /* Call the class data out function to manage the request */
                    if (pdev->p_class[idx]->setup != NULL) {
                        pdev->class_id = idx;
                        ret = (usb_device_status)(pdev->p_class[idx]->setup(
                            pdev, req));
                    } else {
                        /* should never reach this condition */
                        ret = USBD_FAIL;
                    }
                } else {
                    /* No relative interface found */
                    ret = USBD_FAIL;
                }

                if ((req->w_length == 0U) && (ret == USBD_OK)) {
                    usb_device_ctrl_send_status(pdev);
                }
            } else {
                usb_device_ctrl_error(pdev);
            }
            break;

        default:
            usb_device_ctrl_error(pdev);
            break;
        }
        break;

    default:
        usb_device_ctrl_error(pdev);
        break;
    }
}

#define USB_FEATURE_EP_HALT 0x00U

void USBD_StdEPReq(usb_device_handle_t *pdev, usb_device_setup_request_t *req) {
    usb_device_endpoint_t *pep;
    uint8_t ep_addr;
    uint8_t idx;

    ep_addr = LOBYTE(req->w_index);

    switch (req->bm_request & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_CLASS:
    case USB_REQ_TYPE_VENDOR:
        /* Get the class index relative to this endpoint */
        idx = 0x00U;
        if (((uint8_t)idx != 0xFFU) && (idx < USBD_MAX_SUPPORTED_CLASS)) {
            pdev->class_id = idx;
            /* Call the class data out function to manage the request */
            if (pdev->p_class[idx]->setup != NULL) {
                pdev->p_class[idx]->setup(pdev, req);
            }
        }
        break;

    case USB_REQ_TYPE_STANDARD:
        switch (req->b_request) {
        case USB_REQ_SET_FEATURE:
            switch (pdev->dev_state) {
            case USBD_STATE_ADDRESSED:
                if ((ep_addr != 0x00U) && (ep_addr != 0x80U)) {
                    hal_usb_stall_endpoint(pdev->p_data, ep_addr);
                    hal_usb_stall_endpoint(pdev->p_data, 0x80U);
                } else {
                    usb_device_ctrl_error(pdev);
                }
                break;

            case USBD_STATE_CONFIGURED:
                if (req->w_value == USB_FEATURE_EP_HALT) {
                    if ((ep_addr != 0x00U) && (ep_addr != 0x80U) &&
                        (req->w_length == 0x00U)) {
                        hal_usb_stall_endpoint(pdev->p_data, ep_addr);
                    }
                }
                usb_device_ctrl_send_status(pdev);

                break;

            default:
                usb_device_ctrl_error(pdev);
                break;
            }
            break;

        case USB_REQ_CLEAR_FEATURE:

            switch (pdev->dev_state) {
            case USBD_STATE_ADDRESSED:
                if ((ep_addr != 0x00U) && (ep_addr != 0x80U)) {
                    hal_usb_stall_endpoint(pdev->p_data, ep_addr);
                    hal_usb_stall_endpoint(pdev->p_data, 0x80U);
                } else {
                    usb_device_ctrl_error(pdev);
                }
                break;

            case USBD_STATE_CONFIGURED:
                if (req->w_value == USB_FEATURE_EP_HALT) {
                    if ((ep_addr & 0x7FU) != 0x00U) {
                        hal_usb_clear_stall_endpoint(pdev->p_data, ep_addr);
                    }
                    usb_device_ctrl_send_status(pdev);

                    /* Get the class index relative to this interface */
                    idx = 0x00U;
                    if (((uint8_t)idx != 0xFFU) &&
                        (idx < USBD_MAX_SUPPORTED_CLASS)) {
                        pdev->class_id = idx;
                        /* Call the class data out function to manage the
                         * request */
                        if (pdev->p_class[idx]->setup != NULL) {
                            pdev->p_class[idx]->setup(pdev, req);
                        }
                    }
                }
                break;

            default:
                usb_device_ctrl_error(pdev);
                break;
            }
            break;

        case USB_REQ_GET_STATUS:
            switch (pdev->dev_state) {
            case USBD_STATE_ADDRESSED:
                if ((ep_addr != 0x00U) && (ep_addr != 0x80U)) {
                    usb_device_ctrl_error(pdev);
                    break;
                }
                pep = ((ep_addr & 0x80U) == 0x80U)
                          ? &pdev->ep_in[ep_addr & 0x7FU]
                          : &pdev->ep_out[ep_addr & 0x7FU];

                pep->status = 0x0000U;

                usb_device_ctrl_send_data(pdev, (uint8_t *)&pep->status, 2U);
                break;

            case USBD_STATE_CONFIGURED:
                if ((ep_addr & 0x80U) == 0x80U) {
                    if (pdev->ep_in[ep_addr & 0xFU].is_used == 0U) {
                        usb_device_ctrl_error(pdev);
                        break;
                    }
                } else {
                    if (pdev->ep_out[ep_addr & 0xFU].is_used == 0U) {
                        usb_device_ctrl_error(pdev);
                        break;
                    }
                }

                pep = ((ep_addr & 0x80U) == 0x80U)
                          ? &pdev->ep_in[ep_addr & 0x7FU]
                          : &pdev->ep_out[ep_addr & 0x7FU];

                if ((ep_addr == 0x00U) || (ep_addr == 0x80U)) {
                    pep->status = 0x0000U;
                } else if (hal_usb_stall_endpoint(pdev->p_data, ep_addr) !=
                           0U) {
                    pep->status = 0x0001U;
                } else {
                    pep->status = 0x0000U;
                }

                usb_device_ctrl_send_data(pdev, (uint8_t *)&pep->status, 2U);
                break;

            default:
                usb_device_ctrl_error(pdev);
                break;
            }
            break;

        default:
            usb_device_ctrl_error(pdev);
            break;
        }
        break;

    default:
        usb_device_ctrl_error(pdev);
        break;
    }
}

void USBD_LL_SetupStage(usb_device_handle_t *pdev, uint8_t *psetup) {

    USBD_ParseSetupRequest(&pdev->request, psetup);

    pdev->ep0_state = USBD_EP0_SETUP;

    pdev->ep0_data_len = pdev->request.w_length;

    switch (pdev->request.bm_request & 0x1FU) {
    case USB_REQ_RECIPIENT_DEVICE:
        USBD_StdDevReq(pdev, &pdev->request);
        break;

    case USB_REQ_RECIPIENT_INTERFACE:
        USBD_StdItfReq(pdev, &pdev->request);
        break;

    case USB_REQ_RECIPIENT_ENDPOINT:
        USBD_StdEPReq(pdev, &pdev->request);
        break;

    default:
        hal_usb_stall_endpoint(pdev->p_data,
                               (pdev->request.bm_request & 0x80U));
        break;
    }
}

void USBD_LL_DataOutStage(usb_device_handle_t *pdev, uint8_t epnum,
                          uint8_t *pdata) {
    usb_device_endpoint_t *pep;
    usb_device_status ret = USBD_OK;
    uint8_t idx;

    if (epnum == 0U) {
        pep = &pdev->ep_out[0];

        if (pdev->ep0_state == USBD_EP0_DATA_OUT) {
            if (pep->rem_length > pep->max_packet) {
                pep->rem_length -= pep->max_packet;

                usb_device_receive_ep(pdev->p_data, 0U, pdata,
                                      MIN(pep->rem_length, pep->max_packet));
            } else {
                /* Find the class ID relative to the current request */
                switch (pdev->request.bm_request & 0x1FU) {
                case USB_REQ_RECIPIENT_DEVICE:
                    /* Device requests must be managed by the first instantiated
                       class (or duplicated by all classes for simplicity) */
                    idx = 0U;
                    break;

                case USB_REQ_RECIPIENT_INTERFACE:
                    idx = 0x00U;
                    break;

                case USB_REQ_RECIPIENT_ENDPOINT:
                    idx = 0x00U;
                    break;

                default:
                    /* Back to the first class in case of doubt */
                    idx = 0U;
                    break;
                }

                if (idx < USBD_MAX_SUPPORTED_CLASS) {
                    /* Setup the class ID and route the request to the relative
                     * class function */
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        if (pdev->p_class[idx]->ep0_rx_ready != NULL) {
                            pdev->class_id = idx;
                            pdev->p_class[idx]->ep0_rx_ready(pdev);
                        }
                    }
                }

                usb_device_ctrl_send_status(pdev);
            }
        }
    } else {
        /* Get the class index relative to this interface */
        idx = 0x00U;

        if (((uint16_t)idx != 0xFFU) && (idx < USBD_MAX_SUPPORTED_CLASS)) {
            /* Call the class data out function to manage the request */
            if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                if (pdev->p_class[idx]->data_out != NULL) {
                    pdev->class_id = idx;
                    ret = (usb_device_status)pdev->p_class[idx]->data_out(
                        pdev, epnum);
                }
            }
            if (ret != USBD_OK) {
                return;
            }
        }
    }
}

static void PCD_EP_ISR_Handler() {
    usb_ll_handle_t *handle = &usb_ll_handle;
    usb_endpoint_t *ep;
    uint16_t count;
    uint16_t wIstr;
    uint16_t wEPVal;
    uint16_t TxPctSize;
    uint8_t epindex;

    /* stay in loop while pending interrupts */
    while ((handle->instance->ISTR & USB_ISTR_CTR) != 0U) {
        wIstr = handle->instance->ISTR;

        /* extract highest priority endpoint number */
        epindex = (uint8_t)(wIstr & USB_ISTR_EP_ID);

        if (epindex == 0U) {
            /* Decode and service control endpoint interrupt */

            /* DIR bit = origin of the interrupt */
            if ((wIstr & USB_ISTR_DIR) == 0U) {
                /* DIR = 0 */

                /* DIR = 0 => IN  int */
                /* DIR = 0 implies that (EP_CTR_TX = 1) always */
                PCD_CLEAR_TX_EP_CTR(handle->instance, 0U);
                ep = &handle->in_ep[0];

                ep->xfer_count = PCD_GET_EP_TX_CNT(handle->instance, ep->num);
                ep->xfer_buff += ep->xfer_count;

                /* TX COMPLETE */
                USBD_LL_DataInStage((usb_device_handle_t *)handle->p_data, 0U,
                                    handle->in_ep[0U].xfer_buff);

                if ((handle->usb_address > 0U) && (ep->xfer_len == 0U)) {
                    handle->instance->DADDR =
                        ((uint16_t)handle->usb_address | USB_DADDR_EF);
                    handle->usb_address = 0U;
                }
            } else {
                /* DIR = 1 */

                /* DIR = 1 & CTR_RX => SETUP or OUT int */
                /* DIR = 1 & (CTR_TX | CTR_RX) => 2 int pending */
                ep = &handle->out_ep[0];
                wEPVal = PCD_GET_ENDPOINT(handle->instance, 0U);

                if ((wEPVal & USB_EP_SETUP) != 0U) {
                    /* Get SETUP Packet */
                    ep->xfer_count =
                        PCD_GET_EP_RX_CNT(handle->instance, ep->num);

                    hal_usb_read_pma((uint8_t *)handle->setup, ep->pmaadress,
                                     (uint16_t)ep->xfer_count);

                    /* SETUP bit kept frozen while CTR_RX = 1 */
                    PCD_CLEAR_RX_EP_CTR(handle->instance, 0U);

                    /* Process SETUP Packet*/
                    USBD_LL_SetupStage(handle->p_data,
                                       (uint8_t *)handle->setup);
                } else if ((wEPVal & USB_EP_CTR_RX) != 0U) {
                    PCD_CLEAR_RX_EP_CTR(handle->instance, 0U);

                    /* Get Control Data OUT Packet */
                    ep->xfer_count =
                        PCD_GET_EP_RX_CNT(handle->instance, ep->num);

                    if ((ep->xfer_count != 0U) && (ep->xfer_buff != 0U)) {
                        hal_usb_read_pma(ep->xfer_buff, ep->pmaadress,
                                         (uint16_t)ep->xfer_count);

                        ep->xfer_buff += ep->xfer_count;

                        /* Process Control Data OUT Packet */
                        USBD_LL_DataOutStage(handle->p_data, 0U,
                                             handle->out_ep[0].xfer_buff);
                    }

                    wEPVal = (uint16_t)PCD_GET_ENDPOINT(handle->instance, 0U);

                    if (((wEPVal & USB_EP_SETUP) == 0U) &&
                        ((wEPVal & USB_EP_RX_STRX) != USB_EP_RX_VALID)) {
                        PCD_SET_EP_RX_STATUS(handle->instance, 0U,
                                             USB_EP_RX_VALID);
                    }
                }
            }
        } else {
            /* Decode and service non control endpoints interrupt */
            /* process related endpoint register */
            wEPVal = PCD_GET_ENDPOINT(handle->instance, epindex);

            if ((wEPVal & USB_EP_CTR_RX) != 0U) {
                /* clear int flag */
                PCD_CLEAR_RX_EP_CTR(handle->instance, epindex);
                ep = &handle->out_ep[epindex];

                /* OUT Single Buffering */
                if (ep->doublebuffer == 0U) {
                    count =
                        (uint16_t)PCD_GET_EP_RX_CNT(handle->instance, ep->num);

                    if (count != 0U) {
                        hal_usb_read_pma(ep->xfer_buff, ep->pmaadress, count);
                    }
                } else {
                    /* manage double buffer bulk out */
                    if (ep->type == EP_TYPE_BULK) {
                        hal_usb_ep_db_receive(handle, ep, wEPVal);
                    } else /* manage double buffer iso out */
                    {
                        /* free EP OUT Buffer */
                        PCD_FREE_USER_BUFFER(handle->instance, ep->num, 0U);

                        if ((PCD_GET_ENDPOINT(handle->instance, ep->num) &
                             USB_EP_DTOG_RX) != 0U) {
                            /* read from endpoint BUF0Addr buffer */
                            count = (uint16_t)PCD_GET_EP_DBUF0_CNT(
                                handle->instance, ep->num);

                            if (count != 0U) {
                                hal_usb_read_pma(ep->xfer_buff, ep->pmaaddr0,
                                                 count);
                            }
                        } else {
                            /* read from endpoint BUF1Addr buffer */
                            count = (uint16_t)PCD_GET_EP_DBUF1_CNT(
                                handle->instance, ep->num);

                            if (count != 0U) {
                                hal_usb_read_pma(ep->xfer_buff, ep->pmaaddr1,
                                                 count);
                            }
                        }
                    }
                }

                /* multi-packet on the NON control OUT endpoint */
                ep->xfer_count += count;
                ep->xfer_buff += count;

                if ((ep->xfer_len == 0U) || (count < ep->maxpacket)) {
                    /* RX COMPLETE */
                    USBD_LL_DataOutStage(handle->p_data, ep->num,
                                         handle->out_ep[ep->num].xfer_buff);
                } else {
                    hal_usb_device_start_ep_xfer(ep);
                }
            }

            if ((wEPVal & USB_EP_CTR_TX) != 0U) {
                ep = &handle->in_ep[epindex];

                /* clear int flag */
                PCD_CLEAR_TX_EP_CTR(handle->instance, epindex);

                if (ep->type == EP_TYPE_ISOC) {
                    ep->xfer_len = 0U;

                    if (ep->doublebuffer != 0U) {
                        if ((wEPVal & USB_EP_DTOG_TX) != 0U) {
                            PCD_SET_EP_DBUF0_CNT(handle->instance, ep->num,
                                                 ep->is_in, 0U);
                        } else {
                            PCD_SET_EP_DBUF1_CNT(handle->instance, ep->num,
                                                 ep->is_in, 0U);
                        }
                    }

                    /* TX COMPLETE */
                    USBD_LL_DataInStage(handle->p_data, ep->num,
                                        handle->in_ep[ep->num].xfer_buff);
                } else {
                    /* Manage Single Buffer Transaction */
                    if ((wEPVal & USB_EP_KIND) == 0U) {
                        /* Multi-packet on the NON control IN endpoint */
                        TxPctSize = (uint16_t)PCD_GET_EP_TX_CNT(
                            handle->instance, ep->num);

                        if (ep->xfer_len > TxPctSize) {
                            ep->xfer_len -= TxPctSize;
                        } else {
                            ep->xfer_len = 0U;
                        }

                        /* Zero Length Packet? */
                        if (ep->xfer_len == 0U) {
                            /* TX COMPLETE */
                            USBD_LL_DataInStage(
                                handle->p_data, ep->num,
                                handle->in_ep[ep->num].xfer_buff);
                        } else {
                            /* Transfer is not yet Done */
                            ep->xfer_buff += TxPctSize;
                            ep->xfer_count += TxPctSize;
                            hal_usb_device_start_ep_xfer(ep);
                        }
                    }
                    /* Double Buffer bulk IN (bulk transfer Len > Ep_Mps) */
                    else {
                        hal_usb_ep_db_transmit(handle, ep, wEPVal);
                    }
                }
            }
        }
    }

    return;
}

#define USB_MAX_EP0_SIZE 64U
#define USBD_EP_TYPE_CTRL 0x00U

void USBD_LL_Reset(usb_device_handle_t *pdev) {

    /* Upon Reset call user call back */
    pdev->dev_state = USBD_STATE_DEFAULT;
    pdev->ep0_state = USBD_EP0_IDLE;
    pdev->dev_config = 0U;
    pdev->dev_remote_wakeup = 0U;
    pdev->dev_test_mode = 0U;

    if (pdev->p_class[0] != NULL) {
        if (pdev->p_class[0]->deinit != NULL) {
            pdev->p_class[0]->deinit(pdev, (uint8_t)pdev->dev_config);
        }
    }

    /* Open EP0 OUT */
    usb_device_open_ep(pdev->p_data, 0x00U, USBD_EP_TYPE_CTRL,
                       USB_MAX_EP0_SIZE);
    pdev->ep_out[0x00U & 0xFU].is_used = 1U;

    pdev->ep_out[0].max_packet = USB_MAX_EP0_SIZE;

    /* Open EP0 IN */
    usb_device_open_ep(pdev->p_data, 0x80U, USBD_EP_TYPE_CTRL,
                       USB_MAX_EP0_SIZE);
    pdev->ep_in[0x80U & 0xFU].is_used = 1U;

    pdev->ep_in[0].max_packet = USB_MAX_EP0_SIZE;
}

void HAL_PCD_ResetCallback(usb_ll_handle_t *handle) {
    /* USER CODE BEGIN HAL_PCD_ResetCallback_PreTreatment */

    /* USER CODE END HAL_PCD_ResetCallback_PreTreatment */
    usb_device_speed speed = USB_DEVICE_SPEED_FULL;

    if (handle->init.speed != 2U) {
        return;
    }
    /* Set Speed. */
    ((usb_device_handle_t *)(handle->p_data))->dev_speed = speed;

    /* Reset Device. */
    USBD_LL_Reset(handle->p_data);
    /* USER CODE BEGIN HAL_PCD_ResetCallback_PostTreatment */

    /* USER CODE END HAL_PCD_ResetCallback_PostTreatment */
}

#define __HAL_PCD_CLEAR_FLAG(__HANDLE__, __INTERRUPT__)                        \
    (((__HANDLE__)->instance->ISTR) &= (uint16_t)(~(__INTERRUPT__)))

typedef enum {
    LPM_L0 = 0x00, /* on */
    LPM_L1 = 0x01, /* LPM L1 sleep */
    LPM_L2 = 0x02, /* suspend */
    LPM_L3 = 0x03, /* off */
} PCD_LPM_StateTypeDef;

typedef enum {
    PCD_LPM_L0_ACTIVE = 0x00, /* on */
    PCD_LPM_L1_ACTIVE = 0x01, /* LPM L1 sleep */
} PCD_LPM_MsgTypeDef;

void USBD_LL_Resume(usb_device_handle_t *pdev) {
    if (pdev->dev_state == USBD_STATE_SUSPENDED) {
        pdev->dev_state = pdev->dev_old_state;
    }
}

void HAL_PCD_ResumeCallback(usb_ll_handle_t *handle) {
    if (handle->init.low_power_enable) {
        /* Reset SLEEPDEEP bit of Cortex System Control Register. */
        SCB->SCR &= (uint32_t)~(
            (uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
        // SystemClockConfig_Resume(); TODO
    }

    USBD_LL_Resume(handle->p_data);
}

void USBD_LL_SOF(usb_device_handle_t *pdev) {
    /* The SOF event can be distributed for all classes that support it */
    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
        if (pdev->p_class[0] != NULL) {
            if (pdev->p_class[0]->sof != NULL) {
                pdev->p_class[0]->sof(pdev);
            }
        }
    }
}

void USBD_LL_Suspend(usb_device_handle_t *pdev) {
    if (pdev->dev_state != USBD_STATE_SUSPENDED) {
        pdev->dev_old_state = pdev->dev_state;
    }

    pdev->dev_state = USBD_STATE_SUSPENDED;
}

void HAL_PCD_SuspendCallback(usb_ll_handle_t *handle) {
    /* Inform USB library that core enters in suspend Mode. */
    USBD_LL_Suspend((usb_device_handle_t *)handle->p_data);
    /* Enter in STOP mode. */
    if (handle->init.low_power_enable) {
        /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register.
         */
        SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk |
                                          SCB_SCR_SLEEPONEXIT_Msk));
    }
}

void USB_LP_IRQHandler(void) {

    gpio_digital_write(PB1, HIGH);

    uint32_t wIstr = READ_REG(USB->ISTR);

    usb_ll_handle_t *handle = &usb_ll_handle;

    if ((wIstr & USB_ISTR_CTR) == USB_ISTR_CTR) {
        /* servicing of the endpoint correct transfer interrupt */
        /* clear of the CTR flag into the sub */
        PCD_EP_ISR_Handler();

        return;
    }

    if ((wIstr & USB_ISTR_RESET) == USB_ISTR_RESET) {
        __HAL_PCD_CLEAR_FLAG(handle, USB_ISTR_RESET);

        HAL_PCD_ResetCallback(handle);

        hal_usb_set_address(handle, 0U);

        return;
    }

    if ((wIstr & USB_ISTR_PMAOVR) == USB_ISTR_PMAOVR) {
        __HAL_PCD_CLEAR_FLAG(handle, USB_ISTR_PMAOVR);

        return;
    }

    if ((wIstr & USB_ISTR_ERR) == USB_ISTR_ERR) {
        __HAL_PCD_CLEAR_FLAG(handle, USB_ISTR_ERR);

        return;
    }

    if ((wIstr & USB_ISTR_WKUP) == USB_ISTR_WKUP) {
        handle->instance->CNTR &= (uint16_t)~(USB_CNTR_LPMODE);
        handle->instance->CNTR &= (uint16_t)~(USB_CNTR_FSUSP);

        if (handle->lpm_state == (uint8_t)LPM_L1) {
            handle->lpm_state = (uint8_t)LPM_L0;
            // HAL_PCDEx_LPM_Callback(handle, PCD_LPM_L0_ACTIVE);
        }

        HAL_PCD_ResumeCallback(handle);

        __HAL_PCD_CLEAR_FLAG(handle, USB_ISTR_WKUP);

        return;
    }

    if ((wIstr & USB_ISTR_SUSP) == USB_ISTR_SUSP) {
        /* Force low-power mode in the macrocell */
        handle->instance->CNTR |= (uint16_t)USB_CNTR_FSUSP;

        /* clear of the ISTR bit must be done after setting of CNTR_FSUSP */
        __HAL_PCD_CLEAR_FLAG(handle, USB_ISTR_SUSP);

        handle->instance->CNTR |= (uint16_t)USB_CNTR_LPMODE;

        HAL_PCD_SuspendCallback(handle);

        return;
    }

    /* Handle LPM Interrupt */
    if ((wIstr & USB_ISTR_L1REQ) == USB_ISTR_L1REQ) {
        __HAL_PCD_CLEAR_FLAG(handle, USB_ISTR_L1REQ);
        if (handle->lpm_state == (uint8_t)LPM_L0) {
            /* Force suspend and low-power mode before going to L1 state*/
            handle->instance->CNTR |= (uint16_t)USB_CNTR_LPMODE;
            handle->instance->CNTR |= (uint16_t)USB_CNTR_FSUSP;

            handle->lpm_state = (uint8_t)LPM_L1;
            handle->besl =
                ((uint32_t)handle->instance->LPMCSR & USB_LPMCSR_BESL) >> 2;
            // HAL_PCDEx_LPM_Callback(handle, PCD_LPM_L1_ACTIVE);
        } else {
            HAL_PCD_SuspendCallback(handle);
        }

        return;
    }

    if ((wIstr & USB_ISTR_SOF) == USB_ISTR_SOF) {
        __HAL_PCD_CLEAR_FLAG(handle, USB_ISTR_SOF);

        USBD_LL_SOF(handle->p_data);

        return;
    }

    if ((wIstr & USB_ISTR_ESOF) == USB_ISTR_ESOF) {
        /* clear ESOF flag in ISTR */
        __HAL_PCD_CLEAR_FLAG(handle, USB_ISTR_ESOF);

        return;
    }
}
