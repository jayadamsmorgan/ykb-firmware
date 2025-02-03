#ifndef HAL_USB_H
#define HAL_USB_H

#include "hal/hal_err.h"
#include "stm32wbxx.h"
#include <stdbool.h>
#include <stdint.h>

#define EP_ADDR_MSK 0x7U
#define PCD_SNG_BUF 0U

#define PCD_GET_ENDPOINT(USBx, bEpNum)                                         \
    (*(__IO uint16_t *)(&(USBx)->EP0R + ((bEpNum) * 2U)))

#define PCD_SET_ENDPOINT(USBx, bEpNum, wRegValue)                              \
    (*(__IO uint16_t *)(&(USBx)->EP0R + ((bEpNum) * 2U)) =                     \
         (uint16_t)(wRegValue))

#define PCD_SET_EP_ADDRESS(USBx, bEpNum, bAddr)                                \
    do {                                                                       \
        uint16_t _wRegVal;                                                     \
                                                                               \
        _wRegVal =                                                             \
            (PCD_GET_ENDPOINT((USBx), (bEpNum)) & USB_EPREG_MASK) | (bAddr);   \
                                                                               \
        PCD_SET_ENDPOINT((USBx), (bEpNum),                                     \
                         (_wRegVal | USB_EP_CTR_RX | USB_EP_CTR_TX));          \
    } while (0) /* PCD_SET_EP_ADDRESS */

#define PCD_SET_EP_TX_ADDRESS(USBx, bEpNum, wAddr)                             \
    do {                                                                       \
        __IO uint16_t *_wRegVal;                                               \
        uint32_t _wRegBase = (uint32_t)USBx;                                   \
                                                                               \
        _wRegBase += (uint32_t)(USBx)->BTABLE;                                 \
        _wRegVal = (__IO uint16_t *)(_wRegBase + 0x400U +                      \
                                     (((uint32_t)(bEpNum) * 8U) * 1U));        \
        *_wRegVal = ((wAddr) >> 1) << 1;                                       \
    } while (0) /* PCD_SET_EP_TX_ADDRESS */

#define PCD_TX_DTOG(USBx, bEpNum)                                              \
    do {                                                                       \
        uint16_t _wEPVal;                                                      \
                                                                               \
        _wEPVal = PCD_GET_ENDPOINT((USBx), (bEpNum)) & USB_EPREG_MASK;         \
                                                                               \
        PCD_SET_ENDPOINT(                                                      \
            (USBx), (bEpNum),                                                  \
            (_wEPVal | USB_EP_CTR_RX | USB_EP_CTR_TX | USB_EP_DTOG_TX));       \
    } while (0) /* PCD_TX_DTOG */

#define PCD_CLEAR_TX_DTOG(USBx, bEpNum)                                        \
    do {                                                                       \
        uint16_t _wRegVal;                                                     \
                                                                               \
        _wRegVal = PCD_GET_ENDPOINT((USBx), (bEpNum));                         \
                                                                               \
        if ((_wRegVal & USB_EP_DTOG_TX) != 0U) {                               \
            PCD_TX_DTOG((USBx), (bEpNum));                                     \
        }                                                                      \
    } while (0) /* PCD_CLEAR_TX_DTOG */

#define PCD_SET_EP_TX_STATUS(USBx, bEpNum, wState)                             \
    do {                                                                       \
        uint16_t _wRegVal;                                                     \
                                                                               \
        _wRegVal = PCD_GET_ENDPOINT((USBx), (bEpNum)) & USB_EPTX_DTOGMASK;     \
        /* toggle first bit ? */                                               \
        if ((USB_EPTX_DTOG1 & (wState)) != 0U) {                               \
            _wRegVal ^= USB_EPTX_DTOG1;                                        \
        }                                                                      \
        /* toggle second bit ?  */                                             \
        if ((USB_EPTX_DTOG2 & (wState)) != 0U) {                               \
            _wRegVal ^= USB_EPTX_DTOG2;                                        \
        }                                                                      \
        PCD_SET_ENDPOINT((USBx), (bEpNum),                                     \
                         (_wRegVal | USB_EP_CTR_RX | USB_EP_CTR_TX));          \
    } while (0) /* PCD_SET_EP_TX_STATUS */

