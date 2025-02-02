#ifndef HAL_BITS_H
#define HAL_BITS_H

#define BITMASK_1BIT 1U
#define BITMASK_2BIT 3U
#define BITMASK_3BIT 7U
#define BITMASK_4BIT 15U
#define BITMASK_5BIT 32U
#define BITMASK_6BIT 63U
#define BITMASK_7BIT 127U
#define BITMASK_8BIT 255U

#define BITMASK_10BIT 1023U
#define BITMASK_12BIT 4095U
#define BITMASK_16BIT 65535U

#define READ_BITS(REG, OFFSET, BITMASK) ((REG >> (OFFSET)) & BITMASK)

#define MODIFY_BITS(REG, OFFSET, VALUE, BITMASK)                               \
    do {                                                                       \
        uint32_t reg = REG;                                                    \
        reg &= ~(BITMASK << (OFFSET));                                         \
        reg |= ((VALUE & BITMASK) << (OFFSET));                                \
        REG = reg;                                                             \
    } while (0)

#endif // HAL_BITS_H
