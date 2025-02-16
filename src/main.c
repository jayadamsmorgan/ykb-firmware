#include "boot0_handler.h"
#include "clock.h"
#include "crs.h"
#include "error_handler.h"
#include "hal/gpio.h"
#include "hal/hal.h"
#include "keyboard.h"
#include "test_button_handler.h"
#include "usb.h"

int main(void) {
    system_init();

    setup_error_handler();

    ERR_H(setup_clock());

    ERR_H(setup_crs());

    ERR_H(hal_init());

    ERR_H(setup_boot0_handler());

    gpio_turn_on_port(PB1.gpio);
    gpio_set_mode(PB1, GPIO_MODE_OUTPUT);

    ERR_H(setup_usb());

    ERR_H(setup_test_button_handler());

    while (true) {
        kb_handle(true);
    }
}
