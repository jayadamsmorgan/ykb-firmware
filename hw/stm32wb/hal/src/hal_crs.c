#include "hal_crs.h"

#include "hal_bits.h"
#include "stm32wbxx.h"

void crs_enable() { SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_CRSEN); }

bool crs_enabled() { return READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_CRSEN); }

void crs_config(const crs_config_t *config) {
    MODIFY_BITS(CRS->CFGR, CRS_CFGR_SYNCPOL_Pos, config->polarity,
                BITMASK_1BIT);
    MODIFY_BITS(CRS->CFGR, CRS_CFGR_SYNCSRC_Pos, config->source, BITMASK_2BIT);
    MODIFY_BITS(CRS->CFGR, CRS_CFGR_SYNCDIV_Pos, config->divider, BITMASK_3BIT);
    MODIFY_BITS(CRS->CFGR, CRS_CFGR_FELIM_Pos, config->limit, BITMASK_8BIT);
    MODIFY_BITS(CRS->CFGR, CRS_CFGR_RELOAD_Pos, config->reload, BITMASK_16BIT);
}

void crs_config_update(crs_config_t *config) {
    if (!config) {
        return;
    }
    config->polarity = READ_BITS(CRS->CFGR, CRS_CFGR_SYNCPOL_Pos, BITMASK_1BIT);
    config->source = READ_BITS(CRS->CFGR, CRS_CFGR_SYNCSRC_Pos, BITMASK_2BIT);
    config->divider = READ_BITS(CRS->CFGR, CRS_CFGR_SYNCDIV_Pos, BITMASK_3BIT);
    config->limit = READ_BITS(CRS->CFGR, CRS_CFGR_FELIM_Pos, BITMASK_8BIT);
    config->reload = READ_BITS(CRS->CFGR, CRS_CFGR_RELOAD_Pos, BITMASK_16BIT);
}

hal_err crs_set_smooth_trimming(crs_smooth_trim_t trim) {
    if (trim > 63U) {
        return ERR_CRS_SET_SM_T_INV_TRIM;
    }
    MODIFY_BITS(CRS->CR, CRS_CR_TRIM_Pos, trim, BITMASK_6BIT);
    return OK;
}

void crs_auto_trimming_enable() { SET_BIT(CRS->CR, CRS_CR_AUTOTRIMEN); }
bool crs_auto_trimming_enabled() {
    return READ_BIT(CRS->CR, CRS_CR_AUTOTRIMEN);
}

void crs_frequency_error_counter_enable() { SET_BIT(CRS->CR, CRS_CR_CEN); }
bool crs_frequency_error_counter_enabled() {
    return READ_BIT(CRS->CR, CRS_CR_CEN);
}
