#ifndef HAL_DMA_H
#define HAL_DMA_H

#include "hal/hal_err.h"
#include "stm32wbxx.h"

#define HAL_DMA_ERROR_NONE 0x00000000U
#define HAL_DMA_ERROR_TE 0x00000001U
#define HAL_DMA_ERROR_NO_XFER 0x00000004U
#define HAL_DMA_ERROR_TIMEOUT 0x00000020U
#define HAL_DMA_ERROR_NOT_SUPPORTED 0x00000100U
#define HAL_DMA_ERROR_SYNC 0x00000200U
#define HAL_DMA_ERROR_REQGEN 0x00000400U

typedef DMA_Channel_TypeDef dma_channel_t;
typedef DMA_TypeDef dma_t;

typedef DMAMUX_Channel_TypeDef dmamux_channel_t;
typedef DMAMUX_ChannelStatus_TypeDef dmamux_channel_status_t;
typedef DMAMUX_RequestGen_TypeDef dmamux_request_generator_t;
typedef DMAMUX_RequestGenStatus_TypeDef dmamux_request_generator_status_t;

typedef enum {
    DMA_REQUEST_MEM2MEM = 0x00000000U,
    DMA_REQUEST_GENERATOR0 = 0x00000001U,
    DMA_REQUEST_GENERATOR1 = 0x00000002U,
    DMA_REQUEST_GENERATOR2 = 0x00000003U,
    DMA_REQUEST_GENERATOR3 = 0x00000004U,
    DMA_REQUEST_ADC1 = 0x00000005U,
    DMA_REQUEST_SPI1_RX = 0x00000006U,
    DMA_REQUEST_SPI1_TX = 0x00000007U,
    DMA_REQUEST_SPI2_RX = 0x00000008U,
    DMA_REQUEST_SPI2_TX = 0x00000009U,
    DMA_REQUEST_I2C1_RX = 0x0000000AU,
    DMA_REQUEST_I2C1_TX = 0x0000000BU,
    DMA_REQUEST_I2C3_RX = 0x0000000CU,
    DMA_REQUEST_I2C3_TX = 0x0000000DU,
    DMA_REQUEST_USART1_RX = 0x0000000EU,
    DMA_REQUEST_USART1_TX = 0x0000000FU,
    DMA_REQUEST_LPUART1_RX = 0x00000010U,
    DMA_REQUEST_LPUART1_TX = 0x00000011U,
    DMA_REQUEST_SAI1_A = 0x00000012U,
    DMA_REQUEST_SAI1_B = 0x00000013U,
    DMA_REQUEST_QUADSPI = 0x00000014U,
    DMA_REQUEST_TIM1_CH1 = 0x00000015U,
    DMA_REQUEST_TIM1_CH2 = 0x00000016U,
    DMA_REQUEST_TIM1_CH3 = 0x00000017U,
    DMA_REQUEST_TIM1_CH4 = 0x00000018U,
    DMA_REQUEST_TIM1_UP = 0x00000019U,
    DMA_REQUEST_TIM1_TRIG = 0x0000001AU,
    DMA_REQUEST_TIM1_COM = 0x0000001BU,
    DMA_REQUEST_TIM2_CH1 = 0x0000001CU,
    DMA_REQUEST_TIM2_CH2 = 0x0000001DU,
    DMA_REQUEST_TIM2_CH3 = 0x0000001EU,
    DMA_REQUEST_TIM2_CH4 = 0x0000001FU,
    DMA_REQUEST_TIM2_UP = 0x00000020U,
    DMA_REQUEST_TIM16_CH1 = 0x00000021U,
    DMA_REQUEST_TIM16_UP = 0x00000022U,
    DMA_REQUEST_TIM17_CH1 = 0x00000023U,
    DMA_REQUEST_TIM17_UP = 0x00000024U,
    DMA_REQUEST_AES1_IN = 0x00000025U,
    DMA_REQUEST_AES1_OUT = 0x00000026U,
    DMA_REQUEST_AES2_IN = 0x00000027U,
    DMA_REQUEST_AES2_OUT = 0x00000028U,
} dma_request;

