#include "gpio.h"

#include "rcc.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void gpio_turn_on_port(enum gpio_port port) {
    switch (port) {
    case GPIO_PORT_A:
        *RCC_AHB2ENR_REG |= 1U;
        break;
    case GPIO_PORT_B:
        *RCC_AHB2ENR_REG |= 2U;
        break;
    case GPIO_PORT_C:
        *RCC_AHB2ENR_REG |= 4U;
        break;
    case GPIO_PORT_D:
        *RCC_AHB2ENR_REG |= 8U;
        break;
    case GPIO_PORT_E:
        *RCC_AHB2ENR_REG |= 16U;
        break;
    case GPIO_PORT_H:
        *RCC_AHB2ENR_REG |= 128U;
        break;
    }
}

void gpio_turn_off_port(enum gpio_port port) {
    switch (port) {
    case GPIO_PORT_A:
        *RCC_AHB2ENR_REG &= ~1U;
        break;
    case GPIO_PORT_B:
        *RCC_AHB2ENR_REG &= ~2U;
        break;
    case GPIO_PORT_C:
        *RCC_AHB2ENR_REG &= ~4U;
        break;
    case GPIO_PORT_D:
        *RCC_AHB2ENR_REG &= ~8U;
        break;
    case GPIO_PORT_E:
        *RCC_AHB2ENR_REG &= ~16U;
        break;
    case GPIO_PORT_H:
        *RCC_AHB2ENR_REG &= ~128U;
        break;
    }
}

void gpio_set_mode(gpio_pin pin, uint8_t mode) {
    switch (pin.port) {
    case GPIO_PORT_A:
        GPIO_A_REG->MODER &= ~(3U << (pin.num * 2));
        GPIO_A_REG->MODER |= (mode & 3U) << (pin.num * 2);
        break;
    case GPIO_PORT_B:
        GPIO_B_REG->MODER &= ~(3U << (pin.num * 2));
        GPIO_B_REG->MODER |= (mode & 3U) << (pin.num * 2);
        break;
    case GPIO_PORT_C:
        GPIO_C_REG->MODER &= ~(3U << (pin.num * 2));
        GPIO_C_REG->MODER |= (mode & 3U) << (pin.num * 2);
        break;
    case GPIO_PORT_D:
        GPIO_D_REG->MODER &= ~(3U << (pin.num * 2));
        GPIO_D_REG->MODER |= (mode & 3U) << (pin.num * 2);
        break;
    case GPIO_PORT_E:
        GPIO_E_REG->MODER &= ~(3U << (pin.num * 2));
        GPIO_E_REG->MODER |= (mode & 3U) << (pin.num * 2);
        break;
    case GPIO_PORT_H:
        GPIO_H_REG->MODER &= ~(3U << (pin.num * 2));
        GPIO_H_REG->MODER |= (mode & 3U) << (pin.num * 2);
        break;
    }
}

void gpio_write(gpio_pin pin, bool val) {
    switch (pin.port) {
    case GPIO_PORT_A:
        GPIO_A_REG->BSRR = (1U << pin.num) << (val ? 0 : 16);
        break;
    case GPIO_PORT_B:
        GPIO_B_REG->BSRR = (1U << pin.num) << (val ? 0 : 16);
        break;
    case GPIO_PORT_C:
        GPIO_C_REG->BSRR = (1U << pin.num) << (val ? 0 : 16);
        break;
    case GPIO_PORT_D:
        GPIO_D_REG->BSRR = (1U << pin.num) << (val ? 0 : 16);
        break;
    case GPIO_PORT_E:
        GPIO_E_REG->BSRR = (1U << pin.num) << (val ? 0 : 16);
        break;
    case GPIO_PORT_H:
        GPIO_H_REG->BSRR = (1U << pin.num) << (val ? 0 : 16);
        break;
    }
}

#ifdef __cplusplus
}
#endif // __cplusplus
