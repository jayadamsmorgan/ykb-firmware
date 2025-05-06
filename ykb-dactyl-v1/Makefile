###############################################################################
# Board/MCU Definitions
###############################################################################
BOARD = STM32WB55xx
MCU   = cortex-m4

DEVICE_NAME = DACTYL_V1

###############################################################################
# Directories
###############################################################################
DEVICE_DIR = .
ROOT_DIR   = ..

DEVICE_SRC_DIR         = $(DEVICE_DIR)/src
DEVICE_INC_DIR         = $(DEVICE_DIR)/include

HAL_DIR                = $(ROOT_DIR)/stm32wb-hal
HAL_SRC_DIR            = $(HAL_DIR)/src
HAL_INC_DIR            = $(HAL_DIR)/include

CMSIS_DEVICE_INC_DIR   = $(ROOT_DIR)/cmsis-device-wb/Include
STARTUP_SRC            = $(ROOT_DIR)/cmsis-device-wb/Source/Templates/gcc/startup_stm32wb55xx_cm4.s

HW_DIR                 = $(ROOT_DIR)/hw/stm32wb

###############################################################################
# Settings
###############################################################################
DEVICE_LEFT_PID = 0x0001
DEVICE_RIGHT_PID = 0x0002

include ../common.mk