#define PCD_SET_EP_RX_ADDRESS(USBx, bEpNum, wAddr)                             \
    do {                                                                       \
        __IO uint16_t *_wRegVal;                                               \
        uint32_t _wRegBase = (uint32_t)USBx;                                   \
                                                                               \
        _wRegBase += (uint32_t)(USBx)->BTABLE;                                 \
        _wRegVal = (__IO uint16_t *)(_wRegBase + 0x400U +                      \
                                     ((((uint32_t)(bEpNum) * 8U) + 4U) * 1U)); \
        *_wRegVal = ((wAddr) >> 1) << 1;                                       \
    } while (0) /* PCD_SET_EP_RX_ADDRESS */

#define PCD_SET_EP_CNT_RX_REG(pdwReg, wCount)                                  \
    do {                                                                       \
        uint32_t wNBlocks;                                                     \
                                                                               \
        *(pdwReg) &= 0x3FFU;                                                   \
                                                                               \
        if ((wCount) == 0U) {                                                  \
            *(pdwReg) |= USB_CNTRX_BLSIZE;                                     \
        } else if ((wCount) <= 62U) {                                          \
            PCD_CALC_BLK2((pdwReg), (wCount), wNBlocks);                       \
        } else {                                                               \
            PCD_CALC_BLK32((pdwReg), (wCount), wNBlocks);                      \
        }                                                                      \
    } while (0) /* PCD_SET_EP_CNT_RX_REG */

#define PCD_SET_EP_RX_CNT(USBx, bEpNum, wCount)                                \
    do {                                                                       \
        uint32_t _wRegBase = (uint32_t)(USBx);                                 \
        __IO uint16_t *_wRegVal;                                               \
                                                                               \
        _wRegBase += (uint32_t)(USBx)->BTABLE;                                 \
        _wRegVal = (__IO uint16_t *)(_wRegBase + 0x400U +                      \
                                     ((((uint32_t)(bEpNum) * 8U) + 6U) * 1U)); \
        PCD_SET_EP_CNT_RX_REG(_wRegVal, (wCount));                             \
    } while (0)

#define PCD_CALC_BLK2(pdwReg, wCount, wNBlocks)                                \
    do {                                                                       \
        (wNBlocks) = (wCount) >> 1;                                            \
        if (((wCount) & 0x1U) != 0U) {                                         \
            (wNBlocks)++;                                                      \
        }                                                                      \
        *(pdwReg) |= (uint16_t)((wNBlocks) << 10);                             \
    } while (0) /* PCD_CALC_BLK2 */

#define USB_CNTRX_BLSIZE (0x1U << 15)

#define PCD_CALC_BLK32(pdwReg, wCount, wNBlocks)                               \
    do {                                                                       \
        (wNBlocks) = (wCount) >> 5;                                            \
        if (((wCount) & 0x1fU) == 0U) {                                        \
            (wNBlocks)--;                                                      \
        }                                                                      \
        *(pdwReg) |= (uint16_t)(((wNBlocks) << 10) | USB_CNTRX_BLSIZE);        \
    } while (0) /* PCD_CALC_BLK32 */

#define PCD_RX_DTOG(USBx, bEpNum)                                              \
    do {                                                                       \
        uint16_t _wEPVal;                                                      \
                                                                               \
        _wEPVal = PCD_GET_ENDPOINT((USBx), (bEpNum)) & USB_EPREG_MASK;         \
                                                                               \
        PCD_SET_ENDPOINT(                                                      \
            (USBx), (bEpNum),                                                  \
            (_wEPVal | USB_EP_CTR_RX | USB_EP_CTR_TX | USB_EP_DTOG_RX));       \
    } while (0) /* PCD_RX_DTOG */

#define PCD_CLEAR_RX_DTOG(USBx, bEpNum)                                        \
    do {                                                                       \
        uint16_t _wRegVal;                                                     \
                                                                               \
        _wRegVal = PCD_GET_ENDPOINT((USBx), (bEpNum));                         \
                                                                               \
        if ((_wRegVal & USB_EP_DTOG_RX) != 0U) {                               \
            PCD_RX_DTOG((USBx), (bEpNum));                                     \
        }                                                                      \
    } while (0) /* PCD_CLEAR_RX_DTOG */

