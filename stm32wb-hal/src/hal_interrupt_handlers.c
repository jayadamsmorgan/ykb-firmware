#include "hal_interrupt_handlers.h"

#include "hal.h"
#include "hal_systick.h"

__weak void NMI_Handler(void) {}

__weak void HardFault_Handler(void) {
    while (1) {
    }
}

__weak void MemManage_Handler(void) {
    while (1) {
    }
}

__weak void BusFault_Handler(void) {
    while (1) {
    }
}

__weak void UsageFault_Handler(void) {
    while (1) {
    }
}

__weak void SVC_Handler(void) {}

__weak void DebugMon_Handler(void) {}

__weak void PendSV_Handler(void) {}

__weak void USB_LP_IRQHandler(void) {}

__weak void SysTick_Handler(void) {}

__weak void EXTI15_10_IRQHandler(void) {}
