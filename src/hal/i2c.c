#include "hal/i2c.h"
#include "hal/hal_err.h"

static volatile i2c_handle_t hal_i2c_handle;

static inline void i2c_init_msp(volatile i2c_handle_t *handle) {
    //
    handle->lock = false;
}

hal_err i2c_init(i2c_init_t *init) {

    if (!init) {
        return ERR_I2C_INIT_ARGNULL;
    }

    if (!init->instance) {
        return ERR_I2C_INIT_INSTANCENULL;
    }

    volatile i2c_handle_t *handle = &hal_i2c_handle;

    hal_i2c_handle.init = *init;

    if (handle->state == HAL_I2C_STATE_RESET) {
        i2c_init_msp(handle);
    }

    return OK;
}

i2c_handle_t *i2c_get_handle() { return (i2c_handle_t *)&hal_i2c_handle; }
