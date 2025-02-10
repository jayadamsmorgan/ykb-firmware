#include "hal/cortex.h"
#include "stm32wb55xx.h"
#include "stm32wbxx.h"
#include "utils/utils.h"

__weak void exti_handler_0() {}
__weak void exti_handler_1() {}
__weak void exti_handler_2() {}
__weak void exti_handler_3() {}
__weak void exti_handler_4() {}
__weak void exti_handler_5() {}
__weak void exti_handler_6() {}
__weak void exti_handler_7() {}
__weak void exti_handler_8() {}
__weak void exti_handler_9() {}
__weak void exti_handler_10() {}
__weak void exti_handler_11() {}
__weak void exti_handler_12() {}
__weak void exti_handler_13() {}
__weak void exti_handler_14() {}
__weak void exti_handler_15() {}

void EXTI0_IRQHandler(void) {
    SET_BIT(EXTI->PR1, EXTI_PR1_PIF0);
    exti_handler_0();
}

void EXTI1_IRQHandler(void) {
    SET_BIT(EXTI->PR1, EXTI_PR1_PIF1);
    exti_handler_1();
}

void EXTI2_IRQHandler(void) {
    SET_BIT(EXTI->PR1, EXTI_PR1_PIF2);
    exti_handler_2();
}

void EXTI3_IRQHandler(void) {
    SET_BIT(EXTI->PR1, EXTI_PR1_PIF3);
    exti_handler_3();
}

void EXTI4_IRQHandler(void) {
    SET_BIT(EXTI->PR1, EXTI_PR1_PIF4);
    exti_handler_4();
}

void EXTI9_5_IRQHandler(void) {
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF5)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF5);
        exti_handler_5();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF6)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF6);
        exti_handler_6();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF7)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF7);
        exti_handler_7();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF8)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF8);
        exti_handler_8();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF9)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF9);
        exti_handler_9();
    }
}

void EXTI15_10_IRQHandler(void) {
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF10)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF10);
        exti_handler_10();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF11)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF11);
        exti_handler_11();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF12)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF12);
        exti_handler_12();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF13)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF13);
        exti_handler_13();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF14)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF14);
        exti_handler_14();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF15)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF15);
        exti_handler_15();
    }
}
