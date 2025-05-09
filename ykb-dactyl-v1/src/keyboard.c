#include "keyboard.h"

#include "error_handler.h"
#include "logging.h"
#include "mappings.h"
#include "memory_map.h"
#include "mux.h"
#include "pinout.h"
#include "settings.h"

#include <stdint.h>
#include <string.h>

//
// KB

extern kb_state_t kb_state;

__ALIGN_BEGIN static uint8_t mappings[KB_KEY_COUNT] __ALIGN_END = {

#ifdef LEFT

    // MUX1:

    KEY70, KEY71, KEY72, KEY61, KEY54, KEY50, KEY51, KEY52, KEY53, KEY_NOKEY,
    KEY60,

    // MUX2:

    KEY40, KEY41, KEY42, KEY43, KEY44, KEY30, KEY31, KEY32, KEY33, KEY34, KEY20,
    KEY21, KEY22, KEY23, KEY24,

    // MUX3:

    KEY10, KEY11, KEY12, KEY13, KEY14, KEY00, KEY01, KEY02, KEY03,

#endif // LEFT

#ifdef RIGHT

    // MUX1:

    KEY80, KEY81, KEY82, KEY91, KEY104, KEY100, KEY101, KEY102, KEY103,
    KEY_NOKEY, KEY90,

    // MUX2:

    KEY110, KEY111, KEY112, KEY113, KEY114, KEY120, KEY121, KEY122, KEY123,
    KEY124, KEY130, KEY131, KEY132, KEY133, KEY134,

    // MUX3:

    KEY140, KEY141, KEY142, KEY143, KEY144, KEY150, KEY151, KEY152, KEY153

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

static inline void kb_activate_mux_adc_channel(mux_t *mux) {
    adc_channel_config_t channel_config;
    channel_config.mode = ADC_CHANNEL_SINGLE_ENDED;
    channel_config.rank = ADC_CHANNEL_RANK_1;
    channel_config.offset_type = ADC_CHANNEL_OFFSET_NONE;
    channel_config.offset = 0;
    channel_config.channel = mux->common.adc_chan;
    channel_config.sampling_time = kb_state.settings.adc_sampling_time;
    hal_err err = adc_config_channel(&channel_config);
    if (err) {
        LOG_CRITICAL("Unable to config ADC channel: Error %d", err);
        ERR_H(err);
    }
}

static inline bool kb_key_pressed_by_threshold(uint8_t key_index, mux_t *mux,
                                               uint8_t channel,
                                               uint16_t *const value) {
    mux_select_channel(mux, channel);

    hal_err err = adc_start();
    if (err) {
        LOG_CRITICAL("ADC read error %d", err);
        ERR_H(err);
    }

    while (adc_conversion_ongoing_regular()) {
    }

    uint16_t tmp_value = adc_get_value();

    if (value) {
        *value = tmp_value;
    }

    uint16_t range =
        kb_state.max_thresholds[key_index] - kb_state.min_thresholds[key_index];

    if (range == 0) {
        return false;
    }

    float percentage =
        (float)(tmp_value - kb_state.min_thresholds[key_index]) / range * 100;
    return percentage >= kb_state.key_thresholds[key_index];
}

static hal_err kb_init_muxes() {

    hal_err err;

    for (uint8_t i = 0; i < 3; i++) {
        mux_t mux = muxes[i];

        LOG_TRACE("Initializing MUX%d...", i + 1);
        err = mux_init(&mux);
        if (err) {
            LOG_CRITICAL("Error initializing MUX%d: %d", i + 1, err);
            return err;
        }
        LOG_TRACE("MUX%d init OK.", i + 1);

        gpio_turn_on_port(mux.common.gpio);
        gpio_set_mode(mux.common, GPIO_MODE_ANALOG);

        LOG_TRACE("MUX%d initialized.", i + 1);

        LOG_TRACE("Setting up ADC for MUX%d...", i + 1);

        if (mux.common.adc_chan == ADC_CHANNEL_NONE) {
            // The selected common pin does not have an ADC channel
            LOG_CRITICAL("Common pin for MUX%d does not have an ADC channel.",
                         i + 1);
            return ERR_KB_COMMON_NO_ADC_CHAN;
        }

        LOG_TRACE("ADC for MUX%d initialized.", i + 1);

        LOG_INFO("MUX%d setup complete.", i + 1);
    }

    return OK;
}

static inline void kb_init_default() {

    uint8_t index = 0;

    for (uint8_t i = 0; i < 3; i++) {
        mux_t *mux = &muxes[i];

        kb_activate_mux_adc_channel(mux);

        for (uint8_t j = 0; j < mux->channel_amount; j++) {
            kb_key_pressed_by_threshold(index, mux, j,
                                        &kb_state.min_thresholds[index]);
            index++;
        }
    }

    for (uint8_t i = 0; i < KB_KEY_COUNT; i++) {
        kb_state.max_thresholds[i] = KB_KEY_MAX_VALUE_THRESHOLD_DEFAULT;
    }

    memset(kb_state.key_thresholds, KB_KEY_THRESHOLD_DEFAULT,
           sizeof(kb_state.key_thresholds));

    memcpy(kb_state.mappings, mappings, sizeof(mappings));
}

hal_err kb_init() {

    LOG_INFO("Setting up...");

    kb_super_init();

    hal_err err;

    LOG_TRACE("Initializing MUXes...");
    err = kb_init_muxes();
    if (err) {
        LOG_ERROR("Unable to init muxes. Error %d", err);
        return err;
    }
    LOG_TRACE("MUXes init OK.");

    if (!kb_load_state_from_eeprom()) {
        LOG_DEBUG("Failed to load state from EEPROM. Loading defaults...");
        kb_init_default();
        LOG_DEBUG("Loaded default state.");
    }

    memset(kb_state.current_values, 0, sizeof(kb_state.current_values));

    LOG_INFO("Setup complete.");

    return OK;
}

static mux_t *last_activated_mux = NULL;

#ifdef DEBUG

bool previously_pressed_keys[KB_KEY_COUNT] = {false};

#endif // DEBUG

void kb_poll_normal() {

    uint8_t index = 0;

    for (uint8_t i = 0; i < 3; i++) {

        mux_t *mux = &muxes[i];

        kb_activate_mux_adc_channel(mux);
        last_activated_mux = mux;

        for (uint8_t j = 0; j < mux->channel_amount; j++) {
            if (kb_key_pressed_by_threshold(index, mux, j,
                                            &kb_state.current_values[index])) {
                uint8_t key = kb_state.mappings[index];
#ifdef DEBUG
                if (!previously_pressed_keys[index]) {
                    LOG_DEBUG(
                        "Key with index %d (HID code %d) pressed. MUX%d, CH%d",
                        index, key, i + 1, j);
                    previously_pressed_keys[index] = true;
                }
#endif // DEBUG
                kb_process_key(key);
            }
#ifdef DEBUG
            else {
                previously_pressed_keys[index] = false;
            }
#endif // DEBUG
            index++;
        }
    }
}

void kb_poll_race() {
    uint8_t index = 0;
    uint16_t highest_value = 0;
    uint8_t pressed_key = KEY_NOKEY;

    for (uint8_t i = 0; i < 3; i++) {
        mux_t *mux = &muxes[i];

        kb_activate_mux_adc_channel(mux);
        last_activated_mux = mux;

        for (uint8_t j = 0; j < mux->channel_amount; j++) {
            if (kb_key_pressed_by_threshold(index, mux, j,
                                            &kb_state.current_values[index])) {
                uint8_t key = kb_state.mappings[index];
#ifdef DEBUG
                if (!previously_pressed_keys[index]) {
                    LOG_DEBUG(
                        "Key with index %d (HID code %d) pressed. MUX%d, CH%d",
                        key, i + 1, j);
                    previously_pressed_keys[index] = true;
                }
#endif // DEBUG
                uint16_t value = kb_state.current_values[index];
                if (highest_value < value) {
                    highest_value = value;
                    pressed_key = key;
                }
            }
#ifdef DEBUG
            else {
                previously_pressed_keys[index] = false;
            }
#endif // DEBUG
        }
    }
    if (pressed_key != KEY_NOKEY) {
        kb_process_key(pressed_key);
    }
}
