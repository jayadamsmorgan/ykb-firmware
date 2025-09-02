#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "hal_err.h"

#define ERR_HARDFAULT -102
#define ERR_BUSFAULT -103
#define ERR_USAGEFAULT -104

#define ERR_KB_COMMON_NO_ADC_CHAN -1001

#define ERR_ADC_RB_TIMEOUT -1002

#define ERR_HSEM_LOCK_ARGNULL -1502
#define ERR_HSEM_LOCK_ALREADY_LOCKED -1503
#define ERR_HSEM_UNLOCK_ARGNULL -1504
#define ERR_HSEM_UNLOCK_ALREADY_UNLOCKED -1503
#define ERR_HSEM_RELEASE_ARGNULL -1504

#define ERR_H(ERR_CODE)                                                        \
    do {                                                                       \
        error_handler(ERR_CODE);                                               \
    } while (0)

void setup_error_handler();

void error_handler(hal_err error_code);

#endif // ERROR_HANDLER_H
