CROSS		?= ~/cross/bin/i686-elf-
CC 		= $(CROSS)gcc
LD		= $(CROSS)ld
OBJCPY		= $(CROSS)objcopy -O binary
ASM		= nasm -f elf32
BUILD_DIR	?= ../../build
ROOT		?= ../..

CFLAGS		= -m32 \
		  -ffreestanding \
		  -fno-builtin \
		  -fno-pie \
		  -nostdlib \
		  -Wall \
		  -Wextra \
		  -O2 \

LDFLAGS		= -T $(ROOT)/linker.ld

all: $(BUILD_DIR)/stage2.bin

$(BUILD_DIR)/main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o $@

$(BUILD_DIR)/stage2_entry.o: stage2_entry.asm a20.asm gdt.asm
	$(ASM) stage2_entry.asm -o $@

$(BUILD_DIR)/stage2.elf:$(BUILD_DIR)/main.o $(BUILD_DIR)/stage2_entry.o
	$(LD) $(BUILD_DIR)/stage2_entry.o $(BUILD_DIR)/main.o -o $@ $(LDFLAGS)

$(BUILD_DIR)/stage2.bin: $(BUILD_DIR)/stage2.elf
	$(OBJCPY) $(BUILD_DIR)/stage2.elf $@

.PHONY: all
