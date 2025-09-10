#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include "hal_adc.h"

#include <stdbool.h>

// ===== Core types =====

typedef GPIO_TypeDef gpio_t;

typedef enum {
    GPIO_MODE_INPUT = 0b00,
    GPIO_MODE_OUTPUT = 0b01,
    GPIO_MODE_AF = 0b10,
    GPIO_MODE_ANALOG = 0b11,
} gpio_mode;

typedef enum {
    GPIO_AF_MODE_0 = 0,
    GPIO_AF_MODE_1 = 1,
    GPIO_AF_MODE_2 = 2,
    GPIO_AF_MODE_3 = 3,
    GPIO_AF_MODE_4 = 4,
    GPIO_AF_MODE_5 = 5,
    GPIO_AF_MODE_6 = 6,
    GPIO_AF_MODE_7 = 7,
    GPIO_AF_MODE_8 = 8,
    GPIO_AF_MODE_9 = 9,
    GPIO_AF_MODE_10 = 10,
    GPIO_AF_MODE_11 = 11,
    GPIO_AF_MODE_12 = 12,
    GPIO_AF_MODE_13 = 13,
    GPIO_AF_MODE_14 = 14,
    GPIO_AF_MODE_15 = 15,
} gpio_af_mode;

typedef enum {
    GPIO_PUPD_NONE = 0b00,
    GPIO_PULLUP = 0b01,
    GPIO_PULLDOWN = 0b10,
} gpio_pupd;

typedef enum {
    GPIO_SPEED_LOW = 0b00,
    GPIO_SPEED_MED = 0b01,
    GPIO_SPEED_FAST = 0b10,
    GPIO_SPEED_HIGH = 0b11,
} gpio_speed;

typedef enum {
    GPIO_OUTPUT_PUSH_PULL = 0,
    GPIO_OUTPUT_OPEN_DRAIN = 1,
} gpio_output_type;

typedef enum {
    GPIO_CALIB_INPUT_SINGLE_ENDED,
    GPIO_CALIB_INPUT_DIFFERENTIAL,
} gpio_calib_input_mode;

#define HIGH true
#define LOW false

// ===== AF feature enums =====

// AF0: SYS_AF
typedef enum {
    GPIO_AF0_FEATURE_NONE = 0,

    GPIO_AF0_SYS_JTMS_SWDIO,
    GPIO_AF0_SYS_JTCK_SWCLK,
    GPIO_AF0_SYS_JTDI,
    GPIO_AF0_SYS_JTDO_TRACE_SWO,
    GPIO_AF0_SYS_NJTRST,
    GPIO_AF0_SYS_TRACECK,
    GPIO_AF0_SYS_TRACED0,
    GPIO_AF0_SYS_TRACED1,
    GPIO_AF0_SYS_TRACED2,
    GPIO_AF0_SYS_TRACED3,
    GPIO_AF0_SYS_MCO,
    GPIO_AF0_SYS_TRIG_INOUT,
    GPIO_AF0_SYS_LSCO,

    GPIO_AF0_SYS_RTC_REFIN,
    GPIO_AF0_SYS_RTC_OUT,
} gpio_af0_feature;

// AF1: TIM1 / TIM2 / LPTIM1
typedef enum {
    GPIO_AF1_FEATURE_NONE = 0,

    GPIO_AF1_TIM1_ETR,
    GPIO_AF1_TIM1_BKIN,
    GPIO_AF1_TIM1_CH1,
    GPIO_AF1_TIM1_CH1N,
    GPIO_AF1_TIM1_CH2,
    GPIO_AF1_TIM1_CH2N,
    GPIO_AF1_TIM1_CH3,
    GPIO_AF1_TIM1_CH3N,
    GPIO_AF1_TIM1_CH4,

    GPIO_AF1_TIM2_CH1,
    GPIO_AF1_TIM2_CH2,
    GPIO_AF1_TIM2_CH3,
    GPIO_AF1_TIM2_CH4,

    GPIO_AF1_LPTIM1_IN1,
    GPIO_AF1_LPTIM1_IN2,
    GPIO_AF1_LPTIM1_OUT,
    GPIO_AF1_LPTIM1_ETR,
} gpio_af1_feature;

