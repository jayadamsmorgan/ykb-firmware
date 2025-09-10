#ifndef LED_COLOR_H
#define LED_COLOR_H

#include <stdint.h>

typedef struct {
    uint8_t r, g, b, w;
} color_t;

#define RGB(R, G, B) {.r = R, .g = G, .b = B}

#define RGBW(R, G, B, W) {.r = R, .g = G, .b = B, .w = W}

#endif // LED_COLOR_H
