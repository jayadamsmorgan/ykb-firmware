#ifndef RCC_H
#define RCC_H

// RCC REG START
//
#define RCC_REG_ADDR 0x58000000

#define RCC_CR_OFFSET 0x000
#define RCC_CR_REG ((volatile uint32_t *)RCC_REG_ADDR + RCC_CR_OFFSET)

#define RCC_ICSCR_OFFSET 0x004
#define RCC_ICSCR_REG ((volatile uint32_t *)RCC_REG_ADDR + RCC_ICSCR_OFFSET)

#define RCC_CFGR_OFFSET 0x008
#define RCC_CFGR_REG ((volatile uint32_t *)RCC_REG_ADDR + RCC_CFGR_OFFSET)

#define RCC_PLLCFGR_OFFSET 0x00C
#define RCC_PLLCFGR_REG                                                        \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_PLLCFGR_OFFSET))

#define RCC_PLLSAI1CFGR_OFFSET 0x010
#define RCC_PLLSAI1CFGR_REG                                                    \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_PLLSAI1CFGR_OFFSET))

#define RCC_CIER_OFFSET 0x018
#define RCC_CIER_REG ((volatile uint32_t *)(RCC_REG_ADDR + RCC_CIER_OFFSET))

#define RCC_CIFR_OFFSET 0x01C
#define RCC_CIFR_REG ((volatile uint32_t *)(RCC_REG_ADDR + RCC_CIFR_OFFSET))

#define RCC_CICR_OFFSET 0x020
#define RCC_CICR_REG ((volatile uint32_t *)(RCC_REG_ADDR + RCC_CICR_OFFSET))

#define RCC_SMPSCR_OFFSET 0x024
#define RCC_SMPSCR_REG ((volatile uint32_t *)(RCC_REG_ADDR + RCC_SMPSCR_OFFSET))

#define RCC_AHB1RSTR_OFFSET 0x028
#define RCC_AHB1RSTR_REG                                                       \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_AHB1RSTR_OFFSET))

#define RCC_AHB2RSTR_OFFSET 0x02C
#define RCC_AHB2RSTR_REG                                                       \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_AHB2RSTR_OFFSET))

#define RCC_AHB3RSTR_OFFSET 0x030
#define RCC_AHB3RSTR_REG                                                       \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_AHB3RSTR_OFFSET))

#define RCC_APB1RSTR1_OFFSET 0x038
#define RCC_APB1RSTR1_REG                                                      \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_APB1RSTR1_OFFSET))

#define RCC_APB1RSTR2_OFFSET 0x03C
#define RCC_APB1RSTR2_REG                                                      \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_APB1RSTR2_OFFSET))

#define RCC_APB2RSTR_OFFSET 0x040
#define RCC_APB2RSTR_REG                                                       \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_APB2RSTR_OFFSET))

#define RCC_APB3RSTR_OFFSET 0x044
#define RCC_APB3RSTR_REG                                                       \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_APB3RSTR_OFFSET))

#define RCC_AHB1ENR_OFFSET 0x048
#define RCC_AHB1ENR_REG                                                        \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_AHB1ENR_OFFSET))

#define RCC_AHB2ENR_OFFSET 0x04C
#define RCC_AHB2ENR_REG                                                        \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_AHB2ENR_OFFSET))

#define RCC_AHB3ENR_OFFSET 0x050
#define RCC_AHB3ENR_REG                                                        \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_AHB3ENR_OFFSET))

#define RCC_APB1ENR1_OFFSET 0x058
#define RCC_APB1ENR1_REG                                                       \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_APB1ENR1_OFFSET))

#define RCC_APB1ENR2_OFFSET 0x05C
#define RCC_APB1ENR2_REG                                                       \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_APB1ENR2_OFFSET))

#define RCC_APB2_ENR_OFFSET 0x060
#define RCC_APB2_ENR_REG                                                       \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_APB2_ENR_OFFSET))

#define RCC_AHB1SMENR_OFFSET 0x068
#define RCC_AHB1SMENR_REG                                                      \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_AHB1SMENR_OFFSET))

#define RCC_AHB2SMENR_OFFSET 0x06C
#define RCC_AHB2SMENR_REG                                                      \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_AHB2SMENR_OFFSET))

