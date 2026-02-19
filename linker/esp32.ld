/* ESP32 Linker Script for Bare-Metal Kernel */

/* Entry point */
ENTRY(_start)

/* Memory regions for ESP32-WROOM-32 */
MEMORY
{
    /* Internal RAM for data */
    dram0_0_seg (RW) : ORIGIN = 0x3FFB0000, LENGTH = 176K

    /* Internal RAM for instructions */
    iram0_0_seg (RWX) : ORIGIN = 0x40080000, LENGTH = 128K

    /* Flash mapped region for code */
    irom0_0_seg (RX) : ORIGIN = 0x400D0000, LENGTH = 3M

    /* Flash mapped region for read-only data */
    drom0_0_seg (R) : ORIGIN = 0x3F400000, LENGTH = 3M
}

/* Define sections */
SECTIONS
{
    /* Interrupt vectors and critical code in IRAM */
    .iram0.vectors :
    {
        _iram_start = ABSOLUTE(.);
        . = ALIGN(4);
        _init_start = ABSOLUTE(.);
        KEEP(*(.iram.vectors))
        *(.entry.text)
        *(.init.literal)
        *(.init)
    } > iram0_0_seg

    /* Text section in IRAM for fast execution */
    .iram0.text :
    {
        . = ALIGN(4);
        *(.iram .iram.*)
        *(.iram0.literal .iram.literal .iram.text.literal .iram.text)
        *libgcc.a:(.literal .text .literal.* .text.*)
    } > iram0_0_seg

    _iram_end = ABSOLUTE(.);

    /* Read-only data in Flash */
    .flash.rodata :
    {
        _rodata_start = ABSOLUTE(.);
        . = ALIGN(4);
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)
        _rodata_end = ABSOLUTE(.);
    } > drom0_0_seg

    /* Code in Flash */
    .flash.text :
    {
        _stext = .;
        . = ALIGN(4);
        *(.literal .text .literal.* .text.*)
        *(.stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
        *(.fini.literal)
        *(.fini)
        *(.gnu.version)
        _etext = .;
    } > irom0_0_seg

    /* Data section in DRAM */
    .dram0.data :
    {
        _data_start = ABSOLUTE(.);
        . = ALIGN(4);
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d.*)
        *(.data1)
        *(.sdata)
        *(.sdata.*)
        *(.gnu.linkonce.s.*)
        *(.sdata2)
        *(.sdata2.*)
        *(.gnu.linkonce.s2.*)
        KEEP(*(.jcr))
        _data_end = ABSOLUTE(.);
    } > dram0_0_seg

    /* BSS section in DRAM */
    .dram0.bss (NOLOAD) :
    {
        _bss_start = ABSOLUTE(.);
        . = ALIGN(4);
        *(.dynsbss)
        *(.sbss)
        *(.sbss.*)
        *(.gnu.linkonce.sb.*)
        *(.scommon)
        *(.sbss2)
        *(.sbss2.*)
        *(.gnu.linkonce.sb2.*)
        *(.dynbss)
        *(.bss)
        *(.bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN(4);
        _bss_end = ABSOLUTE(.);
    } > dram0_0_seg

    /* Heap section */
    .dram0.heap :
    {
        . = ALIGN(4);
        _heap_start = ABSOLUTE(.);
        . = . + 32K;  /* Reserve 32KB for heap */
        _heap_end = ABSOLUTE(.);
    } > dram0_0_seg

    /* Stack section (grows down) */
    .dram0.stack :
    {
        . = ALIGN(16);
        . = . + 8K;  /* 8KB main stack */
        _stack_top = ABSOLUTE(.);
    } > dram0_0_seg

    _end = ABSOLUTE(.);
}
