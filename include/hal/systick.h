#ifndef HAL_SYSTICK_H
#define HAL_SYSTICK_H

#include <stdint.h>

void SysTick_Init();
void SysTick_Handler();
void delay_ms(uint32_t ms);
uint32_t getMillis();

#endif // HAL_SYSTICK_H
