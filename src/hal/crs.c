#include "hal/crs.h"

#include "hal/bits.h"
#include "stm32wbxx.h"

void crs_enable() { SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_CRSEN); }

void crs_config(crs_sync_polarity polarity, crs_sync_source source,
                crs_sync_divider divider, crs_frequency_limit_t limit,
                crs_counter_reload_t reload) {
    MODIFY_BITS(CRS->CFGR, CRS_CFGR_SYNCPOL_Pos, polarity, BITMASK_1BIT);
    MODIFY_BITS(CRS->CFGR, CRS_CFGR_SYNCSRC_Pos, source, BITMASK_2BIT);
    MODIFY_BITS(CRS->CFGR, CRS_CFGR_SYNCDIV_Pos, divider, BITMASK_3BIT);
    MODIFY_BITS(CRS->CFGR, CRS_CFGR_FELIM_Pos, limit, BITMASK_8BIT);
    MODIFY_BITS(CRS->CFGR, CRS_CFGR_RELOAD_Pos, reload, BITMASK_16BIT);
}

hal_err crs_set_smooth_trimming(crs_smooth_trim_t trim) {
    if (trim > 63U) {
        return ERR_CRS_SET_SM_T_INV_TRIM;
    }
    MODIFY_BITS(CRS->CR, CRS_CR_TRIM_Pos, trim, BITMASK_6BIT);
    return OK;
}

void crs_enable_auto_trimming() { SET_BIT(CRS->CR, CRS_CR_AUTOTRIMEN); }

void crs_enable_frequency_error_counter() { SET_BIT(CRS->CR, CRS_CR_CEN); }
