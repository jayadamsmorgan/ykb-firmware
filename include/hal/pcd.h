#ifndef HAL_PCD_H
#define HAL_PCD_H

#include "hal/usb.h"
#include "utils/utils.h"

typedef enum {
    HAL_PCD_STATE_RESET = 0x00,
    HAL_PCD_STATE_READY = 0x01,
    HAL_PCD_STATE_ERROR = 0x02,
    HAL_PCD_STATE_BUSY = 0x03,
    HAL_PCD_STATE_TIMEOUT = 0x04
} PCD_StateTypeDef;

/* Device LPM suspend state */
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

typedef enum {
    PCD_BCD_ERROR = 0xFF,
    PCD_BCD_CONTACT_DETECTION = 0xFE,
    PCD_BCD_STD_DOWNSTREAM_PORT = 0xFD,
    PCD_BCD_CHARGING_DOWNSTREAM_PORT = 0xFC,
    PCD_BCD_DEDICATED_CHARGING_PORT = 0xFB,
    PCD_BCD_DISCOVERY_COMPLETED = 0x00,

} PCD_BCD_MsgTypeDef;

typedef USB_TypeDef PCD_TypeDef;
typedef USB_CfgTypeDef PCD_InitTypeDef;
typedef USB_EPTypeDef PCD_EPTypeDef;

typedef struct {
    PCD_TypeDef *Instance;          /*!< Register base address             */
    PCD_InitTypeDef Init;           /*!< PCD required parameters           */
    __IO uint8_t USB_Address;       /*!< USB Address                       */
    PCD_EPTypeDef IN_ep[8];         /*!< IN endpoint parameters            */
    PCD_EPTypeDef OUT_ep[8];        /*!< OUT endpoint parameters           */
    HAL_LockTypeDef Lock;           /*!< PCD peripheral status             */
    __IO PCD_StateTypeDef State;    /*!< PCD communication state           */
    __IO uint32_t ErrorCode;        /*!< PCD Error code                    */
    uint32_t Setup[12];             /*!< Setup packet buffer               */
    PCD_LPM_StateTypeDef LPM_State; /*!< LPM State                         */
    uint32_t BESL;

    uint32_t lpm_active; /*!< Enable or disable the Link Power Management .
                         This parameter can be set to ENABLE or DISABLE */

    uint32_t battery_charging_active; /*!< Enable or disable Battery charging.
                                      This parameter can be set to ENABLE or
                                      DISABLE        */
    void *pData;                      /*!< Pointer to upper stack Handler */

} PCD_HandleTypeDef;

#define PCD_SPEED_FULL USBD_FS_SPEED

#define PCD_PHY_ULPI 1U
#define PCD_PHY_EMBEDDED 2U
#define PCD_PHY_UTMI 3U

#define __HAL_PCD_GET_FLAG(__HANDLE__, __INTERRUPT__)                          \
    ((USB_ReadInterrupts((__HANDLE__)->Instance) & (__INTERRUPT__)) ==         \
     (__INTERRUPT__))

#define __HAL_PCD_CLEAR_FLAG(__HANDLE__, __INTERRUPT__)                        \
    (((__HANDLE__)->Instance->ISTR) &= (uint16_t)(~(__INTERRUPT__)))

#define __HAL_USB_WAKEUP_EXTI_ENABLE_IT() EXTI->IMR1 |= USB_WAKEUP_EXTI_LINE
#define __HAL_USB_WAKEUP_EXTI_DISABLE_IT() EXTI->IMR1 &= ~(USB_WAKEUP_EXTI_LINE)

HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeInit(PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd);

HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd);
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd);

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd);

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);

HAL_StatusTypeDef HAL_PCD_DevConnect(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DevDisconnect(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address);
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                  uint16_t ep_mps, uint8_t ep_type);
HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                     uint8_t *pBuf, uint32_t len);
HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                      uint8_t *pBuf, uint32_t len);
HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Abort(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);
uint32_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef const *hpcd, uint8_t ep_addr);

PCD_StateTypeDef HAL_PCD_GetState(PCD_HandleTypeDef const *hpcd);

#define USB_WAKEUP_EXTI_LINE                                                   \
    (0x1U << 28) /*!< USB FS EXTI Line WakeUp Interrupt */

#define PCD_EP0MPS_64 EP_MPS_64
#define PCD_EP0MPS_32 EP_MPS_32
#define PCD_EP0MPS_16 EP_MPS_16
#define PCD_EP0MPS_08 EP_MPS_8

