#include "hal_uart.h"

#include "hal.h"
#include "hal_bits.h"
#include "hal_clock.h"
#include "hal_err.h"
#include "hal_gpio.h"
#include "hal_periph_clock.h"
#include "hal_systick.h"

#include "stm32wbxx.h"

#include <stdint.h>
#include <stdlib.h>

//
// Common
//

const uint16_t UARTPrescTable[12] = {1U,  2U,  4U,  6U,  8U,   10U,
                                     12U, 16U, 32U, 64U, 128U, 256U};

// Enable clock + pin setup
static hal_err uart_init_msp(volatile uart_handle_t *handle) {

    gpio_af_mode mode;
    gpio_pin_t tx = handle->init.tx_pin;
    gpio_pin_t rx = handle->init.rx_pin;

    if (handle->instance == LPUART1) {

        if (tx.af8_feature != GPIO_AF8_FEATURE_LPUART1_TX ||
            rx.af8_feature != GPIO_AF8_FEATURE_LPUART1_RX) {

            return ERR_UART_INIT_INV_PINCONFIG;
        }
        clock_lpuart1_enable();
        mode = GPIO_AF_MODE_8;

    } else if (handle->instance == USART1) {

        if (tx.af7_feature != GPIO_AF7_FEATURE_USART1_TX ||
            rx.af7_feature != GPIO_AF7_FEATURE_USART1_RX) {

            return ERR_UART_INIT_INV_PINCONFIG;
        }
        clock_usart1_enable();
        mode = GPIO_AF_MODE_7;

    } else {
        return ERR_UART_INIT_UNKNOWN_INSTANCE;
    }

    handle->lock = false;

    gpio_turn_on_port(tx.gpio);
    gpio_turn_on_port(rx.gpio);
    gpio_set_mode(tx, GPIO_MODE_AF);
    gpio_set_mode(rx, GPIO_MODE_AF);
    gpio_set_af_mode(tx, mode);
    gpio_set_af_mode(rx, mode);
    gpio_set_pupd(tx, GPIO_PULLUP);
    gpio_set_pupd(rx, GPIO_PULLUP);
    gpio_set_speed(tx, GPIO_SPEED_LOW);
    gpio_set_speed(rx, GPIO_SPEED_LOW);

    return OK;
}

static inline void anyuart_advanced_config(volatile uart_handle_t *handle) {

    if (!handle->init.advanced.enable) {
        return;
    }

    if (handle->init.advanced.swap) {
        SET_BIT(handle->instance->CR2, USART_CR2_SWAP);
    } else {
        CLEAR_BIT(handle->instance->CR2, USART_CR2_SWAP);
    }

    if (handle->init.advanced.rx_invert) {
        SET_BIT(handle->instance->CR2, USART_CR2_RXINV);
    } else {
        CLEAR_BIT(handle->instance->CR2, USART_CR2_RXINV);
    }

    if (handle->init.advanced.tx_invert) {
        SET_BIT(handle->instance->CR2, USART_CR2_TXINV);
    } else {
        CLEAR_BIT(handle->instance->CR2, USART_CR2_TXINV);
    }

    if (handle->init.advanced.data_invert) {
        SET_BIT(handle->instance->CR2, USART_CR2_DATAINV);
    } else {
        SET_BIT(handle->instance->CR2, USART_CR2_DATAINV);
    }

    if (handle->init.advanced.disable_overrun) {
        SET_BIT(handle->instance->CR3, USART_CR3_OVRDIS);
    } else {
        CLEAR_BIT(handle->instance->CR3, USART_CR3_OVRDIS);
    }

    if (handle->init.advanced.disable_dma_on_rx_error) {
        SET_BIT(handle->instance->CR3, USART_CR3_DDRE);
    } else {
        CLEAR_BIT(handle->instance->CR3, USART_CR3_DDRE);
    }

    if (handle->init.advanced.auto_baudrate_mode !=
        UART_ADVANCED_AUTOBAUDRATE_DISABLED) {
        SET_BIT(handle->instance->CR2, USART_CR2_ABREN);
        MODIFY_REG(handle->instance->CR2, USART_CR2_ABRMODE,
                   handle->init.advanced.auto_baudrate_mode);
    } else {
        CLEAR_BIT(handle->instance->CR2, USART_CR2_ABREN);
    }
}

