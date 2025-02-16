#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "hal/hal_err.h"
#include "mappings.h"
#include "settings.h"
#include "utils/utils.h"
#include <stdint.h>

// TYPES

// Default mode: default behavior of keyboard.
//
// Race mode: only the key which is pressed the most is activated.
typedef enum { KB_MODE_NORMAL, KB_MODE_RACE } kb_mode;

typedef struct {

    bool lock;

    uint16_t key_threshold;
    uint16_t key_polling_rate;

    kb_mode mode;

    uint16_t *values;

    uint8_t *mappings;

    const uint8_t key_amount;

} kb_state_t;

// FUNCTIONS

// Initialize keyboard.
hal_err kb_init();

// Get current mode of the keyboard.
error_t kb_get_mode(kb_mode *const mode, bool blocking);

// Set current mode of the keyboard.
error_t kb_set_mode(kb_mode new_mode, bool blocking);

// Get current minimal threshold of key registration.
error_t kb_get_min_threshold(uint32_t *threshold, bool blocking);

// Set minimal threshold of key registration.
error_t kb_set_min_threshold(uint32_t threshold, bool blocking);

void kb_poll_normal();
void kb_poll_race();

void kb_handle(bool blocking);

#endif // KEYBOARD_H
