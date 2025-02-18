/*
                  Linker File for Renesas FSP TFM
*/

BL2_HEADER_SIZE   =   (0x400);       /* 1 KB */
BL2_TRAILER_SIZE  =   (0x400);       /* 1 KB */

FLASH_S_PARTITION_SIZE     =     (0x38000); /* S partition: 224 KB */
FLASH_NS_PARTITION_SIZE    =     (0x38000); /* NS partition: 224 KB */
FLASH_AREA_BL2_OFFSET      =     (0x0);
FLASH_AREA_BL2_SIZE        =     (0x18000); /* 96 KB */
CMSE_VENEER_REGION_SIZE    =     (0x400); /* 1KB */
S_DATA_START               =     (0x20000000);
S_DATA_SIZE                =     (0x20000); /* 128 KB */

S_PSP_STACK_SIZE           =     (0x0000800);

FLASH_AREA_0_OFFSET    =    (FLASH_AREA_BL2_OFFSET + FLASH_AREA_BL2_SIZE);
FLASH_AREA_0_SIZE      =    (FLASH_S_PARTITION_SIZE);

FLASH_AREA_1_OFFSET     =   (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE);
FLASH_AREA_1_SIZE     =     (FLASH_NS_PARTITION_SIZE);
/* Secure image secondary slot */
FLASH_AREA_2_OFFSET     =   (FLASH_AREA_1_OFFSET + FLASH_AREA_1_SIZE);

S_IMAGE_PRIMARY_PARTITION_OFFSET   =  (FLASH_AREA_0_OFFSET);
S_IMAGE_SECONDARY_PARTITION_OFFSET =  FLASH_AREA_2_OFFSET;
IMAGE_S_CODE_SIZE = (FLASH_S_PARTITION_SIZE - BL2_HEADER_SIZE - BL2_TRAILER_SIZE);

/* Secure regions */
S_IMAGE_PRIMARY_AREA_OFFSET   =   (S_IMAGE_PRIMARY_PARTITION_OFFSET + BL2_HEADER_SIZE);
S_CODE_START  =  (S_IMAGE_PRIMARY_AREA_OFFSET);
S_CODE_SIZE   =  (IMAGE_S_CODE_SIZE - CMSE_VENEER_REGION_SIZE);
S_CODE_LIMIT  =  (S_CODE_START + S_CODE_SIZE - 1);
NS_IMAGE_PRIMARY_PARTITION_OFFSET = (FLASH_AREA_0_OFFSET + FLASH_S_PARTITION_SIZE);
NS_PARTITION_START  =  NS_IMAGE_PRIMARY_PARTITION_OFFSET;
SECONDARY_PARTITION_START =  (S_IMAGE_SECONDARY_PARTITION_OFFSET);
/* CMSE Veneers region */
CMSE_VENEER_REGION_START = (S_CODE_LIMIT + 1);
BOOT_TFM_SHARED_DATA_SIZE = 0x400;

DATA_FLASH_START = 0x08000000;
DATA_FLASH_LENGTH = 0x0001000;

MEMORY
{
  FLASH    (rx)  : ORIGIN = S_CODE_START, LENGTH = S_CODE_SIZE
  RAM      (rwx) : ORIGIN = S_DATA_START, LENGTH = S_DATA_SIZE
  VENEERS  (rx)  : ORIGIN = CMSE_VENEER_REGION_START, LENGTH = CMSE_VENEER_REGION_SIZE
  DATA_FLASH (rx) : ORIGIN = DATA_FLASH_START, LENGTH = DATA_FLASH_LENGTH
}

__psp_stack_size__ = S_PSP_STACK_SIZE;

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a libc_nano.a)

ENTRY(Reset_Handler)