#define PCD_ENDP0 0U
#define PCD_ENDP1 1U
#define PCD_ENDP2 2U
#define PCD_ENDP3 3U
#define PCD_ENDP4 4U
#define PCD_ENDP5 5U
#define PCD_ENDP6 6U
#define PCD_ENDP7 7U

#define PCD_SNG_BUF 0U
#define PCD_DBL_BUF 1U

/********************  Bit definition for USB_COUNTn_RX register  *************/
#define USB_CNTRX_NBLK_MSK (0x1FU << 10)
#define USB_CNTRX_BLSIZE (0x1U << 15)

#define PCD_SET_ENDPOINT(USBx, bEpNum, wRegValue)                              \
    (*(__IO uint16_t *)(&(USBx)->EP0R + ((bEpNum) * 2U)) =                     \
         (uint16_t)(wRegValue))

#define PCD_GET_ENDPOINT(USBx, bEpNum)                                         \
    (*(__IO uint16_t *)(&(USBx)->EP0R + ((bEpNum) * 2U)))

#define PCD_SET_EPTYPE(USBx, bEpNum, wType)                                    \
    (PCD_SET_ENDPOINT((USBx), (bEpNum),                                        \
                      ((PCD_GET_ENDPOINT((USBx), (bEpNum)) & USB_EP_T_MASK) |  \
                       (wType) | USB_EP_CTR_TX | USB_EP_CTR_RX)))

#define PCD_GET_EPTYPE(USBx, bEpNum)                                           \
    (PCD_GET_ENDPOINT((USBx), (bEpNum)) & USB_EP_T_FIELD)

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

#define PCD_SET_EP_TXRX_STATUS(USBx, bEpNum, wStaterx, wStatetx)               \
    do {                                                                       \
        uint16_t _wRegVal;                                                     \
                                                                               \
        _wRegVal = PCD_GET_ENDPOINT((USBx), (bEpNum)) &                        \
                   (USB_EPRX_DTOGMASK | USB_EPTX_STAT);                        \
        /* toggle first bit ? */                                               \
        if ((USB_EPRX_DTOG1 & (wStaterx)) != 0U) {                             \
            _wRegVal ^= USB_EPRX_DTOG1;                                        \
        }                                                                      \
        /* toggle second bit ? */                                              \
        if ((USB_EPRX_DTOG2 & (wStaterx)) != 0U) {                             \
            _wRegVal ^= USB_EPRX_DTOG2;                                        \
        }                                                                      \
        /* toggle first bit ? */                                               \
        if ((USB_EPTX_DTOG1 & (wStatetx)) != 0U) {                             \
            _wRegVal ^= USB_EPTX_DTOG1;                                        \
        }                                                                      \
        /* toggle second bit ?  */                                             \
        if ((USB_EPTX_DTOG2 & (wStatetx)) != 0U) {                             \
            _wRegVal ^= USB_EPTX_DTOG2;                                        \
        }                                                                      \
                                                                               \
        PCD_SET_ENDPOINT((USBx), (bEpNum),                                     \
                         (_wRegVal | USB_EP_CTR_RX | USB_EP_CTR_TX));          \
    } while (0) /* PCD_SET_EP_TXRX_STATUS */

#define PCD_GET_EP_TX_STATUS(USBx, bEpNum)                                     \
    ((uint16_t)PCD_GET_ENDPOINT((USBx), (bEpNum)) & USB_EPTX_STAT)
#define PCD_GET_EP_RX_STATUS(USBx, bEpNum)                                     \
    ((uint16_t)PCD_GET_ENDPOINT((USBx), (bEpNum)) & USB_EPRX_STAT)

#define PCD_SET_EP_TX_VALID(USBx, bEpNum)                                      \
    (PCD_SET_EP_TX_STATUS((USBx), (bEpNum), USB_EP_TX_VALID))
#define PCD_SET_EP_RX_VALID(USBx, bEpNum)                                      \
    (PCD_SET_EP_RX_STATUS((USBx), (bEpNum), USB_EP_RX_VALID))

#define PCD_GET_EP_TX_STALL_STATUS(USBx, bEpNum)                               \
    (PCD_GET_EP_TX_STATUS((USBx), (bEpNum)) == USB_EP_TX_STALL)
#define PCD_GET_EP_RX_STALL_STATUS(USBx, bEpNum)                               \
    (PCD_GET_EP_RX_STATUS((USBx), (bEpNum)) == USB_EP_RX_STALL)

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

