###############################################################################
# Project Name
###############################################################################
PROJECT_NAME = ykb-$(DEVICE_NAME)-fw

###############################################################################
# Toolchain
###############################################################################
CC       = arm-none-eabi-gcc
OBJCOPY  = arm-none-eabi-objcopy
SIZE     = arm-none-eabi-size

###############################################################################
# Directories
###############################################################################
COMMON_DIR			     = $(ROOT_DIR)/common
COMMON_SRC_DIR           = $(COMMON_DIR)/src
COMMON_INC_DIR           = $(COMMON_DIR)/include
BOOTLOADER_DIR           = $(ROOT_DIR)/bootloader
CONFIG_INC_DIR           = $(DEVICE_DIR)/config

BUILD_DIR                = $(DEVICE_DIR)/build
DEBUG_DIR                = $(BUILD_DIR)/debug
RELEASE_DIR              = $(BUILD_DIR)/release
BUILD_OUTPUTS_DIR        = $(BUILD_DIR)/outputs

LD_SCRIPT                = $(HW_DIR)/ld/application.ld

STARTUP_S                = $(BUILD_DIR)/startup.S
STARTUP_OBJ              = $(BUILD_DIR)/startup.o

DEBUG_LEFT_DIR           = $(DEBUG_DIR)/left/device
DEBUG_RIGHT_DIR          = $(DEBUG_DIR)/right/device
RELEASE_LEFT_DIR         = $(RELEASE_DIR)/left/device
RELEASE_RIGHT_DIR        = $(RELEASE_DIR)/right/device

CMSIS_INC_DIR            = $(ROOT_DIR)/CMSIS_6/CMSIS/Core/Include

# YKB Protocol
YKB_PROTOCOL_INC_DIR     = $(ROOT_DIR)/ykb_protocol

###############################################################################
# Sources
###############################################################################
SRCS                = $(shell find $(COMMON_SRC_DIR) -type f -name '*.c')
SRCS               += $(shell find $(DEVICE_SRC_DIR) -type f -name '*.c')
SRCS               += $(shell find $(HAL_SRC_DIR) -type f -name '*.c')

SYSTEM             += $(HW_DIR)/system/application.c

HEADERS             = $(shell find $(COMMON_INC_DIR) -type f -name '*.h')
HEADERS            += $(shell find $(HAL_INC_DIR) -type f -name '*.h')
HEADERS            += $(shell find $(CONFIG_INC_DIR) -type f -name '*.h')
HEADERS            += $(YKB_PROTOCOL_INC_DIR)/ykb_protocol.h

###############################################################################
# Object Files
###############################################################################
DEBUG_LEFT_OBJS         = $(SRCS:%.c=$(DEBUG_LEFT_DIR)/%.o)
DEBUG_RIGHT_OBJS        = $(SRCS:%.c=$(DEBUG_RIGHT_DIR)/%.o)
DEBUG_SYSTEM            = $(DEBUG_DIR)/system.o

RELEASE_LEFT_OBJS       = $(SRCS:%.c=$(RELEASE_LEFT_DIR)/%.o)
RELEASE_RIGHT_OBJS      = $(SRCS:%.c=$(RELEASE_RIGHT_DIR)/%.o)
RELEASE_SYSTEM          = $(RELEASE_DIR)/system.o

###############################################################################
# Compiler/Linker Flags
###############################################################################
INCLUDES                  = -I$(COMMON_INC_DIR) \
							-I$(DEVICE_INC_DIR) \
							-I$(HAL_INC_DIR) \
                            -I$(CONFIG_INC_DIR) \
                            -I$(CMSIS_INC_DIR) \
                            -I$(CMSIS_DEVICE_INC_DIR) \
                            -I$(YKB_PROTOCOL_INC_DIR)

COMMON_FLAGS              = -Wall -Wextra -Werror \
                            -fdata-sections -ffunction-sections \
                            -std=gnu2x \
                            -fmacro-prefix-map=$(abspath $(ROOT_DIR))=. \
                            -mlittle-endian -mthumb -mthumb-interwork \
                            -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mcpu=$(MCU) -D$(BOARD) -D$(DEVICE_NAME)

