#ifndef INTERFACE_HANDLER_H
#define INTERFACE_HANDLER_H

#include "ykb_protocol.h"

#include <stdint.h>

typedef enum {
    COMMUNICATION_SOURCE_USB,
    COMMUNICATION_SOURCE_BT,
} communication_source;

void interface_handle_new_packet(communication_source source, uint8_t *packet,
                                 uint8_t packet_length);

void interface_send_error(uint8_t request_error, uint8_t error_description);

void interface_handle_get_values_response(communication_source source,
                                          ykb_protocol_t *packet,
                                          uint16_t *values);

#endif // INTERFACE_HANDLER_H
