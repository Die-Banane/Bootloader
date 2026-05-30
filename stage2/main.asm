org 0x7E00
BITS 16

main:
    call clr_screen

    call enable_a20
    cmp ax, 1
    jne .a20_fail

    mov si, a20_msg
    call print
    jmp halt

.a20_fail:
    mov si, a20_fail_msg
    call print
    jmp halt

halt:
    hlt
    jmp halt

%include "stage2/screen.asm"
%include "stage2/a20.asm"

a20_msg: db "a20 gate activated", 0x0d, 0x0a, 0
a20_fail_msg: db "failed to activate a20 gate", 0x0d, 0x0a, 0
