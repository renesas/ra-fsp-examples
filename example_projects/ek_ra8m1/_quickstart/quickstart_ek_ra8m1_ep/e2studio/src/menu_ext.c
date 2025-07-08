/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : menu_ext.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "hal_data.h"
#include "board_cfg.h"
#include "ospi_commands.h"
#include "menu_ext.h"
#include "r_typedefs.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "bsp_api.h"
#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"



/* The RA8M1 OSPI flash is Infineon S28HL512TFPBHI010 */
/* The device is 512M bit (64M Byte) */

/* Actual memory is 64MB. We will only test first 16MB
 * This must be on a 64kByte boundary, for erase to work.
 * */

#define OSPI_TOTAL_TEST_SIZE                    (16 * 1024)
#define OSPI_WEN_BIT_MASK                       (0x02U)
#define OSPI_WIP_BIT_MASK                       (0x01U)
#define OSPI_TEST_PAGE_SIZE                     (512U)
#define OSPI_ERASE_BLOCK_SIZE                   (4096U)
#define DDR_SAMPLING_WINDOW_EXTEND              (1U)

/* Key code for writing PRCR register */
#define BSP_PRV_PRCR_KEY                        (0xA500U)
#define BSP_PRV_PRCR_UNLOCK                     ((BSP_PRV_PRCR_KEY) | 0x3U)
#define BSP_PRV_PRCR_LOCK                       ((BSP_PRV_PRCR_KEY) | 0x0U)

#define OSPI_START_ADDRESS                      (0x90000000)        /* CS1 */

#define STATUS1_ADDRESS                         (0x800000)
#define CFR1_ADDRESS                            (0x800002)
#define CFR2_ADDRESS                            (0x800003)
#define CFR3_ADDRESS                            (0x800004)
#define CFR4_ADDRESS                            (0x800005)
#define CFR5_ADDRESS                            (0x800006)

#define STATUS_WRITE_ENABLED                    (2)

#define CONNECTION_ABORT_CRTL          (0x00)
#define MENU_EXIT_CRTL                 (0x20)
#define MENU_ENTER_RESPONSE_CRTL       (0x09)
#define INPUT_BUFFER                   (0x05)
#define CARRAGE_RETURN                 (0x0D)


/* 8m1 WS/ES using basic driver */
#define BLOCK_LIMIT                    (64)

#define FOURKB_LIMIT                   (BLOCK_LIMIT / 4)

#define MODULE_NAME     "\r\n%d. OCTO-SPI SPEED TEST\r\n"

#define SUB_OPTIONS     "\r\nCompares the write and read times to and from external Octo-SPI" \
                        "\r\nflash memories\r\n" \
                        "\r\n> Enter the text block size " \
                        "\r\n(in multiples of 2 KB, max 64 KB) and press tab to continue : "





/* These commands and registers are for the Infineon S28HS256 and may not match other targets. */
#define READ_SFDP_COMMAND          (0x5AU)
#define READ_SFDP_COMMAND_OPI      (0x5A5AU)
#define READ_REGISTER_COMMAND      (0x65U)
#define READ_REGISTER_COMMAND_OPI  (0x6565U)
#define WRITE_REGISTER_COMMAND     (0x71U)
#define WRITE_REGISTER_COMMAND_OPI (0x7171U)
#define WRITE_ENABLE_COMMAND       (0x06U)
#define WRITE_ENABLE_COMMAND_OPI   (0x0606U)
#define READ_STATUS_COMMAND        (0x05U)
#define READ_STATUS_COMMAND_OPI    (0x0505U)

#define CFR1V_REGISTER_ADDRESS (0x800002U)
#define CFR2V_REGISTER_ADDRESS (0x800003U)
#define CFR3V_REGISTER_ADDRESS (0x800004U)
#define CFR5V_REGISTER_ADDRESS (0x800006U)

#define CFR2V_MEMLAT_Pos       (0U)
#define CFR2V_MEMLAT_Msk       (0x0FU)

