#ifndef EEPROM_H
#define EEPROM_H

#include "hal/flash.h"

#include "settings.h"

#define ERR_EEPROM_INIT_BADPAGERANGE -1301

#define ERR_EEPROM_GET_OUTOFBOUNDS -1302
#define ERR_EEPROM_GET_BADARGS -1303

#define ERR_EEPROM_SAVE_OUTOFBOUNDS -1304
#define ERR_EEPROM_SAVE_BADARGS -1305

hal_err eeprom_init();

size_t eeprom_get_size();

hal_err eeprom_clear();

hal_err eeprom_get(uint32_t address, void *value, size_t size);

hal_err eeprom_save(uint32_t address, void *value, size_t size);

#endif // EEPROM_H
