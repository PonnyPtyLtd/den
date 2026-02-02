# Makefile for SMS Game

# WLA-DX toolchain
WLA := wla-z80
WLALINK := wlalink

# Directories
SRC_DIR := src
BUILD_DIR := build
ASSETS_DIR := assets

# Files
MAIN_ASM := $(SRC_DIR)/game.asm
OBJECTS := $(BUILD_DIR)/game.o
OUTPUT_ROM := $(BUILD_DIR)/game.sms
LINK_FILE := $(BUILD_DIR)/linkfile

# Create build directory if it doesn't exist
$(shell mkdir -p $(BUILD_DIR))

# Default target
all: $(OUTPUT_ROM)

# Link object files into final SMS ROM using wlalink
$(OUTPUT_ROM): $(OBJECTS)
	@echo "Linking ROM..."
	@echo "[objects]" > $(LINK_FILE)
	@echo "$(OBJECTS)" >> $(LINK_FILE)
	$(WLALINK) -d -r -v -S $(LINK_FILE) $(OUTPUT_ROM)

# Assemble source to object file
$(BUILD_DIR)/game.o: $(MAIN_ASM)
	@echo "Assembling $<..."
	$(WLA) -o $@ $<

# Remove all build artifacts
clean:
	rm -f $(BUILD_DIR)/*

# Build and run in Mednafen
run: $(OUTPUT_ROM)
	@echo "Running ROM in Mednafen..."
	mednafen $(OUTPUT_ROM)

.PHONY: all clean run