#define CFR3V_VRGLAT_Pos       (6U)
#define CFR3V_VRGLAT_Msk       (0xC0U)
#define CFR3V_UNHYSA_Pos       (3U)
#define CFR3V_UNHYSA_Msk       (0x08U)

#define CFR5V_SDRDDR_Pos       (1U)
#define CFR5V_SDRDDR_Msk       (0x02U)
#define CFR5V_OPIIT_Pos        (0U)
#define CFR5V_OPIIT_Msk        (0x01U)

#define CFR2V_WRITE_Msk        (0x00U)
#define CFR3V_WRITE_Msk        (0x00U)
#define CFR5V_WRITE_Msk        (0x40U)

#define REG_LATENCY_CODE_SPI   (0x00U)
#define REG_LATENCY_CODE_OPI   (0x03U)

#define MEM_LATENCY_CODE_SPI   (0x08U)
#define MEM_LATENCY_CODE_OPI   (0x0AU)

#define REG_DUMMY_CYCLES_SPI   (0U)
#define REG_DUMMY_CYCLES_OPI   (3U)

#define WRITE_ENABLE_MASK      (0x02U)

#define READ_SFDP_DUMMY_CYCLES (8U)
#define SFDP_SIGNATURE         (0x50444653U)


#define OSPI_TEST_DATA_LENGTH  (0x40U)
#define OSPI_SECTOR_SIZE       (4096U)
#define OSPI_BLOCK_SIZE        (262144U)

#define OSPI_MODE_SPI          (CFR5V_WRITE_Msk)
#define OSPI_MODE_DOPI         (CFR5V_WRITE_Msk | CFR5V_OPIIT_Msk | CFR5V_SDRDDR_Msk)

#define OSPI_RESET_PIN         (BSP_IO_PORT_01_PIN_06)
#define OSPI_RESET_DELAY       (500U)

typedef struct st_clk_settings
{
    cgc_pll_cfg_t pll;
    bsp_clocks_octaclk_div_t div;
} clk_settings_t;



#define WRITE_BLOCK_SIZE (64)
#define TEST_BLOCKS      (64) /* Limit test to 4K as UNHYSA bit is set so device in hybrid mode
                               * where the first few sectors are 4k blocks. */


static void reset_ospi_device (void);
static void wait_for_write (void);
static void write_en (bool is_dopi);
static void oclk_change (clk_settings_t const * const clock_settings);
static void transition_to_dopi (void);

static uint8_t * const gp_ospi_cs1 = (uint8_t *)(void *)0x90000000;


#ifndef USE_TINY_TEST
static uint8_t  g_test_data[OSPI_TEST_DATA_LENGTH] = "";
#else
static uint8_t  g_test_data[OSPI_TEST_DATA_LENGTH] =
{
    0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x08, 0x00, 0x00, 0xF7, 0xFF, 0x00, 0x08, 0xF7, 0x00, 0xF7, // Auto-calibration pattern.
    0x35, 0x35, 0x35, 0x35, 0xBD, 0xFF, 0xEE, 0xF1, 0x36, 0x36, 0x36, 0x36, 0xBC, 0xFF, 0xEF, 0xF2,
    0x37, 0x37, 0x37, 0x37, 0xBF, 0x01, 0xEE, 0xF1, 0x38, 0x38, 0x38, 0x38, 0xBA, 0x1F, 0x6F, 0xF3,
    0xA5, 0xA5, 0xA5, 0xA5, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
};
#endif



static uint32_t g_test_remain           = WRITE_BLOCK_SIZE;
static uint32_t g_test_current_block    = 0;
static uint32_t g_test_current_sub      = 0;
static size_t   g_data_size             = TEST_BLOCKS * WRITE_BLOCK_SIZE;



static clk_settings_t clk_200MHz =
{
    .pll =
    {
        .source_clock = CGC_CLOCK_MAIN_OSC,
        .divider      = BSP_CLOCKS_PLL_DIV_1,
        .multiplier   = BSP_CLOCKS_PLL_MUL(40U, 0U),
        .out_div_p    = CGC_PLL_OUT_DIV_4,
        .out_div_q    = CGC_PLL_OUT_DIV_6, // Not used, set to div/6 (default)
        .out_div_r    = CGC_PLL_OUT_DIV_6, // Not used, set to div/6 (default)
    },
    .div = (bsp_clocks_octaclk_div_t) BSP_CLOCKS_OCTA_CLOCK_DIV_1,
};

