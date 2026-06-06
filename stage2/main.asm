org 0x7E00
BITS 16

main:
    call enable_a20
    cmp ax, 1
    jne halt ;TODO: make error function

;switch to protected mode
    cli
    lgdt [gdtr]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    or eax, 1
    mov cr0, eax
    jmp 0x0008:pm_main

BITS 32
pm_main:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x90000
    mov esp, ebp

halt:
    hlt
    jmp halt

%include "stage2/a20.asm"
%include "stage2/gdt.asm"
