#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include "hal/adc.h"

// TYPES

typedef enum {
    GPIO_AF4_FEATURE_NONE,

    GPIO_AF4_FEATURE_I2C1_SMBA,
    GPIO_AF4_FEATURE_I2C1_SCL,
    GPIO_AF4_FEATURE_I2C1_SDA,

    GPIO_AF4_FEATURE_I2C3_SMBA,
    GPIO_AF4_FEATURE_I2C3_SCL,
    GPIO_AF4_FEATURE_I2C3_SDA,
} gpio_af4_feature;

typedef enum {
    GPIO_AF7_FEATURE_NONE,

    GPIO_AF7_FEATURE_USART1_CK,
    GPIO_AF7_FEATURE_USART1_TX,
    GPIO_AF7_FEATURE_USART1_RX,
    GPIO_AF7_FEATURE_USART1_CTS,
    GPIO_AF7_FEATURE_USART1_RTS_DE,
} gpio_af7_feature;

typedef enum {
    GPIO_AF8_FEATURE_NONE,

    GPIO_AF8_FEATURE_IR_OUT,

    GPIO_AF8_FEATURE_LPUART1_TX,
    GPIO_AF8_FEATURE_LPUART1_RX,
    GPIO_AF8_FEATURE_LPUART1_CTS,
    GPIO_AF8_FEATURE_LPUART1_RTS,
    GPIO_AF8_FEATURE_LPUART1_RTS_DE,
} gpio_af8_feature;

typedef GPIO_TypeDef gpio_t;

typedef struct {

    gpio_t *gpio;

    uint8_t num;

    adc_channel adc_chan;

    gpio_af4_feature af4_feature : 7;
    gpio_af7_feature af7_feature : 6;
    gpio_af8_feature af8_feature : 7;

} gpio_pin_t;

#define HIGH true
#define LOW false

typedef enum {
    GPIO_MODE_INPUT = 0b00,
    GPIO_MODE_OUTPUT = 0b01,
    GPIO_MODE_AF = 0b10,
    GPIO_MODE_ANALOG = 0b11,
} gpio_mode;

typedef enum {
    GPIO_AF_MODE_0 = 0b0000,
    GPIO_AF_MODE_1 = 0b0001,
    GPIO_AF_MODE_2 = 0b0010,
    GPIO_AF_MODE_3 = 0b0011,
    GPIO_AF_MODE_4 = 0b0100,
    GPIO_AF_MODE_5 = 0b0101,
    GPIO_AF_MODE_6 = 0b0110,
    GPIO_AF_MODE_7 = 0b0111,
    GPIO_AF_MODE_8 = 0b1000,
    GPIO_AF_MODE_9 = 0b1001,
    GPIO_AF_MODE_10 = 0b1010,
    GPIO_AF_MODE_11 = 0b1011,
    GPIO_AF_MODE_12 = 0b1100,
    GPIO_AF_MODE_13 = 0b1101,
    GPIO_AF_MODE_14 = 0b1110,
    GPIO_AF_MODE_15 = 0b1111,
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
    GPIO_OUTPUT_PUSH_PULL = 0b0,
    GPIO_OUTPUT_OPEN_DRAIN = 0b1,
} gpio_output_type;

typedef enum {
    GPIO_CALIB_INPUT_SINGLE_ENDED,
    GPIO_CALIB_INPUT_DIFFERENTIAL,
} gpio_calib_input_mode;

// PINS

static const gpio_pin_t PA0 = {
    .gpio = GPIOA,             //
    .num = 0,                  //
    .adc_chan = ADC_CHANNEL_5, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};