// AF2: TIM1 / TIM2
typedef enum {
    GPIO_AF2_FEATURE_NONE = 0,

    GPIO_AF2_TIM1_BKIN2,
    GPIO_AF2_TIM2_ETR,
} gpio_af2_feature;

// AF3: SPI2 / SAI1 / TIM1
typedef enum {
    GPIO_AF3_FEATURE_NONE = 0,

    GPIO_AF3_SPI2_SCK,
    GPIO_AF3_SPI2_MOSI,

    GPIO_AF3_SAI1_PDM_CK1,
    GPIO_AF3_SAI1_PDM_CK2,
    GPIO_AF3_SAI1_PDM_DI1,
    GPIO_AF3_SAI1_PDM_DI2,
    GPIO_AF3_SAI1_PDM_DI3,

    GPIO_AF3_TIM1_BKIN,
} gpio_af3_feature;

// AF4: I2C1 / I2C3
typedef enum {
    GPIO_AF4_FEATURE_NONE = 0,

    GPIO_AF4_I2C1_SMBA,
    GPIO_AF4_I2C1_SCL,
    GPIO_AF4_I2C1_SDA,

    GPIO_AF4_I2C3_SMBA,
    GPIO_AF4_I2C3_SCL,
    GPIO_AF4_I2C3_SDA,
} gpio_af4_feature;

// AF5: SPI1 / SPI2
typedef enum {
    GPIO_AF5_FEATURE_NONE = 0,

    GPIO_AF5_SPI1_NSS,
    GPIO_AF5_SPI1_SCK,
    GPIO_AF5_SPI1_MISO,
    GPIO_AF5_SPI1_MOSI,

    GPIO_AF5_SPI2_NSS,
    GPIO_AF5_SPI2_SCK,
    GPIO_AF5_SPI2_MISO,
    GPIO_AF5_SPI2_MOSI,
} gpio_af5_feature;

// AF6: RF
typedef enum {
    GPIO_AF6_FEATURE_NONE = 0,

    GPIO_AF6_RF_TX_MOD_EXT_PA,

    GPIO_AF6_LSCO,
    GPIO_AF6_MCO,
} gpio_af6_feature;

// AF7: USART1
typedef enum {
    GPIO_AF7_FEATURE_NONE = 0,

    GPIO_AF7_USART1_CK,
    GPIO_AF7_USART1_TX,
    GPIO_AF7_USART1_RX,
    GPIO_AF7_USART1_CTS,
    GPIO_AF7_USART1_RTS_DE,
} gpio_af7_feature;

// AF8: LPUART1 / IR
typedef enum {
    GPIO_AF8_FEATURE_NONE = 0,

    GPIO_AF8_LPUART1_TX,
    GPIO_AF8_LPUART1_RX,
    GPIO_AF8_LPUART1_CTS,
    GPIO_AF8_LPUART1_RTS,
    GPIO_AF8_LPUART1_RTS_DE,

    GPIO_AF8_IR_OUT,
} gpio_af8_feature;

// AF9: TSC
typedef enum {
    GPIO_AF9_FEATURE_NONE = 0,

    GPIO_AF9_TSC_G1_IO1,
    GPIO_AF9_TSC_G1_IO2,
    GPIO_AF9_TSC_G1_IO3,
    GPIO_AF9_TSC_G1_IO4,

    GPIO_AF9_TSC_G2_IO1,
    GPIO_AF9_TSC_G2_IO2,
    GPIO_AF9_TSC_G2_IO3,
    GPIO_AF9_TSC_G2_IO4,

    GPIO_AF9_TSC_G3_IO1,
    GPIO_AF9_TSC_G3_IO2,
    GPIO_AF9_TSC_G3_IO3,
    GPIO_AF9_TSC_G3_IO4,

    GPIO_AF9_TSC_G4_IO1,
    GPIO_AF9_TSC_G4_IO2,
    GPIO_AF9_TSC_G4_IO3,
    GPIO_AF9_TSC_G4_IO4,

    GPIO_AF9_TSC_G5_IO1,
    GPIO_AF9_TSC_G5_IO2,
    GPIO_AF9_TSC_G5_IO3,
    GPIO_AF9_TSC_G5_IO4,

    GPIO_AF9_TSC_G6_IO1,
    GPIO_AF9_TSC_G6_IO2,
    GPIO_AF9_TSC_G6_IO3,
    GPIO_AF9_TSC_G6_IO4,

    GPIO_AF9_TSC_G7_IO1,
    GPIO_AF9_TSC_G7_IO2,
    GPIO_AF9_TSC_G7_IO3,
    GPIO_AF9_TSC_G7_IO4,

    GPIO_AF9_TSC_SYNC,
} gpio_af9_feature;

