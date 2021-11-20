ENTRY(kernel_setup)

SECTIONS
{
	. = 0x200000;
	.bss  : { *(.bss)  }
	.data : { *(.data) }
	.text : { *(.text) }
 }
