#include "clock.h"
#include "hal/flash.h"
#include "hal/gpio.h"

gpio_pin_t green_led = PB0;
gpio_pin_t red_led = PB1;

int main(void) {

    SystemInit();

    setup_clock();

    gpio_turn_on_port(GPIOB);

    gpio_set_mode(green_led, GPIO_MODE_OUTPUT);
    gpio_set_mode(red_led, GPIO_MODE_OUTPUT);

    if (SystemCoreClock == 64000000) {
        gpio_digital_write(green_led, HIGH);
    } else {
        gpio_digital_write(red_led, HIGH);
    }

    return 0;
}
