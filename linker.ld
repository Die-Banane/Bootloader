ENTRY(stage2_entry)
SECTIONS
{
    . = 0x7E00;
    .text :
    {
        *stage2_entry.o(.text)   /* stage2_entry MUSS zuerst kommen */
        *(.text)
    }
    .rodata : { *(.rodata) }
    .data   : { *(.data)   }
    .bss    :
    {
        *(.bss)
        *(COMMON)
    }
}
