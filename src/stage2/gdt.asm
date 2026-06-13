BITS 16

gdtr:
    dw (.gdt_end - .gdt_start) + 8 - 1
    dd .gdt_start - 8	;subtract 8 to place the pointer one entry before the code entry (null descriptor)

.gdt_start:
    .code:
    dw 0xffff		;low 16 bits-limit
    dw 0x0000		;low 16 bits-base
    db 0x00		;mid 8 bits-base
    db 0b10011010	;Access byte (P-DPL-S-E-DC-RW-A)
    db 0b11001111	;Flags (G-DB-L-reserved) | high 4 bits-limit
    db 0x00		;high 8 bits-base

    .data:
    dw 0xffff		;low 16 bits-limit
    dw 0x0000		;low 16 bits-base
    db 0x00		;mid 8 bits-base
    db 0b10010010	;Access byte (P-DPL-S-E-DC-RW-A)
    db 0b11001111	;Flags (G-DB-L-reserved) | high 4 bits-limit
    db 0x00		;high 8 bits-base
.gdt_end:
