#ifndef INTERFACE_HANDLER_H
#define INTERFACE_HANDLER_H

#include "usb/usbd_def.h"

#include <stdint.h>

void interface_handle_new_packet(uint8_t *packet, uint8_t packet_length);

void interface_send_error(uint8_t request_error, uint8_t error_description);

#endif // INTERFACE_HANDLER_H
