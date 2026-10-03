CROSS		?= ~/cross/bin/i686-elf-
CC		:= $(CROSS)gcc
LD		:= $(CROSS)ld
OBJCOPY		:= $(CROSS)objcopy -O binary
ASM		:= nasm -f elf32
BUILD_DIR	?= ../../build

CFLAGS		:= -Iinclude \
		   -m32 \
		   -ffreestanding \
		   -fno-builtin \
		   -fno-pie \
		   -nostdlib \
		   -Wall \
		   -Wextra \
		   -O2 \
		   -MMD \
		   -MP

LDFLAGS		:= -T linker.ld

C_OBJECTS	:= $(BUILD_DIR)/main.o \
		   $(BUILD_DIR)/bios.o \
		   $(BUILD_DIR)/io.o \
		   $(BUILD_DIR)/fs.o \
		   $(BUILD_DIR)/fat.o \
		   $(BUILD_DIR)/fat12.o \
		   $(BUILD_DIR)/fat16.o \
		   $(BUILD_DIR)/fat32.o \
		   $(BUILD_DIR)/partition.o

ASM_OBJECTS	:= $(BUILD_DIR)/entry.o \
		   $(BUILD_DIR)/bios16.o

OBJECTS		:= $(C_OBJECTS) $(ASM_OBJECTS)

ELF		:= $(BUILD_DIR)/stage2.elf
BIN		:= $(BUILD_DIR)/stage2.bin


.PHONY: all

all: $(BIN)


$(BUILD_DIR)/main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o $@

$(BUILD_DIR)/bios.o: bios.c
	$(CC) $(CFLAGS) -c bios.c -o $@

$(BUILD_DIR)/io.o: io.c
	$(CC) $(CFLAGS) -c io.c -o $@

$(BUILD_DIR)/fs.o: fs/fs.c
	$(CC) $(CFLAGS) -c fs/fs.c -o $@

$(BUILD_DIR)/fat.o: fs/fat/fat.c
	$(CC) $(CFLAGS) -c fs/fat/fat.c -o $@

$(BUILD_DIR)/fat12.o: fs/fat/fat12.c
	$(CC) $(CFLAGS) -c fs/fat/fat12.c -o $@

$(BUILD_DIR)/fat16.o: fs/fat/fat16.c
	$(CC) $(CFLAGS) -c fs/fat/fat16.c -o $@

$(BUILD_DIR)/fat32.o: fs/fat/fat32.c
	$(CC) $(CFLAGS) -c fs/fat/fat32.c -o $@

$(BUILD_DIR)/partition.o: fs/partition.c
	$(CC) $(CFLAGS) -c fs/partition.c -o $@


$(BUILD_DIR)/entry.o: entry.asm a20.asm gdt.asm
	$(ASM) entry.asm -o $@

$(BUILD_DIR)/bios16.o: bios16.asm
	$(ASM) bios16.asm -o $@


$(ELF): $(OBJECTS)
	$(LD) $(BUILD_DIR)/entry.o \
	      $(BUILD_DIR)/main.o \
	      $(BUILD_DIR)/bios.o \
	      $(BUILD_DIR)/bios16.o \
	      $(BUILD_DIR)/io.o \
	      $(BUILD_DIR)/fs.o \
	      $(BUILD_DIR)/fat.o \
	      $(BUILD_DIR)/fat12.o \
	      $(BUILD_DIR)/fat16.o \
	      $(BUILD_DIR)/fat32.o \
	      $(BUILD_DIR)/partition.o \
	      -o $@ $(LDFLAGS)


$(BIN): $(ELF)
	$(OBJCOPY) $(ELF) $@

#header dependency tracking (-MMD generated .d files)
-include $(C_OBJECTS:.o=.d)
