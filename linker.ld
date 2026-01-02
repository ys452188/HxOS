ENTRY(kmain)

SECTIONS
{
    . = 0x00100000;

    /* 代码段 */
    .text : {
        KEEP(*(.text.kmain))  /* 确保 kmain 在最前 */
        *(.text*)
    }

    /* 只读数据 (字符串常量在这里) */
    .rodata : {
        *(.rodata*)
        *(.rodata.*)    
    }

    /* 数据段 */
    .data : {
        *(.data*)
    }

    /* BSS段 (未初始化变量) */
    .bss : {
        __bss_start = .;
        *(COMMON)
        *(.bss*)
        __bss_end = .;
    }

    __kernel_end = .;
}