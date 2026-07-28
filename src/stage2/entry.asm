BITS 16

extern c_main
extern _size
global entry

section .data
ebda_start: dd 0
boot_drive: db 0

section .text
entry:
    pop ax
    mov [boot_drive], al
   
;get the starting address of EBDA
    xor eax, eax
    int 0x12
    shl eax, 10
    mov [ebda_start], eax
    
    call enable_a20
    cmp ax, 1
    jne halt 		;TODO: make error function

    cli
    lgdt [gdtr]
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:pm_entry

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
    mov esp, [ebda_start]

    movzx eax, byte [boot_drive]
    push eax
    push _size
    call c_main

pm_halt:
    hlt
    jmp pm_halt

%include "a20.asm"
%include "gdt.asm"
