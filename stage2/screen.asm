;this file contains functions that are
;associated with showing things on the
;screen

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