COMMON_LDFLAGS            = -lc -lm -lnosys --specs=nano.specs -Wl,--gc-sections
LDFLAGS                   = -T $(LD_SCRIPT) $(COMMON_LDFLAGS)

GIT_HASH                  = $(shell git describe --dirty=+ --always)

DEBUG_CFLAGS              = -g -gdwarf-2 -Og -DDEBUG -DGIT_HASH=\"$(GIT_HASH)\"
RELEASE_CFLAGS            = -O3

LEFT_FLAG  = -DLEFT -DUSBD_PID=$(DEVICE_LEFT_PID)
RIGHT_FLAG = -DRIGHT -DUSBD_PID=$(DEVICE_RIGHT_PID)

###############################################################################
# Top-Level Targets
###############################################################################
.PHONY: clean stflash dfuflash help \
        debug debug-left debug-right \
        release release-left release-right \
        stflash-left stflash-right dfuflash-left dfuflash-right \
        bootloader \
		debug-right-full debug-left-full release-right-full release-left-full

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

include $(BOOTLOADER_DIR)/bootloader.mk

###############################################################################
# System
###############################################################################

$(DEBUG_SYSTEM):
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(DEBUG_CFLAGS) $(INCLUDES) -c $(SYSTEM) -o $@

$(RELEASE_SYSTEM):
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(RELEASE_CFLAGS) $(INCLUDES) -c $(SYSTEM) -o $@

###############################################################################
# Debug Left
###############################################################################
DEBUG_LEFT_ELF = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-debug-left.elf
DEBUG_LEFT_BIN = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-debug-left.bin

debug-left: $(DEBUG_LEFT_BIN)

