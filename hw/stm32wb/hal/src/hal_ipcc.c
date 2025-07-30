#include "hal_ipcc.h"

#include "hal_clock.h"
#include "hal_cortex.h"

#include <stddef.h>

hal_err ipcc_init(ipcc_handle_t *handle) {

    if (!handle || !handle->instance) {
        return ERR_IPCC_INIT_BADARGS;
    }

    if (handle->state == IPCC_STATE_RESET) {
        clock_ipcc_enable();
        cortex_nvic_set_priority(IPCC_C1_RX_IRQn, 0, 0);
        cortex_nvic_enable(IPCC_C1_RX_IRQn);
        cortex_nvic_set_priority(IPCC_C1_TX_IRQn, 0, 0);
        cortex_nvic_enable(IPCC_C1_TX_IRQn);
    }

    // Reset IPCC registers for C1
    WRITE_REG(handle->instance->C1CR, 0U);
    ipcc_c1_disable_all_channels(handle->instance);
    ipcc_c1_clear_all_statuses(handle->instance);

    ipcc_c1_enable_it_rxo(handle->instance);
    ipcc_c1_enable_it_txf(handle->instance);

    for (size_t i = 0; i < IPCC_CHANNEL_NUMBER; i++) {
        handle->channel_rx_cb[i] = NULL;
        handle->channel_tx_cb[i] = NULL;
    }

    handle->state = IPCC_STATE_READY;

    return OK;
}
