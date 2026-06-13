ASM		= nasm -f bin
BUILD_DIR	?= ../../build

all: $(BUILD_DIR)/stage1.bin

$(BUILD_DIR)/stage1.bin: boot.asm
	$(ASM) boot.asm -o $@