#define PCD_SET_EP_RX_STATUS(USBx, bEpNum, wState)                             \
    do {                                                                       \
        uint16_t _wRegVal;                                                     \
                                                                               \
        _wRegVal = PCD_GET_ENDPOINT((USBx), (bEpNum)) & USB_EPRX_DTOGMASK;     \
        /* toggle first bit ? */                                               \
        if ((USB_EPRX_DTOG1 & (wState)) != 0U) {                               \
            _wRegVal ^= USB_EPRX_DTOG1;                                        \
        }                                                                      \
        /* toggle second bit ? */                                              \
        if ((USB_EPRX_DTOG2 & (wState)) != 0U) {                               \
            _wRegVal ^= USB_EPRX_DTOG2;                                        \
        }                                                                      \
        PCD_SET_ENDPOINT((USBx), (bEpNum),                                     \
                         (_wRegVal | USB_EP_CTR_RX | USB_EP_CTR_TX));          \
    } while (0) /* PCD_SET_EP_RX_STATUS */

#define PCD_SET_BULK_EP_DBUF(USBx, bEpNum) PCD_SET_EP_KIND((USBx), (bEpNum))

#define PCD_SET_EP_KIND(USBx, bEpNum)                                          \
    do {                                                                       \
        uint16_t _wRegVal;                                                     \
                                                                               \
        _wRegVal = PCD_GET_ENDPOINT((USBx), (bEpNum)) & USB_EPREG_MASK;        \
                                                                               \
        PCD_SET_ENDPOINT(                                                      \
            (USBx), (bEpNum),                                                  \
            (_wRegVal | USB_EP_CTR_RX | USB_EP_CTR_TX | USB_EP_KIND));         \
    } while (0) /* PCD_SET_EP_KIND */

#define PCD_CLEAR_EP_KIND(USBx, bEpNum)                                        \
    do {                                                                       \
        uint16_t _wRegVal;                                                     \
                                                                               \
        _wRegVal = PCD_GET_ENDPOINT((USBx), (bEpNum)) & USB_EPKIND_MASK;       \
                                                                               \
        PCD_SET_ENDPOINT((USBx), (bEpNum),                                     \
                         (_wRegVal | USB_EP_CTR_RX | USB_EP_CTR_TX));          \
    } while (0) /* PCD_CLEAR_EP_KIND */

#define PCD_SET_EP_DBUF0_ADDR(USBx, bEpNum, wBuf0Addr)                         \
    do {                                                                       \
        PCD_SET_EP_TX_ADDRESS((USBx), (bEpNum), (wBuf0Addr));                  \
    } while (0) /* PCD_SET_EP_DBUF0_ADDR */

#define PCD_SET_EP_DBUF1_ADDR(USBx, bEpNum, wBuf1Addr)                         \
    do {                                                                       \
        PCD_SET_EP_RX_ADDRESS((USBx), (bEpNum), (wBuf1Addr));                  \
    } while (0) /* PCD_SET_EP_DBUF1_ADDR */

#define PCD_SET_EP_DBUF_ADDR(USBx, bEpNum, wBuf0Addr, wBuf1Addr)               \
    do {                                                                       \
        PCD_SET_EP_DBUF0_ADDR((USBx), (bEpNum), (wBuf0Addr));                  \
        PCD_SET_EP_DBUF1_ADDR((USBx), (bEpNum), (wBuf1Addr));                  \
    } while (0) /* PCD_SET_EP_DBUF_ADDR */

#define PCD_SET_EP_RX_DBUF0_CNT(USBx, bEpNum, wCount)                          \
    do {                                                                       \
        uint32_t _wRegBase = (uint32_t)(USBx);                                 \
        __IO uint16_t *pdwReg;                                                 \
                                                                               \
        _wRegBase += (uint32_t)(USBx)->BTABLE;                                 \
        pdwReg = (__IO uint16_t *)(_wRegBase + 0x400U +                        \
                                   ((((uint32_t)(bEpNum) * 8U) + 2U) * 1U));   \
        PCD_SET_EP_CNT_RX_REG(pdwReg, (wCount));                               \
    } while (0);

#define PCD_SET_EP_TX_CNT(USBx, bEpNum, wCount)                                \
    do {                                                                       \
        uint32_t _wRegBase = (uint32_t)(USBx);                                 \
        __IO uint16_t *_wRegVal;                                               \
                                                                               \
        _wRegBase += (uint32_t)(USBx)->BTABLE;                                 \
        _wRegVal = (__IO uint16_t *)(_wRegBase + 0x400U +                      \
                                     ((((uint32_t)(bEpNum) * 8U) + 2U) * 1U)); \
        *_wRegVal = (uint16_t)(wCount);                                        \
    } while (0)

