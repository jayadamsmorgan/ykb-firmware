#include "keyboard.h"

#include "eeprom.h"
#include "error_handler.h"
#include "hal/systick.h"
#include "interface_handler.h"
#include "logging.h"
#include "mappings.h"
#include "memory_map.h"
#include "mux.h"
#include "pinout.h"
#include "settings.h"
#include "usb/usbd_hid.h"
#include "ykb_protocol.h"

#include <stdint.h>
#include <string.h>

//
// KB

static kb_state_t kb_state = {
    .settings =
        {
            .mode = KB_MODE_NORMAL,
            .adc_sampling_time = KB_ADC_SAMPLING_DEFAULT,
            .key_polling_rate = KB_DEFAULT_POLLING_RATE,
        },
};

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

typedef struct PACKED {

    kb_state_t state;

    uint16_t crc16;

} kb_eeprom_t;

#define EEPROM_KB_STATE_ADDR EEPROM_START_ADDRESS

static inline void kb_save_to_eeprom() {

    hal_err err;

    kb_eeprom_t to_save = {
        .state = kb_state,
        .crc16 = ykb_crc16((uint8_t *)&kb_state, sizeof(kb_state_t))};

    err = eeprom_clear();
    if (err) {
        LOG_ERROR("Unable to clear EEPROM: %d", err);
        return;
    }
    LOG_TRACE("EEPROM cleared.");

    err = eeprom_save(EEPROM_KB_STATE_ADDR, &to_save, sizeof(to_save));
    if (err) {
        LOG_ERROR("Unable to save to EEPROM: %d", err);
        return;
    }

    LOG_DEBUG("Successfully saved to EEPROM.");
}

static inline bool kb_load_state_from_eeprom() {

    hal_err err;

    kb_eeprom_t to_get = {0};

    LOG_TRACE("Loading state from EEPROM...");
    err = eeprom_get(EEPROM_KB_STATE_ADDR, &to_get, sizeof(to_get));
    if (err) {
        LOG_ERROR("Unable to get from EEPROM: %d", err);
        return false;
    }
    LOG_TRACE("Loaded state from EEPROM.");

    uint16_t crc16 = ykb_crc16((uint8_t *)&to_get.state, sizeof(kb_state_t));
    if (crc16 != to_get.crc16) {
        LOG_DEBUG("Wrong CRC16 EEPROM signature (%d != %d), aborting", crc16,
                  to_get.crc16);
        return false;
    }
    LOG_TRACE("EEPROM CRC16 is correct.");

    memcpy(&kb_state, &to_get.state, sizeof(kb_state_t));

    LOG_DEBUG("Successfully retreived kb_state from EEPROM.");

    return true;
}

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

    hal_err err;

    LOG_TRACE("Initializing MUXes...");
    err = kb_init_muxes();
    if (err) {
        LOG_ERROR("Unable to init muxes. Error %d", err);
        return err;
    }
    LOG_TRACE("MUXes init OK.");

    LOG_TRACE("Initializing EEPROM...");
    err = eeprom_init();
    if (err) {
        LOG_ERROR("Unable to init EEPROM. Error: %d", err);
        return err;
    }
    LOG_TRACE("EEPROM initialized.");

    if (!kb_load_state_from_eeprom()) {
        LOG_DEBUG("Failed to load state from EEPROM. Loading defaults...");
        kb_init_default();
        LOG_DEBUG("Loaded default state.");
    }

    memset(kb_state.current_values, 0, sizeof(kb_state.current_values));

    LOG_INFO("Setup complete.");

    return OK;
}

static uint8_t hid_buff[HID_BUFFER_SIZE];
static uint8_t pressed_amount = 0;

static uint8_t fn_buff[HID_BUFFER_SIZE - 2];
static uint8_t fn_pressed_amount = 0;
static bool fn_pressed;

static uint8_t modifier_map[8] = {0x01, 0x02, 0x04, 0x08,
                                  0x10, 0x20, 0x40, 0x80};

static inline void kb_process_key(uint8_t key) {

    if (key == KEY_FN) {
        fn_pressed = true;
        return;
    }

    if (key == KEY_LAYER) {
        // TODO
        return;
    }

    if (fn_pressed && fn_pressed_amount < HID_BUFFER_SIZE - 2) {

        fn_buff[fn_pressed_amount] = key;
        fn_pressed_amount++;

        return;
    }

    if (key < KEY_LEFTCONTROL) {
        // Regular

        hid_buff[2 + pressed_amount] = key;
        pressed_amount++;

        return;
    }

    if (key < KEY_FN) {
        // Modifiers

        hid_buff[0] |= modifier_map[key - KEY_LEFTCONTROL];
        return;
    }
}

