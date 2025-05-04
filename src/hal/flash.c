#include "hal/flash.h"

#include "hal/bits.h"
#include "hal/hal_err.h"
#include "hal/systick.h"
#include "stm32wbxx.h"
#include "utils/utils.h"

#include <stdint.h>

static flash_state_t flash_state = {.address = 0U,
                                    .amount_to_erase = 0U,
                                    .error_code = HAL_FLASH_ERROR_NONE,
                                    .lock = false,
                                    .page = 0U,
                                    .procedure_ongoing = 0U};

flash_state_t *flash_get_state() { return &flash_state; }

void flash_select_latency(flash_latency latency) {
    MODIFY_BITS(FLASH->ACR, FLASH_ACR_LATENCY_Pos, latency, BITMASK_2BIT);
}

flash_latency flash_get_latency() {
    return READ_BITS(FLASH->ACR, FLASH_ACR_LATENCY_Pos, BITMASK_4BIT);
}

void flash_enable_prefetch() { SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN); }

hal_err flash_unlock() {

    if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U) {
        WRITE_REG(FLASH->KEYR, FLASH_KEY1);
        WRITE_REG(FLASH->KEYR, FLASH_KEY2);

        if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U) {
            return ERR_FLASH_UNLOCK;
        }
    }

    return OK;
}

hal_err flash_lock() {

    SET_BIT(FLASH->CR, FLASH_CR_LOCK);

    if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) == 0U) {
        return ERR_FLASH_LOCK;
    }

    return OK;
}

hal_err flash_wait_for_last_operation(uint32_t timeout) {

    uint32_t tick_start = systick_get_tick();

    while (FLASH_GET_FLAG(FLASH_FLAG_BSY)) {
        if (systick_get_tick() - tick_start >= timeout) {
            return ERR_FLASH_WFOP_BSY_TIMEOUT;
        }
    }

    uint32_t error = FLASH->SR;

    if ((error & FLASH_FLAG_EOP)) {
        FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
    }

    if (error & FLASH_FLAG_OPTVERR) {
        FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
        error &= ~FLASH_FLAG_OPTVERR;
    }

    error &= FLASH_FLAG_SR_ERRORS;

    FLASH_CLEAR_FLAG(error);

    if (error) {
        flash_state.error_code = error;
        return ERR_FLASH_WFOP_ERRFLAG;
    }

    while (FLASH_GET_FLAG(FLASH_FLAG_CFGBSY)) {
        if (systick_get_tick() - tick_start >= timeout) {
            return ERR_FLASH_WFOP_CFGBSY_TIMEOUT;
        }
    }

    return OK;
}

hal_err flash_page_erase(uint32_t page) {
    if (page >= FLASH_PAGE_NB) {
        return ERR_FLASH_PAGE_ERASE_NOTAPAGE;
    }
    MODIFY_REG(FLASH->CR, FLASH_CR_PNB,
               ((page << FLASH_CR_PNB_Pos) | FLASH_CR_PER | FLASH_CR_STRT));
    return OK;
}

static void flash_acknowledge_page_erase(void) {
    CLEAR_BIT(FLASH->CR, (FLASH_CR_PER | FLASH_CR_PNB));
}

hal_err flash_erase(flash_page start_page, uint32_t page_amount,
                    uint32_t *page_error) {

    hal_err err = OK;

    if (flash_state.lock) {
        return ERR_FLASH_ERASE_BUSY;
    }
    flash_state.lock = true;

    flash_state.error_code = HAL_FLASH_ERROR_NONE;

    err = flash_wait_for_last_operation(FLASH_TIMEOUT_VALUE);
    if (err) {
        flash_state.lock = false;
        return err;
    }

    if (page_error) {
        *page_error = 0xFFFFFFFFU;
    }

    uint32_t page_number = flash_get_page(start_page);

    for (uint32_t index = page_number; index < (page_number + page_amount);
         index++) {

        err = flash_page_erase(index);
        if (err) {
            if (page_error) {
                *page_error = index;
            }
            break;
        }

        err = flash_wait_for_last_operation(FLASH_TIMEOUT_VALUE);
        if (err) {
            if (page_error) {
                *page_error = index;
            }
            break;
        }
    }

    flash_acknowledge_page_erase();

    flash_state.lock = false;
    return err;
}

static void flash_program_doubleword(uint32_t address, uint64_t data) {
    /* Set PG bit */
    SET_BIT(FLASH->CR, FLASH_CR_PG);

    /* Program first word */
    *(uint32_t *)address = (uint32_t)data;

    /* Barrier to ensure programming is performed in 2 steps, in right order
      (independently of compiler optimization behavior) */
    __ISB();

    /* Program second word */
    *(uint32_t *)(address + 4U) = (uint32_t)(data >> 32U);
}

static __RAM_FUNC void flash_program_fast(uint32_t address,
                                          uint32_t data_address) {
    uint8_t row_index = 128;
    __IO uint32_t *dest_addr = (__IO uint32_t *)address;
    __IO uint32_t *src_addr = (__IO uint32_t *)data_address;
    uint32_t primask_bit;

    /* Set FSTPG bit */
    SET_BIT(FLASH->CR, FLASH_CR_FSTPG);

    /* Enter critical section: row programming should not be longer than 7 ms */
    primask_bit = __get_PRIMASK();
    __disable_irq();

    /* Program the double word of the row */
    do {
        *dest_addr = *src_addr;
        dest_addr++;
        src_addr++;
        row_index--;
    } while (row_index != 0U);

    /* wait for BSY in order to be sure that flash operation is ended before
       allowing prefetch in flash. Timeout does not return status, as it will
       be anyway done later */
    while (FLASH_GET_FLAG(FLASH_FLAG_BSY) != 0U) {
    }

    /* Exit critical section: restore previous priority mask */
    __set_PRIMASK(primask_bit);
}

hal_err flash_program(flash_typeprogram typeprogram, uint32_t address,
                      uint64_t data) {

    hal_err err = OK;

    if (!IS_ADDR_ALIGNED_64BITS(address)) {
        return ERR_FLASH_PROGRAM_ADDRNOTALIGNED;
    }

    if (!IS_FLASH_PROGRAM_ADDRESS(address)) {
        return ERR_FLASH_PROGRAM_NOTPROGRAMADDR;
    }

    if (typeprogram == FLASH_TYPEPROGRAM_FAST &&
        !IS_FLASH_FAST_PROGRAM_ADDRESS(address)) {
        return ERR_FLASH_PROGRAM_ADDRNOTFASTPROG;
    }

    if (flash_state.lock) {
        return ERR_FLASH_PROGRAM_BUSY;
    }
    flash_state.lock = true;

    flash_state.error_code = HAL_FLASH_ERROR_NONE;

    err = flash_wait_for_last_operation(FLASH_TIMEOUT_VALUE);
    if (err) {
        flash_state.lock = false;
        return err;
    }

    switch (typeprogram) {
    case FLASH_TYPEPROGRAM_DOUBLEWORD:
        flash_program_doubleword(address, data);
        break;
    case FLASH_TYPEPROGRAM_FAST:
        flash_program_fast(address, (uint32_t)data);
        break;
    }

    err = flash_wait_for_last_operation(FLASH_TIMEOUT_VALUE);

    CLEAR_BIT(FLASH->CR, typeprogram);

    flash_state.lock = false;
    return err;
}
