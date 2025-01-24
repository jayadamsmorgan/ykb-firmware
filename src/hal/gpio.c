#include "hal/gpio.h"
#include "hal/bits.h"
#include "stm32wbxx.h"

static gpio_adc_resolution seleceted_adc_res = GPIO_ADC_RES_12BIT;

void gpio_set_mode(gpio_pin_t pin, gpio_mode mode) {
    SET_2BITS(pin.gpio->MODER, pin.num * 2, mode);
}

void gpio_set_af_mode(gpio_pin_t pin, gpio_af_mode mode) {
    if (pin.num > 7) {
        SET_4BITS(pin.gpio->AFR[1], (pin.num - 8) * 4, mode);
    } else {
        SET_4BITS(pin.gpio->AFR[0], pin.num * 4, mode);
    }
}

void gpio_set_pupd(gpio_pin_t pin, gpio_pupd pupd) {
    SET_2BITS(pin.gpio->PUPDR, pin.num * 2, pupd);
}

void gpio_set_speed(gpio_pin_t pin, gpio_speed speed) {
    SET_2BITS(pin.gpio->OSPEEDR, pin.num * 2, speed);
}

void gpio_set_output_type(gpio_pin_t pin, gpio_output_type type) {
    SET_1BIT(pin.gpio->OTYPER, pin.num, type);
}

void gpio_adc_start(bool blocking) {
    CLEAR_BIT(ADC1->ISR, ADC_ISR_ADRDY);
    SET_BIT(ADC1->CR, ADC_CR_ADEN);

    if (!blocking) {
        return;
    }

    while (READ_BIT(ADC1->ISR, ADC_ISR_ADRDY) == 0) {
    }

    CLEAR_BIT(ADC1->ISR, ADC_ISR_ADRDY);
}

void gpio_adc_stop(bool blocking) {
    SET_BIT(ADC1->CR, ADC_CR_ADSTP);
    SET_BIT(ADC1->CR, ADC_CR_JADSTP);

    while (READ_BIT(ADC1->CR, ADC_CR_ADSTART) ||
           READ_BIT(ADC1->CR, ADC_CR_JADSTART)) {
    }
    SET_BIT(ADC1->CR, ADC_CR_ADDIS);

    if (!blocking) {
        return;
    }

    while (READ_BIT(ADC1->CR, ADC_CR_ADEN)) {
    }
}

error_t gpio_adc_calibrate(gpio_calib_input_mode mode, bool blocking,
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

    if (!blocking && !calibration_factor) {
        return 0;
    }

    while (READ_BIT(ADC1->CR, ADC_CR_ADCAL) == 1) {
        asm("nop"); // TODO: Not sure.
    }

    if (!calibration_factor) {
        return 0;
    }

    switch (mode) {

    case GPIO_CALIB_INPUT_SINGLE_ENDED:
        *calibration_factor =
            (ADC1->CALFACT >> ADC_CALFACT_CALFACT_S_Pos) & 0b1111111;
        break;

    case GPIO_CALIB_INPUT_DIFFERENTIAL:
        *calibration_factor =
            (ADC1->CALFACT >> ADC_CALFACT_CALFACT_D_Pos) & 0b1111111;
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
        MODIFY_REG(ADC1->CALFACT, ADC_CALFACT_CALFACT_D,
                   (calibration_factor & 0b1111111)
                       << ADC_CALFACT_CALFACT_D_Pos);
    } else {
        MODIFY_REG(ADC1->CALFACT, ADC_CALFACT_CALFACT_S,
                   (calibration_factor & 0b1111111)
                       << ADC_CALFACT_CALFACT_S_Pos);
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
    if (READ_2BITS(pin.gpio->MODER, pin.num * 2) != GPIO_MODE_ANALOG) {
        return -3;
    }
    if (pin.adc_chan > 9) {
        SET_3BITS(ADC1->SMPR2, (pin.adc_chan - 10) * 3, time);
    } else {
        SET_3BITS(ADC1->SMPR1, pin.adc_chan * 3, time);
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

    SET_2BITS(ADC1->CFGR, ADC_CFGR_RES_Pos, resolution);
    seleceted_adc_res = resolution;

    return 0;
}

void gpio_digital_write(gpio_pin_t pin, bool val) {
    SET_1BIT(pin.gpio->ODR, pin.num, val);
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
        asm("nop");
    }

    if (!data) {
        // Cannot write to data, but no error actually happened
        return 0;
    }

    switch (seleceted_adc_res) {

    case GPIO_ADC_RES_12BIT:
        *data = READ_REG(ADC1->DR) & 12U;
        break;

    case GPIO_ADC_RES_10BIT:
        *data = READ_REG(ADC1->DR) & 10U;
        break;

    case GPIO_ADC_RES_8BIT:
        *data = READ_REG(ADC1->DR) & 8U;
        break;

    case GPIO_ADC_RES_6BIT:
        *data = READ_REG(ADC1->DR) & 6U;
        break;
    }

    return 0;
}
