org 0x7C00
BITS 16

main:

;initialisation
    cli
    jmp 0x0000:.cs_init
.cs_init:
    xor ax, ax
    mov es, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

;error if booting from floppy disk
    cmp dl, 0x80
    jb error

;chech if BIOS suppors extended mode
    mov ah, 0x41
    mov bx, 0x55AA
    int 0x13

    jc error
    cmp bx, 0xAA55
    jne error

    mov ah, 0x42
    mov si, DAP
    int 0x13
    jc error

    jmp 0x7E00

halt:
    hlt
    jmp halt

error:
    jmp halt
;TODO: think of a good way to do proper error handling

DAP:
    .size:		db 0x10
    .unused:		db 0
    .sectors:		dw 4
    .offset:		dw 0x7E00
    .segment:		dw 0
    .start_sector:	dq 1

times 510 - ($ - $$) db 0		;pad the file with 0
dw 0xAA55				;place the boot signature at byte 510 and 511
