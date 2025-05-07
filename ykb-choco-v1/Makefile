###############################################################################
# Board/MCU Definitions
###############################################################################
BOARD = STM32WB55xx
MCU   = cortex-m4

DEVICE_NAME = CHOCO_V1

###############################################################################
# Directories
###############################################################################
DEVICE_DIR = .
ROOT_DIR   = ..

DEVICE_SRC_DIR         = $(DEVICE_DIR)/src
DEVICE_INC_DIR         = $(DEVICE_DIR)/include

HW_DIR                 = $(ROOT_DIR)/hw/stm32wb

HAL_DIR                = $(HW_DIR)/hal
HAL_SRC_DIR            = $(HAL_DIR)/src
HAL_INC_DIR            = $(HAL_DIR)/include

CMSIS_DEVICE_INC_DIR   = $(HW_DIR)/cmsis-device/Include
STARTUP_SRC            = $(HW_DIR)/cmsis-device/Source/Templates/gcc/startup_stm32wb55xx_cm4.s

###############################################################################
# Settings
###############################################################################
DEVICE_LEFT_PID = 0x0003
DEVICE_RIGHT_PID = 0x0004

include ../common.mk
