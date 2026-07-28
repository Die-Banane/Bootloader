ENTRY(entry)
SECTIONS
{
    . = 0x7E00;
    _start = .;
    .text	: { *(.text) }
    .rodata 	: { *(.rodata) }
    .data   	: { *(.data) }
    .bss    	: { *(.bss) }
    _end = .;

    _size = _end - _start; /*size symbol for the loader to know its size at runtime*/
}
