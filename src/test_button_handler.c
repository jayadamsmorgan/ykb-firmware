#include "test_button_handler.h"

#include "hal/gpio.h"
#include "hal/pcd.h"
#include "hal/systick.h"
#include "logging.h"
#include "stm32wbxx.h"
#include "usb/usbd_def.h"
#include "usb/usbd_hid.h"
#include <stdint.h>

gpio_pin_t button = PA10;

hal_err setup_test_button_handler() {

    LOG_INFO("TEST_BTN_HANDLER: Setting up...");

    LOG_DEBUG("TEST_BTN_HANDLER: Button port address: %d", button.gpio);
    LOG_DEBUG("TEST_BTN_HANDLER: Button number: %d", button.num);
    gpio_turn_on_port(button.gpio);

    gpio_set_mode(button, GPIO_MODE_INPUT);
    gpio_set_pupd(button, GPIO_PULLUP);

    hal_err err;

    LOG_TRACE("TEST_BTN_HANDLER: Setting interrupt...");
    err = gpio_set_interrupt_falling(button);
    if (err) {
        LOG_CRITICAL("TEST_BTN_HANDLER: Unable to set interrupt: Error %d",
                     err);
        return err;
    }
    LOG_TRACE("TEST_BTN_HANDLER: Interrupt is set.");

    LOG_TRACE("TEST_BTN_HANDLER: Enabling interrupt...");
    err = gpio_enable_interrupt(button);
    if (err) {
        LOG_CRITICAL("TEST_BTN_HANDLER: Unable to enable interrupt: Error %d",
                     err);
        return err;
    }
    LOG_TRACE("TEST_BTN_HANDLER: Interrupt enabled.");

    LOG_INFO("TEST_BTN_HANDLER: Setup complete.");
    return OK;
}

void GetPointerData(uint8_t *pbuf) {
    if (pbuf[2] < 0x04) {
        pbuf[2] = 0x04;
    } else {
        pbuf[2]++;
    }
}

extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t HID_Buffer[8];
uint8_t HID_empty[8] = {0};

void exti_handler_10() {
    LOG_DEBUG("TEST_BTN_HANDLER: Triggered.");
    if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {
        GetPointerData(HID_Buffer);
        USBD_HID_SendReport(&hUsbDeviceFS, HID_Buffer, 8);
        systick_delay(20);
        USBD_HID_SendReport(&hUsbDeviceFS, HID_empty, 8);
        LOG_TRACE("TEST_BTN_HANDLER: Report sent.");
    }
}