// AF10: USB / QUADSPI
typedef enum {
    GPIO_AF10_FEATURE_NONE = 0,

    GPIO_AF10_USB_DM,
    GPIO_AF10_USB_DP,
    GPIO_AF10_USB_NOE,
    GPIO_AF10_USB_CRS_SYNC,

    GPIO_AF10_QUADSPI_CLK,
    GPIO_AF10_QUADSPI_BK1_NCS,
    GPIO_AF10_QUADSPI_BK1_IO0,
    GPIO_AF10_QUADSPI_BK1_IO1,
    GPIO_AF10_QUADSPI_BK1_IO2,
    GPIO_AF10_QUADSPI_BK1_IO3,
} gpio_af10_feature;

// AF11: LCD
typedef enum {
    GPIO_AF11_FEATURE_NONE = 0,

    GPIO_AF11_LCD_COM0,
    GPIO_AF11_LCD_COM1,
    GPIO_AF11_LCD_COM2,
    GPIO_AF11_LCD_COM3,
    GPIO_AF11_LCD_COM4,
    GPIO_AF11_LCD_COM5,
    GPIO_AF11_LCD_COM6,
    GPIO_AF11_LCD_COM7,
    GPIO_AF11_LCD_SEG0,
    GPIO_AF11_LCD_SEG1,
    GPIO_AF11_LCD_SEG2,
    GPIO_AF11_LCD_SEG3,
    GPIO_AF11_LCD_SEG4,
    GPIO_AF11_LCD_SEG5,
    GPIO_AF11_LCD_SEG6,
    GPIO_AF11_LCD_SEG7,
    GPIO_AF11_LCD_SEG8,
    GPIO_AF11_LCD_SEG9,
    GPIO_AF11_LCD_SEG10,
    GPIO_AF11_LCD_SEG11,
    GPIO_AF11_LCD_SEG12,
    GPIO_AF11_LCD_SEG13,
    GPIO_AF11_LCD_SEG14,
    GPIO_AF11_LCD_SEG15,
    GPIO_AF11_LCD_SEG16,
    GPIO_AF11_LCD_SEG17,
    GPIO_AF11_LCD_SEG18,
    GPIO_AF11_LCD_SEG19,
    GPIO_AF11_LCD_SEG20,
    GPIO_AF11_LCD_SEG21,
    GPIO_AF11_LCD_SEG22,
    GPIO_AF11_LCD_SEG23,
    GPIO_AF11_LCD_SEG24,
    GPIO_AF11_LCD_SEG25,
    GPIO_AF11_LCD_SEG26,
    GPIO_AF11_LCD_SEG27,
    GPIO_AF11_LCD_SEG28,
    GPIO_AF11_LCD_SEG29,
    GPIO_AF11_LCD_SEG30,
    GPIO_AF11_LCD_SEG31,
    GPIO_AF11_LCD_SEG32,
    GPIO_AF11_LCD_SEG33,
    GPIO_AF11_LCD_SEG34,
    GPIO_AF11_LCD_SEG35,
    GPIO_AF11_LCD_SEG36,
    GPIO_AF11_LCD_SEG37,
    GPIO_AF11_LCD_SEG38,
    GPIO_AF11_LCD_SEG39,
    GPIO_AF11_LCD_SEG40,
    GPIO_AF11_LCD_SEG41,
    GPIO_AF11_LCD_SEG42,
    GPIO_AF11_LCD_SEG43,
    GPIO_AF11_LCD_VLCD,
} gpio_af11_feature;

