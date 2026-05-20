org 0x7E00
BITS 16

main:
    call clr_screen

    mov si, msg
    call print

.halt:
    hlt
    jmp .halt

%include "stage2/screen.asm"

msg: db "Hello from stage2", 0x0d, 0x0a, 0
