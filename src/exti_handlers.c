#include "hal/gpio.h"
#include "logging.h"
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
    LOG_TRACE("EXTI0 triggered.");
    exti_handler_0();
}

void EXTI1_IRQHandler(void) {
    SET_BIT(EXTI->PR1, EXTI_PR1_PIF1);
    LOG_TRACE("EXTI1 triggered.");
    exti_handler_1();
}

void EXTI2_IRQHandler(void) {
    SET_BIT(EXTI->PR1, EXTI_PR1_PIF2);
    LOG_TRACE("EXTI2 triggered.");
    exti_handler_2();
}

void EXTI3_IRQHandler(void) {
    SET_BIT(EXTI->PR1, EXTI_PR1_PIF3);
    LOG_TRACE("EXTI3 triggered.");
    exti_handler_3();
}

void EXTI4_IRQHandler(void) {
    SET_BIT(EXTI->PR1, EXTI_PR1_PIF4);
    LOG_TRACE("EXTI4 triggered.");
    exti_handler_4();
}

void EXTI9_5_IRQHandler(void) {
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF5)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF5);
        LOG_TRACE("EXTI5 triggered.");
        exti_handler_5();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF6)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF6);
        LOG_TRACE("EXTI6 triggered.");
        exti_handler_6();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF7)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF7);
        LOG_TRACE("EXTI7 triggered.");
        exti_handler_7();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF8)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF8);
        LOG_TRACE("EXTI8 triggered.");
        exti_handler_8();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF9)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF9);
        LOG_TRACE("EXTI9 triggered.");
        exti_handler_9();
    }
}

void EXTI15_10_IRQHandler(void) {
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF10)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF10);
        LOG_TRACE("EXTI10 triggered.");
        exti_handler_10();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF11)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF11);
        LOG_TRACE("EXTI11 triggered.");
        exti_handler_11();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF12)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF12);
        LOG_TRACE("EXTI12 triggered.");
        exti_handler_12();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF13)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF13);
        LOG_TRACE("EXTI13 triggered.");
        exti_handler_13();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF14)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF14);
        LOG_TRACE("EXTI14 triggered.");
        exti_handler_14();
    }
    if (READ_BIT(EXTI->PR1, EXTI_PR1_PIF15)) {
        SET_BIT(EXTI->PR1, EXTI_PR1_PIF15);
        LOG_TRACE("EXTI15 triggered.");
        exti_handler_15();
    }
}
