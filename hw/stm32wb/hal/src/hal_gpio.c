#include "hal_gpio.h"

#include "hal_bits.h"
#include "hal_cortex.h"
#include "hal_err.h"
#include "stm32wbxx.h"

#include <stdint.h>

void gpio_turn_on_port(gpio_t *port) {
    if (port == GPIOA) {
        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN);
    } else if (port == GPIOB) {
        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN);
    } else if (port == GPIOC) {
        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN);
    } else if (port == GPIOD) {
        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIODEN);
    } else if (port == GPIOE) {
        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOEEN);
    } else if (port == GPIOH) {
        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOHEN);
    }
}

void gpio_turn_off_port(gpio_t *port) {
    if (port == GPIOA) {
        CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN);
    } else if (port == GPIOB) {
        CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN);
    } else if (port == GPIOC) {
        CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN);
    } else if (port == GPIOD) {
        CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIODEN);
    } else if (port == GPIOE) {
        CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOEEN);
    } else if (port == GPIOH) {
        CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOHEN);
    }
}

void gpio_set_mode(gpio_pin_t pin, gpio_mode mode) {
    MODIFY_BITS(pin.gpio->MODER, pin.num * 2, mode, BITMASK_2BIT);
}

void gpio_set_af_mode(gpio_pin_t pin, gpio_af_mode mode) {
    if (pin.num > 7) {
        MODIFY_BITS(pin.gpio->AFR[1], (pin.num - 8) * 4, mode, BITMASK_4BIT);
    } else {
        MODIFY_BITS(pin.gpio->AFR[0], pin.num * 4, mode, BITMASK_4BIT);
    }
}

void gpio_set_pupd(gpio_pin_t pin, gpio_pupd pupd) {
    MODIFY_BITS(pin.gpio->PUPDR, pin.num * 2, pupd, BITMASK_2BIT);
}

void gpio_set_speed(gpio_pin_t pin, gpio_speed speed) {
    MODIFY_BITS(pin.gpio->OSPEEDR, pin.num * 2, speed, BITMASK_2BIT);
}

void gpio_set_output_type(gpio_pin_t pin, gpio_output_type type) {
    MODIFY_BITS(pin.gpio->OTYPER, pin.num, type, BITMASK_1BIT);
}

void gpio_digital_write(gpio_pin_t pin, bool val) {
    MODIFY_BITS(pin.gpio->ODR, pin.num, val, BITMASK_1BIT);
}

bool gpio_digital_read(gpio_pin_t pin) {
    return READ_BITS(pin.gpio->IDR, pin.num, BITMASK_1BIT);
}

hal_err gpio_set_interrupt_falling(gpio_pin_t pin) {
    if (pin.num > 15) {
        return ERR_GPIO_SET_INTF_WR_PN;
    }
    MODIFY_BITS(EXTI->FTSR1, pin.num, 1U, BITMASK_1BIT);
    return OK;
}

hal_err gpio_set_interrupt_rising(gpio_pin_t pin) {
    if (pin.num > 15) {
        return ERR_GPIO_SET_INTR_WR_PN;
    }
    MODIFY_BITS(EXTI->RTSR1, pin.num, 1U, BITMASK_1BIT);
    return OK;
}

hal_err gpio_enable_interrupt(gpio_pin_t pin) {

    IRQn_Type irq;

    switch (pin.num) {
    case 0:
        irq = EXTI0_IRQn;
        break;
    case 1:
        irq = EXTI1_IRQn;
        break;
    case 2:
        irq = EXTI2_IRQn;
        break;
    case 3:
        irq = EXTI3_IRQn;
        break;
    case 4:
        irq = EXTI4_IRQn;
        break;
    case 5:
        irq = EXTI9_5_IRQn;
        break;
    case 6:
        irq = EXTI9_5_IRQn;
        break;
    case 7:
        irq = EXTI9_5_IRQn;
        break;
    case 8:
        irq = EXTI9_5_IRQn;
        break;
    case 9:
        irq = EXTI9_5_IRQn;
        break;
    case 10:
        irq = EXTI15_10_IRQn;
        break;
    case 11:
        irq = EXTI15_10_IRQn;
        break;
    case 12:
        irq = EXTI15_10_IRQn;
        break;
    case 13:
        irq = EXTI15_10_IRQn;
        break;
    case 14:
        irq = EXTI15_10_IRQn;
        break;
    case 15:
        irq = EXTI15_10_IRQn;
        break;
    default:
        return ERR_GPIO_SET_INT_WR_PN;
    }

    uint8_t gpio_chan;

    if (pin.gpio == GPIOA) {
        gpio_chan = 0;
    } else if (pin.gpio == GPIOB) {
        gpio_chan = 1;
    } else if (pin.gpio == GPIOC) {
        gpio_chan = 2;
    } else if (pin.gpio == GPIOD) {
        gpio_chan = 3;
    } else if (pin.gpio == GPIOE) {
        gpio_chan = 4;
    } else if (pin.gpio == GPIOH) {
        gpio_chan = 7;
    } else {
        return ERR_GPIO_SET_INT_WR_GPIO;
    }

    if (pin.num < 4) {
        MODIFY_BITS(SYSCFG->EXTICR[0], pin.num * 4, gpio_chan, BITMASK_3BIT);
    } else if (pin.num < 8) {
        MODIFY_BITS(SYSCFG->EXTICR[1], (pin.num - 4) * 4, gpio_chan,
                    BITMASK_3BIT);
    } else if (pin.num < 12) {
        MODIFY_BITS(SYSCFG->EXTICR[2], (pin.num - 8) * 4, gpio_chan,
                    BITMASK_3BIT);
    } else if (pin.num < 16) {
        MODIFY_BITS(SYSCFG->EXTICR[3], (pin.num - 12) * 4, gpio_chan,
                    BITMASK_3BIT);
    }

    MODIFY_BITS(EXTI->IMR1, pin.num, 1, BITMASK_1BIT);

    // Enable and set Button EXTI Interrupt to the lowest priority
    hal_err err;

    err = cortex_nvic_set_priority(irq, 0x0F, 0x00);
    if (err) {
        return err;
    }

    cortex_nvic_enable(irq);

    return OK;
}
