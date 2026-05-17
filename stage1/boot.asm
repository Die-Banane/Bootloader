org 0x7C00
BITS 16

start:
    jmp main

;TODO: move functions to stage 2
clr_screen:

;save state
    push ax
    push bx
    push cx
    push dx

    mov ah, 0x06
    mov al, 0			;0 = blank the Window
    mov bh, 0x07
    mov ch, 0			;top left corner
    mov cl, 0
    mov dh, 24			;bottom right corner
    mov dl, 79
    int 0x10

;restore state
    pop dx
    pop cx
    pop bx
    pop ax
    ret

;prints a null terminated String to the cursor position
print:
    pushf
    push si
    push ax
    push bx
    xor bh, bh

.loop:
    lodsb
    test al, al
    jz .done
    
    mov ah, 0x0e
    int 0x10

    jmp .loop

.done:
    pop bx
    pop ax
    pop si
    popf
    ret


main:

;initialisation
    xor ax, ax
    mov es, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7C00

;chech if BIOS suppors extended mode
    pusha
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, 0x80
    int 0x13

    jc .error
    cmp bx, 0xAA55
    jne .error
    popa

.error:
;TODO: think of a good way to do proper error handling

.halt:
    hlt
    jmp .halt

DAP:
    .size:		db 0x10
    .unused:		db 0
    .sectors:		dw 4
    .offset:		dw 0x7E00
    .segment:		dw 0
    .start_read:	dq 1

%include "a20.asm"

times 510 - ($ - $$) db 0		;pad the file with 0
dw 0xAA55				;place the boot signature at byte 510 and 511
