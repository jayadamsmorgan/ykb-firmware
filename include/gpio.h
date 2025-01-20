#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>
#include <stdint.h>

#define GPIO_REG_ADDR 0x48000000

struct gpio {
    volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFRL,
        AFRH, BRR;
};

#define GPIO_A_REG_OFFSET 0x0000
#define GPIO_A_REG ((struct gpio *)(GPIO_REG_ADDR + GPIO_A_REG_OFFSET))
#define GPIO_B_REG_OFFSET 0x0400
#define GPIO_B_REG ((struct gpio *)(GPIO_REG_ADDR + GPIO_B_REG_OFFSET))
#define GPIO_C_REG_OFFSET 0x0800
#define GPIO_C_REG ((struct gpio *)(GPIO_REG_ADDR + GPIO_C_REG_OFFSET))
#define GPIO_D_REG_OFFSET 0x0C00
#define GPIO_D_REG ((struct gpio *)(GPIO_REG_ADDR + GPIO_D_REG_OFFSET))
#define GPIO_E_REG_OFFSET 0x1000
#define GPIO_E_REG ((struct gpio *)(GPIO_REG_ADDR + GPIO_E_REG_OFFSET))
#define GPIO_H_REG_OFFSET 0x1C00
#define GPIO_H_REG ((struct gpio *)(GPIO_REG_ADDR + GPIO_H_REG_OFFSET))

// GPIO NUM START
// ************

enum gpio_port {
    GPIO_PORT_A,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_H
};

typedef struct {
    enum gpio_port port : 3;
    uint8_t num : 4;
} gpio_pin;

#define PA0                                                                    \
    (gpio_pin) { .num = 0, .port = GPIO_PORT_A }
#define PA1                                                                    \
    (gpio_pin) { .num = 1, .port = GPIO_PORT_A }
#define PA2                                                                    \
    (gpio_pin) { .num = 2, .port = GPIO_PORT_A }
#define PA3                                                                    \
    (gpio_pin) { .num = 3, .port = GPIO_PORT_A }
#define PA4                                                                    \
    (gpio_pin) { .num = 4, .port = GPIO_PORT_A }
#define PA5                                                                    \
    (gpio_pin) { .num = 5, .port = GPIO_PORT_A }
#define PA6                                                                    \
    (gpio_pin) { .num = 6, .port = GPIO_PORT_A }
#define PA7                                                                    \
    (gpio_pin) { .num = 7, .port = GPIO_PORT_A }
#define PA8                                                                    \
    (gpio_pin) { .num = 8, .port = GPIO_PORT_A }
#define PA9                                                                    \
    (gpio_pin) { .num = 9, .port = GPIO_PORT_A }
#define PA10                                                                   \
    (gpio_pin) { .num = 10, .port = GPIO_PORT_A }
#define PA11                                                                   \
    (gpio_pin) { .num = 11, .port = GPIO_PORT_A }
#define PA12                                                                   \
    (gpio_pin) { .num = 12, .port = GPIO_PORT_A }
#define PA13                                                                   \
    (gpio_pin) { .num = 13, .port = GPIO_PORT_A }
#define PA14                                                                   \
    (gpio_pin) { .num = 14, .port = GPIO_PORT_A }
#define PA15                                                                   \
    (gpio_pin) { .num = 15, .port = GPIO_PORT_A }

#define PB0                                                                    \
    (gpio_pin) { .num = 0, .port = GPIO_PORT_B }
#define PB1                                                                    \
    (gpio_pin) { .num = 1, .port = GPIO_PORT_B }
#define PB2                                                                    \
    (gpio_pin) { .num = 2, .port = GPIO_PORT_B }
#define PB3                                                                    \
    (gpio_pin) { .num = 3, .port = GPIO_PORT_B }
#define PB4                                                                    \
    (gpio_pin) { .num = 4, .port = GPIO_PORT_B }