static const gpio_pin_t PA1 = {
    .gpio = GPIOA,             //
    .num = 1,                  //
    .adc_chan = ADC_CHANNEL_6, //
    .af4_feature = GPIO_AF4_FEATURE_I2C1_SMBA,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PA2 = {
    .gpio = GPIOA,             //
    .num = 2,                  //
    .adc_chan = ADC_CHANNEL_7, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_LPUART1_TX,
};
static const gpio_pin_t PA3 = {
    .gpio = GPIOA,             //
    .num = 3,                  //
    .adc_chan = ADC_CHANNEL_8, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_LPUART1_RX,
};
static const gpio_pin_t PA4 = {
    .gpio = GPIOA,             //
    .num = 4,                  //
    .adc_chan = ADC_CHANNEL_9, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PA5 = {
    .gpio = GPIOA,              //
    .num = 5,                   //
    .adc_chan = ADC_CHANNEL_10, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PA6 = {
    .gpio = GPIOA,              //
    .num = 6,                   //
    .adc_chan = ADC_CHANNEL_11, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_LPUART1_CTS,
};
static const gpio_pin_t PA7 = {
    .gpio = GPIOA,              //
    .num = 7,                   //
    .adc_chan = ADC_CHANNEL_12, //
    .af4_feature = GPIO_AF4_FEATURE_I2C3_SCL,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PA8 = {
    .gpio = GPIOA,              //
    .num = 8,                   //
    .adc_chan = ADC_CHANNEL_15, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_USART1_CK,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PA9 = {
    .gpio = GPIOA,              //
    .num = 9,                   //
    .adc_chan = ADC_CHANNEL_16, //
    .af4_feature = GPIO_AF4_FEATURE_I2C1_SCL,
    .af7_feature = GPIO_AF7_FEATURE_USART1_TX,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};

static const gpio_pin_t PA10 = {
    .gpio = GPIOA,                //
    .num = 10,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_I2C1_SDA,
    .af7_feature = GPIO_AF7_FEATURE_USART1_RX,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PA11 = {
    .gpio = GPIOA,                //
    .num = 11,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_USART1_CTS,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PA12 = {
    .gpio = GPIOA,                //
    .num = 12,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_USART1_RTS_DE,
    .af8_feature = GPIO_AF8_FEATURE_LPUART1_RX,
};
static const gpio_pin_t PA13 = {
    .gpio = GPIOA,                //
    .num = 13,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_IR_OUT,
};
static const gpio_pin_t PA14 = {
    .gpio = GPIOA,                //
    .num = 14,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_I2C1_SMBA,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PA15 = {
    .gpio = GPIOA,                //
    .num = 15,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};

static const gpio_pin_t PB0 = {
    .gpio = GPIOB,                //
    .num = 0,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PB1 = {
    .gpio = GPIOB,                //
    .num = 1,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_LPUART1_RTS_DE,
};
static const gpio_pin_t PB2 = {
    .gpio = GPIOB,                //
    .num = 2,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_I2C3_SMBA,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PB3 = {
    .gpio = GPIOB,                //
    .num = 3,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_USART1_RTS_DE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PB4 = {
    .gpio = GPIOB,                //
    .num = 4,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_I2C3_SDA,
    .af7_feature = GPIO_AF7_FEATURE_USART1_CTS,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PB5 = {
    .gpio = GPIOB,                //
    .num = 5,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_I2C1_SMBA,
    .af7_feature = GPIO_AF7_FEATURE_USART1_CK,
    .af8_feature = GPIO_AF8_FEATURE_LPUART1_TX,
};
static const gpio_pin_t PB6 = {
    .gpio = GPIOB,                //
    .num = 6,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_I2C1_SCL,
    .af7_feature = GPIO_AF7_FEATURE_USART1_TX,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PB7 = {
    .gpio = GPIOB,                //
    .num = 7,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_I2C1_SDA,
    .af7_feature = GPIO_AF7_FEATURE_USART1_RX,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PB8 = {
    .gpio = GPIOB,                //
    .num = 8,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_I2C1_SCL,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PB9 = {
    .gpio = GPIOB,                //
    .num = 9,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_I2C1_SDA,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_IR_OUT,
};

static const gpio_pin_t PB10 = {
    .gpio = GPIOB,                //
    .num = 10,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_I2C3_SCL,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_LPUART1_RX,
};
static const gpio_pin_t PB11 = {
    .gpio = GPIOB,                //
    .num = 11,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_I2C3_SDA,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_LPUART1_TX,
};
static const gpio_pin_t PB12 = {
    .gpio = GPIOB,                //
    .num = 12,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_I2C3_SMBA,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_LPUART1_RTS,
};
static const gpio_pin_t PB13 = {
    .gpio = GPIOB,                //
    .num = 13,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_I2C3_SCL,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_LPUART1_CTS,
};
static const gpio_pin_t PB14 = {
    .gpio = GPIOB,                //
    .num = 14,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_I2C3_SDA,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PB15 = {
    .gpio = GPIOB,                //
    .num = 15,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};

static const gpio_pin_t PC0 = {
    .gpio = GPIOC,             //
    .num = 0,                  //
    .adc_chan = ADC_CHANNEL_1, //
    .af4_feature = GPIO_AF4_FEATURE_I2C3_SCL,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_LPUART1_RX,
};
static const gpio_pin_t PC1 = {
    .gpio = GPIOC,             //
    .num = 1,                  //
    .adc_chan = ADC_CHANNEL_2, //
    .af4_feature = GPIO_AF4_FEATURE_I2C3_SDA,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_LPUART1_TX,
};
static const gpio_pin_t PC2 = {
    .gpio = GPIOC,             //
    .num = 2,                  //
    .adc_chan = ADC_CHANNEL_3, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PC3 = {
    .gpio = GPIOC,             //
    .num = 3,                  //
    .adc_chan = ADC_CHANNEL_4, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PC4 = {
    .gpio = GPIOC,                //
    .num = 4,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PC5 = {
    .gpio = GPIOC,                //
    .num = 5,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PC6 = {
    .gpio = GPIOC,                //
    .num = 6,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};

static const gpio_pin_t PC10 = {
    .gpio = GPIOC,                //
    .num = 10,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PC11 = {
    .gpio = GPIOC,                //
    .num = 11,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PC12 = {
    .gpio = GPIOC,                //
    .num = 12,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PC13 = {
    .gpio = GPIOC,                //
    .num = 13,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PC14 = {
    .gpio = GPIOC,                //
    .num = 14,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};
static const gpio_pin_t PC15 = {
    .gpio = GPIOC,                //
    .num = 15,                    //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};

static const gpio_pin_t PD0 = {
    .gpio = GPIOD,                //
    .num = 0,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};

static const gpio_pin_t PE4 = {
    .gpio = GPIOE,                //
    .num = 4,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};

static const gpio_pin_t PH3 = {
    .gpio = GPIOH,                //
    .num = 3,                     //
    .adc_chan = ADC_CHANNEL_NONE, //
    .af4_feature = GPIO_AF4_FEATURE_NONE,
    .af7_feature = GPIO_AF7_FEATURE_NONE,
    .af8_feature = GPIO_AF8_FEATURE_NONE,
};

// FUNCTIONS

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
