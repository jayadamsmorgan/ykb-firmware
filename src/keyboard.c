#include "keyboard.h"

#include "hal/hal_err.h"
#include "mappings.h"
#include "mux.h"
#include "pinout.h"
#include "settings.h"
#include "usb/usbd_hid.h"
#include <stdint.h>
#include <string.h>

//
// KB

static kb_state_t kb_state = {
    .mode = KB_MODE_NORMAL,
    .key_threshold = KB_KEY_THRESHOLD_DEFAULT,
    .key_amount = KB_KEY_COUNT,
};

__ALIGN_BEGIN static uint8_t mappings[KB_KEY_COUNT] __ALIGN_END = {

#ifdef LEFT

    KEY00,  KEY01,  KEY02,  KEY03,

    KEY10,  KEY11,  KEY12,  KEY13,  KEY14,

    KEY20,  KEY21,  KEY22,  KEY23,  KEY24,

    KEY30,  KEY31,  KEY32,  KEY33,  KEY34,

    KEY40,  KEY41,  KEY42,  KEY43,  KEY44,

    KEY50,  KEY51,  KEY52,  KEY53,  KEY54,

    KEY60,  KEY61,

    KEY70,  KEY71,  KEY72,

#endif // LEFT

#ifdef RIGHT

    KEY80,  KEY81,  KEY82,

    KEY90,  KEY91,

    KEY100, KEY101, KEY102, KEY103, KEY104,

    KEY110, KEY111, KEY112, KEY113, KEY114,

    KEY120, KEY121, KEY122, KEY123, KEY124,

    KEY130, KEY131, KEY132, KEY133, KEY134,

    KEY140, KEY141, KEY142, KEY143, KEY144,

    KEY150, KEY151, KEY152, KEY153,

#endif // RIGHT

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
    (mux_t){
        .ctrls = mux_1_ctrls,             //
        .common = PIN_MUX1_CMN,           //
        .ctrls_amount = 4,                //
        .channel_amount = MUX1_KEY_COUNT, //
    },                                    //
    (mux_t){
        .ctrls = mux_2_ctrls,             //
        .common = PIN_MUX2_CMN,           //
        .channel_amount = MUX2_KEY_COUNT, //
        .ctrls_amount = 4                 //
    },                                    //
    (mux_t){
        .ctrls = mux_3_ctrls,             //
        .common = PIN_MUX3_CMN,           //
        .channel_amount = MUX3_KEY_COUNT, //
        .ctrls_amount = 4                 //
    },                                    //
};

static inline void kb_load_mappings() {
    // TODO
    // kb_state.mappings = ... (load saved from flash)
    // if failure, use default:
    kb_state.mappings = mappings;
}

hal_err kb_init() {

    hal_err err;

    // Init ADC:
    err = gpio_adc_calibrate(GPIO_CALIB_INPUT_SINGLE_ENDED, NULL);
    if (err) {
        return err;
    }
    gpio_adc_start();
    err = gpio_adc_set_resolution(GPIO_ADC_RES_10BIT);
    if (err) {
        return err;
    }

    // Init MUXes:
    for (uint8_t i = 0; i < 3; i++) {
        mux_t mux = muxes[i];

        err = mux_init(&mux);
        if (err) {
            return err;
        }

        err = gpio_adc_set_sampling_time(mux.common, GPIO_ADC_SMP_2_5_CYCLES);
        if (err) {
            return err;
        }
    }

    // Load saved
    kb_load_mappings();

    return OK;
}

error_t kb_get_mode(kb_mode *const mode, bool blocking) {

    if (blocking) {
        while (kb_state.lock) {
        }
    } else if (kb_state.lock) {
        return -1;
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
    } else if (kb_state.lock) {
        return -1;
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
    } else if (kb_state.lock) {
        return -1;
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
    } else if (kb_state.lock) {
        return -1;
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
    return mux_value >= kb_state.key_threshold;
}

static uint8_t hid_buff[8];

void kb_poll_normal() {

    memset(hid_buff, 0, sizeof(hid_buff));

    uint8_t pressed_amount = 2;
    uint8_t index = 0;

    for (uint8_t i = 0; i < 3; i++) {

        mux_t *mux = &muxes[i];

        for (uint8_t j = 0; j < mux->channel_amount; j++) {

            hal_err err = kb_key_pressed_by_threshold(mux, i, NULL);

#if DEBUG
            if (err < 0) {
                // Error occured
                // TODO
            }
#endif // DEBUG
            if (err > 0) {
                // Key press occured
                hid_buff[pressed_amount++] = kb_state.mappings[index + j];
            }
        }

        index += mux->channel_amount;
    }
}

void kb_poll_race() {
    // TODO
}

extern USBD_HandleTypeDef hUsbDeviceFS;

void kb_handle(bool blocking) {

    if (!blocking && kb_state.lock) {
        return;
    }
    while (kb_state.lock) {
    }

    switch (kb_state.mode) {

    case KB_MODE_NORMAL:
        kb_poll_normal();
        break;

    case KB_MODE_RACE:
        kb_poll_race();
        break;
    }

    USBD_HID_SendReport(&hUsbDeviceFS, hid_buff, 8);
}
