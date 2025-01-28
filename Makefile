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
SRC_DIR     = src
INC_DIR     = include
BUILD_DIR   = build
DEBUG_DIR   = $(BUILD_DIR)/debug
RELEASE_DIR = $(BUILD_DIR)/release

# CMSIS / Device-Specific
CMSIS_INC_DIR        = CMSIS_5/CMSIS/Core/Include
CMSIS_DEVICE_INC_DIR = cmsis-device-wb/Include
STARTUP_SRC          = cmsis-device-wb/Source/Templates/gcc/startup_stm32wb55xx_cm4.s
SYSTEM               = cmsis-device-wb/Source/Templates/system_stm32wbxx.c
LD_SCRIPT            = cmsis-device-wb/Source/Templates/gcc/linker/stm32wb55xx_flash_cm4.ld

# TinyUSB library
TINYUSB_DIR = tinyusb/src

# We will copy (rename) the startup .s file into the build directory as .S
STARTUP_S = $(BUILD_DIR)/startup_stm32wb55xx_cm4.S

###############################################################################
# Board/MCU Definitions
###############################################################################
BOARD = STM32WB55xx
MCU   = cortex-m4

###############################################################################
# Sources
###############################################################################
# Find all .c files in SRC_DIR, plus the system file.
# (Remove the startup .s from the normal SRCS, since we handle it separately)
SRCS = $(shell find $(SRC_DIR) -type f -name '*.c')
SRCS += $(SYSTEM)
SRCS += $(shell find $(TINYUSB_DIR) -type f -name '*.c')

###############################################################################
# Object Files
###############################################################################
# Debug object files from .c
DEBUG_OBJS   = $(SRCS:%.c=$(DEBUG_DIR)/%.o)
# Release object files from .c
RELEASE_OBJS = $(SRCS:%.c=$(RELEASE_DIR)/%.o)

# We also need object files for the startup code:
DEBUG_STARTUP_OBJ   = $(DEBUG_DIR)/startup_stm32wb55xx_cm4.o
RELEASE_STARTUP_OBJ = $(RELEASE_DIR)/startup_stm32wb55xx_cm4.o

###############################################################################
# Compiler/Linker Flags
###############################################################################
COMMON_FLAGS = -Wall -Werror -mlittle-endian -mthumb -mthumb-interwork \
               -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mcpu=$(MCU) \
               -I$(INC_DIR) -I$(CMSIS_INC_DIR) -I$(CMSIS_DEVICE_INC_DIR) -I$(TINYUSB_DIR) \
               -D$(BOARD)

# Use '--specs=nosys.specs' so we don't depend on newlib syscalls
LDFLAGS = -T $(LD_SCRIPT) --specs=nosys.specs

# Debug CFLAGS
DEBUG_CFLAGS   = -g -O0 -DDEBUG
# Release CFLAGS (adjust optimization as desired)
RELEASE_CFLAGS = -O2

###############################################################################
# Top-Level Targets
###############################################################################
.PHONY: all debug release clean stflash dfuflash size help

# By default, build debug
all: debug

help:
	@echo "Usage:"
	@echo "  make [debug | release | clean | stflash | dfuflash | size]"
	@echo
	@echo "Targets:"
	@echo "  debug       Build with debug settings"
	@echo "  release     Build with release settings"
	@echo "  clean       Remove all build artifacts"
	@echo "  stflash     Flash the built release binary to target using st-flash"
	@echo "  dfuflash    Flash the built release binary to target using dfu-util"
	@echo "  size        Show the size of the final ELF (release by default)"

###############################################################################
# Debug Build
###############################################################################
debug: $(DEBUG_DIR)/$(PROJECT_NAME).bin

$(DEBUG_DIR)/$(PROJECT_NAME).elf: $(DEBUG_OBJS) $(DEBUG_STARTUP_OBJ)
	@echo "Linking Debug..."
	$(CC) $(COMMON_FLAGS) $(DEBUG_CFLAGS) $^ -o $@ $(LDFLAGS)

# Pattern rule for building .o from .c (Debug)
$(DEBUG_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(DEBUG_CFLAGS) -c $< -o $@

# Compile the startup .S in debug mode
$(DEBUG_STARTUP_OBJ): $(STARTUP_S)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(DEBUG_CFLAGS) -c $< -o $@

###############################################################################
# Release Build
###############################################################################
release: $(RELEASE_DIR)/$(PROJECT_NAME).bin

$(RELEASE_DIR)/$(PROJECT_NAME).elf: $(RELEASE_OBJS) $(RELEASE_STARTUP_OBJ)
	@echo "Linking Release..."
	$(CC) $(COMMON_FLAGS) $(RELEASE_CFLAGS) $^ -o $@ $(LDFLAGS)

# Pattern rule for building .o from .c (Release)
$(RELEASE_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(RELEASE_CFLAGS) -c $< -o $@

# Compile the startup .S in release mode
$(RELEASE_STARTUP_OBJ): $(STARTUP_S)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(RELEASE_CFLAGS) -c $< -o $@

###############################################################################
# Copy / Rename Startup File
###############################################################################
# Copy the original .s to build/ as .S so that it's recognized as 
# assembly-with-preprocessing
$(STARTUP_S): $(STARTUP_SRC)
	@mkdir -p $(dir $@)
	cp $< $@

###############################################################################
# Conversion to Binary / Size
###############################################################################
# Convert release ELF to a binary
$(RELEASE_DIR)/$(PROJECT_NAME).bin: $(RELEASE_DIR)/$(PROJECT_NAME).elf
	$(OBJCOPY) -O binary $< $@

# Convert debug ELF to a binary
$(DEBUG_DIR)/$(PROJECT_NAME).bin: $(DEBUG_DIR)/$(PROJECT_NAME).elf
	$(OBJCOPY) -O binary $< $@

# Show size of the release ELF by default
size: $(RELEASE_DIR)/$(PROJECT_NAME).elf
	@$(SIZE) $<

size-debug: $(DEBUG_DIR)/$(PROJECT_NAME).elf
	@$(SIZE) $<

###############################################################################
# Flash Targets
###############################################################################
stflash: $(RELEASE_DIR)/$(PROJECT_NAME).bin
	@echo "Flashing via st-flash..."
	st-flash --reset write $< 0x08000000

stflash-debug: $(DEBUG_DIR)/$(PROJECT_NAME).bin
	@echo "Flashing via st-flash..."
	st-flash --reset write $< 0x08000000

dfuflash: $(RELEASE_DIR)/$(PROJECT_NAME).bin
	@echo "Flashing via dfu-util..."
	dfu-util -D $< -a 0 -s 0x08000000

dfuflash-debug: $(DEBUG_DIR)/$(PROJECT_NAME).bin
	@echo "Flashing via dfu-util..."
	dfu-util -D $< -a 0 -s 0x08000000

###############################################################################
# Clean
###############################################################################
clean:
	rm -rf $(BUILD_DIR)
