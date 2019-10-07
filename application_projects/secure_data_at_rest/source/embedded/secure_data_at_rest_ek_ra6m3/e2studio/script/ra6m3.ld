/*
                  Linker File for RA6M3 MCU
*/

/* Linker script to configure memory regions. */
MEMORY				
{				
VECTOR_TABLE (rx)  : ORIGIN = 0x00000000, LENGTH = 0x400		/*   1024 bytes */		
  SECURE_PROGRAM (rx)  : ORIGIN = 0x00000400, LENGTH = 0x0007FC00  /*   256K - 1024 bytes */				
  SECURE_DATA  (rw)  : ORIGIN = 0x00080000, LENGTH = 0x00080000  /*   256K bytes */				
				
  FLASH (rx)         : ORIGIN = 0x00100000, LENGTH = 0x100000  /*   512K */				
 SECURE_RAM_PROGRAM (rwx)   : ORIGIN = 0x1FFE0000, LENGTH = 0x00010000  /* xyzK */				
SECURE_RAM (rw)	:  ORIGIN = 0x20010000, LENGTH = 0x0040000  /* 192K */ 			
RAM (rwx)          : ORIGIN = 0x20030000, LENGTH = 0x0050000  /* 192K */				
  DATA_FLASH (rx)    : ORIGIN = 0x40100000, LENGTH = 0x0008000  /*  32K */				
  QSPI_FLASH (rx)    : ORIGIN = 0x60000000, LENGTH = 0x4000000  /*  64M, Change in QSPI section below also */				
  SDRAM (rwx)        : ORIGIN = 0x90000000, LENGTH = 0x2000000  /*  32M */				
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
 *   __end__
 *   end
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
 */
ENTRY(Reset_Handler)

SECTIONS
{
	.vector_table_section :				
	{			
        /* Even though the vector table is not 256 entries (1KB) long, we still allocate that much				
         * space because ROM registers are at address 0x400 and there is very little space				
         * in between. */				
         __Vectors_Start = .;				
		KEEP(*(.fixed_vectors*))			
        KEEP(*(.application_vectors*))			
				
		__Vectors_End = .;		
		__end__ = .;		
				
	} > VECTOR_TABLE

	__Vectors_Size = __Vectors_End - __Vectors_Start;
	
    .secure_text :
    {
        	__SECURE_ROM_Start = .;		
		PROVIDE(__SECURE_ROM_Start = .);		
				
		/* ROM Registers start at address 0x00000400 */		
		KEEP(*(.rom_registers*))

        /* Reserving 0x100 bytes of space for ROM registers. */		
		. = __SECURE_ROM_Start + 0x100;	

	*(.secure_text*)		
				
		/* .ctors */		
		*(.secure_ctors)		
				
		/* .dtors */		
 		*(.secure_dtors)		
				
		__SECURE_ROM_End = .;		
		PROVIDE(__SECURE_ROM_End = .);		
				
	} > SECURE_PROGRAM			
				
	__e_secure_text = .;

	.secure_rodata :			
 	{			
 		__secure_rodata_start__ = .;		
				
 	    . = ALIGN(4);			
 	     			
 		*(.secure_rodata*)		
 		        		
 	    . = ALIGN(4);			
				
		/* All data end */		
		__secure_rodata_end__ = .;		
				
 	} > SECURE_DATA			
 				
 	.secure_data_section : 			
	{			
		__secure_data_start__ = .;		
				
		*(.secure_data*)		
                				
		. = ALIGN(4);		
				
						
		/* All data end */		
		__secure_data_end__ = .;		
				
	} > SECURE_RAM AT>SECURE_DATA			
				
 				
     .secure_noinit (NOLOAD):				
    {				
		. = ALIGN(4);		
        __secure_noinit_start = .;				
        KEEP(*(.secure_noinit))				
        __secure_noinit_end = .;				
    } > SECURE_RAM				
    				
    .secure_bss :				
	{			
		. = ALIGN(4);		
		__secure_bss_start__ = .;		
		*(.secure_bss*)		
		. = ALIGN(4);		
		__secure_bss_end__ = .;		
				
	} > SECURE_RAM			
				
	/* Secure stacks are stored in this section. */			
	.secure_stack_dummy (NOLOAD):			
	{			
        . = ALIGN(8);				
	    __secure_StackLimit = .;			
		KEEP(*(.secure_stack*))		
		__secure_StackTop = .;		
				
	} > SECURE_RAM			
				
	PROVIDE(__secure_stack = __secure_StackTop);			
/* Secure application SRAM code and secure flash driver code are stored in this section. */				
	.secure_sram_program :			
	{			
        . = ALIGN(8);				
	    __secure_sram_program_start = .;			
		KEEP(*(.secure_sram_program*))		
				
		__Code_In_RAM_Start = .;		
		KEEP(*(.code_in_ram*))		
		__Code_In_RAM_End = .;		
				
		__secure_sram_program_end = .;		
				
	} > SECURE_RAM_PROGRAM AT>SECURE_PROGRAM			
				
    .secure_heap (NOLOAD):				
	{			
        . = ALIGN(8);				
		KEEP(*(.secure_heap_d1))		
        . = ALIGN(8);				
		__secure_HeapBase = .;		
		__secure_end__ = .;		
		secure_end = __secure_end__;		
		KEEP(*(.secure_heap*))		
		__secure_HeapLimit = .;		
	} > SECURE_RAM						

	.text :			
	{			
		__ROM_Start = .;		

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
	/* Initialized data section. */			

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
	





	
		*(vtable)		
		*(.data.*)		
        				
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
			
		. = ALIGN(4);		
				
		/* All data end */		
		__data_end__ = .;		
				
	} > RAM AT>FLASH

			
    /* QSPI_FLASH section to be downloaded via debugger */
    .qspi_flash :
    {
        __qspi_flash_start__ = .;
        KEEP(*(.qspi_flash*))
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

    .noinit (NOLOAD):
    {
        . = ALIGN(4);
        __noinit_start = .;
        KEEP(*(.noinit*))
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
        KEEP(*(.heap_d1))
        . = ALIGN(8);
        __HeapBase = .;
        __end__ = .;
        end = __end__;
        KEEP(*(.heap*))
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
	   



    /* Data flash. */
    .data_flash :
    {
        __Data_Flash_Start = .;
        KEEP(*(.data_flash*))
        __Data_Flash_End = .;
    } > DATA_FLASH

    /* SDRAM */
    .sdram (NOLOAD):
    {
        __SDRAM_Start = .;
        KEEP(*(.sdram*))
        KEEP(*(.frame*))
        __SDRAM_End = .;
    } > SDRAM

  
}