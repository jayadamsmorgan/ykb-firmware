###############################################################################
# Directories
###############################################################################
BOOTLOADER_SRC_DIR     = $(BOOTLOADER_DIR)/src
BOOTLOADER_INC_DIR     = $(BOOTLOADER_DIR)/include

BOOTLOADER_LD_SCRIPT   = $(HW_DIR)/ld/bootloader.ld

DEBUG_BOOTLOADER_DIR   = $(DEBUG_DIR)/bootloader/bootloader
RELEASE_BOOTLOADER_DIR = $(RELEASE_DIR)/bootloader/bootloader

###############################################################################
# Sources
###############################################################################
BOOTLOADER_SRCS     = $(shell find $(BOOTLOADER_SRC_DIR) -type f -name '*.c')
BOOTLOADER_SRCS    += $(shell find $(HAL_SRC_DIR) -type f -name '*.c')
BOOTLOADER_SRCS    += $(shell find $(COMMON_SRC_DIR) -type f -name '*.c')

BOOTLOADER_SYSTEM   = $(HW_DIR)/system/bootloader.c

BOOTLOADER_HEADERS  = $(shell find $(BOOTLOADER_INC_DIR) -type f -name '*.h')
BOOTLOADER_HEADERS += $(shell find $(COMMON_INC_DIR) -type f -name '*.h')
BOOTLOADER_HEADERS += $(shell find $(HAL_INC_DIR) -type f -name '*.h')
BOOTLOADER_HEADERS += $(shell find $(CONFIG_INC_DIR) -type f -name '*.h')
BOOTLOADER_HEADERS += $(YKB_PROTOCOL_INC_DIR)/ykb_protocol.h

###############################################################################
# Object Files
###############################################################################
DEBUG_BOOTLOADER_OBJS     = $(BOOTLOADER_SRCS:%.c=$(DEBUG_BOOTLOADER_DIR)/%.o)
DEBUG_BOOTLOADER_SYSTEM   = $(DEBUG_BOOTLOADER_DIR)/system.o
RELEASE_BOOTLOADER_OBJS   = $(BOOTLOADER_SRCS:%.c=$(RELEASE_BOOTLOADER_DIR)/%.o)
RELEASE_BOOTLOADER_SYSTEM = $(RELEASE_BOOTLOADER_DIR)/system.o

###############################################################################
# Compiler/Linker Flags
###############################################################################
BOOTLOADER_INCLUDES       = -I$(COMMON_INC_DIR) \
							-I$(BOOTLOADER_INC_DIR) \
							-I$(HAL_INC_DIR) \
                            -I$(CONFIG_INC_DIR) \
                            -I$(CMSIS_INC_DIR) \
                            -I$(CMSIS_DEVICE_INC_DIR) \
							-I$(YKB_PROTOCOL_INC_DIR)

BOOTLOADER_LDFLAGS        = -T $(BOOTLOADER_LD_SCRIPT) $(COMMON_LDFLAGS)

BOOTLOADER_COMMON_CFLAGS  = -DBOOTLOADER -DUSBD_PID=0xB007
DEBUG_BOOTLOADER_CFLAGS   = $(BOOTLOADER_COMMON_CFLAGS) $(DEBUG_CFLAGS)
RELEASE_BOOTLOADER_CFLAGS = $(BOOTLOADER_COMMON_CFLAGS) -Os

###############################################################################
# Debug Bootloader
###############################################################################
DEBUG_BOOTLOADER_ELF = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-debug-bootloader.elf
DEBUG_BOOTLOADER_BIN = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-debug-bootloader.bin

debug-bootloader: $(DEBUG_BOOTLOADER_BIN)

$(DEBUG_BOOTLOADER_ELF): $(DEBUG_BOOTLOADER_OBJS) $(DEBUG_BOOTLOADER_SYSTEM) $(STARTUP_OBJ)
	@mkdir -p $(dir $@)
	@echo "Linking bootloader..."
	$(CC) $(COMMON_FLAGS) $(BOOTLOADER_INCLUDES) $(DEBUG_BOOTLOADER_CFLAGS) $^ -o $@ $(BOOTLOADER_LDFLAGS)

$(DEBUG_BOOTLOADER_BIN): $(DEBUG_BOOTLOADER_ELF)
	$(OBJCOPY) -O binary $< $@

$(DEBUG_BOOTLOADER_DIR)/%.o: %.c $(BOOTLOADER_HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(BOOTLOADER_INCLUDES) $(DEBUG_BOOTLOADER_CFLAGS) -c $< -o $@

$(DEBUG_BOOTLOADER_SYSTEM):
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(DEBUG_BOOTLOADER_CFLAGS) $(BOOTLOADER_INCLUDES) -c $(BOOTLOADER_SYSTEM) -o $@

###############################################################################
# Release Bootloader
###############################################################################

RELEASE_BOOTLOADER_ELF = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-release-bootloader.elf
RELEASE_BOOTLOADER_BIN = $(BUILD_OUTPUTS_DIR)/$(PROJECT_NAME)-release-bootloader.bin

release-bootloader: $(RELEASE_BOOTLOADER_BIN)

$(RELEASE_BOOTLOADER_ELF): $(RELEASE_BOOTLOADER_OBJS) $(RELEASE_BOOTLOADER_SYSTEM) $(STARTUP_OBJ)
	@mkdir -p $(dir $@)
	@echo "Linking bootloader..."
	$(CC) $(COMMON_FLAGS) $(BOOTLOADER_INCLUDES) $(RELEASE_BOOTLOADER_CFLAGS) $^ -o $@ $(BOOTLOADER_LDFLAGS)

$(RELEASE_BOOTLOADER_BIN): $(RELEASE_BOOTLOADER_ELF)
	$(OBJCOPY) -O binary $< $@

$(RELEASE_BOOTLOADER_DIR)/%.o: %.c $(BOOTLOADER_HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(BOOTLOADER_INCLUDES) $(RELEASE_BOOTLOADER_CFLAGS) -c $< -o $@

$(RELEASE_BOOTLOADER_SYSTEM):
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) $(RELEASE_BOOTLOADER_CFLAGS) $(BOOTLOADER_INCLUDES) -c $(BOOTLOADER_SYSTEM) -o $@

clean-bootloader:
	rm -rf $(DEBUG_BOOTLOADER_DIR)
	rm -rf $(RELEASE_BOOTLOADER_DIR)