/**********************************************************************************************************************
 * Function Name: reset_ospi_device
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void reset_ospi_device(void)
{
    R_BSP_PinAccessEnable();
    R_BSP_PinCfg(OSPI_RESET_PIN, ((uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT |
                                    (uint32_t) IOPORT_CFG_DRIVE_HIGH |
                                    (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW));

    R_BSP_PinWrite(OSPI_RESET_PIN, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay((bsp_delay_units_t)OSPI_RESET_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    R_BSP_PinWrite(OSPI_RESET_PIN, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay((bsp_delay_units_t)OSPI_RESET_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    R_BSP_PinAccessDisable();

}
/**********************************************************************************************************************
 End of function reset_ospi_device
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: write_en
 * Description  : .
 * Argument     : is_dopi
 * Return Value : .
 *********************************************************************************************************************/
static void write_en(bool is_dopi)
{
    spi_flash_direct_transfer_t tfr =
    {
        .command = is_dopi ? WRITE_ENABLE_COMMAND_OPI : WRITE_ENABLE_COMMAND,
        .command_length = is_dopi ? 2U : 1U,
        .address_length = 0U,
        .data_length = 0U,
        .dummy_cycles = 0U
    };
    fsp_err_t err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    assert(FSP_SUCCESS == err);

    tfr = (spi_flash_direct_transfer_t)
    {
        .command = is_dopi ? READ_STATUS_COMMAND_OPI : READ_STATUS_COMMAND,
        .command_length = is_dopi ? 2U : 1U,
        .address_length = is_dopi ? 4U : 0U,    // Address is always sent for any kind of read in DOPI
        .data_length = 1U,
        .dummy_cycles = is_dopi ? REG_DUMMY_CYCLES_OPI : REG_DUMMY_CYCLES_SPI,
    };
    err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    assert(FSP_SUCCESS == err);

    if ((tfr.data & WRITE_ENABLE_MASK) == 0)
    {
        __BKPT(0);
    }
}
/**********************************************************************************************************************
 End of function write_en
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: wait_for_write
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void wait_for_write(void)
{
    spi_flash_status_t status =
    {
        0
    };

    do
    {
        assert (FSP_SUCCESS == g_ospi0.p_api -> statusGet(g_ospi0.p_ctrl, &status));
    }
    while (status.write_in_progress);
}
/**********************************************************************************************************************
 End of function wait_for_write
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: oclk_change
 * Description  : .
 * Argument     : clock_settings
 * Return Value : .
 *********************************************************************************************************************/
static void oclk_change(clk_settings_t const * const clock_settings)
{
    g_cgc.p_api->open(g_cgc.p_ctrl, g_cgc.p_cfg);

    /* Stop the restart PLL2 with appropriate configurations. */
    assert(FSP_SUCCESS == g_cgc.p_api->clockStop(g_cgc.p_ctrl, CGC_CLOCK_PLL2));
    assert(FSP_SUCCESS == g_cgc.p_api->clockStart(g_cgc.p_ctrl, CGC_CLOCK_PLL2, &clock_settings->pll));

    /* Now update the octaclk divider. */
    bsp_octaclk_settings_t octaclk_settings;
    octaclk_settings.source_clock = BSP_CLOCKS_CLOCK_PLL2;
    octaclk_settings.divider      = clock_settings->div;
    R_BSP_OctaclkUpdate(&octaclk_settings);

    g_cgc.p_api->close(g_cgc.p_ctrl);
}
/**********************************************************************************************************************
 End of function oclk_change
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: transition_to_dopi
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void transition_to_dopi(void)
{
    fsp_err_t err = FSP_SUCCESS;

    spi_flash_direct_transfer_t tfr =
    {
        .command = WRITE_REGISTER_COMMAND,
        .command_length = 1U,
        .address_length = 3U,
        .data_length = 1U,
        .dummy_cycles = 0U
    };

    write_en(false);


    /* Transition to DOPI. */
    tfr.address = CFR5V_REGISTER_ADDRESS;
    tfr.data = OSPI_MODE_DOPI;
    err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    assert (FSP_SUCCESS == err);

    /* Change clock speed. */
    oclk_change(&clk_200MHz);

    /* Change the protocol mode of the driver. */
    /* DS will auto-calibrate in this call. */
    err = g_ospi0.p_api->spiProtocolSet(g_ospi0.p_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
    assert (FSP_SUCCESS == err);

    /* Read the mode register to verify it changed to DOPI */
    tfr = (spi_flash_direct_transfer_t) {
        .command = READ_REGISTER_COMMAND_OPI,
        .command_length = 2U,
        .address = CFR5V_REGISTER_ADDRESS,
        .address_length = 4U,
        .data = 0U,
        .data_length = 1U,
        .dummy_cycles = REG_DUMMY_CYCLES_OPI
    };
    err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    assert(FSP_SUCCESS == err);
    assert(OSPI_MODE_DOPI == (tfr.data & 0xFF));   // Need to mask here because DOPI always reads 2 bytes at a time.
}
/**********************************************************************************************************************
 End of function transition_to_dopi
 *********************************************************************************************************************/






