#include "settings.h"

#include "checks.h"

#if defined(ENABLE_DEFAULT_INTERRUPTS) && ENABLE_DEFAULT_INTERRUPTS == 1

#include "hal_pcd.h"

#include "logging.h"

extern PCD_HandleTypeDef hpcd_USB_FS;

void NMI_Handler(void) { LOG_TRACE("NMI_Handler."); }

void HardFault_Handler(void) {
    LOG_CRITICAL("HARDFAULT.");
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

#if defined(USB_ENABLED) && USB_ENABLED == 1
void USB_LP_IRQHandler(void) { HAL_PCD_IRQHandler(&hpcd_USB_FS); }
#endif // USB_ENABLED

#endif // ENABLE_DEFAULT_INTERRUPTS
