#include "keyboard.h"

#include "error_handler.h"
#include "hal_adc.h"
#include "hal_gpio.h"
#include "keys.h"
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

__ALIGN_BEGIN static uint8_t mappings[KB_KEY_COUNT * KB_LAYER_COUNT] __ALIGN_END =
    {

#ifdef LEFT
        KEY00,          KEY01,          KEY02,

        KEY10,          KEY11,          KEY12,

        KEY20,          KEY21,          KEY22,

        KEY30,          KEY31,          KEY32,

        KEY40,          KEY41,          KEY42,

        KEY50,          KEY51,          KEY52,

        KEY60,          KEY61,          KEY62,          KEY63,

        KEY00_LAYER_1,  KEY01_LAYER_1,  KEY02_LAYER_1,

        KEY10_LAYER_1,  KEY11_LAYER_1,  KEY12_LAYER_1,

        KEY20_LAYER_1,  KEY21_LAYER_1,  KEY22_LAYER_1,

        KEY30_LAYER_1,  KEY31_LAYER_1,  KEY32_LAYER_1,

        KEY40_LAYER_1,  KEY41_LAYER_1,  KEY42_LAYER_1,

        KEY50_LAYER_1,  KEY51_LAYER_1,  KEY52_LAYER_1,

        KEY60_LAYER_1,  KEY61_LAYER_1,  KEY62_LAYER_1,  KEY63_LAYER_1,

#endif // LEFT

#ifdef RIGHT
        KEY70,          KEY71,          KEY72,          KEY73,

        KEY80,          KEY81,          KEY82,

        KEY90,          KEY91,          KEY92,

        KEY100,         KEY101,         KEY102,

        KEY110,         KEY111,         KEY112,

        KEY120,         KEY121,         KEY122,

        KEY130,         KEY131,         KEY132,

        KEY70_LAYER_1,  KEY71_LAYER_1,  KEY72_LAYER_1,  KEY73_LAYER_1,

        KEY80_LAYER_1,  KEY81_LAYER_1,  KEY82_LAYER_1,

        KEY90_LAYER_1,  KEY91_LAYER_1,  KEY92_LAYER_1,

        KEY100_LAYER_1, KEY101_LAYER_1, KEY102_LAYER_1,

        KEY110_LAYER_1, KEY111_LAYER_1, KEY112_LAYER_1,

        KEY120_LAYER_1, KEY121_LAYER_1, KEY122_LAYER_1,

        KEY130_LAYER_1, KEY131_LAYER_1, KEY132_LAYER_1,

#endif // RIGHT

};

__ALIGN_BEGIN static const gpio_pin_t hall_en_pins[KB_KEY_COUNT] __ALIGN_END = {

    PIN_HALL_EN_1,  PIN_HALL_EN_2,  PIN_HALL_EN_3,  PIN_HALL_EN_4,
    PIN_HALL_EN_5,  PIN_HALL_EN_6,  PIN_HALL_EN_7,  PIN_HALL_EN_8,
    PIN_HALL_EN_9,  PIN_HALL_EN_10, PIN_HALL_EN_11, PIN_HALL_EN_12,
    PIN_HALL_EN_13, PIN_HALL_EN_14, PIN_HALL_EN_15, PIN_HALL_EN_16,
    PIN_HALL_EN_17, PIN_HALL_EN_18, PIN_HALL_EN_19, PIN_HALL_EN_20,
    PIN_HALL_EN_21, PIN_HALL_EN_22,

};

