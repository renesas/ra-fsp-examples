MEMORY
{

    SECURE_RAM_PROGRAM (rwx) : ORIGIN = SECURE_RAM_PROGRAM_START, LENGTH = SECURE_RAM_PROGRAM_LENGTH
    SECURE_RAM_DATA (rw) : ORIGIN = SECURE_RAM_DATA_START, LENGTH = SECURE_RAM_DATA_LENGTH
    RAM (rwx) : ORIGIN = RAM_START, LENGTH = RAM_LENGTH
    SECURE_FLASH_PROGRAM (rx) : ORIGIN = SECURE_FLASH_PROGRAM_START, LENGTH = SECURE_FLASH_PROGRAM_LENGTH
    SECURE_FLASH_DATA (rw) : ORIGIN = SECURE_FLASH_DATA_START, LENGTH = SECURE_FLASH_DATA_LENGTH
    FLASH (rx) : ORIGIN = FLASH_START, LENGTH = FLASH_LENGTH
    DATA_FLASH (rx) : ORIGIN = DATA_FLASH_START, LENGTH = DATA_FLASH_LENGTH
    QSPI_FLASH (rx) : ORIGIN = QSPI_FLASH_START, LENGTH = QSPI_FLASH_LENGTH
    SDRAM (rwx) : ORIGIN = SDRAM_START, LENGTH = SDRAM_LENGTH
    OPTION_SETTING_OFS0 (r) : ORIGIN = OPTION_SETTING_OFS0_START, LENGTH = OPTION_SETTING_OFS0_LENGTH
    OPTION_SETTING_OFS1 (r) : ORIGIN = OPTION_SETTING_OFS1_START, LENGTH = OPTION_SETTING_OFS1_LENGTH
    OPTION_SETTING_SECMPU (r) : ORIGIN = OPTION_SETTING_SECMPU_START, LENGTH = OPTION_SETTING_SECMPU_LENGTH
    OPTION_SETTING_OSIS (r) : ORIGIN = OPTION_SETTING_OSIS_START, LENGTH = OPTION_SETTING_OSIS_LENGTH
    FLASH_GAP (rx) : ORIGIN = FLASH_GAP_START, LENGTH = FLASH_GAP_LENGTH
}

/* code entry point...need to define to keep crt0 _start out */
ENTRY( Reset_Handler)
/* Library configurations */
GROUP(libgcc.a libc.a libm.a)

