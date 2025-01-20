#include "keyboard.h"
#include "mux.h"
#include <stdlib.h>

unsigned key_threshold = KB_KEY_THRESHOLD_DEFAULT;

void kb_init() { mux_init(); }

int8_t kb_poll(kb_key_t **pressed) {

    kb_key_t *pressed_keys = malloc(sizeof(kb_key_t) * 36);

    if (!pressed_keys) {
        return -1;
    }

    uint8_t pressed_amount = 0;

    for (uint8_t i = 0; i < 3; i++) {

        mux_select(i);

        for (uint8_t j = 0; j < 12; j++) {

            mux_select_chan(j);

            uint8_t value = mux_read();

            if (value < key_threshold) {
                continue;
            }

            uint8_t num = (uint8_t)(i * 12) + j;

            pressed_keys[pressed_amount++] =
                (kb_key_t){.num = num, .value = value};
        }
    }

    if (pressed_amount == 0) {
        free(pressed_keys);
        return 0;
    }

    pressed_keys = realloc(pressed_keys, sizeof(kb_key_t) * pressed_amount);

    *pressed = pressed_keys;

    return (int8_t)pressed_amount;
}
