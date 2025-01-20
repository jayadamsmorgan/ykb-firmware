#include "uart.h"

#include "clock.h"
#include "gpio.h"
#include "rcc.h"
#include "system.h"

void uart_init(unsigned long baud) {
    gpio_set_mode(PB6, GPIO_MODE_AF); // TX
    gpio_set_mode(PB7, GPIO_MODE_AF); // RX
    *RCC_APB2_ENR_REG |= BIT(14);
    GPIO_B_REG->AFRL |= 7UL << (4 * 7) | 7UL << (4 * 6);
    USART1->CR1 = 0;
    USART1->BRR = get_sys_freq() / baud;
    USART1->CR1 = BITS3(0, 2, 3);
}

int uart_read_ready() {
    return USART1->ISR & BIT(5); // If RXNE bit is set, data is ready
}

uint8_t uart_read_byte() { return (uint8_t)(USART1->RDR & 0xFF); }

uint8_t uart_read_byte_blocking() {
    while (uart_read_ready() == 0) {
        spin(1);
    }
    return uart_read_byte();
}

void uart_write_byte(uint8_t byte) {
    USART1->TDR = byte;
    while ((USART1->ISR & BIT(7)) == 0)
        spin(1);
}

void uart_write_buf(char *buf, size_t len) {
    while (len-- > 0)
        uart_write_byte(*(uint8_t *)buf++);
}