/**********************************************************************************************************************
 * Function Name: write_test_opi
 * Description  : .
 * Argument     : num_blocks
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t write_test_opi(uint32_t num_blocks)
{
    fsp_err_t err = FSP_SUCCESS;

    srand(0);
    volatile uint8_t data = (uint8_t)rand();

    g_test_current_block = 0;

    /* Write the test data in SPI mode. */
    while (--num_blocks)
    {
        g_test_remain      = WRITE_BLOCK_SIZE;
        g_test_current_sub = 0;
        while (g_test_remain)
        {
            g_test_remain--;
            data = (uint8_t)rand();
            g_test_data[g_test_current_sub++] = data;
        }

        if (FSP_SUCCESS == err)
        {
            R_GPT_Start(g_memory_performance.p_ctrl);
            err = g_ospi0.p_api->write(g_ospi0.p_ctrl, (g_test_data), (uint8_t *) (gp_ospi_cs1 +
                    (g_test_current_block * WRITE_BLOCK_SIZE)), WRITE_BLOCK_SIZE);
            R_GPT_Stop(g_memory_performance.p_ctrl);

            g_test_current_block++;

            if (FSP_SUCCESS == err)
            {
                wait_for_write();
            }
        }
    }
    return (err);
}
/**********************************************************************************************************************
 End of function write_test_opi
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: read_test_opi
 * Description  : .
 * Argument     : num_blocks
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t read_test_opi(uint32_t num_blocks)
{
    fsp_err_t err = FSP_SUCCESS;

    srand(0);
    volatile uint8_t data = (uint8_t)rand();

    g_data_size = TEST_BLOCKS * WRITE_BLOCK_SIZE;

    /* Try to read some data in standard SPI mode.. */
    for (size_t i = 0; i < num_blocks; i++)
    {
        data = (uint8_t)rand();
        if (gp_ospi_cs1[i] != data)
        {
            err = FSP_ERR_INVALID_DATA;
            i   = num_blocks + 1;
        }
    }
    return (err);
}
/**********************************************************************************************************************
 End of function read_test_opi
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: ospi_flash_open_test
 * Description  : .
 * Argument     : erase_size_msk
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t ospi_flash_open_test(uint32_t * erase_size_msk)
{
    fsp_err_t err = FSP_SUCCESS;
    reset_ospi_device();

    spi_flash_direct_transfer_t test_tfr =
    {
    .command = READ_SFDP_COMMAND,
    .command_length = 1,
    .address = 0,
    .address_length = 3,
    .data = 0,
    .data_length = 4,
    .dummy_cycles = READ_SFDP_DUMMY_CYCLES
    };

    if (FSP_SUCCESS == err)
    {
        err = g_ospi0.p_api->open(g_ospi0.p_ctrl, g_ospi0.p_cfg);

        R_XSPI0->LIOCFGCS_b[g_ospi0_ctrl.channel].DDRSMPEX = DDR_SAMPLING_WINDOW_EXTEND;

        err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &test_tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);

        if (SFDP_SIGNATURE != test_tfr.data)
        {
#ifdef USE_ENHANCED_MESSAGING
            print_to_console((uint8_t *)"Error reading from device, SFDP_SIGNATURE failed \r\n");
#endif
            err = FSP_ERR_NOT_INITIALIZED;

        }

        /* Check to see what the flash layout is. Only the correct erase command
         * will work for a specific layout. Not all targets are like this but the
         * S28H is picky. */
        test_tfr = (spi_flash_direct_transfer_t)
        {
            .command = READ_REGISTER_COMMAND,
            .command_length = 1,
            .address = CFR3V_REGISTER_ADDRESS,
            .address_length = 3,
            .data = 0,
            .data_length = 1,
            .dummy_cycles = 0U
        };
    }

    if (FSP_SUCCESS == err)
    {
#ifdef USE_ENHANCED_MESSAGING
        print_to_console((uint8_t *)"   SFDP_SIGNATURE ready successfully checking UNHYSA bit \r\n");
#endif
        err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &test_tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        assert(FSP_SUCCESS == err);

#ifdef USE_ENHANCED_MESSAGING
        if ((test_tfr.data & CFR3V_UNHYSA_Msk) == CFR3V_UNHYSA_Msk)
        {
            print_to_console((uint8_t *) "   UNHYSA bit is set to OSPI_BLOCK_SIZE   (262144U)\r\n");
        }
        else
        {
            print_to_console((uint8_t *) "   UNHYSA bit is set to OSPI_SECTOR_SIZE  (4096U)\r\n");
        }
#endif
        *erase_size_msk = (test_tfr.data & CFR3V_UNHYSA_Msk) ? OSPI_BLOCK_SIZE : OSPI_SECTOR_SIZE;

    }

    return (err);
}
/**********************************************************************************************************************
 End of function ospi_flash_open_test
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: ospi_performance_test
 * Description  : .
 * Arguments    : data_size
 *              : ospi_performance_write_result
 *              : ospi_performance_read_result
 * Return Value : .
 *********************************************************************************************************************/