$(DEBUG_LEFT_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(DEBUG_CFLAGS) $(LEFT_FLAG) -c $< -o $@

$(DEBUG_LEFT_ELF): $(DEBUG_LEFT_OBJS) $(DEBUG_SYSTEM) $(STARTUP_OBJ)
	@mkdir -p $(dir $@)
	@echo "Linking Debug Left..."
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(DEBUG_CFLAGS) $^ -o $@ $(LDFLAGS)

$(DEBUG_LEFT_BIN): $(DEBUG_LEFT_ELF)
	$(OBJCOPY) -O binary $< $@

###############################################################################
# Debug Right
###############################################################################
DEBUG_RIGHT_ELF = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-debug-right.elf
DEBUG_RIGHT_BIN = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-debug-right.bin

debug-right: $(DEBUG_RIGHT_BIN)

$(DEBUG_RIGHT_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(DEBUG_CFLAGS) $(RIGHT_FLAG) -c $< -o $@

$(DEBUG_RIGHT_ELF): $(DEBUG_RIGHT_OBJS) $(DEBUG_SYSTEM) $(STARTUP_OBJ)
	@mkdir -p $(dir $@)
	@echo "Linking Debug Right..."
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(DEBUG_CFLAGS) $^ -o $@ $(LDFLAGS)

$(DEBUG_RIGHT_BIN): $(DEBUG_RIGHT_ELF)
	$(OBJCOPY) -O binary $< $@

###############################################################################
# Release Left
###############################################################################
RELEASE_LEFT_ELF = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-release-left.elf
RELEASE_LEFT_BIN = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-release-left.bin

release-left: $(RELEASE_LEFT_BIN)

$(RELEASE_LEFT_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(RELEASE_CFLAGS) $(LEFT_FLAG) -c $< -o $@

$(RELEASE_LEFT_ELF): $(RELEASE_LEFT_OBJS) $(RELEASE_SYSTEM) $(STARTUP_OBJ)
	@mkdir -p $(dir $@)
	@echo "Linking Release Left..."
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(RELEASE_CFLAGS) $^ -o $@ $(LDFLAGS)

$(RELEASE_LEFT_BIN): $(RELEASE_LEFT_ELF)
	$(OBJCOPY) -O binary $< $@

###############################################################################
# Release Right
###############################################################################
RELEASE_RIGHT_ELF = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-release-right.elf
RELEASE_RIGHT_BIN = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-release-right.bin

release-right: $(RELEASE_RIGHT_BIN)

$(RELEASE_RIGHT_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(RELEASE_CFLAGS) $(RIGHT_FLAG) -c $< -o $@

$(RELEASE_RIGHT_ELF): $(RELEASE_RIGHT_OBJS) $(RELEASE_SYSTEM) $(STARTUP_OBJ)
	@echo "Linking Release Right..."
	$(CC) $(COMMON_FLAGS) $(INCLUDES) $(RELEASE_CFLAGS) $^ -o $@ $(LDFLAGS)

$(RELEASE_RIGHT_BIN): $(RELEASE_RIGHT_ELF)
	$(OBJCOPY) -O binary $< $@

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
DEBUG_RIGHT_FULL_HEX = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-debug-right-full.hex
DEBUG_RIGHT_FULL_BIN = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-debug-right-full.bin

debug-right-full: $(DEBUG_RIGHT_FULL_BIN)

$(DEBUG_RIGHT_FULL_HEX): $(DEBUG_BOOTLOADER_BIN) $(DEBUG_RIGHT_BIN)
	@echo "Merging Debug Right into full firmware..."
	srec_cat $(DEBUG_BOOTLOADER_BIN) -Binary -offset 0x00000000 \
	         $(DEBUG_RIGHT_BIN) -Binary -offset 0x00014000 \
	         -o $@ -Intel

$(DEBUG_RIGHT_FULL_BIN): $(DEBUG_RIGHT_FULL_HEX)
	srec_cat $< -Intel -o $@ -Binary

DEBUG_LEFT_FULL_HEX = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-debug-left-full.hex
DEBUG_LEFT_FULL_BIN = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-debug-left-full.bin

debug-left-full: $(DEBUG_LEFT_FULL_BIN)

$(DEBUG_LEFT_FULL_HEX): $(DEBUG_BOOTLOADER_BIN) $(DEBUG_LEFT_BIN)
	@echo "Merging Debug Left into full firmware..."
	srec_cat $(DEBUG_BOOTLOADER_BIN) -Binary -offset 0x00000000 \
	         $(DEBUG_LEFT_BIN) -Binary -offset 0x00014000 \
	         -o $@ -Intel

$(DEBUG_LEFT_FULL_BIN): $(DEBUG_LEFT_FULL_HEX)
	srec_cat $< -Intel -o $@ -Binary

RELEASE_RIGHT_FULL_HEX = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-release-right-full.hex
RELEASE_RIGHT_FULL_BIN = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-release-right-full.bin

release-right-full: $(RELEASE_RIGHT_FULL_BIN)

$(RELEASE_RIGHT_FULL_HEX): $(RELEASE_BOOTLOADER_BIN) $(RELEASE_RIGHT_BIN)
	@echo "Merging Release Right into full firmware..."
	srec_cat $(RELEASE_BOOTLOADER_BIN) -Binary -offset 0x00000000 \
	         $(RELEASE_RIGHT_BIN) -Binary -offset 0x00014000 \
	         -o $@ -Intel

$(RELEASE_RIGHT_FULL_BIN): $(RELEASE_RIGHT_FULL_HEX)
	srec_cat $< -Intel -o $@ -Binary

RELEASE_LEFT_FULL_HEX = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-release-left-full.hex
RELEASE_LEFT_FULL_BIN = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-release-left-full.bin

release-left-full: $(RELEASE_LEFT_FULL_BIN)

$(RELEASE_LEFT_FULL_HEX): $(RELEASE_BOOTLOADER_BIN) $(RELEASE_LEFT_BIN)
	@echo "Merging Release Left into full firmware..."
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
clean-application:
	rm -rf $(DEBUG_DIR)/left
	rm -rf $(DEBUG_DIR)/right
	rm -rf $(RELEASE_DIR)/left
	rm -rf $(RELEASE_DIR)/right

clean:
	rm -rf $(BUILD_DIR)

