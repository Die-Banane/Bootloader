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
    
    call clr_screen

;put msg string to the screen
    mov si, a20_msg
    call print

    call enable_a20
    call print

.end:
    hlt
    jmp .end

%include "a20.asm"

a20_msg: db "activating A20-Line...", 0x0d, 0x0a, 0

times 510 - ($ - $$) db 0	;pad the file with 0
db 0x55				;place the boot signature at byte 510 and 511
db 0xAA