#define PB5                                                                    \
    (gpio_pin) { .num = 5, .port = GPIO_PORT_B }
#define PB6                                                                    \
    (gpio_pin) { .num = 6, .port = GPIO_PORT_B }
#define PB7                                                                    \
    (gpio_pin) { .num = 7, .port = GPIO_PORT_B }
#define PB8                                                                    \
    (gpio_pin) { .num = 8, .port = GPIO_PORT_B }
#define PB9                                                                    \
    (gpio_pin) { .num = 9, .port = GPIO_PORT_B }
#define PB10                                                                   \
    (gpio_pin) { .num = 10, .port = GPIO_PORT_B }
#define PB11                                                                   \
    (gpio_pin) { .num = 11, .port = GPIO_PORT_B }
#define PB12                                                                   \
    (gpio_pin) { .num = 12, .port = GPIO_PORT_B }
#define PB13                                                                   \
    (gpio_pin) { .num = 13, .port = GPIO_PORT_B }
#define PB14                                                                   \
    (gpio_pin) { .num = 14, .port = GPIO_PORT_B }
#define PB15                                                                   \
    (gpio_pin) { .num = 15, .port = GPIO_PORT_B }

#define PC0                                                                    \
    (gpio_pin) { .num = 0, .port = GPIO_PORT_C }
#define PC1                                                                    \
    (gpio_pin) { .num = 1, .port = GPIO_PORT_C }
#define PC2                                                                    \
    (gpio_pin) { .num = 2, .port = GPIO_PORT_C }
#define PC3                                                                    \
    (gpio_pin) { .num = 3, .port = GPIO_PORT_C }
#define PC4                                                                    \
    (gpio_pin) { .num = 4, .port = GPIO_PORT_C }
#define PC5                                                                    \
    (gpio_pin) { .num = 5, .port = GPIO_PORT_C }
#define PC6                                                                    \
    (gpio_pin) { .num = 6, .port = GPIO_PORT_C }
#define PC10                                                                   \
    (gpio_pin) { .num = 7, .port = GPIO_PORT_C }
#define PC11                                                                   \
    (gpio_pin) { .num = 8, .port = GPIO_PORT_C }
#define PC12                                                                   \
    (gpio_pin) { .num = 9, .port = GPIO_PORT_C }
#define PC13                                                                   \
    (gpio_pin) { .num = 10, .port = GPIO_PORT_C }
#define PC14                                                                   \
    (gpio_pin) { .num = 11, .port = GPIO_PORT_C }
#define PC15                                                                   \
    (gpio_pin) { .num = 12, .port = GPIO_PORT_C }

#define PD0                                                                    \
    (gpio_pin) { .num = 13, .port = GPIO_PORT_D }
#define PD1                                                                    \
    (gpio_pin) { .num = 14, .port = GPIO_PORT_D }

#define PE4                                                                    \
    (gpio_pin) { .num = 0, .port = GPIO_PORT_E }

#define PH3                                                                    \
    (gpio_pin) { .num = 0, .port = GPIO_PORT_H }

// Not sure about port C, D, E and H pin nums...

#define PIN_BOOT0 PH3

#define PIN_LED_RED PB1
#define PIN_LED_GREEN PB0
#define PIN_LED_BLUE PA4

#define PIN_SW1 PC4
#define PIN_SW2 PD0
#define PIN_SW3 PD1

// GPIO NUM END
// ************

enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };

#define HIGH true
#define LOW false

void gpio_turn_on_port(enum gpio_port port);

void gpio_turn_off_port(enum gpio_port port);

void gpio_set_mode(gpio_pin pin, uint8_t mode);

void gpio_write(gpio_pin pin, bool val);

void gpio_analog_write(gpio_pin pin, uint8_t val);

bool gpio_read(gpio_pin pin);

uint8_t gpio_analog_read(gpio_pin pin);

#endif // GPIO_H
