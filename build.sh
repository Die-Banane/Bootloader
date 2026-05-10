#!/bin/bash

nasm -f bin main.asm -o main.bin

dd if=/dev/zero of=floppy.img bs=512 count=2880
dd if=main.bin of=floppy.img bs=512 conv=notrunc