#define PCD_SET_OUT_STATUS(USBx, bEpNum) PCD_SET_EP_KIND((USBx), (bEpNum))
#define PCD_CLEAR_OUT_STATUS(USBx, bEpNum) PCD_CLEAR_EP_KIND((USBx), (bEpNum))

#define PCD_SET_BULK_EP_DBUF(USBx, bEpNum) PCD_SET_EP_KIND((USBx), (bEpNum))
#define PCD_CLEAR_BULK_EP_DBUF(USBx, bEpNum) PCD_CLEAR_EP_KIND((USBx), (bEpNum))

#define PCD_CLEAR_RX_EP_CTR(USBx, bEpNum)                                      \
    do {                                                                       \
        uint16_t _wRegVal;                                                     \
                                                                               \
        _wRegVal =                                                             \
            PCD_GET_ENDPOINT((USBx), (bEpNum)) & (0x7FFFU & USB_EPREG_MASK);   \
                                                                               \
        PCD_SET_ENDPOINT((USBx), (bEpNum), (_wRegVal | USB_EP_CTR_TX));        \
    } while (0) /* PCD_CLEAR_RX_EP_CTR */

#define PCD_CLEAR_TX_EP_CTR(USBx, bEpNum)                                      \
    do {                                                                       \
        uint16_t _wRegVal;                                                     \
                                                                               \
        _wRegVal =                                                             \
            PCD_GET_ENDPOINT((USBx), (bEpNum)) & (0xFF7FU & USB_EPREG_MASK);   \
                                                                               \
        PCD_SET_ENDPOINT((USBx), (bEpNum), (_wRegVal | USB_EP_CTR_RX));        \
    } while (0) /* PCD_CLEAR_TX_EP_CTR */

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

#define PCD_GET_EP_ADDRESS(USBx, bEpNum)                                       \
    ((uint8_t)(PCD_GET_ENDPOINT((USBx), (bEpNum)) & USB_EPADDR_FIELD))

#define PCD_EP_TX_CNT(USBx, bEpNum)                                            \
    ((uint16_t *)((((uint32_t)(USBx)->BTABLE + ((uint32_t)(bEpNum) * 8U) +     \
                    2U) *                                                      \
                   PMA_ACCESS) +                                               \
                  ((uint32_t)(USBx) + 0x400U)))

#define PCD_EP_RX_CNT(USBx, bEpNum)                                            \
    ((uint16_t *)((((uint32_t)(USBx)->BTABLE + ((uint32_t)(bEpNum) * 8U) +     \
                    6U) *                                                      \
                   PMA_ACCESS) +                                               \
                  ((uint32_t)(USBx) + 0x400U)))

#define PCD_SET_EP_TX_ADDRESS(USBx, bEpNum, wAddr)                             \
    do {                                                                       \
        __IO uint16_t *_wRegVal;                                               \
        uint32_t _wRegBase = (uint32_t)USBx;                                   \
                                                                               \
        _wRegBase += (uint32_t)(USBx)->BTABLE;                                 \
        _wRegVal =                                                             \
            (__IO uint16_t *)(_wRegBase + 0x400U +                             \
                              (((uint32_t)(bEpNum) * 8U) * PMA_ACCESS));       \
        *_wRegVal = ((wAddr) >> 1) << 1;                                       \
    } while (0) /* PCD_SET_EP_TX_ADDRESS */

#define PCD_SET_EP_RX_ADDRESS(USBx, bEpNum, wAddr)                             \
    do {                                                                       \
        __IO uint16_t *_wRegVal;                                               \
        uint32_t _wRegBase = (uint32_t)USBx;                                   \
                                                                               \
        _wRegBase += (uint32_t)(USBx)->BTABLE;                                 \
        _wRegVal = (__IO uint16_t *)(_wRegBase + 0x400U +                      \
                                     ((((uint32_t)(bEpNum) * 8U) + 4U) *       \
                                      PMA_ACCESS));                            \
        *_wRegVal = ((wAddr) >> 1) << 1;                                       \
    } while (0) /* PCD_SET_EP_RX_ADDRESS */

#define PCD_GET_EP_TX_ADDRESS(USBx, bEpNum)                                    \
    ((uint16_t)*PCD_EP_TX_ADDRESS((USBx), (bEpNum)))
#define PCD_GET_EP_RX_ADDRESS(USBx, bEpNum)                                    \
    ((uint16_t)*PCD_EP_RX_ADDRESS((USBx), (bEpNum)))

