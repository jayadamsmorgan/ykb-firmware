#include "hal/clock.h"
#include "hal/bits.h"
#include "stm32wb55xx.h"
#include "stm32wbxx.h"
#include <errno.h>

typedef enum {
    CLOCK_SOURCE_HSI16,
    CLOCK_SOURCE_HSI48,
    CLOCK_SOURCE_MSI,
    CLOCK_SOURCE_HSE32,
    CLOCK_SOURCE_PLL,
} clock_source;

typedef enum {
    CLOCK_MSI_100_KHZ = 0b0001,
    CLOCK_MSI_200_KHZ = 0b0010,
    CLOCK_MSI_400_KHZ = 0b0011,
    CLOCK_MSI_800_KHZ = 0b0100,
    CLOCK_MSI_1_MHZ = 0b0101,
    CLOCK_MSI_2_MHZ = 0b0110,
    CLOCK_MSI_4_MHZ = 0b0111,
    CLOCK_MSI_8_MHZ = 0b1000,
    CLOCK_MSI_16_MHZ = 0b1001,
    CLOCK_MSI_24_MHZ = 0b1010,
    CLOCK_MSI_32_MHZ = 0b1011,
    CLOCK_MSI_64_MHZ = 0b1100,
} clock_msi_freq;

error_t clock_msi_select_freq(clock_msi_freq freq) {
    if (READ_BIT(RCC->CR, RCC_CR_MSION) == 0) {
        return -1;
    }

    SET_4BITS(RCC->CR, RCC_CR_MSIRANGE_Pos, freq);
    return 0;
}

error_t clock_msi_enable_pll() {
    if (READ_BIT(RCC->CR, RCC_CR_MSION) == 0) {
        return -1;
    }

    SET_BIT(RCC->CR, RCC_CR_MSIPLLEN);
    return 0;
}

error_t clock_select_source(clock_source source) {
    switch (source) {

    case CLOCK_SOURCE_HSI16:
        if (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == 0) {
            return -1;
        }
        SET_BIT(RCC->CR, RCC_CR_HSION);
        break;

    case CLOCK_SOURCE_HSI48:
        if (READ_BIT(RCC->CRRCR, RCC_CRRCR_HSI48RDY) == 0) {
            return -1;
        }
        SET_BIT(RCC->CRRCR, RCC_CRRCR_HSI48ON);
        break;

    case CLOCK_SOURCE_MSI:
        if (READ_BIT(RCC->CR, RCC_CR_MSIRDY) == 0) {
            return -1;
        }
        SET_BIT(RCC->CR, RCC_CR_MSION);
        break;

    case CLOCK_SOURCE_HSE32:
        if (READ_BIT(RCC->CR, RCC_CR_HSERDY) == 0) {
            return -1;
        }
        SET_BIT(RCC->CR, RCC_CR_HSEON);
        break;

    case CLOCK_SOURCE_PLL:
        break;
    }
    return 0;
}
