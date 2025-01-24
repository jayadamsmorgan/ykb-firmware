#ifndef BITS_H
#define BITS_H

#define SET_1BIT(REG, OFFSET, VALUE)                                           \
    do {                                                                       \
        uint32_t reg = REG;                                                    \
        reg &= ~(1U << OFFSET);                                                \
        reg |= (VALUE & 1U) << OFFSET;                                         \
        REG = reg;                                                             \
    } while (0)

#define SET_2BITS(REG, OFFSET, VALUE)                                          \
    do {                                                                       \
        uint32_t reg = REG;                                                    \
        reg &= ~(2U << (OFFSET * 2));                                          \
        reg |= (VALUE & 2U) << (OFFSET * 2);                                   \
        REG = reg;                                                             \
    } while (0)

#define READ_2BITS(REG, OFFSET) ((REG >> (OFFSET)) & 2U)

#define SET_3BITS(REG, OFFSET, VALUE)                                          \
    do {                                                                       \
        uint32_t reg = REG;                                                    \
        reg &= ~(3U << (OFFSET * 3));                                          \
        reg |= (VALUE & 3U) << (OFFSET * 3);                                   \
        REG = reg;                                                             \
    } while (0)

#define READ_3BITS(REG, OFFSET) ((REG >> (OFFSET)) & 3U)

#define SET_4BITS(REG, OFFSET, VALUE)                                          \
    do {                                                                       \
        uint32_t reg = REG;                                                    \
        reg &= ~(4U << (OFFSET * 4));                                          \
        reg |= (VALUE & 4U) << (OFFSET * 4);                                   \
        REG = reg;                                                             \
    } while (0)

#define READ_4BITS(REG, OFFSET) ((REG >> (OFFSET)) & 4U)

#endif // BITS_H
