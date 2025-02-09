// #include "boot0_handler.h"
// #include "clock.h"
// #include "error_handler.h"
// #include "hal/gpio.h"
// #include "hal/hal.h"
// #include "stm32wbxx.h"
//
// int main(void) {
//
//     system_init();
//
//     setup_error_handler();
//
//     ERR_H(setup_clock());
//
//     ERR_H(hal_init());
//
//     ERR_H(setup_boot0_handler());
//
//     gpio_turn_on_port(GPIOB);
//     gpio_set_mode(PB1, GPIO_MODE_OUTPUT);
//
//     while (true) {
//     }
// }

#include "boot0_handler.h"
#include "clock.h"
#include "crs.h"
#include "error_handler.h"
#include "hal/gpio.h"
#include "hal/hal.h"
#include "test_button_handler.h"
#include "usb.h"

int main(void) {
    system_init();

    setup_error_handler();

    ERR_H(setup_clock());

    ERR_H(setup_crs());

    ERR_H(hal_init());

    ERR_H(setup_boot0_handler());

    ERR_H(setup_usb());

    ERR_H(setup_test_button_handler());

    gpio_turn_on_port(PB1.gpio);
    gpio_set_mode(PB1, GPIO_MODE_OUTPUT);

    while (1) {
        __WFI();
    }
}
