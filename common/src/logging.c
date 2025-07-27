#include "logging.h"

#include "pinout.h"
#include "settings.h"

#ifdef DEBUG

#if defined(DEBUG_UART_ENABLED) && DEBUG_UART_ENABLED == 1
#define LOGGING_BACKEND_UART
#endif // DEBUG_UART_ENABLED

#if defined(DEBUG_SWO_ENABLED) && DEBUG_SWO_ENABLED == 1

#ifndef DEBUG_SWO_BAUDRATE
#define DEBUG_SWO_BAUDRATE 2000000
#endif // DEBUG_SWO_BAUDRATE

#define LOGGING_BACKEND_SWO
#endif // DEBUG_SWO_ENABLED

// Backends for logging could be used simultaneously, but no reason to have
// none enabled.
#if !defined(LOGGING_BACKEND_UART) && !defined(LOGGING_BACKEND_SWO)
#error At least one backend should be used for debug logging (DEBUG_UART_ENABLED = 1 and/or DEBUG_SWO_ENABLED = 1)
#endif // !(DEBUG_UART_ENABLED == 1) && !(DEBUG_SWO_ENABLED == 1)

#ifdef LOGGING_BACKEND_UART

#ifndef DEBUG_UART_INSTANCE
#error DEBUG_UART_ENABLED but no UART instance selected with DEBUG_UART_INSTANCE
#endif // DEBUG_UART_INSTANCE

#ifndef PIN_DEBUG_UART_RX
#error Debug UART is enabled but PIN_DEBUG_UART_RX is undefined
#endif // PIN_DEBUG_UART_RX

#ifndef PIN_DEBUG_UART_TX
#error Debug UART is enabled but PIN_DEBUG_UART_TX is undefined
#endif // PIN_DEBUG_UART_TX

#endif // LOGGING_BACKEND_UART

#include "hal_uart.h"

#include "utils/utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifndef LOG_STR_MAX_LEN
#define LOG_STR_MAX_LEN 64
#endif // LOG_STR_MAX_LEN

#ifndef LOG_STR_QUEUE_LEN
#define LOG_STR_QUEUE_LEN 50
#endif // LOG_STR_QUEUE_LEN

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

#ifdef BOOTLOADER
    printf("[BL]: ");
#endif // BOOTLOADER

    switch (level) {
    case LOG_LEVEL_TRACE:
        printf("[TRACE]: ");
        break;
    case LOG_LEVEL_DEBUG:
        printf("[DEBUG]: ");
        break;
    case LOG_LEVEL_INFO:
        printf("[INFO]: ");
        break;
    case LOG_LEVEL_ERROR:
        printf("[ERROR]: ");
        break;
    case LOG_LEVEL_CRITICAL:
        printf("[CRITICAL]: ");
        break;
    }

    printf("(%s at %d): ", file_name, line);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\r\n");
}

static uart_handle_t uart_handle;

#ifdef LOGGING_BACKEND_UART
static inline hal_err setup_uart_logging() {
    uart_init_t init;
    init.instance = DEBUG_UART_INSTANCE;
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
    init.rx_pin = PIN_DEBUG_UART_RX;
    init.tx_pin = PIN_DEBUG_UART_TX;

    hal_err err;

    err = uart_init(&uart_handle, &init);
    if (err) {
        return err;
    }

    err = uart_set_txfifo_threshold(&uart_handle, UART_TXFIFO_THRESHOLD_1_8);
    if (err) {
        return err;
    }

    err = uart_set_rxfifo_threshold(&uart_handle, UART_RXFIFO_THRESHOLD_1_8);
    if (err) {
        return err;
    }

    err = uart_fifo_disable(&uart_handle);
    if (err) {
        return err;
    }

#ifdef PIN_SERIAL_ACTIVITY_LED
    gpio_turn_on_port(PIN_SERIAL_ACTIVITY_LED.gpio);
    gpio_set_mode(PIN_SERIAL_ACTIVITY_LED, GPIO_MODE_OUTPUT);
#endif // PIN_SERIAL_ACTIVITY_LED

    return OK;
}
#endif // LOGGING_BACKEND_UART

#ifdef LOGGING_BACKEND_SWO

