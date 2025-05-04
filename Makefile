###############################################################################
# Project Name
###############################################################################
PROJECT_NAME = ykb-firmware

###############################################################################
# Toolchain
###############################################################################
CC       = arm-none-eabi-gcc
OBJCOPY  = arm-none-eabi-objcopy
SIZE     = arm-none-eabi-size

###############################################################################
# Directories
###############################################################################
SRC_DIR                = src
INC_DIR                = include
BOOTLOADER_DIR         = bootloader
CONFIG_INC_DIR         = $(INC_DIR)/config
BUILD_DIR              = build
DEBUG_DIR              = $(BUILD_DIR)/debug
RELEASE_DIR            = $(BUILD_DIR)/release
LD_DIR                 = ld

DEBUG_BOOTLOADER_DIR   = $(DEBUG_DIR)/bootloader
DEBUG_LEFT_DIR         = $(DEBUG_DIR)/left
DEBUG_RIGHT_DIR        = $(DEBUG_DIR)/right
RELEASE_BOOTLOADER_DIR = $(RELEASE_DIR)/bootloader
RELEASE_LEFT_DIR       = $(RELEASE_DIR)/left
RELEASE_RIGHT_DIR      = $(RELEASE_DIR)/right

# YKB Protocol
YKB_PROTOCOL_INC_DIR   = ykb_protocol

# CMSIS / Device-Specific
CMSIS_INC_DIR          = CMSIS_5/CMSIS/Core/Include
CMSIS_DEVICE_INC_DIR   = cmsis-device-wb/Include
STARTUP_SRC            = cmsis-device-wb/Source/Templates/gcc/startup_stm32wb55xx_cm4.s
SYSTEM                 = cmsis-device-wb/Source/Templates/system_stm32wbxx.c
LD_SCRIPT              = $(LD_DIR)/application.ld
BOOTLOADER_LD_SCRIPT   = $(LD_DIR)/bootloader.ld

STARTUP_S              = $(BUILD_DIR)/startup_stm32wb55xx_cm4.S
STARTUP_OBJ            = $(BUILD_DIR)/startup_stm32wb55xx_cm4.o

###############################################################################
# Board/MCU Definitions
###############################################################################
BOARD = STM32WB55xx
MCU   = cortex-m4

###############################################################################
# Sources
###############################################################################
SRCS                = $(shell find $(SRC_DIR) -type f -name '*.c')

HEADERS             = $(shell find $(INC_DIR) -type f -name '*.h')
HEADERS            += $(YKB_PROTOCOL_INC_DIR)/ykb_protocol.h

BOOTLOADER_SRCS     = $(shell find $(BOOTLOADER_DIR) -type f -name '*.c')
BOOTLOADER_SRCS    += $(shell find $(SRC_DIR)/hal -type f -name '*.c')
BOOTLOADER_SRCS    += $(SYSTEM)
BOOTLOADER_SRCS    += $(SRC_DIR)/clock.c \
					  $(SRC_DIR)/error_handler.c \
					  $(SRC_DIR)/logging.c \
					  $(SRC_DIR)/syscalls.c

BOOTLOADER_HEADERS  = $(shell find $(INC_DIR)/hal -type f -name '*.h')
BOOTLOADER_HEADERS += $(shell find $(INC_DIR)/utils -type f -name '*.h')

###############################################################################
# Object Files
###############################################################################
DEBUG_LEFT_OBJS         = $(SRCS:%.c=$(DEBUG_LEFT_DIR)/%.o)
DEBUG_RIGHT_OBJS        = $(SRCS:%.c=$(DEBUG_RIGHT_DIR)/%.o)
DEBUG_BOOTLOADER_OBJS   = $(BOOTLOADER_SRCS:%.c=$(DEBUG_BOOTLOADER_DIR)/%.o)

RELEASE_LEFT_OBJS       = $(SRCS:%.c=$(RELEASE_LEFT_DIR)/%.o)
RELEASE_RIGHT_OBJS      = $(SRCS:%.c=$(RELEASE_RIGHT_DIR)/%.o)
RELEASE_BOOTLOADER_OBJS = $(BOOTLOADER_SRCS:%.c=$(RELEASE_BOOTLOADER_DIR)/%.o)