SECTIONS
{
    /***** QSPI_FLASH memory section allocations ******/
    .qspi_flash.startof (READONLY) :
    {
        __ddsc_QSPI_FLASH_START = .;

    }> QSPI_FLASH
    /***** SDRAM memory section allocations ******/
    .sdram.startof :
    {
        __ddsc_SDRAM_START = .;

    }> SDRAM
    /* sdram initialized from qspi_flash */
    __sdram_from_qspi_flash$$ : ALIGN(4)
    {
        __sdram_from_qspi_flash$$Base = .;__sdram_from_qspi_flash$$Load = LOADADDR(__sdram_from_qspi_flash$$);
        /* section.sdram.from_qspi_flash */
        *(.sdram_from_qspi_flash)
        /* section.sdram.code_from_qspi_flash */
        *(.sdram_code_from_qspi_flash)
        __sdram_from_qspi_flash$$Limit = .;
    }> SDRAM AT > QSPI_FLASH

    /***** RAM memory section allocations ******/
    .ram.startof :
    {
        __ddsc_RAM_START = .;

    }> RAM
    __ram_dtc_vector$$ (NOLOAD) :
    {
        __ram_dtc_vector$$Base = .;
        *(.fsp_dtc_vector_table)
        __ram_dtc_vector$$Limit = .;
    }> RAM
    /* ram initialized from qspi_flash */
    __ram_from_qspi_flash$$ : ALIGN(4)
    {
        __ram_from_qspi_flash$$Base = .;__ram_from_qspi_flash$$Load = LOADADDR(__ram_from_qspi_flash$$);
        /* section.ram.from_qspi_flash */
        *(.ram_from_qspi_flash)
        /* section.ram.code_from_qspi_flash */
        *(.ram_code_from_qspi_flash)
        __ram_from_qspi_flash$$Limit = .;
    }> RAM AT > QSPI_FLASH

    __qspi_flash_readonly$$ (READONLY) :
    {
        __qspi_flash_readonly$$Base = .;
        /* section.qspi_flash.readonly */
        *(.qspi_flash)
        /* section.qspi_flash.code */
        *(.qspi_flash_code)
        __qspi_flash_readonly$$Limit = .;
    }> QSPI_FLASH
    __qspi_flash_noinit$$ (NOLOAD) :
    {
        __qspi_flash_noinit$$Base = .;
        /* section.qspi_flash.noinit */
        *(.qspi_flash_noinit)
        __qspi_flash_noinit$$Limit = .;
    }> QSPI_FLASH
    .qspi_flash.endof ALIGN(.,512) (READONLY) :
    {
        __ddsc_QSPI_FLASH_END = .;

    }> QSPI_FLASH

    /***** DATA_FLASH memory section allocations ******/
    .data_flash.startof (READONLY) :
    {
        __ddsc_DATA_FLASH_START = .;

    }> DATA_FLASH
    /***** SDRAM memory section allocations ******/
    /* sdram initialized from data_flash */
    __sdram_from_data_flash$$ : ALIGN(4)
    {
        __sdram_from_data_flash$$Base = .;__sdram_from_data_flash$$Load = LOADADDR(__sdram_from_data_flash$$);
        /* section.sdram.from_data_flash */
        *(.sdram_from_data_flash)
        /* section.sdram.code_from_data_flash */
        *(.sdram_code_from_data_flash)
        __sdram_from_data_flash$$Limit = .;
    }> SDRAM AT > DATA_FLASH

    /***** RAM memory section allocations ******/
    /* ram initialized from data_flash */
    __ram_from_data_flash$$ : ALIGN(4)
    {
        __ram_from_data_flash$$Base = .;__ram_from_data_flash$$Load = LOADADDR(__ram_from_data_flash$$);
        /* section.ram.from_data_flash */
        *(.ram_from_data_flash)
        /* section.ram.code_from_data_flash */
        *(.ram_code_from_data_flash)
        __ram_from_data_flash$$Limit = .;
    }> RAM AT > DATA_FLASH

    __data_flash_readonly$$ (READONLY) :
    {
        __data_flash_readonly$$Base = .;
        /* section.data_flash.readonly */
        *(.data_flash)
        /* section.data_flash.code */
        *(.data_flash_code)
        __data_flash_readonly$$Limit = .;
    }> DATA_FLASH
    __data_flash_noinit$$ (NOLOAD) :
    {
        __data_flash_noinit$$Base = .;
        /* section.data_flash.noinit */
        *(.data_flash_noinit)
        __data_flash_noinit$$Limit = .;
    }> DATA_FLASH
    .data_flash.endof ALIGN(.,512) (READONLY) :
    {
        __ddsc_DATA_FLASH_END = .;

    }> DATA_FLASH

    /***** FLASH memory section allocations ******/
    .flash.startof (READONLY) :
    {
        __ddsc_FLASH_START = .;

    }> FLASH_GAP
    /* MCU vector table */
    __flash_vectors$$ (READONLY) :
    {
        __flash_vectors$$Base = .; _VECTORS = .;
        KEEP(*(.fixed_vectors))
        KEEP(*(.application_vectors))
        __flash_vectors$$Limit = .;
    }> FLASH_GAP
    /* Sections that can be used to fill flash gap */
    __flash_readonly_gap$$ (READONLY) :
    {
        __flash_readonly_gap$$Base = .;
        /* section.flash.readonly_gap */
        *bsp_linker.?*(.rodata.*)
        *(.flash_gap)
        /* section.flash.code_gap */
        *(.flash_gap_code)
        __flash_readonly_gap$$Limit = .;
    }> FLASH_GAP

    /**** Customize start *****/
    .secure_text :
	{
		__SECURE_ROM_Start = .;
		PROVIDE(__SECURE_ROM_Start = .);
		 *  (  .secure_text*  )
		 *  (  .secure_ctors  )
		 *  (  .secure_dtors  )
		__SECURE_ROM_End = .;
		PROVIDE(__SECURE_ROM_End = .);
	} >SECURE_FLASH_PROGRAM
	__e_secure_text = .;
	.secure_rodata :
	{
		__secure_rodata_start__ = .;
		. = ALIGN ( 4 ) ;
		 *  (  .secure_rodata*  )
		. = ALIGN ( 4 ) ;
		__secure_rodata_end__ = .;
	} >SECURE_FLASH_DATA
	.secure_data_section :
	{
		__secure_data_start__ = .;
		 *  (  .secure_data*  )
		. = ALIGN ( 4 ) ;
		__secure_data_end__ = .;
	} >SECURE_RAM_DATA AT>SECURE_FLASH_DATA
	.secure_bss :
	{
		. = ALIGN ( 4 ) ;
		__secure_bss_start__ = .;
		 *  (  .secure_bss*  )
		. = ALIGN ( 4 ) ;
		__secure_bss_end__ = .;
	} >SECURE_RAM_DATA
	PROVIDE(__secure_stack = __secure_StackTop);
	.secure_sram_program :
	{
		. = ALIGN ( 8 ) ;
		__secure_sram_program_start = .;
		KEEP( *  (  .secure_sram_program*  ))
		
		__ram_from_flash$$Base = .;
		KEEP( *  (  .ram_from_flash*  ))
		__ram_from_flash$$Limit = .;
		
		__secure_sram_program_end = .;
	} >SECURE_RAM_PROGRAM AT>SECURE_FLASH_PROGRAM
    
    /**** Customize end *****/

    /***** FLASH memory section allocations ******/
    __flash_noinit$$ (NOLOAD) :
    {
        __flash_noinit$$Base = .;
        /* section.flash.noinit */
        *(.flash_noinit)
        __flash_noinit$$Limit = .;
    }> FLASH
    /***** SDRAM memory section allocations ******/
    /* sdram initialized from flash */
    __sdram_from_flash$$ : ALIGN(4)
    {
        __sdram_from_flash$$Base = .;__sdram_from_flash$$Load = LOADADDR(__sdram_from_flash$$);
        /* section.sdram.from_flash */
        *(.sdram_from_flash)
        /* section.sdram.code_from_flash */
        *(.sdram_code_from_flash)
        __sdram_from_flash$$Limit = .;
    }> SDRAM AT > FLASH
    /* Non-initialized sdram */
    __sdram_noinit$$ (NOLOAD) : ALIGN(4)
    {
        __sdram_noinit$$Base = .;
        /* section.sdram.noinit */
        *(.sdram_noinit)
        __sdram_noinit$$Limit = .;
    }> SDRAM
    /* Zeroed sdram */
    __sdram_zero$$ (NOLOAD) : ALIGN(4)
    {
        __sdram_zero$$Base = .;
        /* section.sdram.zero */
        *(.sdram)
        __sdram_zero$$Limit = .;
    }> SDRAM
    .sdram.endof ALIGN(.,512) :
    {
        __ddsc_SDRAM_END = .;

    }> SDRAM

    /***** RAM memory section allocations ******/
    /* ram initialized from flash */
    __ram_from_flash$$ : ALIGN(4)
    {
        __ram_from_flash$$Base = .;__ram_from_flash$$Load = LOADADDR(__ram_from_flash$$);
        /* section.ram.from_flash */
        *(.ram_from_flash)
        /* section.ram.code_from_flash */
        *(.ram_code_from_flash)
        *(.data*)
        *(vtable)
        __ram_from_flash$$Limit = .;
    }> RAM AT > FLASH
    /* Non-initialized ram */
    __ram_noinit$$ (NOLOAD) : ALIGN(4)
    {
        __ram_noinit$$Base = .;
        /* section.ram.noinit */
        *(.bss.g_heap)
        *(.bss.g_main_stack)
        *(.ram_noinit)
        *(.noinit)
        __ram_noinit$$Limit = .;
    }> RAM
    /* Zeroed ram */
    __ram_zero$$ (NOLOAD) : ALIGN(4)
    {
        __ram_zero$$Base = .;
        /* section.ram.zero */
        *(.ram)
        *(.bss*)
        __ram_zero$$Limit = .;
    }> RAM
    /* Thread Stacks */
    __ram_thread_stack$$ (NOLOAD) : ALIGN(8)
    {
        __ram_thread_stack$$Base = .;
        KEEP(*(.stack?*))
        __ram_thread_stack$$Limit = .;
    }> RAM
    .ram.endof ALIGN(.,512) :
    {
        __ddsc_RAM_END = .;

    }> RAM

    __flash_readonly$$ (READONLY) :
    {
        __flash_readonly$$Base = .;
        /* section.flash.readonly */
        *(.flash)
        /* section.flash.code */
        *(.flash_code)
        *(.text*)
        *(.rodata*)
        KEEP(*(.mcuboot_sce9_key))
        KEEP(*(.version))
        __flash_readonly$$Limit = .;
    }> FLASH
    __flash_ctor$$ (READONLY) :
    {

        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        EXCLUDE_FILE (*crtend?.o *crtend.o) *(.ctors)
        *(SORT(.ctors.*))
        *(.ctors)
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        EXCLUDE_FILE (*crtend?.o *crtend.o) *(.dtors)
        *(SORT(.dtors.*))
        *(.dtors)

    }> FLASH
    __flash_preinit_array$$ (READONLY) : ALIGN(4)
    {
        __preinit_array_start = .;
        KEEP(*(.preinit_array))
        __preinit_array_end = .;
    }> FLASH
    __flash_.got$$ (READONLY) :
    {

        *(.got.plt)
        *(.got)

    }> FLASH
    __flash_init_array$$ (READONLY) : ALIGN(4)
    {
        __init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end = .;
    }> FLASH
    __flash_fini_array$$ (READONLY) : ALIGN(4)
    {
        __fini_array_start = .;
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        __fini_array_end = .;
    }> FLASH
    /* .ARM.extab sections contain exception unwinding information. */
    __flash_arm.extab$$ (READONLY) :
    {

        *(.ARM.extab*)
        *(.gnu.linkonce.armextab.*)

    }> FLASH
    /* .ARM.exidx sections contains index entries for section unwinding. */
    __flash_arm.exidx$$ (READONLY) :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
        __exidx_end = .;
    }> FLASH
    .flash.endof ALIGN(.,512) (READONLY) :
    {
        __ddsc_FLASH_END = .;

    }> FLASH

    /***** OPTION_SETTING_OFS0 memory section allocations ******/
    .option_setting_ofs0.startof (READONLY) :
    {
        __ddsc_OPTION_SETTING_OFS0_START = .;

    }> OPTION_SETTING_OFS0
    /* Option Function Select Register 0 */
    __option_setting_ofs0_reg$$ (READONLY) :
    {
        __option_setting_ofs0_reg$$Base = .;
        KEEP(*(.option_setting_ofs0))
        __option_setting_ofs0_reg$$Limit = .;
    }> OPTION_SETTING_OFS0
    .option_setting_ofs0.endof (READONLY) :
    {
        __ddsc_OPTION_SETTING_OFS0_END = .;

    }> OPTION_SETTING_OFS0

    /***** OPTION_SETTING_OFS1 memory section allocations ******/
    .option_setting_ofs1.startof (READONLY) :
    {
        __ddsc_OPTION_SETTING_OFS1_START = .;

    }> OPTION_SETTING_OFS1
    /* Option Function Select Register 1 */
    __option_setting_ofs1_reg$$ (READONLY) :
    {
        __option_setting_ofs1_reg$$Base = .;
        KEEP(*(.option_setting_ofs1))
        __option_setting_ofs1_reg$$Limit = .;
    }> OPTION_SETTING_OFS1
    .option_setting_ofs1.endof (READONLY) :
    {
        __ddsc_OPTION_SETTING_OFS1_END = .;

    }> OPTION_SETTING_OFS1

    /***** OPTION_SETTING_SECMPU memory section allocations ******/
    .option_setting_secmpu.startof (READONLY) :
    {
        __ddsc_OPTION_SETTING_SECMPU_START = .;

    }> OPTION_SETTING_SECMPU
    /* Security MPU Registers */
    __option_setting_secmpu_reg$$ (READONLY) :
    {
        __option_setting_secmpu_reg$$Base = .;
        KEEP(*(.option_setting_secmpu))
        __option_setting_secmpu_reg$$Limit = .;
    }> OPTION_SETTING_SECMPU
    .option_setting_secmpu.endof (READONLY) :
    {
        __ddsc_OPTION_SETTING_SECMPU_END = .;

    }> OPTION_SETTING_SECMPU

    /***** OPTION_SETTING_OSIS memory section allocations ******/
    .option_setting_osis.startof (READONLY) :
    {
        __ddsc_OPTION_SETTING_OSIS_START = .;

    }> OPTION_SETTING_OSIS
    /* OCD/Serial Programmer ID setting register */
    __option_setting_osis_reg$$ (READONLY) :
    {
        __option_setting_osis_reg$$Base = .;
        KEEP(*(.option_setting_osis))
        __option_setting_osis_reg$$Limit = .;
    }> OPTION_SETTING_OSIS
    .option_setting_osis.endof (READONLY) :
    {
        __ddsc_OPTION_SETTING_OSIS_END = .;

    }> OPTION_SETTING_OSIS

}

