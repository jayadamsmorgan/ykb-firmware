#if defined(DEBUG) && defined(HAL_UART_ENABLED)

#include "logging.h"

#include "hal/uart.h"
#include "settings.h"
#include "utils/utils.h"
#include <stdarg.h>
#include <string.h>

static log_level __level = LOG_LEVEL;
static bool logging_set_up = false;

void _log(log_level level, const char *format, ...) {

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

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\r\n");
}

hal_err setup_logging() {

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

    logging_set_up = true;

    return OK;
}

#define LOG_STR_MAX_LEN 64
#define LOG_STR_QUEUE_LEN 20

typedef struct __string_to_write {
    char data[LOG_STR_MAX_LEN];
    int len;
} string_to_write;

static string_to_write log_str_queue[LOG_STR_QUEUE_LEN];
static uint8_t log_str_queue_index = 0;

int _write(int file, char *ptr, int len) {
    UNUSED(file);

    if (logging_set_up) {

        while (log_str_queue_index > 0) {
            log_str_queue_index--;
            gpio_digital_write(PB0, HIGH);
            string_to_write str = log_str_queue[log_str_queue_index];
            uart_transmit((uint8_t *)str.data, str.len, 0xFFFF);
            gpio_digital_write(PB0, LOW);
        }

        gpio_digital_write(PB0, HIGH);
        uart_transmit((uint8_t *)ptr, len, 0xFFFF);
        gpio_digital_write(PB0, LOW);

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
