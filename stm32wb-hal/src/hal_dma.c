#include "hal_dma.h"

#include "hal.h"
#include "hal_err.h"
#include "stm32wbxx.h"

static dma_handle_t *hal_dma_active_handlers[14];

hal_err dma_init(dma_handle_t *handle) {

    if (!handle) {
        return ERR_DMA_INIT_BADARGS;
    }

    if ((uint32_t)handle->instance < (uint32_t)DMA2_Channel1) {
        handle->channel_index =
            (((uint32_t)handle->instance - (uint32_t)DMA1_Channel1) /
             ((uint32_t)DMA1_Channel2 - (uint32_t)DMA1_Channel1))
            << 2U;

        handle->dma_base = DMA1;
    } else {
        handle->channel_index =
            (((uint32_t)handle->instance - (uint32_t)DMA2_Channel1) /
             ((uint32_t)DMA2_Channel2 - (uint32_t)DMA2_Channel1))
            << 2U;
        handle->dma_base = DMA2;
    }

    handle->state = HAL_DMA_STATE_BUSY;

    uint32_t tmp;

    tmp = handle->instance->CCR;

    CLEAR_BIT(tmp,
              (DMA_CCR_PL | DMA_CCR_MSIZE | DMA_CCR_PSIZE | DMA_CCR_MINC |
               DMA_CCR_PINC | DMA_CCR_CIRC | DMA_CCR_DIR | DMA_CCR_MEM2MEM));

    tmp |= handle->init.direction | handle->init.peripheralAddrIncrement |
           handle->init.memoryAddrIncrement | handle->init.peripheral_align |
           handle->init.memory_align | handle->init.mode |
           handle->init.priority;

    WRITE_REG(handle->instance->CCR, tmp);

    if ((uint32_t)handle->instance < (uint32_t)DMA2_Channel1) {
        handle->dmamux_channel =
            (DMAMUX1_Channel0 + (handle->channel_index >> 2U));
    } else {
        handle->dmamux_channel =
            (DMAMUX1_Channel7 + (handle->channel_index >> 2U));
    }

    handle->dmamux_channel_status = DMAMUX1_ChannelStatus;

    uint32_t channel_number = (((uint32_t)handle->instance & 0xFFU) - 8U) / 20U;
    handle->dmamux_channel_status_mask = 1UL << (channel_number & 0x1FU);

    if (handle->init.direction == DMA_TRANSFER_MEMORY_TO_MEMORY) {
        handle->init.request = DMA_REQUEST_MEM2MEM;
    }

    WRITE_REG(handle->dmamux_channel->CCR,
              (handle->init.request & DMAMUX_CxCR_DMAREQ_ID));
    WRITE_REG(handle->dmamux_channel_status->CFR,
              handle->dmamux_channel_status_mask);

    if (handle->init.request > 0U && handle->init.request <= 3) {

        uint32_t request = handle->init.request & DMAMUX_CxCR_DMAREQ_ID;

        handle->dmamux_request_generator = (DMAMUX_RequestGen_TypeDef *)((
            uint32_t)(((uint32_t)DMAMUX1_RequestGenerator0) +
                      ((request - 1U) * 4U)));
        handle->dmamux_request_generator_status = DMAMUX1_RequestGenStatus;
        handle->dmamux_request_generator_status_mask =
            1UL << ((request - 1U) & 0x3U);

        WRITE_REG(handle->dmamux_request_generator->RGCR, 0U);
        WRITE_REG(handle->dmamux_request_generator_status->RGCFR,
                  handle->dmamux_request_generator_status_mask);
    } else {
        handle->dmamux_request_generator = 0U;
        handle->dmamux_request_generator_status = 0U;
        handle->dmamux_request_generator_status_mask = 0U;
    }

    handle->error = HAL_DMA_ERROR_NONE;
    handle->state = HAL_DMA_STATE_READY;
    handle->lock = false;

    return OK;
}

