#ifndef ADC_H
#define ADC_H

#include "hal/hal_err.h"
#include <stdint.h>

hal_err setup_adc();

hal_err setup_tempsensor();

hal_err adc_read_blocking(uint32_t *value);

#endif // ADC_H