// AF12: COMP1/COMP2/TIM1
typedef enum {
    GPIO_AF12_FEATURE_NONE = 0,

    GPIO_AF12_COMP1_OUT,
    GPIO_AF12_COMP2_OUT,

    GPIO_AF12_TIM1_BKIN,
    GPIO_AF12_TIM1_BKIN2,
} gpio_af12_feature;

// AF13: SAI1
typedef enum {
    GPIO_AF13_FEATURE_NONE = 0,

    GPIO_AF13_SAI1_EXTCLK,
    GPIO_AF13_SAI1_MCLK_A,
    GPIO_AF13_SAI1_MCLK_B,
    GPIO_AF13_SAI1_SCK_A,
    GPIO_AF13_SAI1_SCK_B,
    GPIO_AF13_SAI1_FS_A,
    GPIO_AF13_SAI1_FS_B,
    GPIO_AF13_SAI1_SD_A,
    GPIO_AF13_SAI1_SD_B,
} gpio_af13_feature;

// AF14: TIM2 / TIM16 / TIM17 / LPTIM2
typedef enum {
    GPIO_AF14_FEATURE_NONE = 0,

    GPIO_AF14_TIM2_ETR,
    GPIO_AF14_TIM2_CH1,

    GPIO_AF14_TIM16_CH1,
    GPIO_AF14_TIM16_CH1N,
    GPIO_AF14_TIM16_BKIN,

    GPIO_AF14_TIM17_CH1,
    GPIO_AF14_TIM17_CH1N,
    GPIO_AF14_TIM17_BKIN,

    GPIO_AF14_LPTIM2_IN1,
    GPIO_AF14_LPTIM2_IN2,
    GPIO_AF14_LPTIM2_ETR,
    GPIO_AF14_LPTIM2_OUT,
} gpio_af14_feature;

// AF15: EVENTOUT
typedef enum {
    GPIO_AF15_FEATURE_NONE = 0,

    GPIO_AF15_EVENTOUT,
} gpio_af15_feature;

// ===== Per-pin structure =====

typedef struct {
    gpio_t *gpio;
    uint8_t num;
    adc_channel adc_chan : 5;

    gpio_af0_feature af0_feature : 4;
    gpio_af1_feature af1_feature : 5;
    gpio_af2_feature af2_feature : 2;
    gpio_af3_feature af3_feature : 4;
    gpio_af4_feature af4_feature : 3;
    gpio_af5_feature af5_feature : 4;
    gpio_af6_feature af6_feature : 2;
    gpio_af7_feature af7_feature : 3;
    gpio_af8_feature af8_feature : 3;
    gpio_af9_feature af9_feature : 5;
    gpio_af10_feature af10_feature : 4;
    gpio_af11_feature af11_feature : 6;
    gpio_af12_feature af12_feature : 3;
    gpio_af13_feature af13_feature : 4;
    gpio_af14_feature af14_feature : 4;
    gpio_af15_feature af15_feature : 1;
} gpio_pin_t;

// ===== Pin declarations =====

