org 0x7C00
BITS 16

start:
    jmp main

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

main:
;initialisation
    xor ax, ax
    mov es, ax
    mov ds, ax

    mov ss, ax
    mov sp, 0x7C00
    
    call clr_screen

;put msg string to the screen
    mov ah, 0x13
    mov al, 1
    mov bh, 0
    mov bl, 0x07
    mov cx, 10
    mov dh, 0
    mov dl, 0
    mov bp, msg
    int 0x10

.halt:
    hlt
    jmp .halt

msg: db "booting..."

times 510 - ($ - $$) db 0	;pad the file with 0
db 0x55				;place the boot signature at byte 510 and 511
db 0xAA
