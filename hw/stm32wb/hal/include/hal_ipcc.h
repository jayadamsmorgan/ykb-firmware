#ifndef HAL_IPCC_H
#define HAL_IPCC_H

#include "hal_err.h"
#include "hal_exti.h"
#include "stm32wbxx.h"

typedef IPCC_TypeDef ipcc_t;

typedef enum {
    IPCC_CHANNEL_1 = 0U,
    IPCC_CHANNEL_2 = 1U,
    IPCC_CHANNEL_3 = 2U,
    IPCC_CHANNEL_4 = 3U,
    IPCC_CHANNEL_5 = 4U,
    IPCC_CHANNEL_6 = 5U,
} ipcc_channel;

typedef enum {
    IPCC_STATE_RESET = 0U,
    IPCC_STATE_READY = 1U,
    IPCC_STATE_BUSY = 2U,
} ipcc_state;

typedef enum {
    IPCC_DIR_TX = 0U,
    IPCC_DIR_RX = 1U,
} ipcc_direction;

typedef struct __ipcc_handle_t ipcc_handle_t;

typedef void (*ipcc_channel_cb_t)(ipcc_handle_t handle, ipcc_channel channel,
                                  ipcc_direction dir);

struct __ipcc_handle_t {

    ipcc_t *instance;

    ipcc_channel_cb_t channel_rx_cb[IPCC_CHANNEL_NUMBER];
    ipcc_channel_cb_t channel_tx_cb[IPCC_CHANNEL_NUMBER];

    uint32_t callback_request;

    __IO ipcc_state state;
};

static inline void ipcc_wakeup_it_enable() {
    hal_exti_enable_it(EXTI_EVENT_36);
}

static inline void ipcc_wakeup_it_disable() {
    hal_exti_disable_it(EXTI_EVENT_36);
}

static inline bool ipcc_wakeup_it_enabled() {
    return hal_exti_enabled_it(EXTI_EVENT_36);
}

static inline void ipcc_c1_enable_it_rxo(ipcc_t *instance) {
    SET_BIT(instance->C1CR, IPCC_C1CR_RXOIE);
}

static inline void ipcc_c1_disable_it_rxo(ipcc_t *instance) {
    CLEAR_BIT(instance->C1CR, IPCC_C1CR_RXOIE);
}

static inline bool ipcc_c1_it_rxo_enabled(ipcc_t *instance) {
    return READ_BIT(instance->C1CR, IPCC_C1CR_RXOIE);
}

static inline void ipcc_c1_enable_it_txf(ipcc_t *instance) {
    SET_BIT(instance->C1CR, IPCC_C1CR_TXFIE);
}

static inline void ipcc_c1_disable_it_txf(ipcc_t *instance) {
    CLEAR_BIT(instance->C1CR, IPCC_C1CR_TXFIE);
}

static inline bool ipcc_c1_it_txf_enabled(ipcc_t *instance) {
    return READ_BIT(instance->C1CR, IPCC_C1CR_TXFIE);
}

static inline void ipcc_c2_enable_it_rxo(ipcc_t *instance) {
    SET_BIT(instance->C2CR, IPCC_C2CR_RXOIE);
}

static inline void ipcc_c2_disable_it_rxo(ipcc_t *instance) {
    CLEAR_BIT(instance->C2CR, IPCC_C2CR_RXOIE);
}

static inline bool ipcc_c2_it_rxo_enabled(ipcc_t *instance) {
    return READ_BIT(instance->C2CR, IPCC_C2CR_RXOIE);
}

static inline void ipcc_c2_enable_it_txf(ipcc_t *instance) {
    SET_BIT(instance->C2CR, IPCC_C2CR_TXFIE);
}

static inline void ipcc_c2_disable_it_txf(ipcc_t *instance) {
    CLEAR_BIT(instance->C2CR, IPCC_C2CR_TXFIE);
}

static inline bool ipcc_c2_it_txf_enabled(ipcc_t *instance) {
    return READ_BIT(instance->C2CR, IPCC_C2CR_TXFIE);
}

static inline void ipcc_c1_enable_tx_channel(ipcc_t *instance,
                                             ipcc_channel channel) {
    CLEAR_BIT(instance->C1MR, 1U << channel << IPCC_C1MR_CH1FM_Pos);
}

static inline void ipcc_c1_disable_tx_channel(ipcc_t *instance,
                                              ipcc_channel channel) {
    SET_BIT(instance->C1MR, 1U << channel << IPCC_C1MR_CH1FM_Pos);
}