#define PCD_CALC_BLK32(pdwReg, wCount, wNBlocks)                               \
    do {                                                                       \
        (wNBlocks) = (wCount) >> 5;                                            \
        if (((wCount) & 0x1fU) == 0U) {                                        \
            (wNBlocks)--;                                                      \
        }                                                                      \
        *(pdwReg) |= (uint16_t)(((wNBlocks) << 10) | USB_CNTRX_BLSIZE);        \
    } while (0) /* PCD_CALC_BLK32 */

#define PCD_CALC_BLK2(pdwReg, wCount, wNBlocks)                                \
    do {                                                                       \
        (wNBlocks) = (wCount) >> 1;                                            \
        if (((wCount) & 0x1U) != 0U) {                                         \
            (wNBlocks)++;                                                      \
        }                                                                      \
        *(pdwReg) |= (uint16_t)((wNBlocks) << 10);                             \
    } while (0) /* PCD_CALC_BLK2 */

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

#define PCD_SET_EP_RX_DBUF0_CNT(USBx, bEpNum, wCount)                          \
    do {                                                                       \
        uint32_t _wRegBase = (uint32_t)(USBx);                                 \
        __IO uint16_t *pdwReg;                                                 \
                                                                               \
        _wRegBase += (uint32_t)(USBx)->BTABLE;                                 \
        pdwReg = (__IO uint16_t *)(_wRegBase + 0x400U +                        \
                                   ((((uint32_t)(bEpNum) * 8U) + 2U) *         \
                                    PMA_ACCESS));                              \
        PCD_SET_EP_CNT_RX_REG(pdwReg, (wCount));                               \
    } while (0)

#define PCD_SET_EP_TX_CNT(USBx, bEpNum, wCount)                                \
    do {                                                                       \
        uint32_t _wRegBase = (uint32_t)(USBx);                                 \
        __IO uint16_t *_wRegVal;                                               \
                                                                               \
        _wRegBase += (uint32_t)(USBx)->BTABLE;                                 \
        _wRegVal = (__IO uint16_t *)(_wRegBase + 0x400U +                      \
                                     ((((uint32_t)(bEpNum) * 8U) + 2U) *       \
                                      PMA_ACCESS));                            \
        *_wRegVal = (uint16_t)(wCount);                                        \
    } while (0)

#define PCD_SET_EP_RX_CNT(USBx, bEpNum, wCount)                                \
    do {                                                                       \
        uint32_t _wRegBase = (uint32_t)(USBx);                                 \
        __IO uint16_t *_wRegVal;                                               \
                                                                               \
        _wRegBase += (uint32_t)(USBx)->BTABLE;                                 \
        _wRegVal = (__IO uint16_t *)(_wRegBase + 0x400U +                      \
                                     ((((uint32_t)(bEpNum) * 8U) + 6U) *       \
                                      PMA_ACCESS));                            \
        PCD_SET_EP_CNT_RX_REG(_wRegVal, (wCount));                             \
    } while (0)

#define PCD_GET_EP_TX_CNT(USBx, bEpNum)                                        \
    ((uint32_t)(*PCD_EP_TX_CNT((USBx), (bEpNum))) & 0x3ffU)
#define PCD_GET_EP_RX_CNT(USBx, bEpNum)                                        \
    ((uint32_t)(*PCD_EP_RX_CNT((USBx), (bEpNum))) & 0x3ffU)

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

#define PCD_GET_EP_DBUF0_ADDR(USBx, bEpNum)                                    \
    (PCD_GET_EP_TX_ADDRESS((USBx), (bEpNum)))
#define PCD_GET_EP_DBUF1_ADDR(USBx, bEpNum)                                    \
    (PCD_GET_EP_RX_ADDRESS((USBx), (bEpNum)))

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
                                       PMA_ACCESS));                           \
                *_wEPRegVal = (uint16_t)(wCount);                              \
            }                                                                  \
        }                                                                      \
    } while (0) /* SetEPDblBuf1Count */

#define PCD_SET_EP_DBUF_CNT(USBx, bEpNum, bDir, wCount)                        \
    do {                                                                       \
        PCD_SET_EP_DBUF0_CNT((USBx), (bEpNum), (bDir), (wCount));              \
        PCD_SET_EP_DBUF1_CNT((USBx), (bEpNum), (bDir), (wCount));              \
    } while (0) /* PCD_SET_EP_DBUF_CNT */

#define PCD_GET_EP_DBUF0_CNT(USBx, bEpNum) (PCD_GET_EP_TX_CNT((USBx), (bEpNum)))
#define PCD_GET_EP_DBUF1_CNT(USBx, bEpNum) (PCD_GET_EP_RX_CNT((USBx), (bEpNum)))

#endif // HAL_PCD_H
