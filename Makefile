###############################################################################
# Project Name
###############################################################################
PROJECT_NAME = dactyl-cc-he-firmware

###############################################################################
# Toolchain
###############################################################################
CC       = arm-none-eabi-gcc
OBJCOPY  = arm-none-eabi-objcopy
SIZE     = arm-none-eabi-size

###############################################################################
# Directories
###############################################################################
SRC_DIR        = src
INC_DIR        = include
CONFIG_INC_DIR = $(INC_DIR)/config
BUILD_DIR      = build
DEBUG_DIR      = $(BUILD_DIR)/debug
RELEASE_DIR    = $(BUILD_DIR)/release

DEBUG_LEFT_DIR   = $(DEBUG_DIR)/left
DEBUG_RIGHT_DIR  = $(DEBUG_DIR)/right
RELEASE_LEFT_DIR = $(RELEASE_DIR)/left
RELEASE_RIGHT_DIR= $(RELEASE_DIR)/right

# CMSIS / Device-Specific
CMSIS_INC_DIR        = CMSIS_5/CMSIS/Core/Include
CMSIS_DEVICE_INC_DIR = cmsis-device-wb/Include
STARTUP_SRC          = cmsis-device-wb/Source/Templates/gcc/startup_stm32wb55xx_cm4.s
SYSTEM               = cmsis-device-wb/Source/Templates/system_stm32wbxx.c
LD_SCRIPT            = cmsis-device-wb/Source/Templates/gcc/linker/stm32wb55xx_flash_cm4.ld

STARTUP_S   = $(BUILD_DIR)/startup_stm32wb55xx_cm4.S
STARTUP_OBJ = $(BUILD_DIR)/startup_stm32wb55xx_cm4.o

###############################################################################
# Board/MCU Definitions
###############################################################################
BOARD = STM32WB55xx
MCU   = cortex-m4

###############################################################################
# Sources
###############################################################################
SRCS = $(shell find $(SRC_DIR) -type f -name '*.c')
SRCS += $(SYSTEM)

HEADERS = $(shell find $(INC_DIR) -type f -name '*.h')

###############################################################################
# Object Files
###############################################################################
DEBUG_LEFT_OBJS   = $(SRCS:%.c=$(DEBUG_LEFT_DIR)/%.o)
DEBUG_RIGHT_OBJS  = $(SRCS:%.c=$(DEBUG_RIGHT_DIR)/%.o)

RELEASE_LEFT_OBJS   = $(SRCS:%.c=$(RELEASE_LEFT_DIR)/%.o)
RELEASE_RIGHT_OBJS  = $(SRCS:%.c=$(RELEASE_RIGHT_DIR)/%.o)

###############################################################################
# Compiler/Linker Flags
###############################################################################
GIT_HASH = $(shell git describe --dirty=+ --always)

INCLUDES = -I$(INC_DIR) \
           -I$(CONFIG_INC_DIR) \
           -I$(CMSIS_INC_DIR) \
           -I$(CMSIS_DEVICE_INC_DIR)

COMMON_FLAGS = -Wall -Wextra -Werror \
               -fdata-sections -ffunction-sections \
               -std=gnu2x \
               -mlittle-endian -mthumb -mthumb-interwork \
               -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mcpu=$(MCU) -D$(BOARD) \
               $(INCLUDES)

LDFLAGS = -T $(LD_SCRIPT) -lc -lm -lnosys --specs=nano.specs -Wl,--gc-sections

DEBUG_CFLAGS   = -g -gdwarf-2 -Og -DHAL_UART_ENABLED -DDEBUG -DGIT_HASH=\"$(GIT_HASH)\"
RELEASE_CFLAGS = -O2

LEFT_FLAG  = -DLEFT
RIGHT_FLAG = -DRIGHT

###############################################################################
# Top-Level Targets
###############################################################################
.PHONY: clean stflash dfuflash size help \
        debug debug-left debug-right \
        release release-left release-right \
        stflash-left stflash-right dfuflash-left dfuflash-right

# By default, build debug
all: debug

help:
	@echo "Usage:"
	@echo "  make [debug | release | clean | stflash | dfuflash | size]"
	@echo "  Also you can build specific sides with e.g. debug-left, debug-right, etc."
	@echo
	@echo "Targets:"
	@echo "  debug         Build Debug (both Left and Right)"
	@echo "  release       Build Release (both Left and Right)"
	@echo "  debug-left    Build Debug Left"
	@echo "  debug-right   Build Debug Right"
	@echo "  release-left  Build Release Left"
	@echo "  release-right Build Release Right"
	@echo "  clean         Remove all build artifacts"
	@echo "  stflash       Flash the built release-right binary by default"
	@echo "  dfuflash      Flash the built release-right binary by default"
	@echo "  size          Show the size of release-right ELF by default"

###############################################################################
# Aggregate Targets
###############################################################################
debug: debug-left debug-right
release: release-left release-right

###############################################################################
# Debug Left
###############################################################################
DEBUG_LEFT_ELF = $(DEBUG_LEFT_DIR)/$(PROJECT_NAME)-left.elf
DEBUG_LEFT_BIN = $(DEBUG_LEFT_DIR)/$(PROJECT_NAME)-left.bin

debug-left: $(DEBUG_LEFT_BIN)

