ASM	= nasm
QEMU	= qemu-system-x86_64
BUILD	= build

.PHONY: all clean run

all: $(BUILD)/disk.img

$(BUILD)/boot.bin: stage1/boot.asm
	$(ASM) -f bin -o $@ stage1/boot.asm

$(BUILD)/stage2.bin: stage2/main.asm stage2/a20.asm stage2/screen.asm
	$(ASM) -f bin -o $@ stage2/main.asm

$(BUILD)/disk.img: $(BUILD)/boot.bin $(BUILD)/stage2.bin
	dd if=/dev/zero of=$(BUILD)/disk.img bs=512 count=2048
	dd if=$(BUILD)/boot.bin of=$(BUILD)/disk.img bs=512 seek=0 conv=notrunc
	dd if=$(BUILD)/stage2.bin of=$(BUILD)/disk.img bs=512 seek=1 conv=notrunc

run: $(BUILD)/disk.img
	$(QEMU) -drive file=$(BUILD)/disk.img,format=raw,if=ide,index=0,media=disk

clean:
	rm -f $(BUILD)/*
