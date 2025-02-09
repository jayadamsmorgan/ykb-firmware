#include "stm32wbxx_it.h"
#include "hal/gpio.h"
#include "stm32wbxx_hal.h"
#include "stm32wbxx_hal_pcd.h"

extern PCD_HandleTypeDef hpcd_USB_FS;

void NMI_Handler(void) {}

void HardFault_Handler(void) {
    while (1) {
    }
}

void MemManage_Handler(void) {
    while (1) {
    }
}

void BusFault_Handler(void) {
    while (1) {
    }
}

void UsageFault_Handler(void) {
    while (1) {
    }
}

void SVC_Handler(void) {}

void DebugMon_Handler(void) {}

void PendSV_Handler(void) {}

void USB_LP_IRQHandler(void) {
    gpio_digital_write(PB1, HIGH);
    HAL_PCD_IRQHandler(&hpcd_USB_FS);
    gpio_digital_write(PB1, LOW);
}
