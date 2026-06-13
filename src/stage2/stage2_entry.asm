BITS 16

extern c_main
global stage2_entry

stage2_entry:
    call enable_a20
    cmp ax, 1
    jne halt ;TODO: make error function

;switch to protected mode
    cli
    lgdt [gdtr]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x0008:pm_entry

halt:
    hlt
    jmp halt

BITS 32
pm_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x90000
    mov esp, ebp

    mov al, 'A'
    mov ah, 0x0f
    mov [0xb8000], ax
    call c_main

pm_halt:
    hlt
    jmp pm_halt

%include "a20.asm"
%include "gdt.asm"