SECTIONS
{
    .TFM_VECTORS : ALIGN(4)
    {
        __vectors_start__ = .;
        KEEP(*(.fixed_vectors*))
        KEEP(*(.application_vectors*))
        *startup*(.text*)
        . = ALIGN(4);
        __vectors_end__ = .;
        . = ALIGN(4);
    } > FLASH

    .copy.table : ALIGN(4)
    {
        __copy_table_start__ = .;
        LONG (LOADADDR(.TFM_DATA))
        LONG (ADDR(.TFM_DATA))
        LONG (SIZEOF(.TFM_DATA))

        LONG (LOADADDR(.TFM_UNPRIV_DATA))
        LONG (ADDR(.TFM_UNPRIV_DATA))
        LONG (SIZEOF(.TFM_UNPRIV_DATA))

/* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */
        LONG (LOADADDR(.TFM_SP_ITS_LINKER_DATA))
        LONG (ADDR(.TFM_SP_ITS_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_ITS_LINKER_DATA))
/* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

/* TFM_PARTITION_CRYPTO */
        LONG (LOADADDR(.TFM_SP_CRYPTO_LINKER_DATA))
        LONG (ADDR(.TFM_SP_CRYPTO_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_CRYPTO_LINKER_DATA))
/* TFM_PARTITION_CRYPTO */

/* TFM_PARTITION_PLATFORM */
        LONG (LOADADDR(.TFM_SP_PLATFORM_LINKER_DATA))
        LONG (ADDR(.TFM_SP_PLATFORM_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_PLATFORM_LINKER_DATA))
/* TFM_PARTITION_PLATFORM */

/* TFM_PARTITION_INITIAL_ATTESTATION */
        LONG (LOADADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_DATA))
        LONG (ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER_DATA))
/* TFM_PARTITION_INITIAL_ATTESTATION */

        __copy_table_end__ = .;
    } > FLASH

    .zero.table : ALIGN(4)
    {
        __zero_table_start__ = .;
        LONG (ADDR(.TFM_BSS))
        LONG (SIZEOF(.TFM_BSS))

/* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */
        LONG (ADDR(.TFM_SP_ITS_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_ITS_LINKER_BSS))

        LONG (ADDR(.TFM_SP_ITS_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_ITS_LINKER_STACK))
/* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

/* TFM_PARTITION_CRYPTO */
        LONG (ADDR(.TFM_SP_CRYPTO_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_CRYPTO_LINKER_BSS))

        LONG (ADDR(.TFM_SP_CRYPTO_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_CRYPTO_LINKER_STACK))
/* TFM_PARTITION_CRYPTO */

/* TFM_PARTITION_PLATFORM */
        LONG (ADDR(.TFM_SP_PLATFORM_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_PLATFORM_LINKER_BSS))

        LONG (ADDR(.TFM_SP_PLATFORM_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_PLATFORM_LINKER_STACK))
/* TFM_PARTITION_PLATFORM */

/* TFM_PARTITION_INITIAL_ATTESTATION */
        LONG (ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER_BSS))

        LONG (ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER_STACK))
/* TFM_PARTITION_INITIAL_ATTESTATION */

        __zero_table_end__ = .;
    } > FLASH

    .TFM_UNPRIV_CODE : ALIGN(32)
    {
        *libc_nano*:*(.text*)
        *libc_nano*:*(.rodata*)
        *tfm_spm_services.o(.text*)
        *tfm_spm_services.o(.rodata*)
        *platform_retarget_dev.o(.text*)
        *platform_retarget_dev.o(.rodata*)
        *device_definition.o(.text*)
        *device_definition.o(.rodata*)
        *(SFN)
        *libgcc*:*(.text*)
        *libgcc*:*(.rodata*)
        *libtfmsprt*:*(.text*)
        *libtfmsprt*:*(.rodata*)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_UNPRIV_CODE$$RO$$Base = ADDR(.TFM_UNPRIV_CODE);
    Image$$TFM_UNPRIV_CODE$$RO$$Limit = ADDR(.TFM_UNPRIV_CODE) + SIZEOF(.TFM_UNPRIV_CODE);

    /**** PSA RoT RO part (CODE + RODATA) start here */
    Image$$TFM_PSA_CODE_START$$Base = .;

/* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */
    .TFM_SP_ITS_LINKER : ALIGN(32)
    {
        *tfm_internal_trusted_storage*:*(.text*)
        *tfm_internal_trusted_storage*:*(.rodata*)
        *(TFM_SP_ITS_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_ITS_LINKER$$RO$$Base = ADDR(.TFM_SP_ITS_LINKER);
    Image$$TFM_SP_ITS_LINKER$$RO$$Limit = ADDR(.TFM_SP_ITS_LINKER) + SIZEOF(.TFM_SP_ITS_LINKER);
    Image$$TFM_SP_ITS_LINKER$$Base = ADDR(.TFM_SP_ITS_LINKER);
    Image$$TFM_SP_ITS_LINKER$$Limit = ADDR(.TFM_SP_ITS_LINKER) + SIZEOF(.TFM_SP_ITS_LINKER);
/* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

/* TFM_PARTITION_CRYPTO */
    .TFM_SP_CRYPTO_LINKER : ALIGN(32)
    {
        *tfm_crypto*:*(.text*)
        *tfm_crypto*:*(.rodata*)
        *(TFM_SP_CRYPTO_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_CRYPTO_LINKER$$RO$$Base = ADDR(.TFM_SP_CRYPTO_LINKER);
    Image$$TFM_SP_CRYPTO_LINKER$$RO$$Limit = ADDR(.TFM_SP_CRYPTO_LINKER) + SIZEOF(.TFM_SP_CRYPTO_LINKER);
    Image$$TFM_SP_CRYPTO_LINKER$$Base = ADDR(.TFM_SP_CRYPTO_LINKER);
    Image$$TFM_SP_CRYPTO_LINKER$$Limit = ADDR(.TFM_SP_CRYPTO_LINKER) + SIZEOF(.TFM_SP_CRYPTO_LINKER);
 /* TFM_PARTITION_CRYPTO */

/* TFM_PARTITION_PLATFORM */
    .TFM_SP_PLATFORM_LINKER : ALIGN(32)
    {
        *tfm_platform*:*(.text*)
        *tfm_platform*:*(.rodata*)
        *(TFM_SP_PLATFORM_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_PLATFORM_LINKER$$RO$$Base = ADDR(.TFM_SP_PLATFORM_LINKER);
    Image$$TFM_SP_PLATFORM_LINKER$$RO$$Limit = ADDR(.TFM_SP_PLATFORM_LINKER) + SIZEOF(.TFM_SP_PLATFORM_LINKER);
    Image$$TFM_SP_PLATFORM_LINKER$$Base = ADDR(.TFM_SP_PLATFORM_LINKER);
    Image$$TFM_SP_PLATFORM_LINKER$$Limit = ADDR(.TFM_SP_PLATFORM_LINKER) + SIZEOF(.TFM_SP_PLATFORM_LINKER);
 /* TFM_PARTITION_PLATFORM */

/* TFM_PARTITION_INITIAL_ATTESTATION */
    .TFM_SP_INITIAL_ATTESTATION_LINKER : ALIGN(32)
    {
        *tfm_attest*:*(.text*)
        *tfm_attest*:*(.rodata*)
        *(TFM_SP_INITIAL_ATTESTATION_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER$$RO$$Base = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER);
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER$$RO$$Limit = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER) + SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER);
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER$$Base = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER);
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER$$Limit = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER) + SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER);
 /* TFM_PARTITION_INITIAL_ATTESTATION */

    /**** PSA RoT RO part (CODE + RODATA) end here */
    Image$$TFM_PSA_CODE_END$$Base = .;

    /**** APPLICATION RoT RO part (CODE + RODATA) start here */
    Image$$TFM_APP_CODE_START$$Base = .;

    /**** APPLICATION RoT RO part (CODE + RODATA) end here */
    Image$$TFM_APP_CODE_END$$Base = .;


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


    .ER_TFM_CODE : ALIGN(4)
    {
        *(.text*)

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

        KEEP(*(.eh_frame*))
    } > FLASH

    /**** Base address of secure data area */
    .tfm_secure_data_start :
    {
        . = ABSOLUTE(S_DATA_START) ;
    } > RAM

    /*
     * MPU on Armv6-M/v7-M core in multi-core topology may require more strict
     * alignment that MPU region base address must align with the MPU region
     * size.
     * As a result, in multi-core topology, to save memory resource and MPU
     * regions, unprivileged data sections and privileged data sections are
     * separated and gathered in unprivileged/privileged data area respectively.
     * Keep BL2 shared data and MSP stack at the beginning of the secure data
     * area in single Armv8-M topology, while move the two areas to the
     * beginning of privileged data region in multi-core topology.
     */


    /* shared_data and msp_stack are overlapping on purpose when
     * msp_stack is extended until the beginning of RAM, when shared_date
     * was read out by partitions
     */
    .tfm_bl2_shared_data : ALIGN(32)
    {
        . += BOOT_TFM_SHARED_DATA_SIZE;
    } > RAM

    .noinit (NOLOAD):
    {
        . = ALIGN(4);
        __noinit_start = .;
        KEEP(*(.noinit*))
        . = ALIGN(8);
        __noinit_end = .;
    } > RAM

    .msp_stack : ALIGN(32)
    {
        KEEP(*(.stack));
    } > RAM
    Image$$ARM_LIB_STACK_MSP$$ZI$$Base = ADDR(.msp_stack);
    Image$$ARM_LIB_STACK_MSP$$ZI$$Limit = ADDR(.msp_stack) + SIZEOF(.msp_stack);

    /* PSP is unprivileged in single-core topology */
    .psp_stack : ALIGN(32)
    {
        . += __psp_stack_size__;
    } > RAM
    Image$$ARM_LIB_STACK$$ZI$$Base = ADDR(.psp_stack);
    Image$$ARM_LIB_STACK$$ZI$$Limit = ADDR(.psp_stack) + SIZEOF(.psp_stack);


    .TFM_UNPRIV_DATA : ALIGN(32)
    {
        */tfm_spm_services.o(.data*)
        */platform_retarget_dev.o(.data*)
        */device_definition.o(.data*)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_UNPRIV_DATA$$RW$$Base = ADDR(.TFM_UNPRIV_DATA);
    Image$$TFM_UNPRIV_DATA$$RW$$Limit = ADDR(.TFM_UNPRIV_DATA) + SIZEOF(.TFM_UNPRIV_DATA);

    .TFM_UNPRIV_BSS (NOLOAD): ALIGN(32)
    {
        */tfm_spm_services.o(.bss*)
        */platform_retarget_dev.o(.bss*)
        */device_definition.o(.bss*)
        */tfm_spm_services.o(COMMON)
        */platform_retarget_dev.o(COMMON)
        */device_definition.o(COMMON)
        . = ALIGN(32);
    } > RAM
    Image$$TFM_UNPRIV_DATA$$ZI$$Base = ADDR(.TFM_UNPRIV_BSS);
    Image$$TFM_UNPRIV_DATA$$ZI$$Limit = ADDR(.TFM_UNPRIV_BSS) + SIZEOF(.TFM_UNPRIV_BSS);

    /**** APPLICATION RoT DATA start here */
    Image$$TFM_APP_RW_STACK_START$$Base = .;


    /**** APPLICATION RoT DATA end here */
    Image$$TFM_APP_RW_STACK_END$$Base = .;


    .heap (NOLOAD): ALIGN(8)
    {
        __end__ = .;
        PROVIDE(end = .);
        __HeapBase = .;
        KEEP(*(.heap))
        __HeapLimit = .;
        __heap_limit = .; /* Add for _sbrk */
    } > RAM


    /**** PSA RoT DATA start here */
    Image$$TFM_PSA_RW_STACK_START$$Base = .;

/* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */
    .TFM_SP_ITS_LINKER_DATA : ALIGN(32)
    {
        *tfm_internal_trusted_storage*:*(.data*)
        *(TFM_SP_ITS_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_ITS_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_ITS_LINKER_DATA);
    Image$$TFM_SP_ITS_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_ITS_LINKER_DATA) + SIZEOF(.TFM_SP_ITS_LINKER_DATA);

    .TFM_SP_ITS_LINKER_BSS (NOLOAD): ALIGN(32)
    {
        start_of_TFM_SP_ITS_LINKER = .;
        *tfm_internal_trusted_storage*:*(.bss*)
        *tfm_internal_trusted_storage*:*(COMMON)
        *(TFM_SP_ITS_ATTR_ZI)
        . += (. - start_of_TFM_SP_ITS_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM
    Image$$TFM_SP_ITS_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_ITS_LINKER_BSS);
    Image$$TFM_SP_ITS_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_ITS_LINKER_BSS) + SIZEOF(.TFM_SP_ITS_LINKER_BSS);

    .TFM_SP_ITS_LINKER_STACK : ALIGN(128)
    {
        . += 0x680;
    } > RAM
    Image$$TFM_SP_ITS_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_ITS_LINKER_STACK);
    Image$$TFM_SP_ITS_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_ITS_LINKER_STACK) + SIZEOF(.TFM_SP_ITS_LINKER_STACK);

 /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

/*  TFM_PARTITION_CRYPTO */
    .TFM_SP_CRYPTO_LINKER_DATA : ALIGN(32)
    {
        *tfm_crypto*:*(.data*)
        *(TFM_SP_CRYPTO_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_CRYPTO_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_CRYPTO_LINKER_DATA);
    Image$$TFM_SP_CRYPTO_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_CRYPTO_LINKER_DATA) + SIZEOF(.TFM_SP_CRYPTO_LINKER_DATA);

    .TFM_SP_CRYPTO_LINKER_BSS (NOLOAD): ALIGN(32)
    {
        start_of_TFM_SP_CRYPTO_LINKER = .;
        *tfm_crypto*:*(.bss*)
        *tfm_crypto*:*(COMMON)
        *(TFM_SP_CRYPTO_ATTR_ZI)
        . += (. - start_of_TFM_SP_CRYPTO_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM
    Image$$TFM_SP_CRYPTO_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_CRYPTO_LINKER_BSS);
    Image$$TFM_SP_CRYPTO_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_CRYPTO_LINKER_BSS) + SIZEOF(.TFM_SP_CRYPTO_LINKER_BSS);

    .TFM_SP_CRYPTO_LINKER_STACK : ALIGN(128)
    {
        . += 0x2000;
    } > RAM
    Image$$TFM_SP_CRYPTO_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_CRYPTO_LINKER_STACK);
    Image$$TFM_SP_CRYPTO_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_CRYPTO_LINKER_STACK) + SIZEOF(.TFM_SP_CRYPTO_LINKER_STACK);

 /* TFM_PARTITION_CRYPTO */