###############################################################################
# Compiler/Linker Flags
###############################################################################
INCLUDES                  = -I$(INC_DIR) \
                            -I$(CONFIG_INC_DIR) \
                            -I$(CMSIS_INC_DIR) \
                            -I$(CMSIS_DEVICE_INC_DIR) \
                            -I$(YKB_PROTOCOL_INC_DIR)

BOOTLOADER_INCLUDES       = -I$(INC_DIR) \
                            -I$(CONFIG_INC_DIR) \
                            -I$(CMSIS_INC_DIR) \
                            -I$(CMSIS_DEVICE_INC_DIR)

COMMON_FLAGS              = -Wall -Wextra -Werror \
                            -fdata-sections -ffunction-sections \
                            -std=gnu2x \
                            -fmacro-prefix-map=$(abspath .)=. \
                            -mlittle-endian -mthumb -mthumb-interwork \
                            -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mcpu=$(MCU) -D$(BOARD)

COMMON_LDFLAGS            = -lc -lm -lnosys --specs=nano.specs -Wl,--gc-sections
LDFLAGS                   = -T $(LD_SCRIPT) $(COMMON_LDFLAGS)
BOOTLOADER_LDFLAGS        = -T $(BOOTLOADER_LD_SCRIPT) $(COMMON_LDFLAGS)

GIT_HASH                  = $(shell git describe --dirty=+ --always)

DEBUG_CFLAGS              = -g -gdwarf-2 -Og -DHAL_LPUART_ENABLED -DDEBUG -DGIT_HASH=\"$(GIT_HASH)\"
DEBUG_BOOTLOADER_CFLAGS   = $(DEBUG_CFLAGS) -DBOOTLOADER
RELEASE_CFLAGS            = -O3
RELEASE_BOOTLOADER_CFLAGS = -Os -DBOOTLOADER

LEFT_FLAG  = -DLEFT
RIGHT_FLAG = -DRIGHT

###############################################################################
# Top-Level Targets
###############################################################################
.PHONY: clean stflash dfuflash help \
        debug debug-left debug-right \
        release release-left release-right \
        stflash-left stflash-right dfuflash-left dfuflash-right \
        bootloader \
		debug-right-full debug-left-full release-right-full release-left-full

# By default, build debug
all: debug bootloader

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
debug: debug-left-full debug-right-full
release: release-left-full release-right-full


###############################################################################
# Debug Bootloader
###############################################################################
DEBUG_BOOTLOADER_ELF = $(DEBUG_BOOTLOADER_DIR)/$(PROJECT_NAME)-bootloader.elf
DEBUG_BOOTLOADER_BIN = $(DEBUG_BOOTLOADER_DIR)/$(PROJECT_NAME)-bootloader.bin

debug-bootloader: $(DEBUG_BOOTLOADER_BIN)

$(DEBUG_BOOTLOADER_ELF): $(DEBUG_BOOTLOADER_OBJS) $(STARTUP_OBJ)
	@echo "Linking bootloader..."
	$(CC) $(COMMON_FLAGS) $(BOOTLOADER_INCLUDES) $(DEBUG_BOOTLOADER_CFLAGS) $^ -o $@ $(BOOTLOADER_LDFLAGS)

$(DEBUG_BOOTLOADER_BIN): $(DEBUG_BOOTLOADER_ELF)
	$(OBJCOPY) -O binary $< $@

