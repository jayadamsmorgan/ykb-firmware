#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#ifndef KB_KEY_THRESHOLD_DEFAULT
#define KB_KEY_THRESHOLD_DEFAULT 200
#endif // KB_KEY_THRESHOLD_DEFAULT

// TYPES

typedef struct {
    uint8_t num;
    uint8_t value;
} kb_key_t;

// Default mode: default behavior of keyboard.
//
// Race mode: only the key which is pressed the most is activated.
typedef enum { KB_MODE_DEFAULT, KB_MODE_RACE } kb_mode;

// FUNCTIONS

// Initialize keyboard.
void kb_init();

// Get current mode of the keyboard.
kb_mode get_kb_mode();

// Set current mode of the keyboard.
void set_kb_mode(kb_mode mode);

// Get current minimal threshold of key registration.
unsigned get_kb_min_threshold();

// Set minimal threshold of key registration.
void set_kb_min_threshold(unsigned threshold);

// Poll the keyboard for the pressed keys.
//
// Accepts pointer to uninitialized array of keys and fills it up.
//
// Returns:
//  amount of pressed keys
//  or -1 if error occured
//
// Caller needs to free the passed array once he is done using it.
int8_t kb_poll(kb_key_t **pressed_keys);

#endif // KEYBOARD_H
