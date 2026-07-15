ASM		= nasm -f bin
BUILD_DIR	?= ../../build

all: $(BUILD_DIR)/stage1.bin

$(BUILD_DIR)/stage1.bin: mbr.asm
	$(ASM) mbr.asm -o $@
