#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include "hal/hal_err.h"
#include "usb/usb_descriptors.h"
#include <stdint.h>

#define USBD_MAX_SUPPORTED_CLASS 1U

struct _usb_device_endpoint_t;
struct _usb_device_setup_request_t;
struct _usb_device_class_t;
struct _usb_device_handle_t;

typedef struct _usb_device_endpoint_t {
    uint32_t status;
    uint32_t total_length;
    uint32_t rem_length;
    uint32_t max_packet;
    uint16_t is_used;
    uint16_t b_interval;
} usb_device_endpoint_t;

typedef struct _usb_device_setup_request_t {
    uint8_t bm_request;
    uint8_t b_request;
    uint16_t w_value;
    uint16_t w_index;
    uint16_t w_length;
} usb_device_setup_request_t;

typedef struct _usb_device_class_t {
    uint8_t (*init)(struct _usb_device_handle_t *pdev, uint8_t cfgidx);
    uint8_t (*deinit)(struct _usb_device_handle_t *pdev, uint8_t cfgidx);
    /* Control Endpoints*/
    uint8_t (*setup)(struct _usb_device_handle_t *pdev,
                     usb_device_setup_request_t *req);
    uint8_t (*ep0_tx_sent)(struct _usb_device_handle_t *pdev);
    uint8_t (*ep0_rx_ready)(struct _usb_device_handle_t *pdev);
    /* Class Specific Endpoints*/
    uint8_t (*data_in)(struct _usb_device_handle_t *pdev, uint8_t epnum);
    uint8_t (*data_out)(struct _usb_device_handle_t *pdev, uint8_t epnum);
    uint8_t (*sof)(struct _usb_device_handle_t *pdev);
    uint8_t (*iso_in_incomplete)(struct _usb_device_handle_t *pdev,
                                 uint8_t epnum);
    uint8_t (*iso_out_incomplete)(struct _usb_device_handle_t *pdev,
                                  uint8_t epnum);

    uint8_t *(*get_hs_config_descriptor)(uint16_t *length);
    uint8_t *(*get_fs_config_descriptor)(uint16_t *length);
    uint8_t *(*get_other_speed_config_descriptor)(uint16_t *length);
    uint8_t *(*get_device_qualifier_descriptor)(uint16_t *length);
} usb_device_class_t;

typedef struct _usb_device_handle_t {
    uint8_t id;
    uint32_t dev_config;
    uint32_t dev_default_config;
    uint32_t dev_config_status;
    usb_device_speed dev_speed;
    usb_device_endpoint_t ep_in[16];
    usb_device_endpoint_t ep_out[16];
    volatile uint32_t ep0_state;
    uint32_t ep0_data_len;
    volatile uint8_t dev_state;
    volatile uint8_t dev_old_state;
    uint8_t dev_address;
    uint8_t dev_connection_status;
    uint8_t dev_test_mode;
    uint32_t dev_remote_wakeup;
    uint8_t conf_idx;

    usb_device_setup_request_t request;
    usb_device_descriptors_t *p_desc;
    usb_device_class_t *p_class[USBD_MAX_SUPPORTED_CLASS];
    void *p_class_data;
    void *p_class_data_cmsit[USBD_MAX_SUPPORTED_CLASS];
    void *p_user_data[USBD_MAX_SUPPORTED_CLASS];
    void *p_data;
    void *p_bos_desc;
    void *p_conf_desc;
    uint32_t class_id;
    uint32_t num_classes;
} usb_device_handle_t;

hal_err usb_device_init();

#endif // USB_DEVICE_H
