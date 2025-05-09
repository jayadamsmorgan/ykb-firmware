#ifndef CONFIG_PINOUT_H
#define CONFIG_PINOUT_H

#include "hal_gpio.h"

#define PIN_ERROR_HANDLER_LED PC4
#define PIN_SERIAL_ACTIVITY_LED PC5
#define PIN_CAPSLOCK_LED PC6

// Dactyl V1 was supposed to have LEDs, so it's here just in case
// #define SK6812_ENABLED 1
// #define PIN_SK6812_LED PA3

#define PIN_DEBUG_UART_RX PB10
#define PIN_DEBUG_UART_TX PA2

// Mux pins, only used in src/keyboard.c
#define PIN_MUX1_CMN PC0
#define PIN_MUX1_A PB7
#define PIN_MUX1_B PB6
#define PIN_MUX1_C PB5
#define PIN_MUX1_D PB4

#define PIN_MUX2_CMN PC1
#define PIN_MUX2_A PD0
#define PIN_MUX2_B PC12
#define PIN_MUX2_C PC11
#define PIN_MUX2_D PC10

#define PIN_MUX3_CMN PC2
#define PIN_MUX3_A PB15
#define PIN_MUX3_B PB14
#define PIN_MUX3_C PB13
#define PIN_MUX3_D PB12

#endif // CONFIG_PINOUT_H