void ospi_performance_test(uint32_t data_size,
                            uint32_t *ospi_performance_write_result,
                            uint32_t *ospi_performance_read_result)
{

    fsp_err_t err         = FSP_SUCCESS;
    uint32_t  erase_size  = 0;
    bool_t    test_res    = 0;

    timer_status_t status;

#ifndef USE_TINY_TEST

    if (FSP_SUCCESS == err)
    {
        err = ospi_flash_open_test(&erase_size);
    }

    if (FSP_SUCCESS == test_res)
    {
        /* Clean the OSPI device up before testing (dev only) */
        for (uint32_t nb = 00; nb < FOURKB_LIMIT;  nb++)
        {
            err = g_ospi0.p_api->erase(g_ospi0.p_ctrl, (uint8_t *) (gp_ospi_cs1 + (4096 * nb)), erase_size);
            assert(FSP_SUCCESS == err);
            wait_for_write();
        }
    }

    if (FSP_SUCCESS == test_res)
    {
        /* Change to DOPI. */
        transition_to_dopi();

        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)
                "\r\nWriting the text block to external Octo-SPI flash memory...\r\n");

        R_GPT_Reset(g_memory_performance.p_ctrl);

        test_res = write_test_opi(data_size / 64);

        R_GPT_StatusGet(g_memory_performance.p_ctrl, &status);
        *ospi_performance_write_result = status.counter;

        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"Writing to flash completed\r\n");
    }

    if (FSP_SUCCESS == test_res)
    {

    /* ignoring -Wpointer-sign is OK for a constant string */
    print_to_console((uint8_t *)"\r\nReading the text block from external Octo-SPI flash memory...\r\n");

    R_GPT_Reset(g_memory_performance.p_ctrl);
    R_GPT_Start(g_memory_performance.p_ctrl);

    test_res = read_test_opi(data_size / 64);

    R_GPT_Stop(g_memory_performance.p_ctrl);
    R_GPT_StatusGet(g_memory_performance.p_ctrl, &status);
    *ospi_performance_read_result = status.counter;
    R_GPT_Reset(g_memory_performance.p_ctrl);

    /* ignoring -Wpointer-sign is OK for a constant string */
    print_to_console((uint8_t *)"Reading from flash completed\r\n");

    }

    if (FSP_SUCCESS == test_res)
    {
        /* Restore to SPI mode */

        err = g_ospi0.p_api->close(g_ospi0.p_ctrl);

    }