static inline hal_err lpuart_config_brr(volatile uart_handle_t *handle) {

    periph_clock_uart_source clock_source = periph_clock_get_lpuart1_source();

    uint32_t pclk;

    switch (clock_source) {
    case PERIPHCLK_UART_CLKSOURCE_PCLK:
        pclk = clock_get_pclk1_frequency();
        break;
    case PERIPHCLK_UART_CLKSOURCE_SYSCLK:
        pclk = clock_get_system_clock();
        break;
    case PERIPHCLK_UART_CLKSOURCE_HSI:
        pclk = HSI_VALUE;
        break;
    case PERIPHCLK_UART_CLKSOURCE_LSE:
        pclk = LSE_VALUE;
        break;
    default:
        return ERR_UART_CONFIG_INV_CS;
    }

    if (pclk == 0) {
        return ERR_UART_CONFIG_INV_PCLK;
    }

    uint32_t uart_clock = (pclk / UARTPrescTable[handle->init.clock_prescaler]);

    if ((uart_clock < (3U * handle->init.baudrate)) ||
        (uart_clock > (4096U * handle->init.baudrate))) {
        return ERR_UART_CONFIG_INV_BR;
    }

    uint32_t usartdiv = (uart_clock * 256U + handle->init.baudrate / 2U) /
                        handle->init.baudrate;

    /* Check computed UsartDiv value is in allocated range
       (it is forbidden to write values lower than 0x300 in the
       LPUART_BRR register) */
    if ((usartdiv < 0x00000300U) || (usartdiv > 0x000FFFFFU)) {
        return ERR_UART_CONFIG_INV_DIV;
    }

    WRITE_REG(handle->instance->BRR, usartdiv);

    return OK;
}

static inline hal_err uart_config_brr(volatile uart_handle_t *handle) {

    periph_clock_uart_source clock_source = periph_clock_get_usart1_source();

    uint32_t pclk;

    switch (clock_source) {
    case PERIPHCLK_UART_CLKSOURCE_PCLK:
        pclk = clock_get_pclk2_frequency();
        break;
    case PERIPHCLK_UART_CLKSOURCE_SYSCLK:
        pclk = clock_get_system_clock();
        break;
    case PERIPHCLK_UART_CLKSOURCE_HSI:
        pclk = HSI_VALUE;
        break;
    case PERIPHCLK_UART_CLKSOURCE_LSE:
        pclk = LSE_VALUE;
        break;
    default:
        return ERR_UART_CONFIG_INV_CS;
    }

    if (pclk == 0) {
        return ERR_UART_CONFIG_INV_PCLK;
    }

    uint32_t usartdiv;
    if (handle->init.oversampling == UART_OVERSAMPLING_8) {

        usartdiv = (pclk / UARTPrescTable[handle->init.clock_prescaler] * 2U +
                    handle->init.baudrate / 2U) /
                   handle->init.baudrate;

        if (usartdiv < 0x10U || usartdiv > 0x0000FFFFU) {
            return ERR_UART_CONFIG_INV_DIV;
        }

        uint32_t brrtemp = usartdiv & 0xFFF0U;
        brrtemp |= (usartdiv & 0x000FU) >> 1U;
        WRITE_REG(handle->instance->BRR, brrtemp);

    } else {

        usartdiv = (pclk / UARTPrescTable[handle->init.clock_prescaler] +
                    handle->init.baudrate / 2U) /
                   handle->init.baudrate;

        if (usartdiv < 0x10U || usartdiv > 0x0000FFFFU) {
            return ERR_UART_CONFIG_INV_DIV;
        }

        WRITE_REG(handle->instance->BRR, usartdiv);
    }

    return OK;
}

static inline hal_err anyuart_config(volatile uart_handle_t *handle) {

    uint32_t clearmask;

    // CR1
    clearmask = (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE |
                 USART_CR1_RE | USART_CR1_OVER8 | USART_CR1_FIFOEN);
    MODIFY_REG(handle->instance->CR1, clearmask,
               handle->init.word_length | handle->init.parity |
                   handle->init.mode | handle->init.oversampling);

    // CR2
    MODIFY_REG(handle->instance->CR2, USART_CR2_STOP, handle->init.stop_bits);

    // CR3
    if (handle->instance != LPUART1) {
        MODIFY_REG(handle->instance->CR3, USART_CR3_ONEBIT,
                   handle->init.one_bit_sampling);
    }
    clearmask = (USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_TXFTCFG |
                 USART_CR3_RXFTCFG);
    MODIFY_REG(handle->instance->CR3, clearmask, handle->init.flow_control);

    // PRESC
    MODIFY_BITS(handle->instance->PRESC, USART_PRESC_PRESCALER_Pos,
                handle->init.clock_prescaler, BITMASK_4BIT);

    // BRR
    if (handle->instance == LPUART1) {
        hal_err err = lpuart_config_brr(handle);
        if (err) {
            return err;
        }
    }

    if (handle->instance == USART1) {
        hal_err err = uart_config_brr(handle);
        if (err) {
            return err;
        }
    }

    // Initialize the number of data to process during RX/TX ISR execution
    handle->rx_data_process_amount = 1;
    handle->tx_data_process_amount = 1;

    handle->rx_ISR = NULL;
    handle->tx_ISR = NULL;

    return OK;
}