$(DEBUG_LEFT_ELF): $(DEBUG_LEFT_OBJS) $(STARTUP_OBJ)
	@echo "Linking Debug Left..."
	$(CC) $(COMMON_FLAGS) $(DEBUG_CFLAGS) $(LEFT_FLAG) $^ -o $@ $(LDFLAGS)

$(DEBUG_LEFT_BIN): $(DEBUG_LEFT_ELF)
	$(OBJCOPY) -O binary $< $@

$(DEBUG_LEFT_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(DEBUG_CFLAGS) $(LEFT_FLAG) -c $< -o $@

###############################################################################
# Debug Right
###############################################################################
DEBUG_RIGHT_ELF = $(DEBUG_RIGHT_DIR)/$(PROJECT_NAME)-right.elf
DEBUG_RIGHT_BIN = $(DEBUG_RIGHT_DIR)/$(PROJECT_NAME)-right.bin

debug-right: $(DEBUG_RIGHT_BIN)

$(DEBUG_RIGHT_ELF): $(DEBUG_RIGHT_OBJS) $(STARTUP_OBJ)
	@echo "Linking Debug Right..."
	$(CC) $(COMMON_FLAGS) $(DEBUG_CFLAGS) $(RIGHT_FLAG) $^ -o $@ $(LDFLAGS)

$(DEBUG_RIGHT_BIN): $(DEBUG_RIGHT_ELF)
	$(OBJCOPY) -O binary $< $@

$(DEBUG_RIGHT_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(DEBUG_CFLAGS) $(RIGHT_FLAG) -c $< -o $@

###############################################################################
# Release Left
###############################################################################
RELEASE_LEFT_ELF = $(RELEASE_LEFT_DIR)/$(PROJECT_NAME)-left.elf
RELEASE_LEFT_BIN = $(RELEASE_LEFT_DIR)/$(PROJECT_NAME)-left.bin

release-left: $(RELEASE_LEFT_BIN)

$(RELEASE_LEFT_ELF): $(RELEASE_LEFT_OBJS) $(STARTUP_OBJ)
	@echo "Linking Release Left..."
	$(CC) $(COMMON_FLAGS) $(RELEASE_CFLAGS) $(LEFT_FLAG) $^ -o $@ $(LDFLAGS)

$(RELEASE_LEFT_BIN): $(RELEASE_LEFT_ELF)
	$(OBJCOPY) -O binary $< $@

$(RELEASE_LEFT_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(RELEASE_CFLAGS) $(LEFT_FLAG) -c $< -o $@

###############################################################################
# Release Right
###############################################################################
RELEASE_RIGHT_ELF = $(RELEASE_RIGHT_DIR)/$(PROJECT_NAME)-right.elf
RELEASE_RIGHT_BIN = $(RELEASE_RIGHT_DIR)/$(PROJECT_NAME)-right.bin

release-right: $(RELEASE_RIGHT_BIN)

$(RELEASE_RIGHT_ELF): $(RELEASE_RIGHT_OBJS) $(STARTUP_OBJ)
	@echo "Linking Release Right..."
	$(CC) $(COMMON_FLAGS) $(RELEASE_CFLAGS) $(RIGHT_FLAG) $^ -o $@ $(LDFLAGS)

$(RELEASE_RIGHT_BIN): $(RELEASE_RIGHT_ELF)
	$(OBJCOPY) -O binary $< $@

$(RELEASE_RIGHT_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(RELEASE_CFLAGS) $(RIGHT_FLAG) -c $< -o $@

###############################################################################
# Single Startup Object (common to all)
###############################################################################
# Copy .s to .S so it can be preprocessed (if needed), then compile once
$(STARTUP_S): $(STARTUP_SRC)
	@mkdir -p $(dir $@)
	cp $< $@

$(STARTUP_OBJ): $(STARTUP_S)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) -c $< -o $@

###############################################################################
# Size
###############################################################################
# By default, show the size of the 'release-right' ELF.
size: size-right

size-left: $(RELEASE_LEFT_ELF)
	@$(SIZE) $<

size-right: $(RELEASE_RIGHT_ELF)
	@$(SIZE) $<

size-debug-left: $(DEBUG_LEFT_ELF)
	@$(SIZE) $<

size-debug-right: $(DEBUG_RIGHT_ELF)
	@$(SIZE) $<

###############################################################################
# Flash Targets
###############################################################################
# By default, flash release-right to address 0x08000000.
stflash: stflash-right

stflash-right: $(RELEASE_RIGHT_BIN)
	@echo "Flashing (Right) via st-flash..."
	st-flash --reset write $< 0x08000000

stflash-left: $(RELEASE_LEFT_BIN)
	@echo "Flashing (Left) via st-flash..."
	st-flash --reset write $< 0x08000000

dfuflash: dfuflash-right

dfuflash-right: $(RELEASE_RIGHT_BIN)
	@echo "Flashing (Right) via dfu-util..."
	dfu-util -D $< -a 0 -s 0x08000000

dfuflash-left: $(RELEASE_LEFT_BIN)
	@echo "Flashing (Left) via dfu-util..."
	dfu-util -D $< -a 0 -s 0x08000000

###############################################################################
# Clean
###############################################################################
clean:
	rm -rf $(BUILD_DIR)
