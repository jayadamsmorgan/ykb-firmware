#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define _STR(x) #x
#define STR(x) _STR(x)

#ifndef UNUSED
#define UNUSED(X) (void)(X)
#endif // UNUSED

#define __RAM_FUNC __attribute__((section(".RamFunc")))

typedef enum {
    HAL_OK = 0x00,
    HAL_ERROR = 0x01,
    HAL_BUSY = 0x02,
    HAL_TIMEOUT = 0x03
} HAL_StatusTypeDef;

typedef enum { HAL_UNLOCKED = 0x00, HAL_LOCKED = 0x01 } HAL_LockTypeDef;

#define HAL_IS_BIT_SET(REG, BIT) (((REG) & (BIT)) == (BIT))
#define HAL_IS_BIT_CLR(REG, BIT) (((REG) & (BIT)) == 0U)

#define __HAL_LINKDMA(__HANDLE__, __PPP_DMA_FIELD__, __DMA_HANDLE__)           \
    do {                                                                       \
        (__HANDLE__)->__PPP_DMA_FIELD__ = &(__DMA_HANDLE__);                   \
        (__DMA_HANDLE__).Parent = (__HANDLE__);                                \
    } while (0)

#define __HAL_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = 0)

#define __HAL_LOCK(__HANDLE__)                                                 \
    do {                                                                       \
        if ((__HANDLE__)->Lock == HAL_LOCKED) {                                \
            return HAL_BUSY;                                                   \
        } else {                                                               \
            (__HANDLE__)->Lock = HAL_LOCKED;                                   \
        }                                                                      \
    } while (0)

#define __HAL_UNLOCK(__HANDLE__)                                               \
    do {                                                                       \
        (__HANDLE__)->Lock = HAL_UNLOCKED;                                     \
    } while (0)

#if defined(__ARMCC_VERSION) &&                                                \
    (__ARMCC_VERSION >= 6010050) /* ARM Compiler V6 */
#ifndef __weak
#define __weak __attribute__((weak))
#endif /* __weak */
#ifndef __packed
#define __packed __attribute__((packed))
#endif                                        /* __packed */
#elif defined(__GNUC__) && !defined(__CC_ARM) /* GNU Compiler */
#ifndef __weak
#define __weak __attribute__((weak))
#endif /* __weak */
#ifndef __packed
#define __packed __attribute__((__packed__))
#endif /* __packed */
#endif /* __GNUC__ */

/* Macro to get variable aligned on 4-bytes, for __ICCARM__ the directive
 * "#pragma data_alignment=4" must be used instead */
#if defined(__ARMCC_VERSION) &&                                                \
    (__ARMCC_VERSION >= 6010050) /* ARM Compiler V6 */
#ifndef __ALIGN_BEGIN
#define __ALIGN_BEGIN
#endif /* __ALIGN_BEGIN */
#ifndef __ALIGN_END
#define __ALIGN_END __attribute__((aligned(4)))
#endif                                        /* __ALIGN_END */
#elif defined(__GNUC__) && !defined(__CC_ARM) /* GNU Compiler */
#ifndef __ALIGN_END
#define __ALIGN_END __attribute__((aligned(4)))
#endif /* __ALIGN_END */
#ifndef __ALIGN_BEGIN
#define __ALIGN_BEGIN
#endif /* __ALIGN_BEGIN */
#else
#ifndef __ALIGN_END
#define __ALIGN_END
#endif /* __ALIGN_END */
#ifndef __ALIGN_BEGIN
#if defined(__CC_ARM) /* ARM Compiler V5 */
#define __ALIGN_BEGIN __align(4)
#elif defined(__ICCARM__) /* IAR Compiler */
#define __ALIGN_BEGIN
#endif /* __CC_ARM */
#endif /* __ALIGN_BEGIN */
#endif /* __GNUC__ */

#if defined(__CC_ARM) ||                                                       \
    (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
/* ARM Compiler V4/V5 and V6
   --------------------------
   RAM functions are defined using the toolchain options.
   Functions that are executed in RAM should reside in a separate source module.
   Using the 'Options for File' dialog you can simply change the 'Code / Const'
   area of a module to a memory space in physical RAM.
   Available memory areas are declared in the 'Target' tab of the 'Options for
   Target' dialog.
*/
#define __RAM_FUNC

#elif defined(__ICCARM__)
/* ICCARM Compiler
   ---------------
   RAM functions are defined using a specific toolchain keyword "__ramfunc".
*/
#define __RAM_FUNC __ramfunc

#elif defined(__GNUC__)
/* GNU Compiler
   ------------
  RAM functions are defined using a specific toolchain attribute
   "__attribute__((section(".RamFunc")))".
*/
#define __RAM_FUNC __attribute__((section(".RamFunc")))

#endif /* __CC_ARM || ... */

#if defined(__CC_ARM) ||                                                       \
    (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) ||              \
    defined(__GNUC__)
/* ARM V4/V5 and V6 & GNU Compiler
   -------------------------------
*/
#define __NOINLINE __attribute__((noinline))

#elif defined(__ICCARM__)
/* ICCARM Compiler
   ---------------
*/
#define __NOINLINE _Pragma("optimize = no_inline")

#endif /* __CC_ARM || ... */

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