// --- PORT A ---
static const gpio_pin_t PA0 = {
    .gpio = GPIOA,
    .num = 0,
    .adc_chan = ADC_CHANNEL_5,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM2_CH1,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_COMP1_OUT,
    .af13_feature = GPIO_AF13_SAI1_EXTCLK,
    .af14_feature = GPIO_AF14_TIM2_ETR,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA1 = {
    .gpio = GPIOA,
    .num = 1,
    .adc_chan = ADC_CHANNEL_6,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM2_CH2,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_I2C1_SMBA,
    .af5_feature = GPIO_AF5_SPI1_SCK,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG0,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA2 = {
    .gpio = GPIOA,
    .num = 2,
    .adc_chan = ADC_CHANNEL_7,
    .af0_feature = GPIO_AF0_SYS_LSCO,
    .af1_feature = GPIO_AF1_TIM2_CH3,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_LPUART1_TX,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_QUADSPI_BK1_NCS,
    .af11_feature = GPIO_AF11_LCD_SEG1,
    .af12_feature = GPIO_AF12_COMP2_OUT,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA3 = {
    .gpio = GPIOA,
    .num = 3,
    .adc_chan = ADC_CHANNEL_8,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM2_CH4,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_SAI1_PDM_CK1,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_LPUART1_RX,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_QUADSPI_CLK,
    .af11_feature = GPIO_AF11_LCD_SEG2,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_MCLK_A,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA4 = {
    .gpio = GPIOA,
    .num = 4,
    .adc_chan = ADC_CHANNEL_9,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_SPI1_NSS,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG5,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_FS_B,
    .af14_feature = GPIO_AF14_LPTIM2_OUT,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA5 = {
    .gpio = GPIOA,
    .num = 5,
    .adc_chan = ADC_CHANNEL_10,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM2_CH1,
    .af2_feature = GPIO_AF2_TIM2_ETR,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_SPI1_SCK,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_SD_B,
    .af14_feature = GPIO_AF14_LPTIM2_ETR,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA6 = {
    .gpio = GPIOA,
    .num = 6,
    .adc_chan = ADC_CHANNEL_11,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM1_BKIN,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_SPI1_MISO,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_LPUART1_CTS,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_QUADSPI_BK1_IO3,
    .af11_feature = GPIO_AF11_LCD_SEG3,
    .af12_feature = GPIO_AF12_TIM1_BKIN,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_TIM16_CH1,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA7 = {
    .gpio = GPIOA,
    .num = 7,
    .adc_chan = ADC_CHANNEL_12,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM1_CH1N,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_I2C3_SCL,
    .af5_feature = GPIO_AF5_SPI1_MOSI,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_QUADSPI_BK1_IO2,
    .af11_feature = GPIO_AF11_LCD_SEG4,
    .af12_feature = GPIO_AF12_COMP2_OUT,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_TIM17_CH1,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA8 = {
    .gpio = GPIOA,
    .num = 8,
    .adc_chan = ADC_CHANNEL_15,
    .af0_feature = GPIO_AF0_SYS_MCO,
    .af1_feature = GPIO_AF1_TIM1_CH1,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_SAI1_PDM_CK2,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_USART1_CK,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_COM0,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_SCK_A,
    .af14_feature = GPIO_AF14_LPTIM2_OUT,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA9 = {
    .gpio = GPIOA,
    .num = 9,
    .adc_chan = ADC_CHANNEL_16,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM1_CH2,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_SAI1_PDM_DI2,
    .af4_feature = GPIO_AF4_I2C1_SCL,
    .af5_feature = GPIO_AF5_SPI2_SCK,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_USART1_TX,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_COM1,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_FS_A,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA10 = {
    .gpio = GPIOA,
    .num = 10,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM1_CH3,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_SAI1_PDM_DI1,
    .af4_feature = GPIO_AF4_I2C1_SDA,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_USART1_RX,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_USB_CRS_SYNC,
    .af11_feature = GPIO_AF11_LCD_COM2,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_SD_A,
    .af14_feature = GPIO_AF14_TIM17_BKIN,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA11 = {
    .gpio = GPIOA,
    .num = 11,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM1_CH4,
    .af2_feature = GPIO_AF2_TIM1_BKIN2,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_SPI1_MISO,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_USART1_CTS,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_USB_DM,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_TIM1_BKIN2,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA12 = {
    .gpio = GPIOA,
    .num = 12,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM1_ETR,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_SPI1_MOSI,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_USART1_RTS_DE,
    .af8_feature = GPIO_AF8_LPUART1_RX,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_USB_DP,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA13 = {
    .gpio = GPIOA,
    .num = 13,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_JTMS_SWDIO,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_IR_OUT,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_USB_NOE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_SD_B,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA14 = {
    .gpio = GPIOA,
    .num = 14,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_JTCK_SWCLK,
    .af1_feature = GPIO_AF1_LPTIM1_OUT,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_I2C1_SMBA,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG5,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_FS_B,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PA15 = {
    .gpio = GPIOA,
    .num = 15,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_JTDI,
    .af1_feature = GPIO_AF1_TIM2_CH1,
    .af2_feature = GPIO_AF2_TIM2_ETR,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_SPI1_NSS,
    .af6_feature = GPIO_AF6_MCO,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG17,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

// --- PORT B ---
static const gpio_pin_t PB0 = {
    .gpio = GPIOB,
    .num = 0,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_RF_TX_MOD_EXT_PA,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_COMP1_OUT,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB1 = {
    .gpio = GPIOB,
    .num = 1,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_LPUART1_RTS_DE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_LPTIM2_IN1,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB2 = {
    .gpio = GPIOB,
    .num = 2,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_RTC_OUT,
    .af1_feature = GPIO_AF1_LPTIM1_OUT,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_I2C3_SMBA,
    .af5_feature = GPIO_AF5_SPI1_NSS,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_VLCD,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_EXTCLK,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB3 = {
    .gpio = GPIOB,
    .num = 3,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_JTDO_TRACE_SWO,
    .af1_feature = GPIO_AF1_TIM2_CH2,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_SPI1_SCK,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_USART1_RTS_DE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG7,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_SCK_B,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB4 = {
    .gpio = GPIOB,
    .num = 4,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_NJTRST,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_I2C3_SDA,
    .af5_feature = GPIO_AF5_SPI1_MISO,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_USART1_CTS,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G2_IO1,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG8,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_MCLK_B,
    .af14_feature = GPIO_AF14_TIM17_BKIN,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB5 = {
    .gpio = GPIOB,
    .num = 5,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_LPTIM1_IN1,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_I2C1_SMBA,
    .af5_feature = GPIO_AF5_SPI1_MOSI,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_USART1_CK,
    .af8_feature = GPIO_AF8_LPUART1_TX,
    .af9_feature = GPIO_AF9_TSC_G2_IO2,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG9,
    .af12_feature = GPIO_AF12_COMP2_OUT,
    .af13_feature = GPIO_AF13_SAI1_SD_B,
    .af14_feature = GPIO_AF14_TIM16_BKIN,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB6 = {
    .gpio = GPIOB,
    .num = 6,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_MCO,
    .af1_feature = GPIO_AF1_LPTIM1_ETR,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_I2C1_SCL,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_USART1_TX,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G2_IO3,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG6,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_FS_B,
    .af14_feature = GPIO_AF14_TIM16_CH1N,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB7 = {
    .gpio = GPIOB,
    .num = 7,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_LPTIM1_IN2,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_TIM1_BKIN,
    .af4_feature = GPIO_AF4_I2C1_SDA,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_USART1_RX,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G2_IO4,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG21,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_TIM17_CH1N,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB8 = {
    .gpio = GPIOB,
    .num = 8,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM1_CH2N,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_SAI1_PDM_CK1,
    .af4_feature = GPIO_AF4_I2C1_SCL,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_QUADSPI_BK1_IO1,
    .af11_feature = GPIO_AF11_LCD_SEG16,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_MCLK_A,
    .af14_feature = GPIO_AF14_TIM16_CH1N,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB9 = {
    .gpio = GPIOB,
    .num = 9,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM1_CH3N,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_SAI1_PDM_DI2,
    .af4_feature = GPIO_AF4_I2C1_SDA,
    .af5_feature = GPIO_AF5_SPI2_NSS,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_IR_OUT,
    .af9_feature = GPIO_AF9_TSC_G7_IO4,
    .af10_feature = GPIO_AF10_QUADSPI_BK1_IO0,
    .af11_feature = GPIO_AF11_LCD_COM3,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_FS_A,
    .af14_feature = GPIO_AF14_TIM17_CH1,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB10 = {
    .gpio = GPIOB,
    .num = 10,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM2_CH3,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_I2C3_SCL,
    .af5_feature = GPIO_AF5_SPI2_SCK,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_LPUART1_RX,
    .af9_feature = GPIO_AF9_TSC_SYNC,
    .af10_feature = GPIO_AF10_QUADSPI_CLK,
    .af11_feature = GPIO_AF11_LCD_SEG10,
    .af12_feature = GPIO_AF12_COMP1_OUT,
    .af13_feature = GPIO_AF13_SAI1_SCK_A,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB11 = {
    .gpio = GPIOB,
    .num = 11,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM2_CH4,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_I2C3_SDA,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_LPUART1_TX,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_QUADSPI_BK1_NCS,
    .af11_feature = GPIO_AF11_LCD_SEG11,
    .af12_feature = GPIO_AF12_COMP2_OUT,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB12 = {
    .gpio = GPIOB,
    .num = 12,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM1_BKIN,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_TIM1_BKIN,
    .af4_feature = GPIO_AF4_I2C3_SMBA,
    .af5_feature = GPIO_AF5_SPI2_NSS,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_LPUART1_RTS,
    .af9_feature = GPIO_AF9_TSC_G1_IO1,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG12,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_FS_A,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB13 = {
    .gpio = GPIOB,
    .num = 13,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM1_CH1N,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_I2C3_SCL,
    .af5_feature = GPIO_AF5_SPI2_SCK,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_LPUART1_CTS,
    .af9_feature = GPIO_AF9_TSC_G1_IO2,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG13,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_SCK_A,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB14 = {
    .gpio = GPIOB,
    .num = 14,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM1_CH2N,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_I2C3_SDA,
    .af5_feature = GPIO_AF5_SPI2_MISO,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G1_IO3,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG14,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_MCLK_A,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PB15 = {
    .gpio = GPIOB,
    .num = 15,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_RTC_REFIN,
    .af1_feature = GPIO_AF1_TIM1_CH3N,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_SPI2_MOSI,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G1_IO4,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG15,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_SD_A,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

// --- PORT C ---
static const gpio_pin_t PC0 = {
    .gpio = GPIOC,
    .num = 0,
    .adc_chan = ADC_CHANNEL_1,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_LPTIM1_IN1,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_I2C3_SCL,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_LPUART1_RX,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG18,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_LPTIM2_IN1,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PC1 = {
    .gpio = GPIOC,
    .num = 1,
    .adc_chan = ADC_CHANNEL_2,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_LPTIM1_OUT,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_SPI2_MOSI,
    .af4_feature = GPIO_AF4_I2C3_SDA,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_LPUART1_TX,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG19,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PC2 = {
    .gpio = GPIOC,
    .num = 2,
    .adc_chan = ADC_CHANNEL_3,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_LPTIM1_IN2,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_SPI2_MISO,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG20,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PC3 = {
    .gpio = GPIOC,
    .num = 3,
    .adc_chan = ADC_CHANNEL_4,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_LPTIM1_ETR,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_SAI1_PDM_DI1,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_SPI2_MOSI,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_VLCD,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_SD_A,
    .af14_feature = GPIO_AF14_LPTIM2_ETR,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PC4 = {
    .gpio = GPIOC,
    .num = 4,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG22,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PC5 = {
    .gpio = GPIOC,
    .num = 5,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_SAI1_PDM_DI3,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG23,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PC6 = {
    .gpio = GPIOC,
    .num = 6,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G4_IO1,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG24,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PC10 = {
    .gpio = GPIOC,
    .num = 10,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_TRACED1,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G3_IO2,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_COM4,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PC11 = {
    .gpio = GPIOC,
    .num = 11,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G3_IO3,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_COM5,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PC12 = {
    .gpio = GPIOC,
    .num = 12,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_TRACED3,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_LSCO,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G3_IO4,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_COM6,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PC13 = {
    .gpio = GPIOC,
    .num = 13,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PC14 = {
    .gpio = GPIOC,
    .num = 14,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PC15 = {
    .gpio = GPIOC,
    .num = 15,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

// --- PORT D ---
static const gpio_pin_t PD0 = {
    .gpio = GPIOD,
    .num = 0,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_SPI2_NSS,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD1 = {
    .gpio = GPIOD,
    .num = 1,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_SPI2_SCK,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD2 = {
    .gpio = GPIOD,
    .num = 2,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_TRACED2,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_SYNC,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_COM7,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD3 = {
    .gpio = GPIOD,
    .num = 3,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_SPI2_SCK,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_SPI2_MISO,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_QUADSPI_BK1_NCS,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD4 = {
    .gpio = GPIOD,
    .num = 4,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_SPI2_MOSI,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G5_IO1,
    .af10_feature = GPIO_AF10_QUADSPI_BK1_IO0,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD5 = {
    .gpio = GPIOD,
    .num = 5,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G5_IO2,
    .af10_feature = GPIO_AF10_QUADSPI_BK1_IO1,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_MCLK_B,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD6 = {
    .gpio = GPIOD,
    .num = 6,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_SAI1_PDM_DI1,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G5_IO3,
    .af10_feature = GPIO_AF10_QUADSPI_BK1_IO2,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_SD_A,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD7 = {
    .gpio = GPIOD,
    .num = 7,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G5_IO4,
    .af10_feature = GPIO_AF10_QUADSPI_BK1_IO3,
    .af11_feature = GPIO_AF11_LCD_SEG39,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD8 = {
    .gpio = GPIOD,
    .num = 8,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_TIM1_BKIN2,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG28,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD9 = {
    .gpio = GPIOD,
    .num = 9,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_TRACED0,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG29,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD10 = {
    .gpio = GPIOD,
    .num = 10,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_TRIG_INOUT,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G6_IO1,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG30,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD11 = {
    .gpio = GPIOD,
    .num = 11,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G6_IO2,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG31,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_LPTIM2_ETR,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD12 = {
    .gpio = GPIOD,
    .num = 12,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G6_IO3,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG32,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_LPTIM2_IN1,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD13 = {
    .gpio = GPIOD,
    .num = 13,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G6_IO4,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG33,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_LPTIM2_OUT,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD14 = {
    .gpio = GPIOD,
    .num = 14,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM1_CH1,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG34,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PD15 = {
    .gpio = GPIOD,
    .num = 15,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM1_CH2,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG35,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

// --- PORT E / H ---
static const gpio_pin_t PE0 = {
    .gpio = GPIOE,
    .num = 0,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_TIM1_ETR,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G7_IO3,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG36,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_TIM16_CH1,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PE1 = {
    .gpio = GPIOE,
    .num = 1,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G7_IO2,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG37,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_TIM17_CH1,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PE2 = {
    .gpio = GPIOE,
    .num = 2,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_TRACECK,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_SAI1_PDM_CK1,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_TSC_G7_IO1,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_LCD_SEG38,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_SAI1_MCLK_A,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PE3 = {
    .gpio = GPIOE,
    .num = 3,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PE4 = {
    .gpio = GPIOE,
    .num = 4,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PH0 = {
    .gpio = GPIOH,
    .num = 0,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PH1 = {
    .gpio = GPIOH,
    .num = 1,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_FEATURE_NONE,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

static const gpio_pin_t PH3 = {
    .gpio = GPIOH,
    .num = 3,
    .adc_chan = ADC_CHANNEL_NONE,
    .af0_feature = GPIO_AF0_SYS_LSCO,
    .af1_feature = GPIO_AF1_FEATURE_NONE,
    .af2_feature = GPIO_AF2_FEATURE_NONE,
    .af3_feature = GPIO_AF3_FEATURE_NONE,
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af5_feature = GPIO_AF5_FEATURE_NONE,
    .af6_feature = GPIO_AF6_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
    .af9_feature = GPIO_AF9_FEATURE_NONE,
    .af10_feature = GPIO_AF10_FEATURE_NONE,
    .af11_feature = GPIO_AF11_FEATURE_NONE,
    .af12_feature = GPIO_AF12_FEATURE_NONE,
    .af13_feature = GPIO_AF13_FEATURE_NONE,
    .af14_feature = GPIO_AF14_FEATURE_NONE,
    .af15_feature = GPIO_AF15_EVENTOUT,
};

// ===== API =====

void gpio_turn_on_port(gpio_t *port);
void gpio_turn_off_port(gpio_t *port);
void gpio_set_mode(gpio_pin_t pin, gpio_mode mode);
void gpio_set_af_mode(gpio_pin_t pin, gpio_af_mode mode);
void gpio_set_pupd(gpio_pin_t pin, gpio_pupd pupd);
void gpio_set_speed(gpio_pin_t pin, gpio_speed speed);
void gpio_set_output_type(gpio_pin_t pin, gpio_output_type type);
void gpio_digital_write(gpio_pin_t pin, bool val);
bool gpio_digital_read(gpio_pin_t pin);
hal_err gpio_enable_interrupt(gpio_pin_t pin);
hal_err gpio_set_interrupt_falling(gpio_pin_t pin);
hal_err gpio_set_interrupt_rising(gpio_pin_t pin);

#endif // HAL_GPIO_H