/* TFM_PARTITION_PLATFORM */
    .TFM_SP_PLATFORM_LINKER_DATA : ALIGN(32)
    {
        *tfm_platform*:*(.data*)
        *(TFM_SP_PLATFORM_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_PLATFORM_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_PLATFORM_LINKER_DATA);
    Image$$TFM_SP_PLATFORM_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_PLATFORM_LINKER_DATA) + SIZEOF(.TFM_SP_PLATFORM_LINKER_DATA);

    .TFM_SP_PLATFORM_LINKER_BSS (NOLOAD): ALIGN(32)
    {
        start_of_TFM_SP_PLATFORM_LINKER = .;
        *tfm_platform*:*(.bss*)
        *tfm_platform*:*(COMMON)
        *(TFM_SP_PLATFORM_ATTR_ZI)
        . += (. - start_of_TFM_SP_PLATFORM_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM
    Image$$TFM_SP_PLATFORM_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_PLATFORM_LINKER_BSS);
    Image$$TFM_SP_PLATFORM_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_PLATFORM_LINKER_BSS) + SIZEOF(.TFM_SP_PLATFORM_LINKER_BSS);

    .TFM_SP_PLATFORM_LINKER_STACK : ALIGN(128)
    {
        . += 0x0400;
    } > RAM
    Image$$TFM_SP_PLATFORM_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_PLATFORM_LINKER_STACK);
    Image$$TFM_SP_PLATFORM_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_PLATFORM_LINKER_STACK) + SIZEOF(.TFM_SP_PLATFORM_LINKER_STACK);

 /* TFM_PARTITION_PLATFORM */

