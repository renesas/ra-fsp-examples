/*
                  Linker File for Renesas FSP TFM
*/

RAM_START = 0x20020000;
RAM_LENGTH = 0x20000;
FLASH_START = (0x50000 + 0x400);
FLASH_LENGTH = ((0x38000 - 0x400) -0x400);
DATA_FLASH_START = 0x08001000;
DATA_FLASH_LENGTH = 0x1000;

SDRAM_START = 0x90000000;
SDRAM_LENGTH = 0x0;
QSPI_FLASH_START = 0x60000000;
QSPI_FLASH_LENGTH = 0x4000000;
OSPI_DEVICE_0_START = 0x68000000;
OSPI_DEVICE_0_LENGTH = 0x8000000;
OSPI_DEVICE_1_START = 0x70000000;
OSPI_DEVICE_1_LENGTH = 0x10000000;


QSPI_FLASH_PRV_LENGTH = DEFINED(QSPI_FLASH_SIZE) ? ABSOLUTE(QSPI_FLASH_SIZE) : ABSOLUTE(QSPI_FLASH_LENGTH);
OSPI_DEVICE_0_PRV_LENGTH = DEFINED(OSPI_DEVICE_0_SIZE) ? ABSOLUTE(OSPI_DEVICE_0_SIZE) : ABSOLUTE(OSPI_DEVICE_0_LENGTH);
OSPI_DEVICE_1_PRV_LENGTH = DEFINED(OSPI_DEVICE_1_SIZE) ? ABSOLUTE(OSPI_DEVICE_1_SIZE) : ABSOLUTE(OSPI_DEVICE_1_LENGTH);

/* Define memory regions. */
MEMORY
{
  FLASH (rx)      : ORIGIN = FLASH_START, LENGTH = FLASH_LENGTH
  RAM (rwx)       : ORIGIN = RAM_START, LENGTH = RAM_LENGTH
  DATA_FLASH (rx) : ORIGIN = DATA_FLASH_START, LENGTH = DATA_FLASH_LENGTH
  QSPI_FLASH (rx) : ORIGIN = QSPI_FLASH_START, LENGTH = QSPI_FLASH_PRV_LENGTH
  OSPI_DEVICE_0 (rx) : ORIGIN = OSPI_DEVICE_0_START, LENGTH = OSPI_DEVICE_0_PRV_LENGTH
  OSPI_DEVICE_1 (rx) : ORIGIN = OSPI_DEVICE_1_START, LENGTH = OSPI_DEVICE_1_PRV_LENGTH
  SDRAM (rwx)     : ORIGIN = SDRAM_START, LENGTH = SDRAM_LENGTH
}

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __Vectors_End
 *   __Vectors_Size
 *   __qspi_flash_start__
 *   __qspi_flash_end__
 *   __qspi_flash_code_size__
 *   __qspi_region_max_size__
 *   __qspi_region_start_address__
 *   __qspi_region_end_address__
 *   __ospi_device_0_start__
 *   __ospi_device_0_end__
 *   __ospi_device_0_code_size__
 *   __ospi_device_1_start__
 *   __ospi_device_1_end__
 *   __ospi_device_1_code_size__
 *   __ospi_region_max_size__
 *   __ospi_region_start_address__
 *   __ospi_region_end_address__
 */
ENTRY(Reset_Handler)

