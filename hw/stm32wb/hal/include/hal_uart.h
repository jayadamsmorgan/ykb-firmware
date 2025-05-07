#ifndef HAL_UART_H
#define HAL_UART_H

#include "hal_err.h"
#include "hal_gpio.h"
#include "stm32wbxx.h"

#include <stdint.h>

typedef USART_TypeDef uart_t;

#define HAL_UART_STATE_RESET 0x00000000U
#define HAL_UART_STATE_READY 0x00000020U
#define HAL_UART_STATE_BUSY 0x00000024U
#define HAL_UART_STATE_BUSY_TX 0x00000021U
#define HAL_UART_STATE_BUSY_RX 0x00000022U
#define HAL_UART_STATE_BUSY_TX_RX 0x00000023U
#define HAL_UART_STATE_TIMEOUT 0x000000A0U
#define HAL_UART_STATE_ERROR 0x000000E0U

#define HAL_UART_ERROR_NONE 0x00000000U
#define HAL_UART_ERROR_PE 0x00000001U
#define HAL_UART_ERROR_NE 0x00000002U
#define HAL_UART_ERROR_FE 0x00000004U
#define HAL_UART_ERROR_ORE 0x00000008U
#define HAL_UART_ERROR_DMA 0x00000010U
#define HAL_UART_ERROR_RTO 0x00000020U

#ifndef HAL_UART_TIMEOUT_VALUE
#define HAL_UART_TIMEOUT_VALUE 0x1FFFFFFU
#endif // HAL_UART_TIMEOUT_VALUE

typedef enum {
    UART_MODE_TX = USART_CR1_TE,
    UART_MODE_RX = USART_CR1_RE,
    UART_MODE_TX_RX = USART_CR1_TE | USART_CR1_RE,
} uart_mode;

typedef enum {
    UART_WORD_LENGTH_8B = 0U,
    UART_WORD_LENGTH_9B = USART_CR1_M1,
    UART_WORD_LENGTH_7B = USART_CR1_M0,
} uart_word_length;

typedef enum {
    UART_STOPBITS_0_5 = USART_CR2_STOP_0,
    UART_STOPBITS_1 = 0U,
    UART_STOPBITS_1_5 = USART_CR2_STOP_0 | USART_CR2_STOP_1,
    UART_STOPBITS_2 = USART_CR2_STOP_1,
} uart_stop_bits;

typedef enum {
    UART_PARITY_NONE = 0U,
    UART_PARITY_EVEN = USART_CR1_PCE,
    UART_PARITY_ODD = USART_CR1_PCE | USART_CR1_PS,
} uart_parity;

typedef enum {
    UART_HW_FLOW_CONTROL_NONE = 0U,
    UART_HW_FLOW_CONTROL_RTS = USART_CR3_RTSE,
    UART_HW_FLOW_CONTROL_CTS = USART_CR3_CTSE,
    UART_HW_FLOW_CONTROL_RTS_CTS = USART_CR3_RTSE | USART_CR3_CTSE,
} uart_hw_flow_control;

typedef enum {
    UART_OVERSAMPLING_16 = 0U,
    UART_OVERSAMPLING_8 = USART_CR1_OVER8,
} uart_oversampling;

typedef enum {
    UART_ONE_BIT_SAMPLING_DISABLE = 0U,
    UART_ONE_BIT_SAMPLING_ENABLE = USART_CR3_ONEBIT,
} uart_one_bit_sampling;

typedef enum {
    UART_CLOCK_PRESCALER_DIV1 = 0U,
    UART_CLOCK_PRESCALER_DIV2 = 1U,
    UART_CLOCK_PRESCALER_DIV4 = 2U,
    UART_CLOCK_PRESCALER_DIV6 = 3U,
    UART_CLOCK_PRESCALER_DIV8 = 4U,
    UART_CLOCK_PRESCALER_DIV10 = 5U,
    UART_CLOCK_PRESCALER_DIV12 = 6U,
    UART_CLOCK_PRESCALER_DIV16 = 7U,
    UART_CLOCK_PRESCALER_DIV32 = 8U,
    UART_CLOCK_PRESCALER_DIV64 = 9U,
    UART_CLOCK_PRESCALER_DIV128 = 10U,
    UART_CLOCK_PRESCALER_DIV256 = 11U,
} uart_clock_prescaler;

typedef enum {
    UART_ADVANCED_AUTOBAUDRATE_DISABLED = -1,
    UART_ADVANCED_AUTOBAUDRATE_ON_START_BIT = 0U,
    UART_ADVANCED_AUTOBAUDRATE_ON_FALLING_EDGE = USART_CR2_ABRMODE_0,
    UART_ADVANCED_AUTOBAUDRATE_ON_0X7F_FRAME = USART_CR2_ABRMODE_1,
    UART_ADVANCED_AUTOBAUDRATE_ON_0X55_FRAME = USART_CR2_ABRMODE,
} uart_advanced_auto_baudrate_mode;

