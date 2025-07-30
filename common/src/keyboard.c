#include "keyboard.h"

#include "eeprom.h"
#include "hal_systick.h"
#include "keys.h"
#include "logging.h"
#include "mappings.h"
#include "memory_map.h"
#include "pinout.h"

#include "usb/usbd_hid.h"

static uint8_t hid_buff[HID_BUFFER_SIZE];
static uint8_t pressed_amount = 0;

static uint8_t fn_buff[HID_BUFFER_SIZE - 2];
static uint8_t fn_pressed_amount = 0;
static bool fn_pressed;

static uint8_t modifier_map[8] = {0x01, 0x02, 0x04, 0x08,
                                  0x10, 0x20, 0x40, 0x80};

static ykb_protocol_t values_request;
static ykb_protocol_t *values_request_ptr;
static communication_source values_request_src;

typedef struct PACKED {

    kb_state_t state;

    uint16_t crc16;

} kb_eeprom_t;

#define EEPROM_KB_STATE_ADDR EEPROM_START_ADDRESS

kb_state_t kb_state = {
    .settings =
        {
            .mode = KB_MODE_NORMAL,
            .adc_sampling_time = KB_ADC_SAMPLING_DEFAULT,
            .key_polling_rate = KB_DEFAULT_POLLING_RATE,
        },
    .layer = 0,
};

void kb_super_init() {
#ifdef PIN_CAPSLOCK_LED
    gpio_turn_on_port(PIN_CAPSLOCK_LED.gpio);
    gpio_set_mode(PIN_CAPSLOCK_LED, GPIO_MODE_OUTPUT);
#endif // PIN_CAPSLOCK_LED
}

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

bool kb_load_state_from_eeprom() {

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

#ifdef PIN_CAPSLOCK_LED
static bool capslock_status = false;

static inline void toggle_capslock_led() {
    capslock_status = !capslock_status;
    gpio_digital_write(PIN_CAPSLOCK_LED, capslock_status);
}
#endif // PIN_CAPSLOCK_LED

void kb_process_key(uint8_t key) {

    if (pressed_amount >= HID_BUFFER_SIZE - 2) {
        return;
    }

    if (key == KEY_FN) {
        fn_pressed = true;
        return;
    }

#if LAYER_COUNT > 1
    if (key == KEY_LAYER) {
        kb_state.layer++;
        if (kb_state.layer == LAYER_COUNT) {
            kb_state.layer = 0;
        }
        return;
    }
#endif // LAYER_COUNT > 1

#ifdef PIN_CAPSLOCK_LED
    if (key == KEY_CAPSLOCK) {
        toggle_capslock_led();
        return;
    }
#endif // PIN_CAPSLOCK_LED

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

void kb_request_values(communication_source source, ykb_protocol_t *protocol) {
    memcpy(&values_request, protocol, sizeof(ykb_protocol_t));
    values_request_ptr = &values_request;
    values_request_src = source;
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

#if defined(USB_ENABLED) && USB_ENABLED == 1
extern USBD_HandleTypeDef hUsbDeviceFS;
#endif // USB_ENABLED

static uint32_t previous_poll_time = 0;

void kb_handle() {

    if (systick_get_tick() - previous_poll_time >= KB_DEFAULT_POLLING_RATE) {

        if (hid_buff[0] != KEY_NOKEY || hid_buff[2] != KEY_NOKEY) {
            memset(hid_buff, 0, HID_BUFFER_SIZE);
            pressed_amount = 0;
        }

        switch (kb_state.settings.mode) {

        case KB_MODE_NORMAL:
            kb_poll_normal();
            break;

        case KB_MODE_RACE:
            kb_poll_race();
            break;
        }

        if (fn_pressed) {
            kb_process_fn_buff();
        }
    }

    if (values_request_ptr) {
        interface_handle_get_values_response(
            values_request_src, values_request_ptr, kb_state.current_values);
        values_request_ptr = NULL;
    }

#if defined(USB_ENABLED) && USB_ENABLED == 1
    USBD_HID_SendReport(&hUsbDeviceFS, HID_EPIN_ADDR, hid_buff,
                        HID_BUFFER_SIZE);
#endif // USB_ENABLED
}
