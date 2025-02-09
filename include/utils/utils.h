#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdlib.h>

#define ALIGNED __attribute__((aligned(4)))

#ifndef UNUSED
#define UNUSED(X) (void)(X)
#endif // UNUSED

#ifndef LOBYTE
#define LOBYTE(x) ((uint8_t)((x) & 0x00FFU))
#endif /* LOBYTE */

#ifndef HIBYTE
#define HIBYTE(x) ((uint8_t)(((x) & 0xFF00U) >> 8U))
#endif /* HIBYTE */

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif /* MIN */

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif /* MAX */

#ifndef ATTR_WEAK
#define ATTR_WEAK __attribute__((weak))
#endif // ATTR_WEAK

static inline uint16_t SWAPBYTE(uint8_t *addr) {
    uint16_t _SwapVal;
    uint16_t _Byte1;
    uint16_t _Byte2;
    uint8_t *_pbuff = addr;

    _Byte1 = *(uint8_t *)_pbuff;
    _pbuff++;
    _Byte2 = *(uint8_t *)_pbuff;

    _SwapVal = (_Byte2 << 8) | _Byte1;

    return _SwapVal;
}

static inline void int_to_unicode(uint32_t value, uint8_t *pbuf, uint8_t len) {
    uint8_t idx = 0;

    for (idx = 0; idx < len; idx++) {
        if (((value >> 28)) < 0xA) {
            pbuf[2 * idx] = (value >> 28) + '0';
        } else {
            pbuf[2 * idx] = (value >> 28) + 'A' - 10;
        }

        value = value << 4;

        pbuf[2 * idx + 1] = 0;
    }
}

static inline uint8_t get_string_length(uint8_t *buf) {
    uint8_t len = 0U;
    uint8_t *pbuff = buf;

    while (*pbuff != (uint8_t)'\0') {
        len++;
        pbuff++;
    }

    return len;
}

static inline void ascii_to_unicode(uint8_t *desc, uint8_t *unicode,
                                    uint16_t *len, uint8_t type,
                                    uint16_t max_length) {
    uint8_t idx = 0U;
    uint8_t *pdesc;

    if (desc == NULL) {
        return;
    }

    pdesc = desc;
    *len = (uint16_t)get_string_length(pdesc) * 2U + 2U;
    if (*len > max_length) {
        *len = max_length;
    }

    unicode[idx] = *(uint8_t *)len;
    idx++;
    unicode[idx] = type;
    idx++;

    while (*pdesc != (uint8_t)'\0') {
        unicode[idx] = *pdesc;
        pdesc++;
        idx++;

        unicode[idx] = 0U;
        idx++;
    }
}

#endif // UTILS_H
