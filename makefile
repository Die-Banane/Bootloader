MAKE		= make
BUILD_DIR	= build
SRC_DIR		= src


all: image

always:
	mkdir -p $(BUILD_DIR)

image: always stage1 stage2
	dd if=/dev/zero of=$(BUILD_DIR)/disk.img bs=512 count=2048
	dd if=$(BUILD_DIR)/stage1.bin of=$(BUILD_DIR)/disk.img conv=notrunc bs=512 seek=0
	dd if=$(BUILD_DIR)/stage2.bin of=$(BUILD_DIR)/disk.img conv=notrunc bs=512 seek=1

stage1: always
	$(MAKE) -C $(SRC_DIR)/stage1 BUILD_DIR=$(abspath $(BUILD_DIR))

stage2: always
	$(MAKE) -C $(SRC_DIR)/stage2 BUILD_DIR=$(abspath $(BUILD_DIR)) ROOT=$(abspath ./)

clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean always stage1 stage2
