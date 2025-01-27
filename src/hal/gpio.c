#include "hal/gpio.h"
#include "hal/bits.h"
#include "stm32wbxx.h"

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
}

error_t gpio_adc_calibrate(gpio_calib_input_mode mode,
                           uint8_t *const calibration_factor) {

    if (READ_BIT(ADC1->CR, ADC_CR_DEEPPWD) == 1) {
        return -1;
    }
    if (READ_BIT(ADC1->CR, ADC_CR_ADVREGEN) == 0) {
        return -2;
    }
    if (READ_BIT(ADC1->CR, ADC_CR_ADEN) == 1) {
        return -3;
    }

    if (mode == GPIO_CALIB_INPUT_DIFFERENTIAL) {
        SET_BIT(ADC1->CR, ADC_CR_ADCALDIF);
    }

    SET_BIT(ADC1->CR, ADC_CR_ADCAL);

    while (READ_BIT(ADC1->CR, ADC_CR_ADCAL) == 1) {
    }

    if (!calibration_factor) {
        return 0;
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

    return 0;
}

error_t gpio_adc_apply_calibration(gpio_calib_input_mode mode,
                                   const uint8_t calibration_factor) {
    if (READ_BIT(ADC1->CR, ADC_CR_ADEN) == 0) {
        return -2;
    }
    if (READ_BIT(ADC1->CR, ADC_CR_ADSTART) == 1) {
        return -3;
    }
    if (READ_BIT(ADC1->CR, ADC_CR_JADSTART) == 1) {
        return -4;
    }
    if (mode == GPIO_CALIB_INPUT_DIFFERENTIAL) {
        MODIFY_BITS(ADC1->CALFACT, ADC_CALFACT_CALFACT_D_Pos,
                    calibration_factor, BITMASK_7BIT);
    } else {
        MODIFY_BITS(ADC1->CALFACT, ADC_CALFACT_CALFACT_S_Pos,
                    calibration_factor, BITMASK_7BIT);
    }
    return 0;
}

error_t gpio_adc_set_sampling_time(gpio_pin_t pin,
                                   gpio_adc_sampling_time time) {
    if (pin.adc_chan <= -1) {
        return -1;
    }
    if (pin.adc_chan > 18) {
        return -2;
    }
    if (READ_BITS(pin.gpio->MODER, pin.num * 2, BITMASK_2BIT) !=
        GPIO_MODE_ANALOG) {
        return -3;
    }
    if (pin.adc_chan > 9) {
        MODIFY_BITS(ADC1->SMPR2, (pin.adc_chan - 10) * 3, time, BITMASK_3BIT);
    } else {
        MODIFY_BITS(ADC1->SMPR1, pin.adc_chan * 3, time, BITMASK_3BIT);
    }
    return 0;
}

error_t gpio_adc_set_resolution(gpio_adc_resolution resolution) {

    if (READ_BIT(ADC1->CR, ADC_CR_ADSTART) == 1) {
        return -1;
    }
    if (READ_BIT(ADC1->CR, ADC_CR_JADSTART) == 1) {
        return -2;
    }

    MODIFY_BITS(ADC1->CFGR, ADC_CFGR_RES_Pos, resolution, BITMASK_2BIT);
    seleceted_adc_res = resolution;

    return 0;
}

void gpio_digital_write(gpio_pin_t pin, bool val) {
    MODIFY_BITS(pin.gpio->ODR, pin.num, val, BITMASK_1BIT);
}

bool gpio_digital_read(gpio_pin_t pin) {
    return READ_BIT(pin.gpio->IDR, pin.num);
}

error_t gpio_analog_read(gpio_pin_t pin, uint32_t *const data) {

    if (pin.adc_chan <= -1) {
        return -1;
    }
    if (pin.adc_chan > 18) {
        return -2;
    }
    if (READ_BIT(ADC1->CR, ADC_CR_ADEN) == 0) {
        return -3;
    }

    SET_BIT(ADC1->CR, ADC_CR_ADSTART);
    while (READ_BIT(ADC1->ISR, ADC_ISR_EOC) == 0) {
    }

    if (!data) {
        // Cannot write to data, but no error actually happened
        return 0;
    }

    switch (seleceted_adc_res) {

    case GPIO_ADC_RES_12BIT:
        *data = READ_REG(ADC1->DR) & BITMASK_12BIT;
        break;

    case GPIO_ADC_RES_10BIT:
        *data = READ_REG(ADC1->DR) & BITMASK_10BIT;
        break;

    case GPIO_ADC_RES_8BIT:
        *data = READ_REG(ADC1->DR) & BITMASK_8BIT;
        break;

    case GPIO_ADC_RES_6BIT:
        *data = READ_REG(ADC1->DR) & BITMASK_6BIT;
        break;
    }

    return 0;
}
