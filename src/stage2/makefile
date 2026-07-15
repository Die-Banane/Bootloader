CROSS		?= ~/cross/bin/i686-elf-
CC 		= $(CROSS)gcc
LD		= $(CROSS)ld
OBJCPY		= $(CROSS)objcopy -O binary
ASM		= nasm -f elf32
BUILD_DIR	?= ../../build

CFLAGS		= -m32 \
		  -ffreestanding \
		  -fno-builtin \
		  -fno-pie \
		  -nostdlib \
		  -Wall \
		  -Wextra \
		  -O2 \
		  -g

LDFLAGS		= -T linker.ld

all: $(BUILD_DIR)/stage2.bin

$(BUILD_DIR)/main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o $@

$(BUILD_DIR)/bios.o: bios.c
	$(CC) $(CFLAGS) -c bios.c -o $@

$(BUILD_DIR)/io.o: io.c
	$(CC) $(CFLAGS) -c io.c -o $@

$(BUILD_DIR)/entry.o: entry.asm a20.asm gdt.asm
	$(ASM) entry.asm -o $@

$(BUILD_DIR)/bios16.o: bios16.asm
	$(ASM) bios16.asm -o $@

$(BUILD_DIR)/stage2.elf: $(BUILD_DIR)/main.o $(BUILD_DIR)/entry.o $(BUILD_DIR)/bios16.o $(BUILD_DIR)/bios.o $(BUILD_DIR)/io.o
	$(LD) $(BUILD_DIR)/entry.o $(BUILD_DIR)/main.o $(BUILD_DIR)/bios.o $(BUILD_DIR)/bios16.o $(BUILD_DIR)/io.o -o $@ $(LDFLAGS)

$(BUILD_DIR)/stage2.bin: $(BUILD_DIR)/stage2.elf
	$(OBJCPY) $(BUILD_DIR)/stage2.elf $@

.PHONY: all