static inline void kb_process_fn_buff() {
    // TODO

    memset(fn_buff, 0, HID_BUFFER_SIZE - 2);
    fn_pressed = false;
    fn_pressed_amount = 0;
}

static mux_t *last_activated_mux = NULL;

void kb_get_settings(uint8_t *buffer) {
    if (!buffer) {
        return;
    }
    memcpy(buffer, &kb_state.settings, sizeof(kb_settings_t));
}

void kb_get_mappings(uint8_t *buffer) {
    if (!buffer) {
        return;
    }
    memcpy(buffer, kb_state.mappings, sizeof(kb_state.mappings));
}

static ykb_protocol_t values_request;
static ykb_protocol_t *values_request_ptr;

void kb_request_values(ykb_protocol_t *protocol) {
    memcpy(&values_request, protocol, sizeof(ykb_protocol_t));
    values_request_ptr = &values_request;
}

void kb_get_thresholds(uint8_t *buffer) {
    if (!buffer) {
        return;
    }
    memcpy(buffer, kb_state.key_thresholds, sizeof(kb_state.key_thresholds));
    memcpy(&buffer[sizeof(kb_state.key_thresholds)], kb_state.min_thresholds,
           sizeof(kb_state.min_thresholds));
    memcpy(&buffer[sizeof(kb_state.key_thresholds) +
                   sizeof(kb_state.min_thresholds)],
           kb_state.max_thresholds, sizeof(kb_state.max_thresholds));
}

void kb_set_settings(kb_settings_t *new_settings) {
    if (!new_settings) {
        return;
    }
    memcpy(&kb_state.settings, new_settings, sizeof(kb_settings_t));
    kb_save_to_eeprom();
}

void kb_set_mappings(uint8_t *new_mappings) {
    if (!new_mappings) {
        return;
    }
    memcpy(kb_state.mappings, new_mappings, sizeof(kb_state.mappings));
    kb_save_to_eeprom();
}

void kb_set_thresholds(uint8_t *new_thresholds) {
    if (!new_thresholds) {
        return;
    }
    memcpy(kb_state.key_thresholds, new_thresholds,
           sizeof(kb_state.key_thresholds));
    kb_save_to_eeprom();
}

void kb_calibrate(uint16_t *min_thresholds, uint16_t *max_thresholds) {
    if (!min_thresholds || !max_thresholds) {
        return;
    }
    memcpy(kb_state.min_thresholds, min_thresholds,
           sizeof(kb_state.min_thresholds));
    memcpy(kb_state.max_thresholds, max_thresholds,
           sizeof(kb_state.max_thresholds));
    kb_save_to_eeprom();
}

#ifdef DEBUG

bool previously_pressed_keys[KB_KEY_COUNT] = {false};

#endif // DEBUG

void kb_poll_normal() {

    if (hid_buff[0] != KEY_NOKEY || hid_buff[2] != KEY_NOKEY) {
        memset(hid_buff, 0, HID_BUFFER_SIZE);
        pressed_amount = 0;
    }

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
                if (pressed_amount >= HID_BUFFER_SIZE - 2) {
                    return;
                }
            }
#ifdef DEBUG
            else {
                previously_pressed_keys[index] = false;
            }
#endif // DEBUG
            index++;
        }
    }

    if (fn_pressed) {
        kb_process_fn_buff();
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

extern USBD_HandleTypeDef hUsbDeviceFS;

static uint32_t previous_poll_time = 0;

void kb_handle() {

    if (systick_get_tick() - previous_poll_time >= KB_DEFAULT_POLLING_RATE) {

        switch (kb_state.settings.mode) {

        case KB_MODE_NORMAL:
            kb_poll_normal();
            break;

        case KB_MODE_RACE:
            kb_poll_race();
            break;
        }
    }

    if (values_request_ptr) {
        interface_handle_get_values_response(values_request_ptr,
                                             kb_state.current_values);
        values_request_ptr = NULL;
    }

    USBD_HID_SendReport(&hUsbDeviceFS, HID_EPIN_ADDR, hid_buff,
                        HID_BUFFER_SIZE);
}
