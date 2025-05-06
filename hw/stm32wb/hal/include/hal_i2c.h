#ifndef HAL_I2C_H
#define HAL_I2C_H

#include "hal_err.h"
#include "stm32wbxx.h"

#include <stdint.h>

typedef I2C_TypeDef i2c_t;

typedef enum {
    I2C_ADDR_MODE_7BIT,
    I2C_ADDR_MODE_10BIT,
} i2c_addressing_mode;

typedef enum {
    I2C_ADDR2_NO_MASK = 0U,
    I2C_ADDR2_MASK01 = 1U,
    I2C_ADDR2_MASK02 = 2U,
    I2C_ADDR2_MASK03 = 3U,
    I2C_ADDR2_MASK04 = 4U,
    I2C_ADDR2_MASK05 = 5U,
    I2C_ADDR2_MASK06 = 6U,
    I2C_ADDR2_MASK07 = 7U,
} i2c_address2_mask;

typedef struct {

    i2c_t *instance;

    uint32_t timing;

    i2c_addressing_mode addressing_mode;

    // addressing_mode -> address1 : 7b || address1 : 10b
    uint16_t address1;

    bool dual_addressing_enable;

    // enable_dual_addressing -> address2
    uint8_t address2;

    // enable_dual_addressing -> address2_mask
    i2c_address2_mask address2_mask;

    bool enable_general_call;

    bool enable_no_stretch;

} i2c_init_t;

typedef enum {
    HAL_I2C_STATE_RESET = 0x00U,
    HAL_I2C_STATE_READY = 0x20U,
    HAL_I2C_STATE_BUSY = 0x24U,
    HAL_I2C_STATE_BUSY_TX = 0x21U,
    HAL_I2C_STATE_BUSY_RX = 0x22U,
    HAL_I2C_STATE_LISTEN = 0x28U,
    HAL_I2C_STATE_BUSY_TX_LISTEN = 0x29U,
    HAL_I2C_STATE_BUSY_RX_LISTEN = 0x2AU,
    HAL_I2C_STATE_ABORT = 0x60U,
} i2c_state;

typedef enum {
    I2C_MODE_NONE = 0x00U,
    I2C_MODE_MASTER = 0x10U,
    I2C_MODE_SLAVE = 0x20U,
    I2C_MODE_MEM = 0x30U,
} i2c_mode;

typedef enum {
    HAL_I2C_ERROR_NONE = 0x00000000U,
    HAL_I2C_ERROR_BERR = 0x00000001U,
    HAL_I2C_ERROR_ARLO = 0x00000002U,
    HAL_I2C_ERROR_AF = 0x00000004U,
    HAL_I2C_ERROR_OVR = 0x00000008U,
    HAL_I2C_ERROR_DMA = 0x00000010U,
    HAL_I2C_ERROR_TIMEOUT = 0x00000020U,
    HAL_I2C_ERROR_SIZE = 0x00000040U,
    HAL_I2C_ERROR_DMA_PARAM = 0x00000080U,
    HAL_I2C_ERROR_INVALID_PARAM = 0x00000200U,
} i2c_error;

typedef struct {

    i2c_t *instance;

    i2c_init_t init;

    i2c_mode mode;

    i2c_state state;
    i2c_error error;

    bool lock;

} i2c_handle_t;

hal_err i2c_init(i2c_init_t *init);

i2c_handle_t *i2c_get_handle();

#endif // HAL_I2C_H
