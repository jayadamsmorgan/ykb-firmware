#include "test_button_handler.h"

#include "hal/gpio.h"
#include "hal/systick.h"
#include "stm32wb55xx.h"
#include "stm32wbxx.h"
#include "usbd_def.h"
#include "usbd_hid.h"

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
    static int8_t cnt = 0;
    int8_t x = 0, y = 0;

    if (cnt++ > 0) {
        x = 5;
    } else {
        x = -5;
    }
    pbuf[0] = 0;
    pbuf[1] = x;
    pbuf[2] = y;
    pbuf[3] = 0;
}

extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t HID_Buffer[4];

void EXTI15_10_IRQHandler(void) {
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF10)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF10);
        if ((hUsbDeviceFS.dev_remote_wakeup == 1) &&
            (hUsbDeviceFS.dev_state == USBD_STATE_SUSPENDED)) {
            /* Activate Remote wakeup */
            HAL_PCD_ActivateRemoteWakeup(
                (PCD_HandleTypeDef *)(&hUsbDeviceFS.pData));

            /* Remote wakeup delay */
            systick_delay(10);

            /* Disable Remote wakeup */
            HAL_PCD_DeActivateRemoteWakeup(
                (PCD_HandleTypeDef *)(&(hUsbDeviceFS.pData)));

            /* change state to configured */
            hUsbDeviceFS.dev_state = USBD_STATE_CONFIGURED;

            /* Change remote_wakeup feature to 0 */
            hUsbDeviceFS.dev_remote_wakeup = 0;
        } else if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {
            GetPointerData(HID_Buffer);
            USBD_HID_SendReport(&hUsbDeviceFS, HID_Buffer, 4);
        } else {
            /* ... */
        }
    }
}
