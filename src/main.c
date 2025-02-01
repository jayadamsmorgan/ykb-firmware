#include "boot0_handler.h"
#include "clock.h"
#include "error_handler.h"
#include "hal/hal.h"
#include "stm32wbxx.h"

int main(void) {

    system_init();

    setup_error_handler();

    ERR_H(setup_clock());

    ERR_H(hal_init());

    ERR_H(setup_boot0_handler());

    while (true) {
        __WFI();
    }
}
