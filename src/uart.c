#include "uart.h"

#define HAL_UART_ENABLED

#include "hal/hal_err.h"
#include "hal/uart.h"

hal_err setup_uart() {

    uart_init_t init;
    init.baudrate = 115200;
    init.word_length = UART_WORD_LENGTH_8B;
    init.stop_bits = UART_STOPBITS_1;
    init.parity = UART_PARITY_NONE;
    init.mode = UART_MODE_TX_RX;
    init.flow_control = UART_HW_FLOW_CONTROL_NONE;
    init.oversampling = UART_OVERSAMPLING_16;
    init.one_bit_sampling = UART_ONE_BIT_SAMPLING_DISABLE;
    init.clock_prescaler = UART_CLOCK_PRESCALER_DIV1;
    init.advanced.enable = false;
    init.rx_pin = PB7;
    init.tx_pin = PB6;

    hal_err err;

    err = uart_init(&init);
    if (err) {
        return err;
    }

    err = uart_set_txfifo_threshold(UART_TXFIFO_THRESHOLD_1_8);
    if (err) {
        return err;
    }

    err = uart_set_rxfifo_threshold(UART_RXFIFO_THRESHOLD_1_8);
    if (err) {
        return err;
    }

    err = uart_fifo_disable();
    if (err) {
        return err;
    }

    return OK;
}