$(DEBUG_BOOTLOADER_DIR)/%.o: %.c $(BOOTLOADER_HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(BOOTLOADER_INCLUDES) $(DEBUG_BOOTLOADER_CFLAGS) -c $< -o $@

###############################################################################
# Release Bootloader
###############################################################################

RELEASE_BOOTLOADER_ELF = $(RELEASE_BOOTLOADER_DIR)/$(PROJECT_NAME)-bootloader.elf
RELEASE_BOOTLOADER_BIN = $(RELEASE_BOOTLOADER_DIR)/$(PROJECT_NAME)-bootloader.bin

release-bootloader: $(RELEASE_BOOTLOADER_BIN)

$(RELEASE_BOOTLOADER_ELF): $(RELEASE_BOOTLOADER_OBJS) $(STARTUP_OBJ)
	@echo "Linking bootloader..."
	$(CC) $(COMMON_FLAGS) $(BOOTLOADER_INCLUDES) $(RELEASE_BOOTLOADER_CFLAGS) $^ -o $@ $(BOOTLOADER_LDFLAGS)

$(RELEASE_BOOTLOADER_BIN): $(RELEASE_BOOTLOADER_ELF)
	$(OBJCOPY) -O binary $< $@

$(RELEASE_BOOTLOADER_DIR)/%.o: %.c $(BOOTLOADER_HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(BOOTLOADER_INCLUDES) $(RELEASE_BOOTLOADER_CFLAGS) -c $< -o $@

###############################################################################
# Debug Left
###############################################################################
DEBUG_LEFT_ELF = $(DEBUG_LEFT_DIR)/$(PROJECT_NAME)-left.elf
DEBUG_LEFT_BIN = $(DEBUG_LEFT_DIR)/$(PROJECT_NAME)-left.bin

debug-left: $(DEBUG_LEFT_BIN)

$(DEBUG_LEFT_ELF): $(DEBUG_LEFT_OBJS) $(STARTUP_OBJ)
	@echo "Linking Debug Left..."
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(DEBUG_CFLAGS) $(LEFT_FLAG) $^ -o $@ $(LDFLAGS)

$(DEBUG_LEFT_BIN): $(DEBUG_LEFT_ELF)
	$(OBJCOPY) -O binary $< $@

$(DEBUG_LEFT_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(DEBUG_CFLAGS) $(LEFT_FLAG) -c $< -o $@

###############################################################################
# Debug Right
###############################################################################
DEBUG_RIGHT_ELF = $(DEBUG_RIGHT_DIR)/$(PROJECT_NAME)-right.elf
DEBUG_RIGHT_BIN = $(DEBUG_RIGHT_DIR)/$(PROJECT_NAME)-right.bin

debug-right: $(DEBUG_RIGHT_BIN)

$(DEBUG_RIGHT_ELF): $(DEBUG_RIGHT_OBJS) $(STARTUP_OBJ)
	@echo "Linking Debug Right..."
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(DEBUG_CFLAGS) $(RIGHT_FLAG) $^ -o $@ $(LDFLAGS)

$(DEBUG_RIGHT_BIN): $(DEBUG_RIGHT_ELF)
	$(OBJCOPY) -O binary $< $@

$(DEBUG_RIGHT_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(DEBUG_CFLAGS) $(RIGHT_FLAG) -c $< -o $@

###############################################################################
# Release Left
###############################################################################
RELEASE_LEFT_ELF = $(RELEASE_LEFT_DIR)/$(PROJECT_NAME)-left.elf
RELEASE_LEFT_BIN = $(RELEASE_LEFT_DIR)/$(PROJECT_NAME)-left.bin

release-left: $(RELEASE_LEFT_BIN)

$(RELEASE_LEFT_ELF): $(RELEASE_LEFT_OBJS) $(STARTUP_OBJ)
	@echo "Linking Release Left..."
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(RELEASE_CFLAGS) $(LEFT_FLAG) $^ -o $@ $(LDFLAGS)

$(RELEASE_LEFT_BIN): $(RELEASE_LEFT_ELF)
	$(OBJCOPY) -O binary $< $@

$(RELEASE_LEFT_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(RELEASE_CFLAGS) $(LEFT_FLAG) -c $< -o $@

###############################################################################
# Release Right
###############################################################################
RELEASE_RIGHT_ELF = $(RELEASE_RIGHT_DIR)/$(PROJECT_NAME)-right.elf
RELEASE_RIGHT_BIN = $(RELEASE_RIGHT_DIR)/$(PROJECT_NAME)-right.bin

release-right: $(RELEASE_RIGHT_BIN)

$(RELEASE_RIGHT_ELF): $(RELEASE_RIGHT_OBJS) $(STARTUP_OBJ)
	@echo "Linking Release Right..."
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(RELEASE_CFLAGS) $(RIGHT_FLAG) $^ -o $@ $(LDFLAGS)

$(RELEASE_RIGHT_BIN): $(RELEASE_RIGHT_ELF)
	$(OBJCOPY) -O binary $< $@

$(RELEASE_RIGHT_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(RELEASE_CFLAGS) $(RIGHT_FLAG) -c $< -o $@

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
# Full firmware
###############################################################################
DEBUG_RIGHT_FULL_HEX = $(DEBUG_RIGHT_DIR)/$(PROJECT_NAME)-right-full.hex
DEBUG_RIGHT_FULL_BIN = $(DEBUG_RIGHT_DIR)/$(PROJECT_NAME)-right-full.bin

debug-right-full: $(DEBUG_RIGHT_FULL_BIN)

$(DEBUG_RIGHT_FULL_HEX): $(DEBUG_BOOTLOADER_BIN) $(DEBUG_RIGHT_BIN)
	@echo "Merging Debug Right into full firmware..."
	srec_cat $(DEBUG_BOOTLOADER_BIN) -Binary -offset 0x00000000 \
	         $(DEBUG_RIGHT_BIN) -Binary -offset 0x00014000 \
	         -o $@ -Intel

$(DEBUG_RIGHT_FULL_BIN): $(DEBUG_RIGHT_FULL_HEX)
	srec_cat $< -Intel -o $@ -Binary

DEBUG_LEFT_FULL_HEX = $(DEBUG_LEFT_DIR)/$(PROJECT_NAME)-left-full.hex
DEBUG_LEFT_FULL_BIN = $(DEBUG_LEFT_DIR)/$(PROJECT_NAME)-left-full.bin

debug-left-full: $(DEBUG_LEFT_FULL_BIN)

$(DEBUG_LEFT_FULL_HEX): $(DEBUG_BOOTLOADER_BIN) $(DEBUG_LEFT_BIN)
	@echo "Merging Debug Right into full firmware..."
	srec_cat $(DEBUG_BOOTLOADER_BIN) -Binary -offset 0x00000000 \
	         $(DEBUG_LEFT_BIN) -Binary -offset 0x00014000 \
	         -o $@ -Intel

$(DEBUG_LEFT_FULL_BIN): $(DEBUG_LEFT_FULL_HEX)
	srec_cat $< -Intel -o $@ -Binary

RELEASE_RIGHT_FULL_HEX = $(RELEASE_RIGHT_DIR)/$(PROJECT_NAME)-right-full.hex
RELEASE_RIGHT_FULL_BIN = $(RELEASE_RIGHT_DIR)/$(PROJECT_NAME)-right-full.bin

release-right-full: $(RELEASE_RIGHT_FULL_BIN)

$(RELEASE_RIGHT_FULL_HEX): $(RELEASE_BOOTLOADER_BIN) $(RELEASE_RIGHT_BIN)
	@echo "Merging Debug Right into full firmware..."
	srec_cat $(RELEASE_BOOTLOADER_BIN) -Binary -offset 0x00000000 \
	         $(RELEASE_RIGHT_BIN) -Binary -offset 0x00014000 \
	         -o $@ -Intel

$(RELEASE_RIGHT_FULL_BIN): $(RELEASE_RIGHT_FULL_HEX)
	srec_cat $< -Intel -o $@ -Binary

RELEASE_LEFT_FULL_HEX = $(RELEASE_LEFT_DIR)/$(PROJECT_NAME)-left-full.hex
RELEASE_LEFT_FULL_BIN = $(RELEASE_LEFT_DIR)/$(PROJECT_NAME)-left-full.bin

release-left-full: $(RELEASE_LEFT_FULL_BIN)

$(RELEASE_LEFT_FULL_HEX): $(RELEASE_BOOTLOADER_BIN) $(RELEASE_LEFT_BIN)
	@echo "Merging Debug Right into full firmware..."
	srec_cat $(RELEASE_BOOTLOADER_BIN) -Binary -offset 0x00000000 \
	         $(RELEASE_LEFT_BIN) -Binary -offset 0x00014000 \
	         -o $@ -Intel

$(RELEASE_LEFT_FULL_BIN): $(RELEASE_LEFT_FULL_HEX)
	srec_cat $< -Intel -o $@ -Binary


###############################################################################
# Size
###############################################################################

size-bootloader: $(BOOTLOADER_ELF)
	@$(SIZE) $<

size-debug-left: $(DEBUG_LEFT_ELF)
	@$(SIZE) $<

size-debug-right: $(DEBUG_RIGHT_ELF)
	@$(SIZE) $<

size-release-left: $(RELEASE_LEFT_ELF)
	@$(SIZE) $<

size-release-right: $(RELEASE_RIGHT_ELF)
	@$(SIZE) $<

###############################################################################
# Clean
###############################################################################
clean:
	rm -rf $(BUILD_DIR)
