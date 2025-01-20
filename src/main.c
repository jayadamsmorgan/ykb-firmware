#include "clock.h"
#include "gpio.h"
#include "systick.h"
#include "uart.h"

int main(void) {

    select_clock_source(HSI);

    systick_init_default();

    gpio_turn_on_port(GPIO_PORT_B);

    uart_init(115200);

    gpio_set_mode(PIN_LED_GREEN,
                  GPIO_MODE_OUTPUT); // Set blue LED to output mode
    gpio_set_mode(PIN_LED_RED,
                  GPIO_MODE_OUTPUT); // Set blue LED to output mode
    gpio_set_mode(PIN_LED_BLUE,
                  GPIO_MODE_OUTPUT); // Set blue LED to output mode

    uint32_t timer = 0, period = 500; // Declare timer and 500ms period

    bool on = false;

    for (;;) {
        if (timer_expired(&timer, period)) {
            uart_write_buf("h\r", 2);
            gpio_write(PIN_LED_RED, on);
            gpio_write(PIN_LED_BLUE, !on);
            gpio_write(PIN_LED_GREEN, on);
            on = !on;
        }
    }
    return 0;
}

// Startup code
__attribute__((naked, noreturn)) void _reset(void) {
    // memset .bss to zero, and copy .data section to RAM region
    extern long _sbss, _ebss, _sdata, _edata, _sidata;
    for (long *dst = &_sbss; dst < &_ebss; dst++)
        *dst = 0;
    for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;)
        *dst++ = *src++;

    main(); // Call main()
    for (;;)
        (void)0; // Infinite loop in the case if main() returns
}

extern void _estack(void); // Defined in link.ld

// 16 standard and 91 STM32-specific handlers
__attribute__((section(".vectors"))) void (*const tab[16 + 91])(void) = {
    _estack, _reset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SysTick_Handler};