static inline bool ipcc_c1_tx_channel_enabled(ipcc_t *instance,
                                              ipcc_channel channel) {
    return !READ_BIT(instance->C1MR, 1U << channel << IPCC_C1MR_CH1FM_Pos);
}

static inline void ipcc_c1_enable_rx_channel(ipcc_t *instance,
                                             ipcc_channel channel) {
    CLEAR_BIT(instance->C1MR, 1U << channel);
}

static inline void ipcc_c1_disable_rx_channel(ipcc_t *instance,
                                              ipcc_channel channel) {
    SET_BIT(instance->C1MR, 1U << channel);
}

static inline bool ipcc_c1_rx_channel_enabled(ipcc_t *instance,
                                              ipcc_channel channel) {
    return !READ_BIT(instance->C1MR, 1U << channel);
}

static inline void ipcc_c2_enable_tx_channel(ipcc_t *instance,
                                             ipcc_channel channel) {
    CLEAR_BIT(instance->C2MR, 1U << channel << IPCC_C2MR_CH1FM_Pos);
}

static inline void ipcc_c2_disable_tx_channel(ipcc_t *instance,
                                              ipcc_channel channel) {
    SET_BIT(instance->C2MR, 1U << channel << IPCC_C2MR_CH1FM_Pos);
}

static inline bool ipcc_c2_tx_channel_enabled(ipcc_t *instance,
                                              ipcc_channel channel) {
    return !READ_BIT(instance->C2MR, 1U << channel << IPCC_C2MR_CH1FM_Pos);
}

static inline void ipcc_c2_enable_rx_channel(ipcc_t *instance,
                                             ipcc_channel channel) {
    CLEAR_BIT(instance->C2MR, 1U << channel);
}

static inline void ipcc_c2_disable_rx_channel(ipcc_t *instance,
                                              ipcc_channel channel) {
    SET_BIT(instance->C2MR, 1U << channel);
}

static inline bool ipcc_c2_rx_channel_enabled(ipcc_t *instance,
                                              ipcc_channel channel) {
    return !READ_BIT(instance->C2MR, 1U << channel);
}

static inline void ipcc_c1_enable_all_channels(ipcc_t *instance) {
    WRITE_REG(instance->C1MR, 0U);
}

static inline void ipcc_c1_disable_all_channels(ipcc_t *instance) {
    WRITE_REG(instance->C1MR, 0x003F003FU);
}

static inline void ipcc_c2_enable_all_channels(ipcc_t *instance) {
    WRITE_REG(instance->C2MR, 0U);
}

static inline void ipcc_c2_disable_all_channels(ipcc_t *instance) {
    WRITE_REG(instance->C2MR, 0x003F003FU);
}

static inline void ipcc_c1_clear_tx_channel_status(ipcc_t *instance,
                                                   ipcc_channel channel) {
    SET_BIT(instance->C1SCR, 1U << channel << IPCC_C1SCR_CH1S_Pos);
}

static inline void ipcc_c1_clear_rx_channel_status(ipcc_t *instance,
                                                   ipcc_channel channel) {
    SET_BIT(instance->C1SCR, 1U << channel);
}

static inline void ipcc_c1_clear_all_statuses(ipcc_t *instance) {
    WRITE_REG(instance->C1SCR, 0x003F003FU);
}

static inline void ipcc_c2_clear_tx_channel_status(ipcc_t *instance,
                                                   ipcc_channel channel) {
    SET_BIT(instance->C2SCR, 1U << channel << IPCC_C2SCR_CH1S_Pos);
}

static inline void ipcc_c2_clear_rx_channel_status(ipcc_t *instance,
                                                   ipcc_channel channel) {
    SET_BIT(instance->C2SCR, 1U << channel);
}

static inline void ipcc_c2_clear_all_statuses(ipcc_t *instance) {
    WRITE_REG(instance->C2SCR, 0x003F003FU);
}

static inline bool ipcc_c1_channel_occupied(ipcc_t *instance,
                                            ipcc_channel channel) {
    return READ_BIT(instance->C1TOC2SR, 1U << channel);
}

static inline bool ipcc_c2_channel_occupied(ipcc_t *instance,
                                            ipcc_channel channel) {
    return READ_BIT(instance->C2TOC1SR, 1U << channel);
}

hal_err ipcc_init(ipcc_handle_t *handle);

#endif // HAL_IPCC_H
