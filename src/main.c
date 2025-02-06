#include "boot0_handler.h"
#include "clock.h"
#include "error_handler.h"
#include "hal/gpio.h"
#include "hal/hal.h"
#include "stm32wbxx.h"
#include "usb.h"

int main(void) {

    system_init();

    setup_error_handler();

    ERR_H(setup_clock());

    ERR_H(hal_init());

    ERR_H(setup_boot0_handler());

    gpio_turn_on_port(GPIOB);
    gpio_set_mode(PB1, GPIO_MODE_OUTPUT);

    ERR_H(usb_init());

    while (true) {
    }
}
