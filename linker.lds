OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    RAM       (xrw) : ORIGIN = 0x00000000, LENGTH = 32M
    VIDEO_RAM (rw)  : ORIGIN = 0x08000000, LENGTH = 150K  /* 2 × 320×240 framebuffers */
}

PROVIDE(__stack_size = 0x10000);

SECTIONS
{
    .text : {
        *(.text.boot)
        *(.text*)
    } > RAM

    .rodata : { *(.rodata*) } > RAM

    .data : {
        _data_start = .;
        *(.data*)
        PROVIDE(__global_pointer = . + 0x800);
        *(.sdata*)
        _data_end = .;
    } > RAM

    .bss : {
        . = ALIGN(4);
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        _bss_end = .;
    } > RAM

    .stack : {
        PROVIDE(_stack_begin = .);
        . = ALIGN(4);
        . += __stack_size;
        PROVIDE(_stack_end = .);
    } > RAM

    /* Drop GCC metadata sections unused in a freestanding binary. */
    /DISCARD/ : {
        *(.eh_frame)
        *(.eh_frame_hdr)
        *(.comment)
        *(.note*)
    }

    ASSERT(. <= ORIGIN(RAM) + LENGTH(RAM), "RAM overflow: binary exceeds 32M")
}
