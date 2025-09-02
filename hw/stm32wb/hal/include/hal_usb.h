#ifndef HAL_USB_H
#define HAL_USB_H

#include "hal_err.h"
#include "hal_exti.h"
#include "stm32wbxx.h"

#define EP_MPS_64 0U
#define EP_MPS_32 1U
#define EP_MPS_16 2U
#define EP_MPS_8 3U

#define EP_TYPE_CTRL 0U
#define EP_TYPE_ISOC 1U
#define EP_TYPE_BULK 2U
#define EP_TYPE_INTR 3U
#define EP_TYPE_MSK 3U

#define USBD_FS_SPEED 2U

#define BTABLE_ADDRESS 0x000U
#define PMA_ACCESS 1U

#ifndef USB_EP_RX_STRX
#define USB_EP_RX_STRX (0x3U << 12)
#endif /* USB_EP_RX_STRX */

#define EP_ADDR_MSK 0x7U

#ifndef USE_USB_DOUBLE_BUFFER
#define USE_USB_DOUBLE_BUFFER 1U
#endif /* USE_USB_DOUBLE_BUFFER */

typedef struct {
    uint8_t dev_endpoints; /*!< Device Endpoints number.
                                This parameter depends on the used USB core.
                                This parameter must be a number between Min_Data
                              = 1 and Max_Data = 15 */

    uint8_t speed; /*!< USB Core speed.
                        This parameter can be any value of @ref
                      PCD_Speed/HCD_Speed (HCD_SPEED_xxx, HCD_SPEED_xxx) */

    uint8_t ep0_mps; /*!< Set the Endpoint 0 Max Packet size. */

    uint8_t phy_itface; /*!< Select the used PHY interface.
                             This parameter can be any value of @ref
                           PCD_PHY_Module/HCD_PHY_Module  */

    uint8_t Sof_enable; /*!< Enable or disable the output of the SOF signal. */

    uint8_t low_power_enable; /*!< Enable or disable the low Power Mode. */

    uint8_t lpm_enable; /*!< Enable or disable Link Power Management. */

    uint8_t battery_charging_enable; /*!< Enable or disable Battery charging. */
} USB_CfgTypeDef;

typedef struct {
    uint8_t num; /*!< Endpoint number
                      This parameter must be a number between Min_Data = 1 and
                    Max_Data = 15   */

    uint8_t is_in; /*!< Endpoint direction
                        This parameter must be a number between Min_Data = 0 and
                      Max_Data = 1    */

    uint8_t is_stall; /*!< Endpoint stall condition
                           This parameter must be a number between Min_Data = 0
                         and Max_Data = 1    */

    uint8_t type; /*!< Endpoint type
                       This parameter can be any value of @ref USB_LL_EP_Type */

    uint8_t data_pid_start; /*!< Initial data PID
                                 This parameter must be a number between
                               Min_Data = 0 and Max_Data = 1    */

    uint16_t pmaadress; /*!< PMA Address
                             This parameter can be any value between Min_addr =
                           0 and Max_addr = 1K   */

    uint16_t pmaaddr0; /*!< PMA Address0
                            This parameter can be any value between Min_addr = 0
                          and Max_addr = 1K   */

    uint16_t pmaaddr1; /*!< PMA Address1
                            This parameter can be any value between Min_addr = 0
                          and Max_addr = 1K   */

    uint8_t doublebuffer; /*!< Double buffer enable
                               This parameter can be 0 or 1 */

    uint32_t maxpacket; /*!< Endpoint Max packet size
                             This parameter must be a number between Min_Data =
                           0 and Max_Data = 64KB */

    uint8_t *xfer_buff; /*!< Pointer to transfer buffer */

    uint32_t xfer_len; /*!< Current transfer length */

    uint32_t xfer_count; /*!< Partial transfer length in case of multi packet
                            transfer                 */

    uint32_t xfer_len_db; /*!< double buffer transfer length used with bulk
                             double buffer in            */

    uint8_t xfer_fill_db; /*!< double buffer Need to Fill new buffer  used with
                             bulk_in                 */
} USB_EPTypeDef;

static inline void hal_usb_wakeup_it_enable() {
    hal_exti_enable_it(EXTI_EVENT_28);
}

static inline void hal_usb_wakeup_it_disable() {
    hal_exti_disable_it(EXTI_EVENT_28);
}

static inline bool hal_usb_wakeup_it_enabled() {
    return hal_exti_enabled_it(EXTI_EVENT_28);
}

void hal_usb_device_init(USB_CfgTypeDef cfg);
void hal_usb_enable_interrupts();
void hal_usb_disable_interrupts();

hal_err hal_usb_activate_endpoint(USB_EPTypeDef *ep);
hal_err USB_DeactivateEndpoint(USB_TypeDef *USBx, USB_EPTypeDef *ep);
hal_err USB_EPStartXfer(USB_TypeDef *USBx, USB_EPTypeDef *ep);
hal_err USB_EPSetStall(USB_TypeDef *USBx, USB_EPTypeDef *ep);
hal_err USB_EPClearStall(USB_TypeDef *USBx, USB_EPTypeDef *ep);
hal_err USB_EPStopXfer(USB_TypeDef *USBx, USB_EPTypeDef *ep);

hal_err USB_SetDevAddress(USB_TypeDef *USBx, uint8_t address);
hal_err USB_DevConnect(USB_TypeDef *USBx);
hal_err USB_DevDisconnect(USB_TypeDef *USBx);
hal_err USB_StopDevice(USB_TypeDef *USBx);
uint32_t USB_ReadInterrupts(USB_TypeDef const *USBx);
hal_err USB_ActivateRemoteWakeup(USB_TypeDef *USBx);
hal_err USB_DeActivateRemoteWakeup(USB_TypeDef *USBx);

void USB_WritePMA(USB_TypeDef const *USBx, uint8_t *pbUsrBuf,
                  uint16_t wPMABufAddr, uint16_t wNBytes);

void USB_ReadPMA(USB_TypeDef const *USBx, uint8_t *pbUsrBuf,
                 uint16_t wPMABufAddr, uint16_t wNBytes);

#endif // HAL_USB_H
