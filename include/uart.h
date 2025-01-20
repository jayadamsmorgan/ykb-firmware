#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdlib.h>

struct usart {
    volatile uint32_t CR1, CR2, CR3, BRR, GTPR, RTOR, RQR, ISR, ICR, RDR, TDR,
        PRESC;
};
#define USART1 ((struct usart *)(0x40013800))

void uart_init(unsigned long baud);

int uart_read_ready();

uint8_t uart_read_byte();
uint8_t uart_read_byte_blocking();

void uart_write_byte(uint8_t byte);

void uart_write_buf(char *buf, size_t len);

#endif // UART_H