static void dma_set_config(dma_handle_t *handle, uint32_t source_address,
                           uint32_t destination_address, uint32_t length) {
    WRITE_REG(handle->dmamux_channel_status->CFR,
              handle->dmamux_channel_status_mask);

    if (handle->dmamux_request_generator) {
        WRITE_REG(handle->dmamux_request_generator_status->RGCFR,
                  handle->dmamux_request_generator_status_mask);
    }

    WRITE_REG(handle->dma_base->IFCR,
              (DMA_ISR_GIF1 << (handle->channel_index & 0x1CU)));

    WRITE_REG(handle->instance->CNDTR, length);

    if (handle->init.direction == DMA_TRANSFER_MEMORY_TO_PERIPH) {
        WRITE_REG(handle->instance->CPAR, destination_address);

        WRITE_REG(handle->instance->CMAR, source_address);
    } else {
        WRITE_REG(handle->instance->CPAR, source_address);

        WRITE_REG(handle->instance->CMAR, destination_address);
    }
}

hal_err dma_start_it(dma_handle_t *handle, uint32_t source_address,
                     uint32_t destination_address, uint32_t length) {

    if (source_address == 0 || destination_address == 0 || length == 0) {
        return ERR_DMA_STARTIT_BADARGS;
    }

    if (handle->lock || handle->state != HAL_DMA_STATE_READY) {
        return ERR_DMA_STARTIT_BUSY;
    }

    handle->lock = true;

    handle->state = HAL_DMA_STATE_BUSY;
    handle->error = HAL_DMA_ERROR_NONE;

    dma_disable();

    dma_set_config(handle, source_address, destination_address, length);

    if (handle->xfer_half_complete_callback) {
        SET_BIT(handle->instance->CCR,
                (DMA_CCR_TCIE | DMA_CCR_HTIE | DMA_CCR_TEIE));
    } else {
        CLEAR_BIT(handle->instance->CCR, DMA_CCR_HTIE);
        SET_BIT(handle->instance->CCR, (DMA_CCR_TCIE | DMA_CCR_TEIE));
    }

    if ((handle->dmamux_channel->CCR & DMAMUX_CxCR_SE) != 0U) {
        SET_BIT(handle->dmamux_channel->CCR, DMAMUX_CxCR_SOIE);
    }

    if (handle->dmamux_request_generator != 0U) {
        SET_BIT(handle->dmamux_request_generator->RGCR, DMAMUX_RGxCR_OIE);
    }

    if (handle->dma_base == DMA1) {
        hal_dma_active_handlers[handle->channel_index] = handle;
    }
    if (handle->dma_base == DMA2) {
        hal_dma_active_handlers[handle->channel_index * 2] = handle;
    }

    SET_BIT(handle->instance->CCR, DMA_CCR_EN);

    return OK;
}

__weak void dma_irq_handler(dma_handle_t *handle) {

    if (!handle) {
        return;
    }

    const uint32_t flag_it = handle->dma_base->ISR;
    const uint32_t source_it = handle->instance->CCR;

    const uint32_t htif = DMA_ISR_HTIF1 << (handle->channel_index & 0x1CU);
    const uint32_t tcif = DMA_ISR_TCIF1 << (handle->channel_index & 0x1CU);
    const uint32_t teif = DMA_ISR_TEIF1 << (handle->channel_index & 0x1CU);

    if ((source_it & DMA_CCR_HTIE) && (flag_it & htif)) {
        if (!(handle->instance->CCR & DMA_CCR_CIRC)) {
            CLEAR_BIT(handle->instance->CCR, DMA_CCR_HTIE);
        }

        WRITE_REG(handle->dma_base->IFCR, htif);

        if (handle->xfer_half_complete_callback) {
            handle->xfer_half_complete_callback(handle);
        }

        return;
    }

    if ((source_it & DMA_CCR_TCIE) && (flag_it & tcif)) {
        if (!(handle->instance->CCR & DMA_CCR_CIRC)) {
            CLEAR_BIT(handle->instance->CCR, DMA_CCR_TEIE | DMA_CCR_TCIE);
            handle->state = HAL_DMA_STATE_READY;
        }

        WRITE_REG(handle->dma_base->IFCR, tcif);

        handle->lock = false;

        if (handle->xfer_complete_callback) {
            handle->xfer_complete_callback(handle);
        }

        return;
    }

    if ((source_it & DMA_CCR_TEIE) && (flag_it & teif)) {
        CLEAR_BIT(handle->instance->CCR,
                  DMA_CCR_TEIE | DMA_CCR_TCIE | DMA_CCR_HTIE);

        const uint32_t gif = DMA_ISR_GIF1 << (handle->channel_index & 0x1CU);
        WRITE_REG(handle->dma_base->IFCR, gif);

        handle->error = HAL_DMA_ERROR_TE;

        handle->state = HAL_DMA_STATE_READY;

        handle->lock = false;

        if (handle->xfer_error_callback) {
            handle->xfer_error_callback(handle);
        }

        return;
    }
}

