###############################################################################
# Directories
###############################################################################
BOOTLOADER_SRC_DIR     = $(BOOTLOADER_DIR)/src
BOOTLOADER_INC_DIR     = $(BOOTLOADER_DIR)/include

BOOTLOADER_LD_SCRIPT   = $(LD_DIR)/bootloader.ld

BOOTLOADER_BUILD_DIR   = $(BOOTLOADER_DIR)/build
DEBUG_BOOTLOADER_DIR   = $(BOOTLOADER_BUILD_DIR)/debug
RELEASE_BOOTLOADER_DIR = $(BOOTLOADER_BUILD_DIR)/release

###############################################################################
# Sources
###############################################################################
BOOTLOADER_SRCS     = $(shell find $(BOOTLOADER_SRC_DIR) -type f -name '*.c')
BOOTLOADER_SRCS    += $(shell find $(HAL_SRC_DIR) -type f -name '*.c')
BOOTLOADER_SRCS    += $(COMMON_SRC_DIR)/clock.c \
					  $(COMMON_SRC_DIR)/error_handler.c \
					  $(COMMON_SRC_DIR)/logging.c \
					  $(COMMON_SRC_DIR)/syscalls.c

BOOTLOADER_HEADERS  = $(shell find $(BOOTLOADER_INC_DIR) -type f -name '*.h')
BOOTLOADER_HEADERS += $(shell find $(COMMON_INC_DIR) -type f -name '*.h')
BOOTLOADER_HEADERS += $(shell find $(HAL_INC_DIR) -type f -name '*.h')
BOOTLOADER_HEADERS += $(shell find $(CONFIG_INC_DIR) -type f -name '*.h')

###############################################################################
# Object Files
###############################################################################
DEBUG_BOOTLOADER_OBJS   = $(BOOTLOADER_SRCS:%.c=$(DEBUG_BOOTLOADER_DIR)/%.o)
RELEASE_BOOTLOADER_OBJS = $(BOOTLOADER_SRCS:%.c=$(RELEASE_BOOTLOADER_DIR)/%.o)

###############################################################################
# Compiler/Linker Flags
###############################################################################
BOOTLOADER_INCLUDES       = -I$(COMMON_INC_DIR) \
							-I$(BOOTLOADER_INC_DIR) \
							-I$(HAL_INC_DIR) \
                            -I$(CONFIG_INC_DIR) \
                            -I$(CMSIS_INC_DIR) \
                            -I$(CMSIS_DEVICE_INC_DIR)

BOOTLOADER_LDFLAGS        = -T $(BOOTLOADER_LD_SCRIPT) $(COMMON_LDFLAGS)

DEBUG_BOOTLOADER_CFLAGS   = $(DEBUG_CFLAGS) -DBOOTLOADER
RELEASE_BOOTLOADER_CFLAGS = -Os -DBOOTLOADER

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

clean-bootloader:
	rm -rf $(BOOTLOADER_BUILD_DIR)
