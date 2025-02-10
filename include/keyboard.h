#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "hal/hal_err.h"
#include <stdint.h>

// TYPES

typedef struct {
    uint8_t num;
    uint16_t value;
} kb_key_t;

// Default mode: default behavior of keyboard.
//
// Race mode: only the key which is pressed the most is activated.
typedef enum { KB_MODE_NORMAL, KB_MODE_RACE } kb_mode;

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

int8_t kb_poll_normal(kb_key_t **const pressed_keys);
void kb_poll_race(kb_key_t *pressed_key);

#endif // KEYBOARD_H
