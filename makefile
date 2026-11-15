MAKE		= make
BUILD_DIR	= build
SRC_DIR		= src

TOTAL_SECTORS	= 16384
FAT_START_LBA	= 2048
FAT_SECTORS	= $(shell expr $(TOTAL_SECTORS) - $(FAT_START_LBA))

DISK_IMG	= $(BUILD_DIR)/disk.img
FAT_IMG		= $(BUILD_DIR)/fat.img
HELLO_TXT	= $(BUILD_DIR)/HelloWorld.txt

all: stage1 stage2 always

always:
	mkdir -p $(BUILD_DIR)

#image: always stage1 stage2 $(FAT_IMG)
#	dd if=/dev/zero of=$(DISK_IMG) bs=512 count=$(TOTAL_SECTORS)
#	dd if=$(BUILD_DIR)/stage1.bin of=$(DISK_IMG) conv=notrunc bs=512 seek=0
#	dd if=$(BUILD_DIR)/stage2.bin of=$(DISK_IMG) conv=notrunc bs=512 seek=1
#	printf '$(FAT_START_LBA),,06,*\n' | sfdisk --force -u S $(DISK_IMG)
#	dd if=$(FAT_IMG) of=$(DISK_IMG) conv=notrunc bs=512 seek=$(FAT_START_LBA)

#$(HELLO_TXT): always
#	printf 'Hello World\n' > $@

#mkfs.fat counts 1024-byte blocks, FAT_SECTORS is in 512-byte sectors.
#-s 2 keeps the cluster count inside the FAT16 range for a 7 MiB partition.
#$(FAT_IMG): $(HELLO_TXT)
#	rm -f $@
#	mkfs.fat -F 16 -s 2 -C $@ $$(($(FAT_SECTORS) / 2))
#	mcopy -i $@ $(HELLO_TXT) ::/HelloWorld.txt

stage1: always
	$(MAKE) -C $(SRC_DIR)/stage1 BUILD_DIR=$(abspath $(BUILD_DIR))

stage2: always
	$(MAKE) -C $(SRC_DIR)/stage2 BUILD_DIR=$(abspath $(BUILD_DIR)) ROOT=$(abspath ./)

clean:
	rm -rf $(BUILD_DIR)/*

run: all
	qemu-system-x86_64 -drive file=./disk.img,format=raw,if=ide,index=0,media=disk \
	-d int,cpu_reset -no-reboot

.PHONY: all clean run always stage1 stage2
