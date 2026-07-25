MAKE		= make
BUILD_DIR	= build
SRC_DIR		= src

TOTAL_SECTORS	= 8192
FAT_START_LBA	= 2048

all: image

always:
	mkdir -p $(BUILD_DIR)

image: always stage1 stage2
	dd if=/dev/zero of=$(BUILD_DIR)/disk.img bs=512 count=$(TOTAL_SECTORS)
	dd if=$(BUILD_DIR)/stage1.bin of=$(BUILD_DIR)/disk.img conv=notrunc bs=512 seek=0
	dd if=$(BUILD_DIR)/stage2.bin of=$(BUILD_DIR)/disk.img conv=notrunc bs=512 seek=1
	printf '$(FAT_START_LBA),,01,*\n' | sfdisk --force -u S $(BUILD_DIR)/disk.img
	mkfs.fat -F 12 -C $(BUILD_DIR)/fat.img $$((($(TOTAL_SECTORS) - $(FAT_START_LBA)) / 2)) 2>/dev/null
	dd if=$(BUILD_DIR)/fat.img of=$(BUILD_DIR)/disk.img conv=notrunc bs=512 seek=$(FAT_START_LBA)
	rm -f $(BUILD_DIR)/fat.img

stage1: always
	$(MAKE) -C $(SRC_DIR)/stage1 BUILD_DIR=$(abspath $(BUILD_DIR))

stage2: always
	$(MAKE) -C $(SRC_DIR)/stage2 BUILD_DIR=$(abspath $(BUILD_DIR)) ROOT=$(abspath ./)

clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean always stage1 stage2
