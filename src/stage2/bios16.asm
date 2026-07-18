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

    mov dl, [ebp + 8]	;arg1 - drive
    mov ebx, [ebp + 12]	;arg2 - dap pointer

    jmp 0x18:.pm16	;0x18: 16 bit code segment

.pm16:
[BITS 16]
    cli
    mov eax, cr0
    and eax, ~1
    mov cr0, eax

    jmp 0:.rm

.rm:
    mov esi, ebx
    shr esi, 4		;convert the lba pointer to format seg:off (ds:si)
    mov ds, si

    and ebx, 0xf
    mov esi, ebx

;initialize segments
;skip ds because es:di
;points to the dap
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
