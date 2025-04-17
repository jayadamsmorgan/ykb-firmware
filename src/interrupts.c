#include "hal/gpio.h"
#include "hal/pcd.h"
#include "logging.h"

extern PCD_HandleTypeDef hpcd_USB_FS;

void NMI_Handler(void) {}

void HardFault_Handler(void) {
    LOG_CRITICAL("CORE: HARDFAULT.");
    while (1) {
    }
}

void MemManage_Handler(void) {
    LOG_CRITICAL("CORE: MEMMANAGE.");
    while (1) {
    }
}

void BusFault_Handler(void) {
    LOG_CRITICAL("CORE: BUSFAULT.");
    while (1) {
    }
}

void UsageFault_Handler(void) {
    LOG_CRITICAL("CORE: USAGEFAULT.");
    while (1) {
    }
}

void SVC_Handler(void) { LOG_TRACE("CORE: SVC Handler triggered."); }

void DebugMon_Handler(void) { LOG_TRACE("CORE: DebugMon Handler triggered."); }

void PendSV_Handler(void) { LOG_TRACE("CORE: PendSV Handler triggered"); }

void USB_LP_IRQHandler(void) { HAL_PCD_IRQHandler(&hpcd_USB_FS); }