typedef enum {
    DMA_TRANSFER_PERIPH_TO_MEMORY = 0U,
    DMA_TRANSFER_MEMORY_TO_PERIPH = DMA_CCR_DIR,
    DMA_TRANSFER_MEMORY_TO_MEMORY = DMA_CCR_MEM2MEM,
} dma_transfer_direction;

typedef enum {
    DMA_PERIPHERAL_DATA_ALIGN_BYTE = 0U,
    DMA_PERIPHERAL_DATA_ALIGN_HALFWORD = DMA_CCR_PSIZE_0,
    DMA_PERIPHERAL_DATA_ALIGN_WORD = DMA_CCR_PSIZE_1,
} dma_peripheral_data_align;

typedef enum {
    DMA_MEMORY_DATA_ALIGN_BYTE = 0U,
    DMA_MEMORY_DATA_ALIGN_HALFWORD = DMA_CCR_MSIZE_0,
    DMA_MEMORY_DATA_ALIGN_WORD = DMA_CCR_MSIZE_1,
} dma_memory_data_align;

typedef enum {
    DMA_MODE_NORMAL = 0U,
    DMA_MODE_CIRCULAR = DMA_CCR_CIRC,
} dma_mode;

typedef enum {
    DMA_PRIORITY_LOW = 0U,
    DMA_PRIORITY_MEDIUM = DMA_CCR_PL_0,
    DMA_PRIORITY_HIGH = DMA_CCR_PL_1,
    DMA_PRIORITY_VERY_HIGH = DMA_CCR_PL,
} dma_priority;

typedef struct {

    dma_request request;

    dma_transfer_direction direction;

    bool peripheralAddrIncrement;

    bool memoryAddrIncrement;

    dma_peripheral_data_align peripheral_align;

    dma_memory_data_align memory_align;

    dma_mode mode;

    dma_priority priority;

} dma_init_t;

typedef enum {
    HAL_DMA_STATE_RESET = 0x00U,
    HAL_DMA_STATE_READY = 0x01U,
    HAL_DMA_STATE_BUSY = 0x02U,
    HAL_DMA_STATE_TIMEOUT = 0x03U,
} dma_state;

typedef struct __dma_handle_t {

    dma_channel_t *instance;

    dma_init_t init;

    bool lock;

    __IO dma_state state;

    void *parent;

    void (*xfer_complete_callback)(struct __dma_handle_t *handle);
    void (*xfer_half_complete_callback)(struct __dma_handle_t *handle);
    void (*xfer_error_callback)(struct __dma_handle_t *handle);
    void (*xfer_abort_callback)(struct __dma_handle_t *handle);

    __IO uint32_t error;

    uint32_t channel_index;

    dma_t *dma_base;

    dmamux_channel_t *dmamux_channel;
    dmamux_channel_status_t *dmamux_channel_status;
    uint32_t dmamux_channel_status_mask;
    dmamux_request_generator_t *dmamux_request_generator;
    dmamux_request_generator_status_t *dmamux_request_generator_status;
    uint32_t dmamux_request_generator_status_mask;

} dma_handle_t;

hal_err dma_init(dma_handle_t *handle);

static inline void dma1_enable() { SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN); }

static inline void dma2_enable() { SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN); }

static inline void dma_enable() { SET_BIT(DMAMUX1->CCR, DMA_CCR_EN); }

static inline void dma_disable() { CLEAR_BIT(DMAMUX1->CCR, DMA_CCR_EN); }

static inline void dma_mux_enable() {
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMAMUX1EN);
}

hal_err dma_start_it(dma_handle_t *handle, uint32_t source_address,
                     uint32_t destination_address, uint32_t length);

#endif // HAL_DMA_H
