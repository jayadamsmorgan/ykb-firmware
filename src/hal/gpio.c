#include "hal/gpio.h"
#include "stm32wb55xx.h"
#include "stm32wbxx.h"
#include <errno.h>
#include <stdint.h>

#define SET_1BIT(REG, PINNUM, VALUE)                                           \
    do {                                                                       \
        uint32_t reg = REG;                                                    \
        reg &= ~(0b1 << PINNUM);                                               \
        reg |= (VALUE & 0b1) << PINNUM;                                        \
        REG = reg;                                                             \
    } while (0)

#define SET_2BITS(REG, PINNUM, VALUE)                                          \
    do {                                                                       \
        uint32_t reg = REG;                                                    \
        reg &= ~(0b11 << (PINNUM * 2));                                        \
        reg |= (VALUE & 0b11) << (PINNUM * 2);                                 \
        REG = reg;                                                             \
    } while (0)

#define READ_2BITS(REG, PINNUM) ((REG >> (2 * PINNUM)) & 0b11)

#define SET_3BITS(REG, PINNUM, VALUE)                                          \
    do {                                                                       \
        uint32_t reg = REG;                                                    \
        reg &= ~(0b111 << (PINNUM * 3));                                       \
        reg |= (VALUE & 0b111) << (PINNUM * 3);                                \
        REG = reg;                                                             \
    } while (0)

#define SET_4BITS(REG, PINNUM, VALUE)                                          \
    do {                                                                       \
        uint32_t reg = REG;                                                    \
        reg &= ~(0b1111 << (PINNUM * 4));                                      \
        reg |= (VALUE & 0b1111) << (PINNUM * 4);                               \
        REG = reg;                                                             \
    } while (0)

void gpio_set_mode(gpio_pin_t pin, gpio_mode mode) {
    SET_2BITS(pin.gpio->MODER, pin.num, mode);
}

void gpio_set_af_mode(gpio_pin_t pin, gpio_af_mode mode) {
    if (pin.num > 7) {
        SET_4BITS(pin.gpio->AFR[1], pin.num - 8, mode);
    } else {
        SET_4BITS(pin.gpio->AFR[0], pin.num, mode);
    }
}

void gpio_set_pupd(gpio_pin_t pin, gpio_pupd pupd) {
    SET_2BITS(pin.gpio->PUPDR, pin.num, pupd);
}

void gpio_set_speed(gpio_pin_t pin, gpio_speed speed) {
    SET_2BITS(pin.gpio->OSPEEDR, pin.num, speed);
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
        asm("nop"); // TODO: Not sure.
    }

    CLEAR_BIT(ADC1->ISR, ADC_ISR_ADRDY);
}

void gpio_adc_stop(bool blocking) {
    SET_BIT(ADC1->CR, ADC_CR_ADSTP);
    SET_BIT(ADC1->CR, ADC_CR_JADSTP);

    while (READ_BIT(ADC1->CR, ADC_CR_ADSTART) ||
           READ_BIT(ADC1->CR, ADC_CR_JADSTART)) {
        asm("nop"); // TODO: Not sure.
    }
    SET_BIT(ADC1->CR, ADC_CR_ADDIS);

    if (!blocking) {
        return;
    }

    while (READ_BIT(ADC1->CR, ADC_CR_ADEN)) {
        asm("nop"); // TODO: Not sure.
    }
}

// Requires `gpio_adc_end()`
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

// Requires `gpio_adc_calibrate()`, `gpio_adc_start()`
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

typedef enum {
    GPIO_ADC_SMP_2_5_CYCLES = 0b000,
    GPIO_ADC_SMP_6_5_CYCLES = 0b001,
    GPIO_ADC_SMP_12_5_CYCLES = 0b010,
    GPIO_ADC_SMP_24_5_CYCLES = 0b011,
    GPIO_ADC_SMP_47_5_CYCLES = 0b100,
    GPIO_ADC_SMP_92_5_CYCLES = 0b101,
    GPIO_ADC_SMP_247_5_CYCLES = 0b110,
    GPIO_ADC_SMP_640_5_CYCLES = 0b111,
} gpio_adc_sampling_time;

error_t gpio_adc_set_sampling_time(gpio_pin_t pin,
                                   gpio_adc_sampling_time time) {
    if (pin.adc_chan <= -1) {
        return -1;
    }
    if (pin.adc_chan > 18) {
        return -2;
    }
    if (READ_2BITS(pin.gpio->MODER, pin.num) != GPIO_MODE_ANALOG) {
        return -3;
    }
    if (pin.adc_chan > 9) {
        SET_3BITS(ADC1->SMPR2, pin.adc_chan - 10, time);
    } else {
        SET_3BITS(ADC1->SMPR1, pin.adc_chan, time);
    }
    ADC1->SMPR1;
    return 0;
}

void gpio_digital_write(gpio_pin_t pin, bool val) {
    SET_1BIT(pin.gpio->ODR, pin.num, val);
}

bool gpio_digital_read(gpio_pin_t pin) {
    return READ_BIT(pin.gpio->IDR, pin.num);
}

void gpio_analog_write(gpio_pin_t pin, uint8_t val) {}

uint8_t gpio_analog_read(gpio_pin_t pin) {
    if (pin.adc_chan <= -1) {
        return 0;
    }
    if (pin.adc_chan > 18) {
        return 0;
    }
    if (READ_BIT(ADC1->CR, ADC_CR_ADEN) == 0) {
        return 0;
    }
    SET_BIT(ADC1->CR, ADC_CR_ADSTART);
    // while (READ_BIT(ADC1->ISR, ADC_ISR_EOC) == )
    return 0;
    // uint32_t reg = pin.gpio->
}
