#include "keyboard.h"

#include "adc.h"
#include "error_handler.h"
#include "hal/adc.h"
#include "hal/gpio.h"
#include "hal/hal_err.h"
#include "hal/systick.h"
#include "logging.h"
#include "mappings.h"
#include "mux.h"
#include "pinout.h"
#include "settings.h"
#include "usb/usbd_hid.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

//
// KB

static kb_state_t kb_state = {
    .mode = KB_MODE_NORMAL,
    .key_threshold = KB_KEY_THRESHOLD_DEFAULT,
    .key_amount = KB_KEY_COUNT,
    .adc_sampling_time = KB_ADC_SAMPLING_DEFAULT,
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
    LOG_INFO("KB: Loading mappings...");
    // TODO
    // kb_state.mappings = ... (load saved from flash)
    // if failure, use default:
    kb_state.mappings = mappings;
    LOG_INFO("KB: Mappings loaded.");
}

hal_err kb_init() {

    LOG_INFO("KB: Initializing...");

    hal_err err;

    // Init MUXes:
    for (uint8_t i = 0; i < 3; i++) {
        mux_t mux = muxes[i];

        LOG_TRACE("KB: Initializing MUX%d...", i + 1);
        err = mux_init(&mux);
        if (err) {
            LOG_CRITICAL("KB: Error initializing MUX%d: %d", i + 1, err);
            return err;
        }
        LOG_TRACE("KB: MUX%d init OK.", i + 1);

        gpio_turn_on_port(mux.common.gpio);
        gpio_set_mode(mux.common, GPIO_MODE_ANALOG);

        LOG_TRACE("KB: MUX%d initialized.", i + 1);

        LOG_TRACE("KB: Setting up ADC for MUX%d...", i + 1);

        if (mux.common.adc_chan == ADC_CHANNEL_NONE) {
            // The selected common pin does not have an ADC channel
            LOG_CRITICAL(
                "KB: Common pin for MUX%d does not have an ADC channel.",
                i + 1);
            return ERR_KB_COMMON_NO_ADC_CHAN;
        }

        LOG_TRACE("KB: ADC for MUX%d initialized.", i + 1);

        LOG_INFO("KB: MUX%d setup complete.", i + 1);
    }

    // Load saved
    kb_load_mappings();

    LOG_INFO("KB: Setup complete.");

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

    LOG_DEBUG("KB: New mode set: %d.", new_mode);

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

    LOG_DEBUG("KB: New threshold set: %d.", threshold);

    return 0;
}

static inline bool kb_key_pressed_by_threshold(mux_t *mux, uint8_t channel,
                                               uint16_t *const value) {
    mux_select_channel(mux, channel);

    hal_err err = adc_start();
    if (err) {
        LOG_CRITICAL("KB: ADC read error %d", err);
        ERR_H(err);
    }

    while (adc_conversion_ongoing_regular()) {
    }

    uint32_t tmp_value = adc_get_value();

    if (value) {
        *value = tmp_value;
    }

    return tmp_value >= kb_state.key_threshold;
}

static inline uint8_t kb_key_code_by_position(uint8_t mux_number,
                                              uint8_t position) {
    switch (mux_number) {
    case 0:
        return mappings[position];
    case 1:
        return mappings[MUX1_KEY_COUNT + position];
    case 2:
        return mappings[MUX1_KEY_COUNT + MUX2_KEY_COUNT + position];
    }
    return 0;
}

static uint8_t hid_buff[8];

void kb_poll_normal() {

    memset(hid_buff, 0, sizeof(hid_buff));

    uint8_t pressed_amount = 0;

    for (uint8_t i = 0; i < 3; i++) {

        mux_t *mux = &muxes[i];

        adc_channel_config_t channel_config;
        channel_config.mode = ADC_CHANNEL_SINGLE_ENDED;
        channel_config.rank = ADC_CHANNEL_RANK_1;
        channel_config.offset_type = ADC_CHANNEL_OFFSET_NONE;
        channel_config.offset = 0;
        channel_config.channel = mux->common.adc_chan;
        channel_config.sampling_time = KB_ADC_SAMPLING_DEFAULT;
        hal_err err = adc_config_channel(&channel_config);
        if (err) {
            LOG_CRITICAL("KB: Unable to config ADC channel: Error %d", err);
            ERR_H(err);
        }

        for (uint8_t j = 0; j < mux->channel_amount; j++) {
            if (kb_key_pressed_by_threshold(mux, j, NULL)) {
                uint8_t key = kb_key_code_by_position(i, j);
                LOG_DEBUG("KB: Key with HID code %d pressed. MUX%d, CH%d", key,
                          i, j);
                hid_buff[2 + pressed_amount] = key;
                pressed_amount++;
                if (pressed_amount >= 6) {
                    return;
                }
            }
        }
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
