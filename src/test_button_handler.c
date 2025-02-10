#include "test_button_handler.h"

#include "hal/gpio.h"
#include "hal/pcd.h"
#include "hal/systick.h"
#include "stm32wbxx.h"
#include "usb/usbd_def.h"
#include "usb/usbd_hid.h"
#include <stdint.h>

gpio_pin_t button = PA10;

hal_err setup_test_button_handler() {
    gpio_turn_on_port(button.gpio);

    gpio_set_mode(button, GPIO_MODE_INPUT);
    gpio_set_pupd(button, GPIO_PULLUP);

    hal_err err = gpio_set_interrupt_falling(button);
    if (err) {
        return err;
    }

    return gpio_enable_interrupt(button);
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
    if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {
        GetPointerData(HID_Buffer);
        USBD_HID_SendReport(&hUsbDeviceFS, HID_Buffer, 8);
        systick_delay(20);
        USBD_HID_SendReport(&hUsbDeviceFS, HID_empty, 8);
    }
}