#define PCD_SET_EP_DBUF0_CNT(USBx, bEpNum, bDir, wCount)                       \
    do {                                                                       \
        if ((bDir) == 0U) /* OUT endpoint */                                   \
        {                                                                      \
            PCD_SET_EP_RX_DBUF0_CNT((USBx), (bEpNum), (wCount));               \
        } else {                                                               \
            if ((bDir) == 1U) {                                                \
                /* IN endpoint */                                              \
                PCD_SET_EP_TX_CNT((USBx), (bEpNum), (wCount));                 \
            }                                                                  \
        }                                                                      \
    } while (0) /* SetEPDblBuf0Count*/

#define PCD_SET_EP_DBUF1_CNT(USBx, bEpNum, bDir, wCount)                       \
    do {                                                                       \
        uint32_t _wBase = (uint32_t)(USBx);                                    \
        __IO uint16_t *_wEPRegVal;                                             \
                                                                               \
        if ((bDir) == 0U) {                                                    \
            /* OUT endpoint */                                                 \
            PCD_SET_EP_RX_CNT((USBx), (bEpNum), (wCount));                     \
        } else {                                                               \
            if ((bDir) == 1U) {                                                \
                /* IN endpoint */                                              \
                _wBase += (uint32_t)(USBx)->BTABLE;                            \
                _wEPRegVal =                                                   \
                    (__IO uint16_t *)(_wBase + 0x400U +                        \
                                      ((((uint32_t)(bEpNum) * 8U) + 6U) *      \
                                       1U));                                   \
                *_wEPRegVal = (uint16_t)(wCount);                              \
            }                                                                  \
        }                                                                      \
    } while (0) /* SetEPDblBuf1Count */

#define PCD_SET_EP_DBUF_CNT(USBx, bEpNum, bDir, wCount)                        \
    do {                                                                       \
        PCD_SET_EP_DBUF0_CNT((USBx), (bEpNum), (bDir), (wCount));              \
        PCD_SET_EP_DBUF1_CNT((USBx), (bEpNum), (bDir), (wCount));              \
    } while (0) /* PCD_SET_EP_DBUF_CNT */

#define PCD_SET_BULK_EP_DBUF(USBx, bEpNum) PCD_SET_EP_KIND((USBx), (bEpNum))
#define PCD_CLEAR_BULK_EP_DBUF(USBx, bEpNum) PCD_CLEAR_EP_KIND((USBx), (bEpNum))
#define PCD_SET_OUT_STATUS(USBx, bEpNum) PCD_SET_EP_KIND((USBx), (bEpNum))
#define PCD_CLEAR_OUT_STATUS(USBx, bEpNum) PCD_CLEAR_EP_KIND((USBx), (bEpNum))

#define PCD_FREE_USER_BUFFER(USBx, bEpNum, bDir)                               \
    do {                                                                       \
        if ((bDir) == 0U) {                                                    \
            /* OUT double buffered endpoint */                                 \
            PCD_TX_DTOG((USBx), (bEpNum));                                     \
        } else if ((bDir) == 1U) {                                             \
            /* IN double buffered endpoint */                                  \
            PCD_RX_DTOG((USBx), (bEpNum));                                     \
        }                                                                      \
    } while (0)

typedef enum {
    USB_LOCK_UNLOCKED = 0x00, //
    USB_LOCK_LOCKED = 0x01    //
} usb_lock_t;

