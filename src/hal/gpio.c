#include "hal/gpio.h"

#include "hal/bits.h"
#include "hal/cortex.h"
#include "hal/hal_err.h"
#include "stm32wb55xx.h"
#include "stm32wbxx.h"
#include <stdint.h>

static gpio_adc_resolution seleceted_adc_res = GPIO_ADC_RES_12BIT;

void gpio_turn_on_port(volatile GPIO_TypeDef *port) {
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

void gpio_turn_off_port(volatile GPIO_TypeDef *port) {
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

void gpio_adc_start() {
    SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_ADCEN);
    CLEAR_BIT(ADC1->ISR, ADC_ISR_ADRDY);
    SET_BIT(ADC1->CR, ADC_CR_ADEN);

    while (READ_BIT(ADC1->ISR, ADC_ISR_ADRDY) == 0) {
    }

    CLEAR_BIT(ADC1->ISR, ADC_ISR_ADRDY);
}

void gpio_adc_stop() {
    SET_BIT(ADC1->CR, ADC_CR_ADSTP);
    SET_BIT(ADC1->CR, ADC_CR_JADSTP);

    while (READ_BIT(ADC1->CR, ADC_CR_ADSTART) ||
           READ_BIT(ADC1->CR, ADC_CR_JADSTART)) {
    }
    SET_BIT(ADC1->CR, ADC_CR_ADDIS);

    while (READ_BIT(ADC1->CR, ADC_CR_ADEN)) {
    }
    CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_ADCEN);
}

hal_err gpio_adc_calibrate(gpio_calib_input_mode mode,
                           uint8_t *const calibration_factor) {

    if (READ_BIT(ADC1->CR, ADC_CR_DEEPPWD) == 1) {
        return ERR_GPIO_ADC_CALIB_DEEPPWD;
    }
    if (READ_BIT(ADC1->CR, ADC_CR_ADVREGEN) == 0) {
        return ERR_GPIO_ADC_CALIB_ADVREGEN;
    }
    if (READ_BIT(ADC1->CR, ADC_CR_ADEN) == 1) {
        return ERR_GPIO_ADC_CALIB_ADEN;
    }

    if (mode == GPIO_CALIB_INPUT_DIFFERENTIAL) {
        SET_BIT(ADC1->CR, ADC_CR_ADCALDIF);
    }

    SET_BIT(ADC1->CR, ADC_CR_ADCAL);

    while (READ_BIT(ADC1->CR, ADC_CR_ADCAL) == 1) {
    }

    if (!calibration_factor) {
        return OK;
    }

    switch (mode) {

    case GPIO_CALIB_INPUT_SINGLE_ENDED:
        *calibration_factor =
            (ADC1->CALFACT >> ADC_CALFACT_CALFACT_S_Pos) & BITMASK_7BIT;
        break;

    case GPIO_CALIB_INPUT_DIFFERENTIAL:
        *calibration_factor =
            (ADC1->CALFACT >> ADC_CALFACT_CALFACT_D_Pos) & BITMASK_7BIT;
        break;
    }

    return OK;
}

hal_err gpio_adc_apply_calibration(gpio_calib_input_mode mode,
                                   const uint8_t calibration_factor) {
    if (READ_BIT(ADC1->CR, ADC_CR_ADEN) == 0) {
        return ERR_GPIO_ADC_APPLY_CALIB_ADEN;
    }
    if (READ_BIT(ADC1->CR, ADC_CR_ADSTART) == 1) {
        return ERR_GPIO_ADC_APPLY_CALIB_ADSTART;
    }
    if (READ_BIT(ADC1->CR, ADC_CR_JADSTART) == 1) {
        return ERR_GPIO_ADC_APPLY_CALIB_JADSTART;
    }
    if (mode == GPIO_CALIB_INPUT_DIFFERENTIAL) {
        MODIFY_BITS(ADC1->CALFACT, ADC_CALFACT_CALFACT_D_Pos,
                    calibration_factor, BITMASK_7BIT);
    } else {
        MODIFY_BITS(ADC1->CALFACT, ADC_CALFACT_CALFACT_S_Pos,
                    calibration_factor, BITMASK_7BIT);
    }
    return OK;
}

hal_err gpio_adc_set_sampling_time(gpio_pin_t pin,
                                   gpio_adc_sampling_time time) {
    if (pin.adc_chan < 0 || pin.adc_chan > 18) {
        return ERR_GPIO_ADC_SET_ST_NON_ADC;
    }
    if (READ_BITS(pin.gpio->MODER, pin.num * 2, BITMASK_2BIT) !=
        GPIO_MODE_ANALOG) {
        return ERR_GPIO_ADC_SET_ST_NOT_ANALOG;
    }
    if (pin.adc_chan > 9) {
        MODIFY_BITS(ADC1->SMPR2, (pin.adc_chan - 10) * 3, time, BITMASK_3BIT);
    } else {
        MODIFY_BITS(ADC1->SMPR1, pin.adc_chan * 3, time, BITMASK_3BIT);
    }
    return OK;
}

hal_err gpio_adc_set_resolution(gpio_adc_resolution resolution) {

    if (READ_BIT(ADC1->CR, ADC_CR_ADSTART) == 1) {
        return ERR_GPIO_ADC_SET_RES_ADSTART;
    }
    if (READ_BIT(ADC1->CR, ADC_CR_JADSTART) == 1) {
        return ERR_GPIO_ADC_SET_RES_JADSTART;
    }

    MODIFY_BITS(ADC1->CFGR, ADC_CFGR_RES_Pos, resolution, BITMASK_2BIT);
    seleceted_adc_res = resolution;

    return OK;
}

void gpio_digital_write(gpio_pin_t pin, bool val) {
    MODIFY_BITS(pin.gpio->ODR, pin.num, val, BITMASK_1BIT);
}

bool gpio_digital_read(gpio_pin_t pin) {
    return READ_BITS(pin.gpio->IDR, pin.num, BITMASK_1BIT);
}

hal_err gpio_analog_read(gpio_pin_t pin, uint32_t *const data) {

    if (pin.adc_chan < 0 || pin.adc_chan > 18) {
        return ERR_GPIO_ADC_READ_NON_ADC;
    }
    if (READ_BIT(ADC1->CR, ADC_CR_ADEN) == 0) {
        return ERR_GPIO_ADC_READ_ADEN;
    }

    SET_BIT(ADC1->CR, ADC_CR_ADSTART);
    while (READ_BIT(ADC1->ISR, ADC_ISR_EOC) == 0) {
    }

    if (!data) {
        // Cannot write to data, but no error actually happened
        return OK;
    }

    switch (seleceted_adc_res) {

    case GPIO_ADC_RES_12BIT:
        *data = READ_BITS(ADC1->DR, 0, BITMASK_12BIT);
        break;

    case GPIO_ADC_RES_10BIT:
        *data = READ_BITS(ADC1->DR, 0, BITMASK_10BIT);
        break;

    case GPIO_ADC_RES_8BIT:
        *data = READ_REG(ADC1->DR) & BITMASK_8BIT;
        break;

    case GPIO_ADC_RES_6BIT:
        *data = READ_REG(ADC1->DR) & BITMASK_6BIT;
        break;
    }

    return OK;
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