static inline bool kb_key_pressed_by_threshold(uint8_t key_index,
                                               uint16_t *const value) {
    // Turn on HS
    gpio_digital_write(hall_en_pins[key_index], HIGH);

    hal_err err = adc_start();
    if (err) {
        LOG_CRITICAL("ADC read error %d", err);
        ERR_H(err);
    }

    while (adc_conversion_ongoing_regular()) {
    }

    // Turn off HS
    gpio_digital_write(hall_en_pins[key_index], LOW);

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

static inline void kb_init_default() {
    for (uint8_t i = 0; i < KB_KEY_COUNT; i++) {
        kb_key_pressed_by_threshold(i, &kb_state.min_thresholds[i]);
        kb_state.max_thresholds[i] = KB_KEY_MAX_VALUE_THRESHOLD_DEFAULT;
        kb_state.key_thresholds[i] = KB_KEY_THRESHOLD_DEFAULT;
    }
    memcpy(kb_state.mappings, mappings, sizeof(mappings));
}

hal_err kb_init() {

    LOG_INFO("Setting up...");

    kb_super_init();

    LOG_TRACE("Setting up HALL_EN pins...");
    for (uint8_t i = 0; i < KB_KEY_COUNT; i++) {
        gpio_turn_on_port(hall_en_pins[i].gpio);
        gpio_set_mode(hall_en_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_speed(hall_en_pins[i], GPIO_SPEED_HIGH);
    }

    if (PIN_HALL_ADC.adc_chan == ADC_CHANNEL_NONE) {
        LOG_CRITICAL("PIN_HALL_ADC does not have an ADC channel.");
        return ERR_KB_COMMON_NO_ADC_CHAN;
    }

    LOG_TRACE("Setting up ADC...");

    adc_channel_config_t channel_config;
    channel_config.mode = ADC_CHANNEL_SINGLE_ENDED;
    channel_config.rank = ADC_CHANNEL_RANK_1;
    channel_config.offset_type = ADC_CHANNEL_OFFSET_NONE;
    channel_config.offset = 0;
    channel_config.channel = PIN_HALL_ADC.adc_chan;
    channel_config.sampling_time = kb_state.settings.adc_sampling_time;
    hal_err err = adc_config_channel(&channel_config);
    if (err) {
        LOG_CRITICAL("Unable to config ADC channel: Error %d", err);
        ERR_H(err);
    }

    if (!kb_load_state_from_eeprom()) {
        LOG_DEBUG("Failed to load state from EEPROM. Loading defaults...");
        kb_init_default();
        LOG_DEBUG("Loaded default state.");
    }

    memset(kb_state.current_values, 0, sizeof(kb_state.current_values));

    LOG_INFO("Setup complete.");

    return OK;
}

#ifdef DEBUG

bool previously_pressed_keys[KB_KEY_COUNT] = {false};

#endif // DEBUG

void kb_poll_normal() {

    for (uint8_t i = 0; i < KB_KEY_COUNT; i++) {
        if (kb_key_pressed_by_threshold(i, &kb_state.current_values[i])) {
            uint8_t key = kb_state.mappings[i * (kb_state.layer + 1)];
#ifdef DEBUG
            if (!previously_pressed_keys[i]) {
                LOG_DEBUG("Key with index %d (HID code %d) pressed.", i, key);
                previously_pressed_keys[i] = true;
            }
#endif // DEBUG
            kb_process_key(key);
        }
#ifdef DEBUG
        else {
            previously_pressed_keys[i] = false;
        }
#endif // DEBUG
    }
}

void kb_poll_race() {

    uint16_t highest_value = 0;
    uint8_t pressed_key = KEY_NOKEY;

    for (uint8_t i = 0; i < KB_KEY_COUNT; i++) {
        if (kb_key_pressed_by_threshold(i, &kb_state.current_values[i])) {
            uint8_t key = kb_state.mappings[i * (kb_state.layer + 1)];
#ifdef DEBUG
            if (!previously_pressed_keys[i]) {
                LOG_DEBUG("Key with index %d (HID code %d) pressed.", i, key);
                previously_pressed_keys[i] = true;
            }
#endif // DEBUG
            uint16_t value = kb_state.current_values[i];
            if (highest_value < value) {
                highest_value = value;
                pressed_key = key;
            }
        }
#ifdef DEBUG
        else {
            previously_pressed_keys[i] = false;
        }
#endif // DEBUG
    }
    if (pressed_key != KEY_NOKEY) {
        kb_process_key(pressed_key);
    }
}