// TODO: Think of a better way...

__weak void DMA1_Channel1_IRQHandler(void) {
    dma_handle_t *handle = hal_dma_active_handlers[0];
    if (handle) {
        dma_irq_handler(handle);
    }
}
__weak void DMA1_Channel2_IRQHandler(void) {
    dma_handle_t *handle = hal_dma_active_handlers[1];
    if (handle) {
        dma_irq_handler(handle);
    }
}
__weak void DMA1_Channel3_IRQHandler(void) {

    dma_handle_t *handle = hal_dma_active_handlers[2];
    if (handle) {
        dma_irq_handler(handle);
    }
}
__weak void DMA1_Channel4_IRQHandler(void) {

    dma_handle_t *handle = hal_dma_active_handlers[3];
    if (handle) {
        dma_irq_handler(handle);
    }
}
__weak void DMA1_Channel5_IRQHandler(void) {

    dma_handle_t *handle = hal_dma_active_handlers[4];
    if (handle) {
        dma_irq_handler(handle);
    }
}
__weak void DMA1_Channel6_IRQHandler(void) {

    dma_handle_t *handle = hal_dma_active_handlers[5];
    if (handle) {
        dma_irq_handler(handle);
    }
}
__weak void DMA1_Channel7_IRQHandler(void) {

    dma_handle_t *handle = hal_dma_active_handlers[6];
    if (handle) {
        dma_irq_handler(handle);
    }
}
__weak void DMA2_Channel1_IRQHandler(void) {

    dma_handle_t *handle = hal_dma_active_handlers[7];
    if (handle) {
        dma_irq_handler(handle);
    }
}
__weak void DMA2_Channel2_IRQHandler(void) {

    dma_handle_t *handle = hal_dma_active_handlers[8];
    if (handle) {
        dma_irq_handler(handle);
    }
}
__weak void DMA2_Channel3_IRQHandler(void) {

    dma_handle_t *handle = hal_dma_active_handlers[9];
    if (handle) {
        dma_irq_handler(handle);
    }
}
__weak void DMA2_Channel4_IRQHandler(void) {

    dma_handle_t *handle = hal_dma_active_handlers[10];
    if (handle) {
        dma_irq_handler(handle);
    }
}
__weak void DMA2_Channel5_IRQHandler(void) {

    dma_handle_t *handle = hal_dma_active_handlers[11];
    if (handle) {
        dma_irq_handler(handle);
    }
}
__weak void DMA2_Channel6_IRQHandler(void) {

    dma_handle_t *handle = hal_dma_active_handlers[12];
    if (handle) {
        dma_irq_handler(handle);
    }
}
__weak void DMA2_Channel7_IRQHandler(void) {

    dma_handle_t *handle = hal_dma_active_handlers[13];
    if (handle) {
        dma_irq_handler(handle);
    }
}