/* TFM_PARTITION_INITIAL_ATTESTATION */
    .TFM_SP_INITIAL_ATTESTATION_LINKER_DATA : ALIGN(32)
    {
        *tfm_attest*:*(.data*)
        *(TFM_SP_INITIAL_ATTESTATION_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_DATA);
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_DATA) + SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER_DATA);

    .TFM_SP_INITIAL_ATTESTATION_LINKER_BSS (NOLOAD): ALIGN(32)
    {
        start_of_TFM_SP_INITIAL_ATTESTATION_LINKER = .;
        *tfm_attest*:*(.bss*)
        *tfm_attest*:*(COMMON)
        *(TFM_SP_INITIAL_ATTESTATION_ATTR_ZI)
        . += (. - start_of_TFM_SP_INITIAL_ATTESTATION_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_BSS);
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_BSS) + SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER_BSS);

    .TFM_SP_INITIAL_ATTESTATION_LINKER_STACK : ALIGN(128)
    {
        . += 0x0A80;
    } > RAM
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_STACK);
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_STACK) + SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER_STACK);

 /* TFM_PARTITION_INITIAL_ATTESTATION */

    /**** PSA RoT DATA end here */
    Image$$TFM_PSA_RW_STACK_END$$Base = .;

    .TFM_DATA : ALIGN(4)
    {
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE(__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);

    } > RAM AT> FLASH
    Image$$ER_TFM_DATA$$RW$$Base = ADDR(.TFM_DATA);
    Image$$ER_TFM_DATA$$RW$$Limit = ADDR(.TFM_DATA) + SIZEOF(.TFM_DATA);

    .TFM_BSS (NOLOAD): ALIGN(4)
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM
    Image$$ER_TFM_DATA$$ZI$$Base = ADDR(.TFM_BSS);
    Image$$ER_TFM_DATA$$ZI$$Limit = ADDR(.TFM_BSS) + SIZEOF(.TFM_BSS);

    Image$$ER_TFM_DATA$$Base = ADDR(.TFM_DATA);
    Image$$ER_TFM_DATA$$Limit = ADDR(.TFM_DATA) + SIZEOF(.TFM_DATA) + SIZEOF(.TFM_BSS);


    /*
     * Place the CMSE Veneers (containing the SG instruction) after the code, in a
     * separate 32 bytes aligned region so that the SAU can programmed to just set
     * this region as Non-Secure Callable.
     */
    .gnu.sgstubs : ALIGN(32)
    {
        *(.gnu.sgstubs*)
        . = ALIGN(32);
    } > VENEERS AT> VENEERS
    Load$$LR$$LR_VENEER$$Base = ADDR(.gnu.sgstubs);
    Load$$LR$$LR_VENEER$$Limit = ADDR(.gnu.sgstubs) + SIZEOF(.gnu.sgstubs);

    Load$$LR$$LR_NS_PARTITION$$Base = NS_PARTITION_START;

    Load$$LR$$LR_SECONDARY_PARTITION$$Base = SECONDARY_PARTITION_START;

    PROVIDE(__stack = Image$$ARM_LIB_STACK$$ZI$$Limit);
    PROVIDE(__StackLimit = Image$$ARM_LIB_STACK_MSP$$ZI$$Base);
    PROVIDE(__data_start__ = 0);
    PROVIDE(__data_end__   = 0);
    PROVIDE(__etext        = 0);
    PROVIDE(__tz_FLASH_N = 0);
    PROVIDE(__Vectors = __vectors_start__);
}