static void anyuart_end_rx_transfer(volatile uart_handle_t *handle) {
    // Disable RXNE, PE and ERR (Frame error, noise error, overrun error)
    // interrupts
    ATOMIC_CLEAR_BIT(handle->instance->CR1,
                     (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));
    ATOMIC_CLEAR_BIT(handle->instance->CR3, (USART_CR3_EIE | USART_CR3_RXFTIE));

    // In case of reception waiting for IDLE event, disable also the IDLE IE
    // interrupt source
    if (handle->reception_type == UART_RECEPTION_TYPE_TOIDLE) {
        ATOMIC_CLEAR_BIT(handle->instance->CR1, USART_CR1_IDLEIE);
    }

    handle->rx_state = HAL_UART_STATE_READY;
    handle->reception_type = UART_RECEPTION_TYPE_STANDARD;
    handle->rx_ISR = NULL;
}

// Expects an inverted flag_status from what a STM32's HAL would expect,
// just because I think we should wait FOR the flag, not ON the flag
static hal_err anyuart_wait_on_flag(volatile uart_handle_t *handle,
                                    uint32_t flag, flag_status status,
                                    uint32_t timeout) {

    uint32_t tick_start = systick_get_tick();

    while (((handle->instance->ISR & flag) == flag) != status) {
        uint32_t ticks_passed = systick_get_tick() - tick_start;

        if (ticks_passed >= timeout || timeout == 0) {
            return ERR_UART_WOF_TIMEOUT;
        }

        if (flag == USART_ISR_TXE || flag == USART_ISR_TC) {
            continue;
        }

        if (!READ_BIT(handle->instance->CR1, USART_CR1_RE)) {
            continue;
        }

        if (READ_BIT(handle->instance->ISR, USART_ISR_ORE)) {

            WRITE_REG(handle->instance->ICR, USART_ICR_ORECF);

            anyuart_end_rx_transfer(handle);

            handle->error = HAL_UART_ERROR_ORE;
            handle->lock = false;
            return ERR_UART_WOF_BLOCK;
        }

        if (READ_BIT(handle->instance->ISR, USART_ISR_RTOF)) {

            WRITE_REG(handle->instance->ICR, USART_ICR_RTOCF);

            anyuart_end_rx_transfer(handle);

            handle->error = HAL_UART_ERROR_RTO;
            handle->lock = false;
            return ERR_UART_WOF_BLOCK;
        }
    }

    return OK;
}

hal_err anyuart_check_idle_state(volatile uart_handle_t *handle) {

    // Transmitter enabled
    if (READ_BIT(handle->instance->CR1, USART_CR1_TE)) {

        hal_err err = anyuart_wait_on_flag(handle, USART_ISR_TEACK, SET,
                                           HAL_UART_TIMEOUT_VALUE);
        if (err) {

            ATOMIC_CLEAR_BIT(handle->instance->CR1, USART_CR1_TXEIE_TXFNFIE);

            handle->state = HAL_UART_STATE_READY;
            handle->lock = false;
            return err;
        }
    }

    // Receiver enabled
    if (READ_BIT(handle->instance->CR1, USART_CR1_RE)) {

        hal_err err = anyuart_wait_on_flag(handle, USART_ISR_REACK, SET,
                                           HAL_UART_TIMEOUT_VALUE);
        if (err) {

            ATOMIC_CLEAR_BIT(handle->instance->CR1,
                             (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));
            ATOMIC_CLEAR_BIT(handle->instance->CR3, USART_CR3_EIE);

            handle->rx_state = HAL_UART_STATE_READY;
            handle->lock = false;
            return err;
        }
    }

    // Init UART state
    handle->state = HAL_UART_STATE_READY;
    handle->rx_state = HAL_UART_STATE_READY;
    handle->reception_type = UART_RECEPTION_TYPE_STANDARD;
    handle->receive_event = UART_RECEIVE_EVENT_TC;

    handle->lock = false;

    return OK;
}

