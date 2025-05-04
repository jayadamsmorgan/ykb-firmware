#include "hal/pcd.h"
#include "logging.h"

extern PCD_HandleTypeDef hpcd_USB_FS;

void NMI_Handler(void) { LOG_TRACE("NMI_Handler."); }

void HardFault_Handler(void) {
    LOG_CRITICAL("HARDFAULT.");
    LOG_DEBUG("FLASH->SR: %d", FLASH->SR);
    while (1) {
    }
}

void MemManage_Handler(void) {
    LOG_CRITICAL("MEMMANAGE.");
    while (1) {
    }
}

void BusFault_Handler(void) {
    LOG_CRITICAL("BUSFAULT.");
    while (1) {
    }
}

void UsageFault_Handler(void) {
    LOG_CRITICAL("USAGEFAULT.");
    while (1) {
    }
}

void SVC_Handler(void) { LOG_TRACE("SVC Handler triggered."); }

void DebugMon_Handler(void) { LOG_TRACE("DebugMon Handler triggered."); }

void PendSV_Handler(void) { LOG_TRACE("PendSV Handler triggered"); }

void USB_LP_IRQHandler(void) { HAL_PCD_IRQHandler(&hpcd_USB_FS); }
