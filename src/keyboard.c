#include "keyboard.h"

#include "mux.h"
#include "pinout.h"
#include "settings.h"
#include "utils/utils.h"

//
// KB state

#ifdef RIGHT
__ALIGN_BEGIN static uint8_t default_mappings[KB_KEY_COUNT] __ALIGN_END = {
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
};
#else  // LEFT
__ALIGN_BEGIN static uint8_t default_mappings[KB_KEY_COUNT] __ALIGN_END = {
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
    0x04, //
    0x05, //
};
#endif // RIGHT

typedef struct {

    bool lock;

    uint16_t key_threshold;
    kb_mode mode;

    uint16_t values[KB_KEY_COUNT];
    uint8_t *mappings;

    const uint8_t key_amount;

} kb_state_t;

static kb_state_t kb_state = {
    .mode = KB_MODE_NORMAL,
    .key_threshold = KB_KEY_THRESHOLD_DEFAULT,
    .key_amount = KB_KEY_COUNT,
};

//
// MUXes

static gpio_pin_t mux_1_ctrls[4] = {
    PIN_MUX1_A, //
    PIN_MUX1_B, //
    PIN_MUX1_C, //
    PIN_MUX1_D, //
};
static gpio_pin_t mux_2_ctrls[4] = {
    PIN_MUX2_A, //
    PIN_MUX2_B, //
    PIN_MUX2_C, //
    PIN_MUX2_D, //
};
static gpio_pin_t mux_3_ctrls[4] = {
    PIN_MUX3_A, //
    PIN_MUX3_B, //
    PIN_MUX3_C, //
    PIN_MUX3_D, //
};

static mux_t muxes[3] = {
    (mux_t)                 //
    {.ctrls = mux_1_ctrls,  //
     .common = PIN_MUX1_IN, //
     .ctrls_amount = 4},    //
    (mux_t)                 //
    {.ctrls = mux_2_ctrls,  //
     .common = PIN_MUX2_IN, //
     .ctrls_amount = 4},    //
    (mux_t)                 //
    {.ctrls = mux_3_ctrls,  //
     .common = PIN_MUX3_IN, //
     .ctrls_amount = 4},    //
};

hal_err kb_init() {

    hal_err err;

    err = gpio_adc_calibrate(GPIO_CALIB_INPUT_SINGLE_ENDED, NULL);
    if (err) {
        return err;
    }

    gpio_adc_start();

    err = gpio_adc_set_resolution(GPIO_ADC_RES_10BIT);
    if (err) {
        return err;
    }

    for (uint8_t i = 0; i < 3; i++) {
        mux_t mux = muxes[i];

        err = mux_init(&mux);
        if (err) {
            return err;
        }

        gpio_adc_set_sampling_time(mux.common, GPIO_ADC_SMP_2_5_CYCLES);
    }

    return OK;
}

error_t kb_get_mode(kb_mode *const mode, bool blocking) {

    if (blocking) {
        while (kb_state.lock) {
        }
    } else {
        if (kb_state.lock) {
            return -1;
        }
    }
    if (!mode) {
        return -2;
    }

    kb_state.lock = true;
    *mode = kb_state.mode;
    kb_state.lock = false;

    return 0;
}

error_t kb_set_mode(kb_mode new_mode, bool blocking) {

    if (blocking) {
        while (kb_state.lock) {
        }
    } else {
        if (kb_state.lock) {
            return -1;
        }
    }

    kb_state.lock = true;
    kb_state.mode = new_mode;
    kb_state.lock = false;

    return 0;
}

error_t kb_get_min_threshold(uint32_t *threshold, bool blocking) {

    if (blocking) {
        while (kb_state.lock) {
        }
    } else {
        if (kb_state.lock) {
            return -1;
        }
    }

    if (!threshold) {
        return -2;
    }

    *threshold = kb_state.key_threshold;

    return 0;
}

error_t kb_set_min_threshold(uint32_t threshold, bool blocking) {

    if (blocking) {
        while (kb_state.lock) {
        }
    } else {
        if (kb_state.lock) {
            return -1;
        }
    }

    kb_state.lock = true;
    kb_state.key_threshold = threshold;
    kb_state.lock = false;

    return 0;
}

static inline hal_err kb_key_pressed_by_threshold(mux_t *mux, uint8_t channel,
                                                  uint16_t *const value) {
    mux_select_channel(mux, channel);
    uint32_t mux_value;
    hal_err err = gpio_analog_read(mux->common, &mux_value);
    if (err) {
#ifdef DEBUG
        // TODO: error occured, debug it to serial
#endif // DEBUG
        return err;
    }
    if (value) {
        *value = mux_value;
    }
    return mux_value > kb_state.key_threshold;
}

void kb_poll_race(kb_key_t *pressed_key) {
    if (!pressed_key) {
        return;
    }
    kb_key_t pressed;
    for (uint8_t i = 0; i < MUX1_KEY_COUNT; i++) {
        uint16_t temp;
        if (kb_key_pressed_by_threshold(&muxes[0], i, &temp) && temp) {
            if (pressed.value < temp) {
                pressed = (kb_key_t){.value = temp, .num = i};
            }
        }
    }
    for (uint8_t i = 0; i < MUX2_KEY_COUNT; i++) {
        uint16_t temp;
        if (kb_key_pressed_by_threshold(&muxes[1], i, &temp) && temp) {
            if (pressed.value < temp) {
                pressed = (kb_key_t){.value = temp, .num = MUX1_KEY_COUNT + i};
            }
        }
    }
    for (uint8_t i = 0; i < MUX3_KEY_COUNT; i++) {
        uint16_t temp;
        if (kb_key_pressed_by_threshold(&muxes[2], i, &temp) && temp) {
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
        if (kb_key_pressed_by_threshold(&muxes[0], i, &val) > 0) {

            *pressed[pressed_amount++] = (kb_key_t){.num = i, .value = val};
        }
    }
    for (uint8_t i = 0; i < MUX2_KEY_COUNT; i++) {
        uint16_t val;
        if (kb_key_pressed_by_threshold(&muxes[1], i, &val) > 0) {
            *pressed[pressed_amount++] =
                (kb_key_t){.num = MUX1_KEY_COUNT + i, .value = val};
        }
    }
    for (uint8_t i = 0; i < MUX1_KEY_COUNT; i++) {
        uint16_t val;
        if (kb_key_pressed_by_threshold(&muxes[2], i, &val) > 0) {
            *pressed[pressed_amount++] = (kb_key_t){
                .num = MUX1_KEY_COUNT + MUX2_KEY_COUNT + i, .value = val};
        }
    }

    return (int8_t)pressed_amount;
}