typedef struct {

    bool enable : 1;

    bool tx_invert : 1;

    bool rx_invert : 1;

    bool data_invert : 1;

    bool swap : 1;

    bool disable_overrun : 1;

    bool disable_dma_on_rx_error : 1;

    uart_advanced_auto_baudrate_mode auto_baudrate_mode;

} uart_advanced_init_t;

typedef struct {

    uart_t *instance;

    uint32_t baudrate;

    uart_word_length word_length;

    uart_stop_bits stop_bits;

    uart_parity parity;

    uart_mode mode;

    uart_hw_flow_control flow_control;

    uart_oversampling oversampling;

    uart_one_bit_sampling one_bit_sampling;

    uart_clock_prescaler clock_prescaler;

    uart_advanced_init_t advanced;

    gpio_pin_t tx_pin;
    gpio_pin_t rx_pin;

} uart_init_t;

typedef enum {
    UART_RECEPTION_TYPE_STANDARD = 0U,
    UART_RECEPTION_TYPE_TOIDLE = 1U,
    UART_RECEPTION_TYPE_TORTO = 2U,
    UART_RECEPTION_TYPE_TOCHARMATCH = 3U,
} uart_reception_type;

typedef enum {
    UART_RECEIVE_EVENT_TC = 0U,
    UART_RECEIVE_EVENT_HT = 1U,
    UART_RECEIVE_EVENT_IDLE = 2U,
} uart_receive_event;

typedef enum {
    UART_RXFIFO_THRESHOLD_1_8 = 0x00000000U,
    UART_RXFIFO_THRESHOLD_1_4 = USART_CR3_RXFTCFG_0,
    UART_RXFIFO_THRESHOLD_1_2 = USART_CR3_RXFTCFG_1,
    UART_RXFIFO_THRESHOLD_3_4 = USART_CR3_RXFTCFG_0 | USART_CR3_RXFTCFG_1,
    UART_RXFIFO_THRESHOLD_7_8 = USART_CR3_RXFTCFG_2,
    UART_RXFIFO_THRESHOLD_8_8 = USART_CR3_RXFTCFG_2 | USART_CR3_RXFTCFG_0,
} uart_rxfifo_threshold;

typedef enum {
    UART_TXFIFO_THRESHOLD_1_8 = 0x00000000U,
    UART_TXFIFO_THRESHOLD_1_4 = USART_CR3_TXFTCFG_0,
    UART_TXFIFO_THRESHOLD_1_2 = USART_CR3_TXFTCFG_1,
    UART_TXFIFO_THRESHOLD_3_4 = USART_CR3_TXFTCFG_0 | USART_CR3_TXFTCFG_1,
    UART_TXFIFO_THRESHOLD_7_8 = USART_CR3_TXFTCFG_2,
    UART_TXFIFO_THRESHOLD_8_8 = USART_CR3_TXFTCFG_2 | USART_CR3_TXFTCFG_0,
} uart_txfifo_threshold;

typedef struct __uart_handle_t {

    uart_t *instance;

    __IO uint32_t state;
    uint16_t tx_xfer_size;
    __IO uint16_t tx_xfer_count;

    __IO uint32_t rx_state;
    uint16_t rx_xfer_size;
    __IO uint16_t rx_xfer_count;

    uint16_t mask;

    __IO uint32_t error;

    __IO uart_reception_type reception_type;
    __IO uart_receive_event receive_event;

    bool lock;

    uart_init_t init;

    uint16_t tx_data_process_amount;
    uint16_t rx_data_process_amount;

    bool fifo_enabled;

    void (*rx_ISR)(struct __uart_handle_t
                       *handle); /*!< Function pointer on Rx IRQ handler */

    void (*tx_ISR)(struct __uart_handle_t
                       *handle); /*!< Function pointer on Tx IRQ handler */

} uart_handle_t;

hal_err uart_init(uart_handle_t *handle, const uart_init_t *init);

hal_err uart_fifo_enable(uart_handle_t *handle);
hal_err uart_fifo_disable(uart_handle_t *handle);

hal_err uart_set_rxfifo_threshold(uart_handle_t *handle,
                                  uart_rxfifo_threshold threshold);
hal_err uart_set_txfifo_threshold(uart_handle_t *handle,
                                  uart_txfifo_threshold threshold);

hal_err uart_transmit(uart_handle_t *handle, const uint8_t *tx_buffer,
                      uint16_t buffer_size, uint32_t timeout);
hal_err uart_receive(uart_handle_t *handle, uint8_t *rx_buffer,
                     uint16_t buffer_size, uint32_t timeout);

#endif // HAL_UART_H
