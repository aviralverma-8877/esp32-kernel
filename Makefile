# ESP32 Bare-Metal Kernel Makefile

# Toolchain
CROSS_COMPILE ?= xtensa-esp32-elf-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size

# Project name
PROJECT = esp32-kernel

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
LINKER_DIR = linker

# Source files
C_SOURCES = $(wildcard $(SRC_DIR)/boot/*.c) \
            $(wildcard $(SRC_DIR)/kernel/*.c) \
            $(wildcard $(SRC_DIR)/drivers/*.c) \
            $(wildcard $(SRC_DIR)/apps/*.c)

ASM_SOURCES = $(wildcard $(SRC_DIR)/boot/*.S) \
              $(wildcard $(SRC_DIR)/kernel/*.S)

# Object files
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
ASM_OBJECTS = $(patsubst $(SRC_DIR)/%.S, $(BUILD_DIR)/%.o, $(ASM_SOURCES))
OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS)

# Linker script
LINKER_SCRIPT = $(LINKER_DIR)/esp32.ld

# Compiler flags
CFLAGS = -I$(INC_DIR) \
         -mlongcalls \
         -mtext-section-literals \
         -ffunction-sections \
         -fdata-sections \
         -Wall \
         -Werror \
         -Os \
         -g \
         -nostdlib \
         -fno-builtin \
         -fno-common \
         -Wno-error=unused-function \
         -Wno-error=unused-variable

# Assembler flags
ASFLAGS = -I$(INC_DIR) \
          -mlongcalls \
          -x assembler-with-cpp

# Linker flags
LDFLAGS = -T$(LINKER_SCRIPT) \
          -nostdlib \
          -Wl,--gc-sections \
          -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map \
          -Wl,--cref

# Esptool settings for flashing
ESPTOOL ?= esptool.py
ESPTOOL_PORT ?= /dev/ttyUSB0
ESPTOOL_BAUD ?= 921600
FLASH_ADDR ?= 0x1000

# Default target
all: $(BUILD_DIR)/$(PROJECT).bin

# Create build directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/boot
	mkdir -p $(BUILD_DIR)/kernel
	mkdir -p $(BUILD_DIR)/drivers
	mkdir -p $(BUILD_DIR)/apps

# Compile C source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Compile assembly source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S | $(BUILD_DIR)
	@echo "AS $<"
	@$(AS) $(ASFLAGS) -c $< -o $@

# Link into ELF file
$(BUILD_DIR)/$(PROJECT).elf: $(OBJECTS)
	@echo "LD $@"
	@$(CC) $(LDFLAGS) -o $@ $(OBJECTS) -lgcc
	@$(SIZE) $@

# Convert ELF to binary
$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O binary $< $@
	@echo "Kernel binary created: $@"

# Flash to ESP32
flash: $(BUILD_DIR)/$(PROJECT).bin
	@echo "Flashing to ESP32..."
	$(ESPTOOL) --chip esp32 \
	           --port $(ESPTOOL_PORT) \
	           --baud $(ESPTOOL_BAUD) \
	           write_flash \
	           $(FLASH_ADDR) $(BUILD_DIR)/$(PROJECT).bin

# Monitor serial output
monitor:
	@echo "Opening serial monitor on $(ESPTOOL_PORT)..."
	@echo "Press Ctrl+C to exit"
	python -m serial.tools.miniterm $(ESPTOOL_PORT) 115200

# Flash and monitor
flash-monitor: flash
	@sleep 2
	$(MAKE) monitor

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Show help
help:
	@echo "ESP32 Bare-Metal Kernel Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all            - Build the kernel (default)"
	@echo "  flash          - Flash kernel to ESP32"
	@echo "  monitor        - Open serial monitor"
	@echo "  flash-monitor  - Flash and open monitor"
	@echo "  clean          - Remove build artifacts"
	@echo "  help           - Show this help message"
	@echo ""
	@echo "Configuration:"
	@echo "  ESPTOOL_PORT   - Serial port (default: /dev/ttyUSB0)"
	@echo "  ESPTOOL_BAUD   - Baud rate for flashing (default: 921600)"
	@echo "  FLASH_ADDR     - Flash address (default: 0x1000)"
	@echo ""
	@echo "Examples:"
	@echo "  make"
	@echo "  make flash ESPTOOL_PORT=/dev/ttyUSB0"
	@echo "  make monitor"
	@echo "  make clean"

.PHONY: all flash monitor flash-monitor clean help
