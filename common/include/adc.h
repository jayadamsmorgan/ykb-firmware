#ifndef ADC_H
#define ADC_H

#include "hal_err.h"
#include "hal_gpio.h"

hal_err adc_register_channel(const gpio_pin_t *pin, uint8_t *rank_number);

hal_err setup_adc();

#endif // ADC_H
