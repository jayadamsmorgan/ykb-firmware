#include "logging.h"

#if defined(DEBUG) && defined(HAL_LPUART_ENABLED)

#include "config/pinout.h"
#include "hal/uart.h"
#include "settings.h"
#include "utils/utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define LOG_STR_MAX_LEN 64
#define LOG_STR_QUEUE_LEN 50

typedef struct __string_to_write {
    char data[LOG_STR_MAX_LEN];
    int len;
} string_to_write;

static log_level __level = LOG_LEVEL;
static bool logging_set_up = false;

static string_to_write log_str_queue[LOG_STR_QUEUE_LEN];
static uint8_t log_str_queue_index = 0;

void _log(log_level level, const char *file_name, const int line,
          const char *format, ...) {

    if (level < __level) {
        return;
    }

    switch (level) {
    case LOG_LEVEL_TRACE:
        printf("TRACE: ");
        break;
    case LOG_LEVEL_DEBUG:
        printf("DEBUG: ");
        break;
    case LOG_LEVEL_INFO:
        printf("INFO: ");
        break;
    case LOG_LEVEL_ERROR:
        printf("ERROR: ");
        break;
    case LOG_LEVEL_CRITICAL:
        printf("CRITICAL: ");
        break;
    }

    printf("(%s at %d): ", file_name, line);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\r\n");
}

hal_err setup_logging() {

    gpio_turn_on_port(PIN_LED_DBG2.gpio);
    gpio_set_mode(PIN_LED_DBG2, GPIO_MODE_OUTPUT);

    uart_init_t init;
    init.baudrate = 115200;
    init.word_length = UART_WORD_LENGTH_8B;
    init.stop_bits = UART_STOPBITS_1;
    init.parity = UART_PARITY_NONE;
    init.mode = UART_MODE_TX_RX;
    init.flow_control = UART_HW_FLOW_CONTROL_NONE;
    init.oversampling = UART_OVERSAMPLING_16;
    init.one_bit_sampling = UART_ONE_BIT_SAMPLING_DISABLE;
    init.clock_prescaler = UART_CLOCK_PRESCALER_DIV4;
    init.advanced.enable = false;
    init.rx_pin = PIN_LPUART_RX;
    init.tx_pin = PIN_LPUART_TX;

    hal_err err;

    err = lpuart_init(&init);
    if (err) {
        return err;
    }

    err = lpuart_set_txfifo_threshold(UART_TXFIFO_THRESHOLD_1_8);
    if (err) {
        return err;
    }

    err = lpuart_set_rxfifo_threshold(UART_RXFIFO_THRESHOLD_1_8);
    if (err) {
        return err;
    }

    err = lpuart_fifo_disable();
    if (err) {
        return err;
    }

    if (log_str_queue_index > 0) {

        for (uint8_t i = 0; i < log_str_queue_index; i++) {
            gpio_digital_write(PIN_LED_DBG2, HIGH);
            string_to_write str = log_str_queue[i];
            lpuart_transmit((uint8_t *)str.data, str.len, 0xFFFF);
            gpio_digital_write(PIN_LED_DBG2, LOW);
        }

        log_str_queue_index = 0;
    }

    logging_set_up = true;

    return OK;
}

int _write(int file, char *ptr, int len) {
    UNUSED(file);

    if (logging_set_up) {

        gpio_digital_write(PIN_LED_DBG2, HIGH);
        lpuart_transmit((uint8_t *)ptr, len, 0xFFFF);
        gpio_digital_write(PIN_LED_DBG2, LOW);

        return len;
    }

    if (log_str_queue_index >= LOG_STR_QUEUE_LEN) {
        return -1;
    }

    if (len > LOG_STR_MAX_LEN) {
        len = LOG_STR_MAX_LEN;
    }

    memcpy(log_str_queue[log_str_queue_index].data, ptr, len);
    log_str_queue[log_str_queue_index].len = len;
    log_str_queue_index++;

    return len;
}

#endif // DEBUG && HAL_UART_ENABLED
