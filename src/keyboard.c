#include "keyboard.h"

#include "mux.h"
#include "pinout.h"
#include "settings.h"

//
// KB Settings
static uint16_t key_threshold = KB_KEY_THRESHOLD_DEFAULT;

static kb_mode mode = KB_MODE_NORMAL;

//
// MUXes
static mux_t mux1 = {.ctrl = {PIN_MUX1_A, PIN_MUX1_B, PIN_MUX1_C, PIN_MUX1_D},
                     .common = PIN_MUX1_IN};

static mux_t mux2 = {.ctrl = {PIN_MUX2_A, PIN_MUX2_B, PIN_MUX2_C, PIN_MUX2_D},
                     .common = PIN_MUX2_IN};

static mux_t mux3 = {.ctrl = {PIN_MUX3_A, PIN_MUX3_B, PIN_MUX3_C, PIN_MUX3_D},
                     .common = PIN_MUX3_IN};

void kb_init() {
    mux_init(&mux1);
    mux_init(&mux2);
    mux_init(&mux3);
}

kb_mode kb_get_mode() { return mode; }

void kb_set_mode(kb_mode new_mode) { mode = new_mode; }

uint32_t kb_get_min_threshold() { return key_threshold; }

void kb_set_min_threshold(uint32_t threshold) { key_threshold = threshold; }

static inline error_t kb_key_pressed_by_threshold(const mux_t *const mux,
                                                  uint8_t channel,
                                                  uint16_t *const value) {
    mux_select_chan(mux, channel);
    int32_t mux_value = mux_read(mux);
    if (mux_value < 0) {
        // TODO: error occured, debug it to serial
        return mux_value;
    }
    if (value) {
        *value = mux_value;
    }
    return mux_value > key_threshold;
}

void kb_poll_race(kb_key_t *pressed_key) {
    if (!pressed_key) {
        return;
    }
    kb_key_t pressed;
    for (uint8_t i = 0; i < MUX1_KEY_COUNT; i++) {
        uint16_t temp;
        if (kb_key_pressed_by_threshold(&mux1, i, &temp) && temp) {
            if (pressed.value < temp) {
                pressed = (kb_key_t){.value = temp, .num = i};
            }
        }
    }
    for (uint8_t i = 0; i < MUX2_KEY_COUNT; i++) {
        uint16_t temp;
        if (kb_key_pressed_by_threshold(&mux2, i, &temp) && temp) {
            if (pressed.value < temp) {
                pressed = (kb_key_t){.value = temp, .num = MUX1_KEY_COUNT + i};
            }
        }
    }
    for (uint8_t i = 0; i < MUX3_KEY_COUNT; i++) {
        uint16_t temp;
        if (kb_key_pressed_by_threshold(&mux1, i, &temp) && temp) {
            if (pressed.value < temp) {
                pressed = (kb_key_t){
                    .value = temp, .num = MUX1_KEY_COUNT + MUX2_KEY_COUNT + i};
            }
        }
    }
    if (pressed.value != 0) {
        *pressed_key = pressed;
    }
}

int8_t kb_poll_normal(kb_key_t **const pressed) {

    if (!pressed) {
        return 0;
    }

    uint8_t pressed_amount = 0;

    for (uint8_t i = 0; i < MUX1_KEY_COUNT; i++) {
        uint16_t val;
        if (kb_key_pressed_by_threshold(&mux1, i, &val) > 0) {

            *pressed[pressed_amount++] = (kb_key_t){.num = i, .value = val};
        }
    }
    for (uint8_t i = 0; i < MUX2_KEY_COUNT; i++) {
        uint16_t val;
        if (kb_key_pressed_by_threshold(&mux2, i, &val) > 0) {
            *pressed[pressed_amount++] =
                (kb_key_t){.num = MUX1_KEY_COUNT + i, .value = val};
        }
    }
    for (uint8_t i = 0; i < MUX1_KEY_COUNT; i++) {
        uint16_t val;
        if (kb_key_pressed_by_threshold(&mux1, i, &val) > 0) {
            *pressed[pressed_amount++] =
                (kb_key_t){.num = MUX1_KEY_COUNT + MUX2_KEY_COUNT + i,
                           .value = key_threshold};
        }
    }

    return (int8_t)pressed_amount;
}