#define RCC_AHB3SMENR_OFFSET 0x070
#define RCC_AHB3SMENR_REG                                                      \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_AHB3SMENR_OFFSET))

#define RCC_APB1SMENR1_OFFSET 0x078
#define RCC_APB1SMENR1_REG                                                     \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_APB1SMENR1_OFFSET))

#define RCC_APB1SMENR2_OFFSET 0x07C
#define RCC_APB1SMENR2_REG                                                     \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_APB1SMENR2_OFFSET))

#define RCC_APB2SMENR_OFFSET 0x080
#define RCC_APB2SMENR_REG                                                      \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_APB2SMENR_OFFSET))

#define RCC_CCIPR_OFFSET 0x088
#define RCC_CCIPR_REG ((volatile uint32_t *)(RCC_REG_ADDR + RCC_CCIPR_OFFSET))

#define RCC_BDCR_OFFSET 0x090
#define RCC_BDCR_REG ((volatile uint32_t *)(RCC_REG_ADDR + RCC_BDCR_OFFSET))

#define RCC_CSR_OFFSET 0x094
#define RCC_CSR_REG ((volatile uint32_t *)(RCC_REG_ADDR + RCC_CSR_OFFSET))

#define RCC_CRRCR_OFFSET 0x098
#define RCC_CRRCR_REG ((volatile uint32_t *)(RCC_REG_ADDR + RCC_CRRCR_OFFSET))

#define RCC_HSECR_OFFSET 0x09C
#define RCC_HSECR_REG ((volatile uint32_t *)(RCC_REG_ADDR + RCC_HSECR_OFFSET))

#define RCC_EXTCFGR_OFFSET 0x108
#define RCC_EXTCFGR_REG                                                        \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_EXTCFGR_OFFSET))

#define RCC_C2AHB1ENR_OFFSET 0x148
#define RCC_C2AHB1ENR_REG                                                      \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_C2AHB1ENR_OFFSET))

#define RCC_C2AHB2ENR_OFFSET 0x14C
#define RCC_C2AHB2ENR_REG                                                      \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_C2AHB2ENR_OFFSET))

#define RCC_C2AHB3ENR_OFFSET 0x150
#define RCC_C2AHB3ENR_REG                                                      \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_C2AHB3ENR_OFFSET))

#define RCC_C2APB1ENR1_OFFSET 0x158
#define RCC_C2APB1ENR1_REG                                                     \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_C2APB1ENR1_OFFSET))

#define RCC_C2APB1ENR2_OFFSET 0x15C
#define RCC_C2APB1ENR2_REG                                                     \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_C2APB1ENR2_OFFSET))

#define RCC_C2APB2ENR_OFFSET 0x160
#define RCC_C2APB2ENR_REG                                                      \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_C2APB2ENR_OFFSET))

#define RCC_C2APB3ENR_OFFSET 0x164
#define RCC_C2APB3ENR_REG                                                      \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_C2APB3ENR_OFFSET))

#define RCC_C2AHB1SMENR_OFFSET 0x168
#define RCC_C2AHB1SMENR_REG                                                    \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_C2AHB1SMENR_OFFSET))

#define RCC_C2AHB2SMENR_OFFSET 0x16C
#define RCC_C2AHB2SMENR_REG                                                    \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_C2AHB2SMENR_OFFSET))

#define RCC_C2AHB33MENR_OFFSET 0x170
#define RCC_C2AHB33MENR_REG                                                    \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_C2AHB33MENR_OFFSET))

#define RCC_C2APB1SMENR1_OFFSET 0x178
#define RCC_C2APB1SMENR1_REG                                                   \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_C2APB1SMENR1_OFFSET))

#define RCC_C2APB1SMENR2_OFFSET 0x17C
#define RCC_C2APB1SMENR2_REG                                                   \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_C2APB1SMENR2_OFFSET))

#define RCC_C2APB2SMENR_OFFSET 0x180
#define RCC_C2APB2SMENR_REG                                                    \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_C2APB2SMENR_OFFSET))

#define RCC_C2APB3SMENR_OFFSET 0x184
#define RCC_C2APB3SMENR_REG                                                    \
    ((volatile uint32_t *)(RCC_REG_ADDR + RCC_C2APB3SMENR_OFFSET))

// RCC REG END
//

#endif // RCC_H
