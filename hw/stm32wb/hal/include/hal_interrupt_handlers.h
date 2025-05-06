#ifndef HAL_INT_HANDLERS_H
#define HAL_INT_HANDLERS_H

void NMI_Handler(void);

void HardFault_Handler(void);

void MemManage_Handler(void);

void BusFault_Handler(void);

void UsageFault_Handler(void);

void SVC_Handler(void);

void DebugMon_Handler(void);

void PendSV_Handler(void);

void USB_LP_IRQHandler(void);

void EXTI15_10_IRQHandler(void);

#endif // HAL_INT_HANDLERS_H