static inline void anyuart_mask_update(volatile uart_handle_t *handle) {
    if (handle->init.word_length == UART_WORD_LENGTH_9B) {
        if (handle->init.parity == UART_PARITY_NONE) {
            handle->mask = 0x01FFU;
        } else {
            handle->mask = 0x00FFU;
        }
    } else if (handle->init.word_length == UART_WORD_LENGTH_8B) {
        if (handle->init.parity == UART_PARITY_NONE) {
            handle->mask = 0x00FFU;
        } else {
            handle->mask = 0x007FU;
        }
    } else if (handle->init.word_length == UART_WORD_LENGTH_7B) {
        if (handle->init.parity == UART_PARITY_NONE) {
            handle->mask = 0x007FU;
        } else {
            handle->mask = 0x003FU;
        }
    } else {
        handle->mask = 0x0000U;
    }
}

hal_err uart_init(uart_handle_t *handle, const uart_init_t *init) {
    if (!init) {
        return ERR_UART_INIT_ARGNULL;
    }
    handle->init = *init;

    handle->instance = init->instance;

    if (handle->state == HAL_UART_STATE_RESET) {
        hal_err err = uart_init_msp(handle);
        if (err) {
            return err;
        }
    }

    handle->state = HAL_UART_STATE_BUSY;

    CLEAR_BIT(handle->instance->CR1, USART_CR1_UE);

    anyuart_advanced_config(handle);

    hal_err err = anyuart_config(handle);
    if (err) {
        return err;
    }

    CLEAR_BIT(handle->instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
    CLEAR_BIT(handle->instance->CR3,
              (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

    SET_BIT(handle->instance->CR1, USART_CR1_UE);

    anyuart_mask_update(handle);

    return anyuart_check_idle_state(handle);
}

static inline void
anyuart_set_data_process_amounts(volatile uart_handle_t *handle) {

    if (handle->fifo_enabled) {
        handle->tx_data_process_amount = 1U;
        handle->rx_data_process_amount = 1U;
        return;
    }

    static const uint8_t numerator[] = {1U, 1U, 1U, 3U, 7U, 1U, 0U, 0U};
    static const uint8_t denominator[] = {8U, 4U, 2U, 4U, 8U, 1U, 1U, 1U};

    uint8_t rx_fifo_threshold =
        (READ_BIT(handle->instance->CR3, USART_CR3_RXFTCFG) >>
         USART_CR3_RXFTCFG_Pos);

    uint8_t tx_fifo_threshold =
        (READ_BIT(handle->instance->CR3, USART_CR3_TXFTCFG) >>
         USART_CR3_TXFTCFG_Pos);

    handle->tx_data_process_amount =
        (8U * numerator[tx_fifo_threshold]) / denominator[tx_fifo_threshold];
    handle->rx_data_process_amount =
        (8U * numerator[rx_fifo_threshold]) / denominator[rx_fifo_threshold];
}

hal_err uart_set_txfifo_threshold(uart_handle_t *handle,
                                  uart_txfifo_threshold threshold) {

    if (handle->lock) {
        return ERR_UART_SETTXFIFO_BUSY;
    }
    handle->lock = true;
    handle->state = HAL_UART_STATE_BUSY;

    uint32_t cr1_temp = READ_REG(handle->instance->CR1);

    CLEAR_BIT(handle->instance->CR1, USART_CR1_UE);

    MODIFY_REG(handle->instance->CR3, USART_CR3_TXFTCFG, threshold);

    anyuart_set_data_process_amounts(handle);

    WRITE_REG(handle->instance->CR1, cr1_temp);

    handle->state = HAL_UART_STATE_READY;
    handle->lock = false;
    return OK;
}

hal_err uart_set_rxfifo_threshold(uart_handle_t *handle,
                                  uart_rxfifo_threshold threshold) {

    if (handle->lock) {
        return ERR_UART_SETRXFIFO_BUSY;
    }
    handle->lock = true;
    handle->state = HAL_UART_STATE_BUSY;

    uint32_t cr1_temp = READ_REG(handle->instance->CR1);

    CLEAR_BIT(handle->instance->CR1, USART_CR1_UE);

    MODIFY_REG(handle->instance->CR3, USART_CR3_RXFTCFG, threshold);

    anyuart_set_data_process_amounts(handle);

    WRITE_REG(handle->instance->CR1, cr1_temp);

    handle->state = HAL_UART_STATE_READY;
    handle->lock = false;
    return OK;
}

hal_err uart_fifo_enable(uart_handle_t *handle) {
    if (handle->lock) {
        return ERR_UART_FIFOEN_BUSY;
    }
    handle->lock = true;
    handle->state = HAL_UART_STATE_BUSY;

    uint32_t cr1_temp = READ_REG(handle->instance->CR1);

    CLEAR_BIT(handle->instance->CR1, USART_CR1_UE);

    SET_BIT(cr1_temp, USART_CR1_FIFOEN);
    handle->fifo_enabled = true;

    WRITE_REG(handle->instance->CR1, cr1_temp);

    handle->state = HAL_UART_STATE_READY;
    handle->lock = false;
    return OK;
}

hal_err uart_fifo_disable(uart_handle_t *handle) {
    if (handle->lock) {
        return ERR_UART_FIFOEN_BUSY;
    }
    handle->lock = true;
    handle->state = HAL_UART_STATE_BUSY;

    uint32_t cr1_temp = READ_REG(handle->instance->CR1);

    CLEAR_BIT(handle->instance->CR1, USART_CR1_UE);

    CLEAR_BIT(cr1_temp, USART_CR1_FIFOEN);
    handle->fifo_enabled = false;

    WRITE_REG(handle->instance->CR1, cr1_temp);

    handle->state = HAL_UART_STATE_READY;
    handle->lock = false;
    return OK;
}

hal_err uart_receive(uart_handle_t *handle, uint8_t *rx_buffer,
                     uint16_t buffer_size, uint32_t timeout) {

    if (handle->rx_state != HAL_UART_STATE_READY) {
        return ERR_UART_RX_BUSY;
    }

    if (!rx_buffer || buffer_size == 0U) {
        return ERR_UART_RX_BADARGS;
    }

    handle->error = HAL_UART_ERROR_NONE;
    handle->rx_state = HAL_UART_STATE_BUSY_RX;
    handle->reception_type = UART_RECEPTION_TYPE_STANDARD;

    handle->rx_xfer_size = buffer_size;
    handle->rx_xfer_count = buffer_size;

    uint8_t *buff_8bits = NULL;
    uint16_t *buff_16bits = NULL;

    // In case of 9bits/No Parity transfer, pRxData needs to be handled as a
    // uint16_t pointer
    if (handle->init.word_length == UART_WORD_LENGTH_9B &&
        handle->init.parity == UART_PARITY_NONE) {
        buff_16bits = (uint16_t *)rx_buffer;
    } else {
        buff_8bits = rx_buffer;
    }

    while (handle->rx_xfer_count) {
        hal_err err =
            anyuart_wait_on_flag(handle, USART_ISR_RXNE, SET, timeout);
        if (err) {
            handle->rx_state = HAL_UART_STATE_READY;
            return err;
        }

        if (buff_8bits) {
            *buff_8bits = (uint8_t)(handle->instance->RDR & handle->mask);
            buff_8bits++;
        }
        if (buff_16bits) {
            *buff_16bits = (uint16_t)(handle->instance->RDR & handle->mask);
            buff_16bits++;
        }
        handle->rx_xfer_count--;
    }

    handle->rx_state = HAL_UART_STATE_READY;

    return OK;
}

hal_err uart_transmit(uart_handle_t *handle, const uint8_t *tx_buffer,
                      uint16_t buffer_size, uint32_t timeout) {

    if (handle->state != HAL_UART_STATE_READY) {
        return ERR_UART_TX_BUSY;
    }

    if (!tx_buffer || buffer_size == 0U) {
        return ERR_UART_TX_BADARGS;
    }

    handle->error = HAL_UART_ERROR_NONE;
    handle->state = HAL_UART_STATE_BUSY_TX;

    handle->tx_xfer_size = buffer_size;
    handle->tx_xfer_count = buffer_size;

    const uint8_t *buff_8bits = NULL;
    const uint16_t *buff_16bits = NULL;

    // In case of 9bits/No Parity transfer, pRxData needs to be handled as a
    // uint16_t pointer
    if ((handle->init.word_length == UART_WORD_LENGTH_9B) &&
        (handle->init.parity == UART_PARITY_NONE)) {
        buff_16bits = (const uint16_t *)tx_buffer;
    } else {
        buff_8bits = tx_buffer;
    }

    while (handle->tx_xfer_count) {
        hal_err err = anyuart_wait_on_flag(handle, USART_ISR_TXE, SET, timeout);
        if (err) {
            handle->state = HAL_UART_STATE_READY;
            return err;
        }
        if (buff_8bits) {
            handle->instance->TDR = (uint8_t)(*buff_8bits & 0xFFU);
            buff_8bits++;
        }
        if (buff_16bits) {
            handle->instance->TDR = (uint16_t)(*buff_16bits & 0x01FFU);
            buff_16bits++;
        }
        handle->tx_xfer_count--;
    }

    hal_err err = anyuart_wait_on_flag(handle, USART_ISR_TC, SET, timeout);
    if (err) {
        handle->state = HAL_UART_STATE_READY;
        return err;
    }

    handle->state = HAL_UART_STATE_READY;
    return OK;
}
