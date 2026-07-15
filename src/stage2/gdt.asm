BITS 16

section .rodata
gdtr:
    dw (gdt.end - gdt.start) - 1
    dd gdt.start

gdt:
.start:
    ;NULL descriptor
    dq 0

    ;32 bit code segment
    dw 0xffff		;low 16 bits-limit
    dw 0x0000		;low 16 bits-base
    db 0x00		;mid 8 bits-base
    db 0b10011010	;Access byte (P-DPL-S-E-DC-RW-A)
    db 0b11001111	;Flags (G-DB-L-reserved) | high 4 bits-limit
    db 0x00		;high 8 bits-base

    ;32 bit data segment
    dw 0xffff		;low 16 bits-limit
    dw 0x0000		;low 16 bits-base
    db 0x00		;mid 8 bits-base
    db 0b10010010	;Access byte (P-DPL-S-E-DC-RW-A)
    db 0b11001111	;Flags (G-DB-L-reserved) | high 4 bits-limit
    db 0x00		;high 8 bits-base

    ; 16 bit code segment
    dw 0xffff		;low 16 bits-limit
    dw 0x0000		;low 16 bits-base
    db 0x00		;mid 8 bits-base
    db 0b10011010	;Access byte (P-DPL-S-E-DC-RW-A)
    db 0b00001111	;Flags (G-DB-L-reserved) | high 4 bits-limit
    db 0x00		;high 8 bits-base

    ;16 bit data segment
    dw 0xffff		;low 16 bits-limit
    dw 0x0000		;low 16 bits-base
    db 0x00		;mid 8 bits-base
    db 0b10010010	;Access byte (P-DPL-S-E-DC-RW-A)
    db 0b00001111	;Flags (G-DB-L-reserved) | high 4 bits-limit
    db 0x00		;high 8 bits-base

.end:
