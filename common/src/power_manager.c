#include "power_manager.h"

#include "hal_clock.h"
#include "hal_periph_clock.h"
#include "hal_systick.h"

static uint32_t stop_mode_disable = 0;
static uint32_t off_mode_disable = 0;

void power_manager_init() {

    periph_clock_select_rfwakeup_source(PERIPHCLK_RFWAKEUP_LSE);
    periph_clock_select_smps_source(PERIPHCLK_SMPS_HSE);
    periph_clock_select_smps_divider(PERIPHCLK_SMPS_DIV_1);

    clock_stop_wakeup_select_source(CLOCK_STOP_WAKEUP_SOURCE_HSI16);

    stop_mode_disable = 0;
    off_mode_disable = 0;

    HAL_INIT_CRITICAL_SECTION();
}

void power_manager_set_stop_mode(power_manager_pid pid,
                                 power_manager_state state) {
    HAL_ENTER_CRITICAL_SECTION();

    switch (state) {
    case POWER_MANAGER_STATE_LPM_ENABLE:
        CLEAR_BIT(stop_mode_disable, 1 << pid);
        break;
    case POWER_MANAGER_STATE_LPM_DISABLE:
        SET_BIT(stop_mode_disable, 1 << pid);
        break;
    }

    HAL_EXIT_CRITICIAL_SECTION();
}

void power_manager_set_off_mode(power_manager_pid pid,
                                power_manager_state state) {
    HAL_ENTER_CRITICAL_SECTION();

    switch (state) {
    case POWER_MANAGER_STATE_LPM_ENABLE:
        CLEAR_BIT(off_mode_disable, 1 << pid);
        break;
    case POWER_MANAGER_STATE_LPM_DISABLE:
        SET_BIT(off_mode_disable, 1 << pid);
        break;
    }

    HAL_EXIT_CRITICIAL_SECTION();
}

power_manager_mode power_manager_get_mode() {

    HAL_ENTER_CRITICAL_SECTION();

    power_manager_mode mode;

    if (stop_mode_disable)
        mode = POWER_MANAGER_MODE_SLEEP;
    else if (off_mode_disable)
        mode = POWER_MANAGER_MODE_STOP;
    else
        mode = POWER_MANAGER_MODE_OFF;

    HAL_EXIT_CRITICIAL_SECTION();

    return mode;
}

static void power_manager_enter_sleep_mode() {
    systick_suspend();

    __WFE();
}

static void power_manager_exit_sleep_mode() {}

static void power_manager_enter_stop_mode() {}

static void power_manager_exit_stop_mode() {}

static void power_manager_enter_off_mode() {}

static void power_manager_exit_off_mode() {}

void power_manager_enter_low_power() {

    HAL_ENTER_CRITICAL_SECTION();

    power_manager_mode mode = power_manager_get_mode();

    switch (mode) {
    case POWER_MANAGER_MODE_SLEEP:
        power_manager_enter_sleep_mode();
        power_manager_exit_sleep_mode();
        break;
    case POWER_MANAGER_MODE_STOP:
        power_manager_enter_stop_mode();
        power_manager_exit_stop_mode();
        break;
    case POWER_MANAGER_MODE_OFF:
        power_manager_enter_off_mode();
        power_manager_exit_off_mode();
        break;
    }

    HAL_EXIT_CRITICIAL_SECTION();
}
