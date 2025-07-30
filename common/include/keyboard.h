#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "hal_adc.h"
#include "hal_err.h"
#include "interface_handler.h"
#include "settings.h"
#include "ykb_protocol.h"

#include <stdint.h>

#define HID_BUFFER_SIZE 8

// TYPES

#ifdef __GNUC__
#define PACKED __attribute__((__packed__))
#else
#define PACKED
#warning                                                                       \
    "Packed attribute not defined for this compiler, struct padding might occur"
#endif

// Default mode: default behavior of keyboard.
//
// Race mode: only the key which is pressed the most is activated.
typedef enum PACKED {
    KB_MODE_NORMAL = 0U,
    KB_MODE_RACE = 1U,
} kb_mode;

typedef struct PACKED {

    uint16_t key_polling_rate; // TODO implement
    adc_sampling_time adc_sampling_time;
    kb_mode mode;

} kb_settings_t;

typedef struct {

    kb_settings_t settings;

    uint8_t layer;

    uint8_t key_thresholds[KB_KEY_COUNT];

    uint16_t min_thresholds[KB_KEY_COUNT];
    uint16_t max_thresholds[KB_KEY_COUNT];

    uint8_t mappings[KB_KEY_COUNT];

    uint16_t current_values[KB_KEY_COUNT];

} kb_state_t;

// FUNCTIONS

hal_err kb_init();

void kb_poll_normal();
void kb_poll_race();

void kb_process_key(uint8_t key_code);

bool kb_load_state_from_eeprom();
void kb_super_init();

void kb_get_settings(uint8_t *buffer);
void kb_get_mappings(uint8_t *buffer);
void kb_request_values(communication_source source, ykb_protocol_t *protocol);
void kb_get_thresholds(uint8_t *buffer);

void kb_set_settings(kb_settings_t *new_settings);
void kb_set_mappings(uint8_t *new_mappings);
void kb_set_thresholds(uint8_t *new_thresholds);

void kb_calibrate(uint16_t *min_thresholds, uint16_t *max_thresholds);

void kb_handle();

#endif // KEYBOARD_H
