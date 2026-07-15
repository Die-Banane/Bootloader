; Convert linear address to segment:offset address
; Args:
;    1 - linear address
;    2 - (out) target segment (e.g. es)
;    3 - target 32-bit register to use (e.g. eax)
;    4 - target lower 16-bit half of #3 (e.g. ax)
%macro LinearToSegOffset 4

    mov %3, %1      ; linear address to eax
    shr %3, 4
    mov %2, %4
    mov %3, %1      ; linear address to eax
    and %3, 0xf

%endmacro

global BiosReadDisk


[BITS 32]
section .data
stack_seg: dw 0
stack_ptr: dq 0

section .text
BiosReadDisk:
    push ebp
    mov ebp, esp

    mov [stack_seg], ss
    mov [stack_ptr], esp

    mov dl, [ebp + 8]				;arg1 - drive
    mov ebx, [ebp + 12]				;arg2 - dap pointer

    jmp 0x18:.pm16				;0x18: 16 bit code segment

.pm16:
[BITS 16]
    cli
    mov eax, cr0
    and eax, ~1
    mov cr0, eax

    jmp 0:.rm

;initialize segments
;skip ds because es:di
;points to the dap
.rm:
    mov si, bx
    shr si, 4
    mov ds, si

    and bx, 0xf
    mov si, bx

    xor ax, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00	;temporary stack

    stc
    mov ah, 0x42
    int 0x13

;go back to pm
    cli
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:.pm32

.pm32:
[BITS 32]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ax, [stack_seg]
    mov ss, ax

    mov esp, [stack_ptr]

    mov eax, 1
    sbb eax, 0

    pop ebp
    ret
