#include "backlight.h"

#include "hal_gpio.h"

#define BACKLIGHT_ENABLED 1

#define BACKLIGHT_ARGB_ENABLED 1
#define PIN_BACKLIGHT_ARGB_DO PC2

#define BACKLIGHT_RGB_ENABLED 1
#define PIN_BACKLIGHT_RGB_R PB7
#define PIN_BACKLIGHT_RGB_G PB6
#define PIN_BACKLIGHT_RGB_B PB5

#define BACKLIGHT_DEFAULT_COLOR RGB(190, 0, 255)

hal_err setup_backlight() {
    rgb_color_t rgb = RGB(2, 3, 55);
    return OK;
}

void backlight_handle() {}
