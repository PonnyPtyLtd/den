# Makefile for SMS Game

# WLA-DX toolchain
WLA := wla-z80
WLALINK := wlalink

# Directories
SRC_DIR := src
BUILD_DIR := build
ASSETS_DIR := assets

# Files
MAIN_ASM := $(SRC_DIR)/main.asm
OBJECTS := $(BUILD_DIR)/main.o
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

# Generate build timestamp
$(BUILD_DIR)/buildtime.inc: FORCE
	@echo 'BuildTimeString: .db "$(shell date +"%Y-%m-%d %H:%M")",0' > $@

FORCE:

# Assemble source to object file
$(BUILD_DIR)/main.o: $(MAIN_ASM) $(wildcard $(SRC_DIR)/*.inc $(SRC_DIR)/data/*.inc) $(BUILD_DIR)/buildtime.inc
	@echo "Assembling $<..."
	$(WLA) -I $(SRC_DIR) -I $(BUILD_DIR) -o $@ $<

# Remove all build artifacts
clean:
	rm -f $(BUILD_DIR)/*

# Build and run in Mednafen
run: $(OUTPUT_ROM)
	@echo "Running ROM in Mednafen..."
	mednafen $(OUTPUT_ROM)

# Export tiles to BMP (combined BG + sprites, plus font)
tiles-export:
	ruby tools/tile_export.rb tiles_export.bmp font.bmp

# Import edited tiles back from BMP
tiles-import:
	ruby tools/tile_import.rb tiles_export.bmp font.bmp

.PHONY: all clean run tiles-export tiles-import
