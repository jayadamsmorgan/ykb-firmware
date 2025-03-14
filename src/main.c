#include "adc.h"
#include "boot0_handler.h"
#include "clock.h"
#include "crs.h"
#include "error_handler.h"
#include "hal/adc.h"
#include "hal/gpio.h"
#include "hal/hal.h"
#include "hal/systick.h"
#include "keyboard.h"
#include "logging.h"
#include "test_button_handler.h"
#include "usb.h"
#include "version.h"
#include <stdint.h>

int main(void) {

    // Base
    LOG_INFO("Start booting...");
    system_init();
    LOG_TRACE("CORE: System init OK.");
    setup_error_handler();
    ERR_H(setup_clock());
    ERR_H(setup_crs());
    ERR_H(hal_init());
    LOG_TRACE("CORE: HAL init OK.");
    ERR_H(setup_logging());

    // Misc
    ERR_H(setup_boot0_handler());
    ERR_H(setup_test_button_handler());

    // Main
    // ERR_H(kb_init());
    ERR_H(setup_adc());
    // ERR_H(setup_usb());
    adc_channel_config_t channel_config;

    gpio_set_mode(PA5, GPIO_MODE_ANALOG);
    gpio_turn_on_port(PA5.gpio);

    channel_config.channel = PA5.adc_chan;
    channel_config.mode = ADC_CHANNEL_SINGLE_ENDED;
    channel_config.rank = ADC_CHANNEL_RANK_2;
    channel_config.sampling_time = ADC_SMP_92_5_CYCLES;
    channel_config.offset_type = ADC_CHANNEL_OFFSET_NONE;
    channel_config.offset = 0;

    hal_err err = adc_config_channel(&channel_config);
    if (err) {
        LOG_CRITICAL("TEST: Unable to config ADC channel: %d", err);
        return err;
    }
    ERR_H(setup_tempsensor());

    LOG_INFO("MAIN: Successfully booted.");

    // while (true) { // Main loop
    //     // kb_handle(true);
    // }
}