typedef struct {
    uint8_t num;            /*!< Endpoint number
                                 This parameter must be a number between Min_Data = 1 and
                               Max_Data = 15   */
    uint8_t is_in;          /*!< Endpoint direction
                                 This parameter must be a number between Min_Data = 0 and
                               Max_Data = 1    */
    uint8_t is_stall;       /*!< Endpoint stall condition
                                 This parameter must be a number between Min_Data = 0
                               and Max_Data = 1    */
    uint8_t type;           /*!< Endpoint type
                                 This parameter can be any value of @ref USB_LL_EP_Type */
    uint8_t data_pid_start; /*!< Initial data PID
                                 This parameter must be a number between
                               Min_Data = 0 and Max_Data = 1    */
    uint16_t pmaadress;     /*!< PMA Address
                                 This parameter can be any value between Min_addr =
                               0 and Max_addr = 1K   */
    uint16_t pmaaddr0;      /*!< PMA Address0
                                 This parameter can be any value between Min_addr = 0
                               and Max_addr = 1K   */
    uint16_t pmaaddr1;      /*!< PMA Address1
                                 This parameter can be any value between Min_addr = 0
                               and Max_addr = 1K   */
    uint8_t doublebuffer;   /*!< Double buffer enable
                                 This parameter can be 0 or 1 */
    uint32_t maxpacket;     /*!< Endpoint Max packet size
                                 This parameter must be a number between Min_Data =
                               0 and Max_Data = 64KB */
    uint8_t *xfer_buff;     /*!< Pointer to transfer buffer */
    uint32_t xfer_len;      /*!< Current transfer length */
    uint32_t xfer_count;    /*!< Partial transfer length in case of multi packet
                               transfer                 */
    uint32_t xfer_len_db;   /*!< double buffer transfer length used with bulk
                               double buffer in            */
    uint8_t xfer_fill_db; /*!< double buffer Need to Fill new buffer  used with
                             bulk_in                 */
} usb_endpoint_t;

typedef struct {
    uint8_t dev_endpoints; /*!< Device Endpoints number.
                                This parameter depends on the used USB core.
                                This parameter must be a number between Min_Data
                              = 1 and Max_Data = 15 */
    uint8_t speed;         /*!< USB Core speed.
                                This parameter can be any value of @ref
                              PCD_Speed/HCD_Speed (HCD_SPEED_xxx, HCD_SPEED_xxx) */
    uint8_t ep0_mps;       /*!< Set the Endpoint 0 Max Packet size. */
    uint8_t phy_itface;    /*!< Select the used PHY interface.
                                This parameter can be any value of @ref
                              PCD_PHY_Module/HCD_PHY_Module  */
    bool sof_enable; /*!< Enable or disable the output of the SOF signal. */
    bool low_power_enable; /*!< Enable or disable the low Power Mode. */
    bool lpm_enable;       /*!< Enable or disable Link Power Management. */
    bool battery_charging_enable; /*!< Enable or disable Battery charging. */
} usb_config_t;

typedef enum {
    USB_COMM_STATE_RESET = 0x00,
    USB_COMM_STATE_READY = 0x01,
    USB_COMM_STATE_ERROR = 0x02,
    USB_COMM_STATE_BUSY = 0x03,
    USB_COMM_STATE_TIMEOUT = 0x04
} usb_comm_state_t;

typedef struct {
    USB_TypeDef *instance;           /*!< Register base address             */
    usb_config_t init;               /*!< PCD required parameters           */
    volatile uint8_t usb_address;    /*!< USB Address                       */
    usb_endpoint_t in_ep[8];         /*!< IN endpoint parameters            */
    usb_endpoint_t out_ep[8];        /*!< OUT endpoint parameters           */
    usb_lock_t lock;                 /*!< PCD peripheral status             */
    volatile usb_comm_state_t state; /*!< PCD communication state           */
    volatile uint32_t error_code;    /*!< PCD Error code                    */
    uint32_t setup[12];              /*!< Setup packet buffer               */
    usb_comm_state_t lpm_state;      /*!< LPM State                         */
    uint32_t besl;                   /* ?                                   */
    uint32_t lpm_active; /*!< Enable or disable the Link Power Management . This
                            parameter can be set to ENABLE or DISABLE */
    uint32_t battery_charging_active; /*!< Enable or disable Battery charging.
                                      This parameter can be set to ENABLE or
                                      DISABLE        */
    void *p_data;                     /*!< Pointer to upper stack Handler */

} usb_ll_handle_t;

void hal_usb_enable_vdd();

void hal_usb_disable_interrupts();

hal_err hal_usb_init(usb_ll_handle_t *handle);

void hal_usb_pma_config(usb_ll_handle_t *handle, uint16_t ep_addr,
                        uint16_t ep_kind, uint32_t pmaadress);

void hal_usb_device_start_ep_xfer(usb_endpoint_t *ep);

void hal_usb_deactivate_endpoint(usb_endpoint_t *ep);

hal_err hal_usb_activate_endpoint(usb_endpoint_t *ep);

void hal_usb_stall_endpoint(usb_ll_handle_t *handle, uint8_t ep_addr);

void hal_usb_start();

void hal_usb_activate_remote_wakeup();

void hal_usb_deactivate_remote_wakeup();

#endif // HAL_USB_H
