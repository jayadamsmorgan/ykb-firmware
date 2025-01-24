#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

void SysTick_Init();
void SysTick_Handler();
void delay_ms(uint32_t ms);
uint32_t getMillis();

#endif // SYSTICK_H
