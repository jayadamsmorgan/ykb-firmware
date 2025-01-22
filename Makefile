CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

BOARD = STM32WB55xx
MCU = cortex-m4

STARTUP = cmsis-device-wb/Source/Templates/gcc/startup_stm32wb55xx_cm4.s
SYSTEM = cmsis-device-wb/Source/Templates/system_stm32wbxx.c
LD_SCRIPT = cmsis-device-wb/Source/Templates/gcc/linker/stm32wb55xx_flash_cm4.ld
CMSIS_INC_DIR = CMSIS_5/CMSIS/Core/Include
CMSIS_DEVICE_INC_DIR = cmsis-device-wb/Include
INC_DIR = include
SRC_DIR = src
BUILD_DIR = build

SRCS = $(shell find $(SRC_DIR) -type f -name '*.c')

SRCS += $(STARTUP) \
	    $(SYSTEM)

CFLAGS = -g -O2 -Wall -Werror -T $(LD_SCRIPT) \
		 -mlittle-endian -mthumb -mcpu=$(MCU) -mthumb-interwork \
		 -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
		 --specs=nosys.specs \
		 -I$(CMSIS_INC_DIR) \
		 -I$(CMSIS_DEVICE_INC_DIR) \
		 -I$(INC_DIR) \
		 -D$(BOARD) \
		 $(EXTRA_CFLAGS)

compile:
	@mkdir -p build
	$(CC) $(CFLAGS) $(SRCS) -o $(BUILD_DIR)/firmware.elf
	$(OBJCOPY) -O binary $(BUILD_DIR)/firmware.elf $(BUILD_DIR)/firmware.bin

stflash: $(BUILD_DIR)/firmware.bin
	st-flash --reset write $< 0x08000000

dfuflash: $(BUILD_DIR)/firmware.bin
	dfu-util -D $(BUILD_DIR)/firmware.bin -n 26 -a 0

clean:
	rm -rf $(BUILD_DIR)/*

compile_commands: clean
	@compiledb make

.PHONY: compile clean flash compile_commands
