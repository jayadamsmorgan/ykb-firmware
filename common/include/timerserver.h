#ifndef TIMERSERVER_H
#define TIMERSERVER_H

#include "hal_err.h"

#include <stdint.h>

#define ERR_TS_INIT_UNKNOWN_MODE -2100

typedef enum {
    TS_INIT_MODE_FULL = 0U,
    TS_INIT_MODE_LIMITED = 1U,
} timerserver_init_mode;

typedef struct {

    timerserver_init_mode mode;

    uint8_t timers_amount;

} timerserver_init_t;

hal_err timerserver_init(timerserver_init_t init);

#endif // TIMERSERVER_H