#else
#endif

};
/**********************************************************************************************************************
 End of function ospi_performance_test
 *********************************************************************************************************************/


/* Each thread must have a separate print buffer, to avoid clashes on task switching */
static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};
static char_t s_block_sz_str[INPUT_BUFFER]       = {};

/* Block in RAM for performance measure  */
uint8_t perf_read[OSPI_TEST_PAGE_SIZE];

/**********************************************************************************************************************
 * Function Name: validate_user_input
 * Description  : .
 * Argument     : p_input
 * Return Value : .
 *********************************************************************************************************************/
static uint32_t validate_user_input(char_t *p_input)
{
    uint32_t result = INVALID_CHARACTER;
    uint32_t value  = 0;
    uint32_t t      = 0;
    uint32_t c      = 0;

    bool_t processing = true;

    while (true == processing)
    {
        /* Cast to req type */
        c = (uint32_t)(*p_input);

        p_input++;

        if ((c >= '0') && (c <= '9'))
        {

            /* Cast as compiler will interpret result as int */
            value = (uint32_t)(value * 10);

            /* Cast to req type */
            t     = (uint32_t)(atoi((char_t *)(&c)));

            /* Cast as compiler will interpret result as int */
            value = (uint32_t)(value + t);

        }
        else
        {
            if (MENU_ENTER_RESPONSE_CRTL == c)
            {
                result = value;
            }

            if (MENU_EXIT_CRTL == c)
            {
                result = 0;
            }
            break;
        }
    }

    return (result);
}
/**********************************************************************************************************************
 End of function validate_user_input
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ext_display_menu
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
test_fn ext_display_menu(void)
{
    int32_t c                   = -1;
    uint32_t block_size_actual  = 0;
    int32_t block_sz_ndx        = 0;
    int32_t block_sz_limit      = (INPUT_BUFFER-2); /* Allowing for TAB and End of Message */

    timer_info_t timer_info;

    bool_t valid_block_size = false;

    uint32_t value = 0;

    sprintf(s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);
    sprintf(s_print_buffer, MODULE_NAME, g_selected_menu);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);
    sprintf(s_print_buffer, SUB_OPTIONS);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    /* Keep trying to read a valid text block size
     * complete the loop in one of two ways:
     * [1] Valid block size is entered (2K boundary range 2-64K) followed by TAB
     * [2] Space Bar is pressed at any stage
     */

    while (false == valid_block_size)
    {
        /* Reset input state */
        c            = -1;
        block_sz_ndx = 0;

        memset(&s_block_sz_str, 0, INPUT_BUFFER);

        while ((CONNECTION_ABORT_CRTL != c))
        {
            c = input_from_console();

            if (block_sz_ndx < block_sz_limit)
            {
                /* Cast to req type */
                s_block_sz_str[block_sz_ndx] = (char_t)c;
                block_sz_ndx++;
            }
            else
            {
                /* maximum block size exceeded (4 digits / characters entered) */
                s_block_sz_str[block_sz_ndx] = MENU_ENTER_RESPONSE_CRTL;

                c = MENU_ENTER_RESPONSE_CRTL;
                break;
            }

            if (MENU_EXIT_CRTL == c)
            {
                /* Abort the test */
                valid_block_size  = true;
                block_size_actual = 0;
                break;
            }

            if (MENU_ENTER_RESPONSE_CRTL == c)
            {
                break;
            }

            if (CARRAGE_RETURN != c)
            {
                /* Cast for req type */
                sprintf(s_print_buffer, "%c", (char_t)c);

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console((void*)s_print_buffer);
            }
        }

        /* If the input was terminated with a TAB then attempt to process it */
        if (MENU_ENTER_RESPONSE_CRTL == c)
        {
            value = validate_user_input(&s_block_sz_str[0]);
        }

        vTaskDelay(10);

        if ((value > 0) && (value < 2))
        {
            value = INVALID_BLOCK_SIZE;
        }

        if ((value > BLOCK_LIMIT) && (value < INVALID_MARKERS))
        {
            value = INVALID_BLOCK_SIZE;
        }

        if ((value > 0) && ((value % 2) != 0)  && (value < INVALID_MARKERS))
        {
            value = INVALID_BLOCK_BOUNDARY;
        }

        switch (value)
        {
            case INVALID_CHARACTER:
            {
                sprintf(s_print_buffer,
                        "\r\n> Invalid character in entry, enter the text block size specifying a 2K boundary (eg 24)"\
                        " and press tab : ");

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console((void*)s_print_buffer);
                break;
            }

            case INVALID_BLOCK_SIZE:
            {
                sprintf(s_print_buffer, "\r\n> Invalid size, enter the text block size (eg 24) and press tab : ");

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console((void*)s_print_buffer);
                break;
            }

            case INVALID_BLOCK_BOUNDARY:
            {
                sprintf(s_print_buffer,
                        "\r\n> Invalid boundary, enter the text block size specifying a 2K boundary (eg 4)"\
                        " and press tab : ");

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console((void*)s_print_buffer);
                break;
            }

            default:
            {
                valid_block_size = true;
            }
        }
    }

    block_size_actual = value;

    if ((MENU_ENTER_RESPONSE_CRTL == c) && (0 != block_size_actual))
    {
        fsp_err_t      fsp_err = FSP_SUCCESS;

        uint32_t       ospi_read_result  = 0;
        uint32_t       ospi_write_result = 0;

        sprintf(s_print_buffer, "\r\n\r\nGenerated a text block of %2lu KB in SRAM\r\n", block_size_actual);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        print_to_console((void*)s_print_buffer);

        uint32_t ospi_performance_write_result = 0;
        uint32_t ospi_performance_read_result  = 0;

        uint32_t timer_frequency;

        fsp_err = R_GPT_Open(g_memory_performance.p_ctrl, g_memory_performance.p_cfg);

        R_GPT_InfoGet(g_memory_performance.p_ctrl, &timer_info);
        timer_frequency = timer_info.clock_frequency;

        block_size_actual = block_size_actual * 1024;

        ospi_performance_test (block_size_actual, &ospi_performance_write_result, &ospi_performance_read_result);

        R_GPT_Close(g_memory_performance.p_ctrl);

        ospi_write_result = ((100000000 / timer_frequency) * ospi_performance_write_result) / 100;
        ospi_read_result  = ((100000000 / timer_frequency) * ospi_performance_read_result) / 100;

        /* Handle error */
        if (FSP_SUCCESS != fsp_err)
        {
            /* Fatal error */
            SYSTEM_ERROR
        }

        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"\r\n-------------------------------------------------");

        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"\r\nOperation/Flash          Octo-SPI");

        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"\r\n-------------------------------------------------");
        sprintf(s_print_buffer, "\r\nWrite                    %6ld" , ospi_write_result);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        print_to_console((void*)s_print_buffer);
        sprintf(s_print_buffer, "\r\nRead                     %6ld" , ospi_read_result);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        print_to_console((void*)s_print_buffer);

        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"\r\n-------------------------------------------------");

        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"\r\nNote: Times are in microseconds");
        sprintf(s_print_buffer, MENU_RETURN_INFO);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        print_to_console((void*)s_print_buffer);
    }

    while ((CONNECTION_ABORT_CRTL != c))
    {
        if ((MENU_EXIT_CRTL == c) || (0x00 == c))
        {
            break;
        }
        c = input_from_console();
    }

    return (0);
}
/**********************************************************************************************************************
 End of function ext_display_menu
 *********************************************************************************************************************/