static bool swo_available = false;
void swoInit() {}
static inline hal_err setup_swo_logging() {
    gpio_pin_t swd_pins[3] = {
        PA13,
        PA14,
        PB3,
    };
    for (size_t i = 0; i < 3; i++) {
        gpio_set_mode(swd_pins[i], GPIO_MODE_AF);
        gpio_set_speed(swd_pins[i], GPIO_SPEED_HIGH);
        gpio_set_af_mode(swd_pins[i], GPIO_AF_MODE_0);
        gpio_set_pupd(swd_pins[i], GPIO_PUPD_NONE);
    }

    uint32_t swo_prescaler = (SystemCoreClock / DEBUG_SWO_BAUDRATE) - 1u;

    SET_BIT(DCB->DEMCR, DCB_DEMCR_TRCENA_Msk);

    SET_BIT(DBGMCU->CR, DBGMCU_CR_TRACE_IOEN | DBGMCU_CR_DBG_STANDBY |
                            DBGMCU_CR_DBG_STOP | DBGMCU_CR_DBG_STOP);

    TPIU->SPPR = 0x00000002u; // Selected PIN Protocol Register: Select which
                              // protocol to use for trace output (2: SWO)

    TPIU->ACPR = swo_prescaler; // Async Clock Prescaler Register: Scale the
                                // baud rate of the asynchronous output
    ITM->LAR = 0xC5ACCE55u; // ITM Lock Access Register: C5ACCE55 enables more
                            // write access to Control Register 0xE00 :: 0xFFC
    ITM->TCR = 0x0001000Du; // ITM Trace Control Register
    ITM->TPR = ITM_TPR_PRIVMASK_Msk; // ITM Trace Privilege Register: All
                                     // stimulus ports
    ITM->TER = 1;             // ITM Trace Enable Register: Enabled tracing on
                              // stimulus ports. One bit per stimulus port.
    DWT->CTRL = 0x400003FEu;  // Data Watchpoint and Trace Register
    TPIU->FFCR = 0x00000100u; // Formatter and Flush Control Register

    // ITM/SWO works only if enabled from debugger.
    // If ITM stimulus 0 is not free, don't try to send data to SWO
    if (ITM->PORT[0].u8 == 1) {
        swo_available = true;
    }

    return OK;
}
#endif // LOGGING_BACKEND_SWO

#ifdef LOGGING_BACKEND_UART

static inline void write_uart(char *ptr, int len) {
#ifdef PIN_SERIAL_ACTIVITY_LED
    gpio_digital_write(PIN_SERIAL_ACTIVITY_LED, HIGH);
#endif // PIN_SERIAL_ACTIVITY_LED
    uart_transmit(&uart_handle, (uint8_t *)ptr, len, 0xFFFF);
#ifdef PIN_SERIAL_ACTIVITY_LED
    gpio_digital_write(PIN_SERIAL_ACTIVITY_LED, LOW);
#endif // PIN_SERIAL_ACTIVITY_LED
}
#endif // LOGGING_BACKEND_UART

#ifdef LOGGING_BACKEND_SWO
static inline void write_swo(char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        ITM_SendChar(ptr[i]);
    }
}
#endif // LOGGING_BACKEND_SWO

int _write(int file, char *ptr, int len) {
    UNUSED(file);

    if (logging_set_up) {
#ifdef LOGGING_BACKEND_UART
        write_uart(ptr, len);
#endif // LOGGING_BACKEND_UART

#ifdef LOGGING_BACKEND_SWO
        write_swo(ptr, len);
#endif // LOGGING_BACKEND_SWO
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

hal_err setup_logging() {

#ifdef LOGGING_BACKEND_UART
    setup_uart_logging();
#endif // LOGGING_BACKEND_UART

#ifdef LOGGING_BACKEND_SWO
    setup_swo_logging();
#endif // LOGGING_BACKEND_SWO

    if (log_str_queue_index > 0) {

        for (uint8_t i = 0; i < log_str_queue_index; i++) {
            string_to_write str = log_str_queue[i];
            _write(0, str.data, str.len);
        }

        log_str_queue_index = 0;
    }

    logging_set_up = true;

    return OK;
}

#endif // DEBUG