SECTIONS
{
    .text :
    {
        __tz_FLASH_S = ORIGIN(FLASH);
        __ROM_Start = .;

        /* Even though the vector table is not 256 entries (1KB) long, we still allocate that much
         * space because ROM registers are at address 0x400 and there is very little space
         * in between. */
        KEEP(*(.fixed_vectors*))
        KEEP(*(.application_vectors*))
        __Vectors_End = .;

        /* ROM Registers start at address 0x00000400 */
        . = __ROM_Start + 0x400;
        KEEP(*(.rom_registers*))

        /* Reserving 0x100 bytes of space for ROM registers. */
        . = __ROM_Start + 0x500;

        *(.text*)

        KEEP(*(.version))
        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata*)
        __usb_dev_descriptor_start_fs = .;
        KEEP(*(.usb_device_desc_fs*))
        __usb_cfg_descriptor_start_fs = .;
        KEEP(*(.usb_config_desc_fs*))
        __usb_interface_descriptor_start_fs = .;
        KEEP(*(.usb_interface_desc_fs*))
        __usb_descriptor_end_fs = .;
        __usb_dev_descriptor_start_hs = .;
        KEEP(*(.usb_device_desc_hs*))
        __usb_cfg_descriptor_start_hs = .;
        KEEP(*(.usb_config_desc_hs*))
        __usb_interface_descriptor_start_hs = .;
        KEEP(*(.usb_interface_desc_hs*))
        __usb_descriptor_end_hs = .;

        KEEP(*(.eh_frame*))

        __ROM_End = .;
    } > FLASH = 0xFF

    __Vectors_Size = __Vectors_End - __Vectors;

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    /* To copy multiple ROM to RAM sections,
     * uncomment .copy.table section and,
     * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
    /*
    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (__etext)
        LONG (__data_start__)
        LONG (__data_end__ - __data_start__)
        LONG (__etext2)
        LONG (__data2_start__)
        LONG (__data2_end__ - __data2_start__)
        __copy_table_end__ = .;
    } > FLASH
    */

    /* To clear multiple BSS sections,
     * uncomment .zero.table section and,
     * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
    /*
    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG (__bss_end__ - __bss_start__)
        LONG (__bss2_start__)
        LONG (__bss2_end__ - __bss2_start__)
        __zero_table_end__ = .;
    } > FLASH
    */

    __etext = .;

    __tz_RAM_S = ORIGIN(RAM);

    /* If DTC is used, put the DTC vector table at the start of SRAM.
       This avoids memory holes due to 1K alignment required by it. */
    .fsp_dtc_vector_table (NOLOAD) :
    {
        . = ORIGIN(RAM);
        *(.fsp_dtc_vector_table)
    } > RAM

    /* Initialized data section. */
    .data :
    {
        __data_start__ = .;
        . = ALIGN(4);

        __Code_In_RAM_Start = .;

        KEEP(*(.code_in_ram*))
        __Code_In_RAM_End = .;

        *(vtable)
        /* Don't use *(.data*) because it will place data meant for .data_flash in this section. */
        *(.data.*)
        *(.data)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))

        . = ALIGN(4);

        /* All data end */
        __data_end__ = .;

    } > RAM AT > FLASH


    /* TrustZone Secure Gateway Stubs Section. */
    .gnu.sgstubs : ALIGN (1024)
    {
        . = DEFINED(FLASH_NSC_START) ? ABSOLUTE(FLASH_NSC_START) : ALIGN(1024);
        __tz_FLASH_C = .;
        _start_sg = .;
        *(.gnu.sgstubs*)
        . = ALIGN(32);
        _end_sg = .;
    } > FLASH

    __tz_FLASH_N = DEFINED(FLASH_NS_START) ? ABSOLUTE(FLASH_NS_START) : ALIGN(32768);

    /* Note: There are no secure/non-secure boundaries for QSPI.  These symbols are provided for the RA configuration tool. */
    __tz_QSPI_FLASH_S = ORIGIN(QSPI_FLASH);

    /* QSPI_FLASH section to be downloaded via debugger */
    .qspi_flash :
    {
        __qspi_flash_start__ = .;
        KEEP(*(.qspi_flash*))
        KEEP(*(.code_in_qspi*))
        __qspi_flash_end__ = .;
    } > QSPI_FLASH
    __qspi_flash_code_size__ = __qspi_flash_end__ - __qspi_flash_start__;

    /* QSPI_FLASH non-retentive section, creates a copy in internal flash that can be copied to QSPI */
    __qspi_flash_code_addr__ = __etext + (__data_end__ - __data_start__);
    .qspi_non_retentive : AT (__qspi_flash_code_addr__)
    {
        __qspi_non_retentive_start__ = .;
        KEEP(*(.qspi_non_retentive*))
        __qspi_non_retentive_end__ = .;
    } > QSPI_FLASH
    __qspi_non_retentive_size__ = __qspi_non_retentive_end__ - __qspi_non_retentive_start__;

    __qspi_region_max_size__ = 0x4000000;   /* Must be the same as defined in MEMORY above */
    __qspi_region_start_address__ = __qspi_flash_start__;
    __qspi_region_end_address__ = __qspi_flash_start__ + __qspi_region_max_size__;

    /* Note: There are no secure/non-secure boundaries for QSPI.  These symbols are provided for the RA configuration tool. */
    __tz_QSPI_FLASH_N = __qspi_non_retentive_end__;

    /* Note: There are no secure/non-secure boundaries for QSPI.  These symbols are provided for the RA configuration tool. */
    __tz_OSPI_DEVICE_0_S = ORIGIN(OSPI_DEVICE_0);

    /* OSPI_DEVICE_0 section to be downloaded via debugger */
    .OSPI_DEVICE_0 :
    {
        __ospi_device_0_start__ = .;
        KEEP(*(.ospi_device_0*))
        KEEP(*(.code_in_ospi_device_0*))
        __ospi_device_0_end__ = .;
    } > OSPI_DEVICE_0
    __ospi_device_0_code_size__ = __ospi_device_0_end__ - __ospi_device_0_start__;

    /* OSPI_DEVICE_0 non-retentive section, creates a copy in internal flash that can be copied to OSPI */
    __ospi_device_0_code_addr__ = __etext + (__data_end__ - __data_start__);
    .ospi_non_retentive : AT (__ospi_device_0_code_addr__)
    {
        __ospi_non_retentive_start__ = .;
        KEEP(*(.ospi_non_retentive*))
        __ospi_non_retentive_end__ = .;
    } > OSPI_DEVICE_0
    __ospi_non_retentive_size__ = __ospi_non_retentive_end__ - __ospi_non_retentive_start__;

    __ospi_region_max_size__ = 0x8000000;   /* Must be the same as defined in MEMORY above */
    __ospi_region_start_address__ = __ospi_device_0_start__;
    __ospi_region_end_address__ = __ospi_device_0_start__ + __ospi_region_max_size__;

    /* Note: There are no secure/non-secure boundaries for OSPI.  These symbols are provided for the RA configuration tool. */
    __tz_OSPI_DEVICE_0_N = __ospi_non_retentive_end__;

    /* Note: There are no secure/non-secure boundaries for OSPI.  These symbols are provided for the RA configuration tool. */
    __tz_OSPI_DEVICE_1_S = ORIGIN(OSPI_DEVICE_1);

    /* OSPI_DEVICE_1 section to be downloaded via debugger */
    .OSPI_DEVICE_1 :
    {
        __ospi_device_1_start__ = .;
        KEEP(*(.ospi_device_1*))
        KEEP(*(.code_in_ospi_device_1*))
        __ospi_device_1_end__ = .;
    } > OSPI_DEVICE_1
    __ospi_device_1_code_size__ = __ospi_device_1_end__ - __ospi_device_1_start__;

    /* OSPI_DEVICE_1 non-retentive section, creates a copy in internal flash that can be copied to OSPI */
    __ospi_device_1_code_addr__ = __etext + (__data_end__ - __data_start__);
    .ospi_non_retentive : AT (__ospi_device_1_code_addr__)
    {
        __ospi_non_retentive_start__ = .;
        KEEP(*(.ospi_non_retentive*))
        __ospi_non_retentive_end__ = .;
    } > OSPI_DEVICE_1
    __ospi_non_retentive_size__ = __ospi_non_retentive_end__ - __ospi_non_retentive_start__;

    __ospi_region_max_size__ = 0x10000000;   /* Must be the same as defined in MEMORY above */
    __ospi_region_start_address__ = __ospi_device_1_start__;
    __ospi_region_end_address__ = __ospi_device_1_start__ + __ospi_region_max_size__;

    /* Note: There are no secure/non-secure boundaries for OSPI.  These symbols are provided for the RA configuration tool. */
    __tz_OSPI_DEVICE_1_N = __ospi_non_retentive_end__;

    .noinit (NOLOAD):
    {
        . = ALIGN(4);
        __noinit_start = .;
        KEEP(*(.noinit*))
        . = ALIGN(8);
        /* Place the FreeRTOS heap here so that the __HeapLimit calculation does not include the freertos heap. */
        KEEP(*(.heap.*))
        __noinit_end = .;
    } > RAM

    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM

    .heap (NOLOAD):
    {
        . = ALIGN(8);
        __HeapBase = .;
        /* Place the STD heap here. */
        KEEP(*(.heap))
        __HeapLimit = .;
    } > RAM

    /* Stacks are stored in this section. */
    .stack_dummy (NOLOAD):
    {
        . = ALIGN(8);
        __StackLimit = .;
        /* Main stack */
        KEEP(*(.stack))
        __StackTop = .;
        /* Thread stacks */
        KEEP(*(.stack*))
        __StackTopAll = .;
    } > RAM

    PROVIDE(__stack = __StackTopAll);

    /* This symbol represents the end of user allocated RAM. The RAM after this symbol can be used
       at run time for things such as ThreadX memory pool allocations. */
    __RAM_segment_used_end__ = ALIGN(__StackTopAll , 4);

    __tz_RAM_C = DEFINED(RAM_NSC_START) ? ABSOLUTE(RAM_NSC_START) : ALIGN(__RAM_segment_used_end__, 1024);
    __tz_RAM_N = DEFINED(RAM_NS_START) ? ABSOLUTE(RAM_NS_START) : ALIGN(__tz_RAM_C, 8192);

    __tz_DATA_FLASH_S = ORIGIN(DATA_FLASH);

    /* Data flash. */
    .data_flash :
    {
        . = ORIGIN(DATA_FLASH);
        __Data_Flash_Start = .;
        KEEP(*(.data_flash*))
        __Data_Flash_End = .;
    } > DATA_FLASH

    __tz_DATA_FLASH_N = DEFINED(DATA_FLASH_NS_START) ? ABSOLUTE(DATA_FLASH_NS_START) : ALIGN(1024);

    /* Note: There are no secure/non-secure boundaries for SDRAM.  These symbols are provided for the RA configuration tool. */
    __tz_SDRAM_S = ORIGIN(SDRAM);

    /* SDRAM */
    .sdram (NOLOAD):
    {
        __SDRAM_Start = .;
        KEEP(*(.sdram*))
        KEEP(*(.frame*))
        __SDRAM_End = .;
    } > SDRAM

    /* Note: There are no secure/non-secure boundaries for SDRAM.  These symbols are provided for the RA configuration tool. */
    __tz_SDRAM_N = __SDRAM_End;

}

