/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : menu_ext.c
 * Version      : 1.0
 * Description  : Memory demo. Manages demo interface to console and lcd.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "hal_data.h"
#include "board_cfg.h"
#include "r_typedefs.h"

#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "bsp_api.h"

#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "board_hw_cfg.h"
#include "menu_ext.h"
#include "ospi_commands.h"
#include "ospi_b_ep.h"
#include "board_cfg_switch.h"
#include "display_thread_entry.h"


/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* The RA8P1 OSPI flash is Macronix MX25LW51245G */
/* The device is 512M bit (64M Byte) */

#define GPT_CLOCK_MHZ                           (240)

#define OSPI_WEN_BIT_MASK                       (0x02U)
#define OSPI_WIP_BIT_MASK                       (0x01U)
#define OSPI_TEST_PAGE_SIZE                     (512U)
#define OSPI_ERASE_BLOCK_SIZE                   (4096U)

#define WAITING_MESSAGE                         ("Waiting Results")

#define OSPI_START_ADDRESS                      (0x93f00000)        /* CS1 */

#define STATUS_WRITE_ENABLED                    (2)

#define CONNECTION_ABORT_CRTL                   (0x00)
#define MENU_EXIT_CRTL                          (0x20)
#define MENU_ENTER_RESPONSE_CRTL                (0x09)
#define CARRAGE_RETURN                          (0x0D)
#define INPUT_BUFFER                            (0x05)

#define BLOCK_LIMIT                             (64)
#define FOURKB_LIMIT                            (BLOCK_LIMIT / 4)

#undef MODULE_NAME
#define MODULE_NAME     "\r\n%d. OCTO-SPI SPEED DEMONSTRATION\r\n"

#define SUB_OPTIONS     "\r\nCompares the write and read times to and from external Octo-SPI" \
                        "\r\nflash memory\r\n" \
                        "\r\n> Enter the text block size " \
                        "\r\n(in multiples of 2 KB, max 64 KB) and press tab to continue : "

#define OSPI_TEST_DATA_LENGTH                   (0x40U)
#define OSPI_SECTOR_SIZE                        (4096U)
#define OSPI_BLOCK_SIZE                         (262144U)

/* Limit test to 4K */
#define WRITE_BLOCK_SIZE                        (64)
#define TEST_BLOCKS                             (64)

#define MAX_BLOCK_SIZE                          (64 * 1024)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
/* Block in RAM for performance measurement */
static uint8_t s_read_block[MAX_BLOCK_SIZE];

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

static void wait_for_write(void);
void ospi_performance_test(uint32_t data_size, uint32_t *ospi_performance_write_result,
        uint32_t *ospi_performance_read_result);
void clear_console(void);
static uint32_t validate_user_input(char_t *p_input);
static fsp_err_t write_test_opi(uint32_t num_blocks);
static fsp_err_t read_test_opi(uint32_t block_size);

static uint8_t *const gp_ospi_cs1 = (uint8_t *) OSPI_START_ADDRESS;

#ifndef USE_TINY_TEST
static uint8_t g_test_data[OSPI_TEST_DATA_LENGTH] = "";
#else
static uint8_t  g_test_data[OSPI_TEST_DATA_LENGTH] =
{
    0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x08, 0x00, 0x00, 0xF7, 0xFF, 0x00, 0x08, 0xF7, 0x00, 0xF7, // Auto-calibration pattern.
    0x35, 0x35, 0x35, 0x35, 0xBD, 0xFF, 0xEE, 0xF1, 0x36, 0x36, 0x36, 0x36, 0xBC, 0xFF, 0xEF, 0xF2,
    0x37, 0x37, 0x37, 0x37, 0xBF, 0x01, 0xEE, 0xF1, 0x38, 0x38, 0x38, 0x38, 0xBA, 0x1F, 0x6F, 0xF3,
    0xA5, 0xA5, 0xA5, 0xA5, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
};
#endif

static uint32_t s_test_remain           = WRITE_BLOCK_SIZE;
static uint32_t s_test_current_block    = 0;
static uint32_t s_test_current_sub      = 0;
static size_t   s_data_size             = TEST_BLOCKS * WRITE_BLOCK_SIZE;

static char_t   s_block_sz_str[INPUT_BUFFER] = {};

static uint32_t g_counter = 0;

st_rgb565_text_block_page_t g_ext_page =
{
    13, // Number of lines required (including blanks)
    " OCTO-SPI SPEED DEMONSTRATION", /* Title */
    {
       {{PAGE_LEFT_EDGE, LINE(1)  + BLOCK(0)},"INSTRUCTIONS"},
       {{PAGE_LEFT_EDGE, LINE(2)  + BLOCK(0)},"Compares the write and read times to and from external Octo-SPI flash memory."},
       {{PAGE_LEFT_EDGE, LINE(3)  + BLOCK(0)}," "},
       {{PAGE_LEFT_EDGE, LINE(5)  + BLOCK(0)},"Operation/Flash"},
       {{PAGE_1ST_TAB,   LINE(5)  + BLOCK(0)},"Octo-SPI"},
       {{PAGE_LEFT_EDGE, LINE(6)  + BLOCK(0)},"___________________________________________"},
       {{PAGE_LEFT_EDGE, LINE(7)  + BLOCK(0)},"Write"},
       {{PAGE_1ST_TAB,   LINE(7)  + BLOCK(0)},""},
       {{PAGE_LEFT_EDGE, LINE(8)  + BLOCK(0)},"Read"},
       {{PAGE_1ST_TAB,   LINE(8)  + BLOCK(0)},""},
       {{PAGE_LEFT_EDGE, LINE(9)  + BLOCK(0)},""},

       {{PAGE_LEFT_EDGE, LINE(11)  + BLOCK(0)},"NOTE"},
       {{PAGE_LEFT_EDGE, LINE(12) + BLOCK(0)},"Menu & Close icons Disabled"},
    }
};

/**********************************************************************************************************************
 * Function Name: ospi_performance_test
 * Description  : erase an area of flash, then write to it followed by a read to verify written data
 * Arguments    : data_size
 *              : ospi_performance_write_result
 *              : ospi_performance_read_result
 * Return Value : None
 *********************************************************************************************************************/
void ospi_performance_test(uint32_t data_size, uint32_t *ospi_performance_write_result,
        uint32_t *ospi_performance_read_result)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t erase_size = 4096;

    /* OSPI device already opened as it is used for the PLCD graphics */
    err = ospi_b_init();

    /* Clean the OSPI device up before testing */
    err = ospi_b_set_protocol_to_spi ();

    if (FSP_SUCCESS == err)
    {
        for (uint32_t nb = 0; nb < FOURKB_LIMIT; nb++)
        {
            err = g_ospi0.p_api->erase(g_ospi0.p_ctrl, (uint8_t*) (gp_ospi_cs1 + (erase_size * nb)), erase_size);
            assert(FSP_SUCCESS == err);
            wait_for_write ();
        }
    }
    else
    {
        print_to_console("Failed to set device to SPI mode\r\n");
    }

    err = ospi_b_set_protocol_to_opi();
    if (FSP_SUCCESS == err)
    {
        print_to_console("\r\nWriting the text block to external Octo-SPI flash memory ...\r\n");

        err = write_test_opi(data_size / 64);

        *ospi_performance_write_result = g_counter;

        print_to_console("Writing to flash completed\r\n");

        if (FSP_SUCCESS != err)
        {
            print_to_console("Flash write failed\r\n");
        }
    }
    else
    {
        print_to_console("Failed to set device to OPI mode\r\n");
    }

    if (FSP_SUCCESS == err)
    {
        print_to_console ("\r\nReading the text block from external Octo-SPI flash memory ...\r\n");

        err = read_test_opi (data_size / 64);

        *ospi_performance_read_result = g_counter;

        print_to_console("Reading from flash completed\r\n");

        if (FSP_SUCCESS != err)
        {
            print_to_console("Flash read / verify failed\r\n");
        }
    }
}
/**********************************************************************************************************************
 End of function ospi_performance_test
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: ext_display_menu
 * Description  : Implements the external flash memory menu
 * Return Value : 0
 *********************************************************************************************************************/
test_fn ext_display_menu(void)
{
    bool_t   valid_block_size  = false;
    int32_t c = -1;

    uint32_t block_size_actual = 0;
    int32_t  block_sz_ndx      = 0;
    int32_t  block_sz_limit    = (INPUT_BUFFER - 2); /* Allowing for TAB and End of Message */
    uint32_t value             = 0;
	
    char_t res_str[24];

    clear_console();

    sprintf(res_str, WAITING_MESSAGE);

    sprintf(g_ext_page.text_block[7].line, res_str);
    sprintf(g_ext_page.text_block[9].line, res_str);

    sprintf(res_str," ");
    sprintf(g_ext_page.text_block[10].line, res_str);

    dsp_set_background(LCD_FULL_BG_EXTERNAL_MEMORY);

    sprintf (s_print_buffer, MODULE_NAME, g_selected_menu);
    print_to_console(s_print_buffer);
    print_to_console(SUB_OPTIONS);

    /* Keep trying to read a valid text block size
     * complete the loop in one of two ways:
     * [1] Valid block size is entered (2K boundary range 2-64K) followed by TAB
     * [2] Space Bar is pressed at any stage
     */

    while (false == valid_block_size)
    {
        /* Reset input state */
        c = -1;

        block_sz_ndx = 0;
        memset (&s_block_sz_str, 0, INPUT_BUFFER);

        while ((CONNECTION_ABORT_CRTL != c))
        {
            c = input_from_console();

            if ((block_sz_ndx < block_sz_limit) && (BACK_SPACE != c) && (BACK_SPACE_PUTTY != c) && (CARRAGE_RETURN != c)
                    && (MENU_ENTER_RESPONSE_CRTL != c))
            {
                /* Cast to req type */
                s_block_sz_str[block_sz_ndx] = (char_t)c;
                block_sz_ndx++;
            }
            else if (((BACK_SPACE == c) || (BACK_SPACE_PUTTY == c)) && (block_sz_ndx > 0))
            {
                s_block_sz_str[block_sz_ndx - 1] = '\0';
                block_sz_ndx--;
                print_to_console("\b \b");
            }
            else if ((0 == block_sz_ndx) || (CARRAGE_RETURN == c))
            {
                __NOP();
            }
            else
            {
                /* maximum block size exceeded (4 digits / characters entered) */
                s_block_sz_str[block_sz_ndx] = MENU_ENTER_RESPONSE_CRTL;
                c                            = MENU_ENTER_RESPONSE_CRTL;
                break;
            }

            if (MENU_EXIT_CRTL == c)
            {
                /* Abort the test */
                valid_block_size = true;
                block_size_actual = 0;
                break;
            }

            if ((MENU_ENTER_RESPONSE_CRTL == c) || (CARRAGE_RETURN == c))
            {
                break;
            }

            if ((CARRAGE_RETURN != c) && (BACK_SPACE != c))
            {
                sprintf (s_print_buffer, "%c", (char_t) c);
                print_to_console(s_print_buffer);
            }
        }

        /* If the input was terminated with a TAB then attempt to process it */
        if ((MENU_ENTER_RESPONSE_CRTL == c) || (CARRAGE_RETURN == c))
        {
            value = validate_user_input (&s_block_sz_str[0]);
        }

        vTaskDelay (10);

        if (value < 2)
        {
            value = INVALID_BLOCK_SIZE;
        }

        if ((value > BLOCK_LIMIT) && (value < INVALID_MARKERS))
        {
            value = INVALID_BLOCK_SIZE;
        }

        if ((value > 0) && ((value % 2) != 0) && (value < INVALID_MARKERS))
        {
            value = INVALID_BLOCK_BOUNDARY;
        }

        switch (value)
        {
            case INVALID_CHARACTER:
            {
                print_to_console("\r\n> Invalid character in entry, enter the text block size specifying a 2K boundary (e.g. 24)"
                         " and press tab: ");
                break;
            }

            case INVALID_BLOCK_SIZE:
            {
                print_to_console("\r\n> Invalid size, enter the text block size (e.g. 24) and press tab: ");
                break;
            }

            case INVALID_BLOCK_BOUNDARY:
            {
                print_to_console("\r\n> Invalid boundary, enter the text block size specifying a 2K boundary (e.g. 24)"
                         " and press tab : ");
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
        fsp_err_t fsp_err = FSP_SUCCESS;

        uint32_t ospi_performance_write_result = 0;
        uint32_t ospi_performance_read_result = 0;
        uint32_t ospi_read_result = 0;
        uint32_t ospi_write_result = 0;

        sprintf (s_print_buffer, "\r\n\r\nGenerated a text block of %2lu KB in SRAM\r\n", block_size_actual);
        print_to_console(s_print_buffer);

        block_size_actual = block_size_actual * 1024;

        ospi_performance_test (block_size_actual, &ospi_performance_write_result, &ospi_performance_read_result);

        ospi_write_result =  ospi_performance_write_result / GPT_CLOCK_MHZ;
        ospi_read_result  =  ospi_performance_read_result / GPT_CLOCK_MHZ;

        /* Handle error */
        if (FSP_SUCCESS != fsp_err)
        {
            /* Fatal error */
            SYSTEM_ERROR_CFG_
        }

        print_to_console("\r\n-------------------------------------------------");
        print_to_console("\r\nOperation/Flash          Octo-SPI");
        print_to_console("\r\n-------------------------------------------------");

        sprintf (s_print_buffer, "\r\nWrite                    %6ld", ospi_write_result);
        print_to_console(s_print_buffer);

        sprintf (s_print_buffer, "\r\nRead                     %6ld", ospi_read_result);
        print_to_console(s_print_buffer);

        print_to_console("\r\n-------------------------------------------------");
        print_to_console("\r\nNote: Times are in microseconds");
        print_to_console(MENU_RETURN_INFO);

        sprintf(res_str,"%6ld uS", ospi_write_result);
        sprintf(g_ext_page.text_block[7].line, res_str);
        sprintf(res_str,"%6ld uS", ospi_read_result);
        sprintf(g_ext_page.text_block[9].line, res_str);

        sprintf(res_str,"Block Size %6ld Bytes", block_size_actual);
        sprintf(g_ext_page.text_block[10].line, res_str);

    }

    while ((CONNECTION_ABORT_CRTL != c))
    {
        if ((MENU_EXIT_CRTL == c) || (0x00 == c))
        {
            break;
        }

        c = input_from_console ();
    }

    return (0);
}
/**********************************************************************************************************************
 End of function ext_display_menu
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: clear_console
 * Description  : Send escape codes to clear the console and home the cursor
 * Return Value : None
 *********************************************************************************************************************/
void clear_console(void)
{
    print_to_console("\x1b[2J\x1b[H");
}
/**********************************************************************************************************************
 End of function clear_console
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wait_for_write
 * Description  : Read the status register until it indicates that the write has completed
 * Return Value : None
 *********************************************************************************************************************/
static void wait_for_write(void)
{
    spi_flash_status_t status =
    { 0 };

    do
    {
        assert(FSP_SUCCESS == g_ospi0.p_api->statusGet (g_ospi0.p_ctrl, &status));
    }
    while (status.write_in_progress);
}
/**********************************************************************************************************************
 End of function wait_for_write
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: write_test_opi
 * Description  : write data to the OSPI flash and use a timer to time the write duration
 * Argument     : num_blocks
 * Return Value : FSP_SUCCESS on success or an error code on failure
 *********************************************************************************************************************/
static fsp_err_t write_test_opi(uint32_t num_blocks)
{
    fsp_err_t err;
    fsp_err_t retval = FSP_SUCCESS;
    timer_status_t timer_status;

    srand(0);
    volatile uint8_t data = (uint8_t) rand();

    R_GPT_Open(g_memory_performance.p_ctrl, g_memory_performance.p_cfg);

    FSP_CRITICAL_SECTION_DEFINE;
    FSP_CRITICAL_SECTION_ENTER;

    /* Write the test data in OPI mode */
    g_counter = 0;

    for (s_test_current_block = 0; s_test_current_block < num_blocks; s_test_current_block++)
    {
        s_test_remain = WRITE_BLOCK_SIZE;
        s_test_current_sub = 0;

        while (s_test_remain > 0)
        {
            s_test_remain--;
            data = (uint8_t) rand();
            g_test_data[s_test_current_sub++] = data;
        }

        /* Only Time the write function that includes the wait_for_write */
        R_GPT_Start(g_memory_performance.p_ctrl);

        err = g_ospi0.p_api->write (g_ospi0.p_ctrl, (g_test_data),
                                    (uint8_t *) (gp_ospi_cs1 + (s_test_current_block * WRITE_BLOCK_SIZE)),
                                    WRITE_BLOCK_SIZE);

        wait_for_write();

        R_GPT_Stop(g_memory_performance.p_ctrl);
        R_GPT_StatusGet(g_memory_performance.p_ctrl, &timer_status);

        if (err != FSP_SUCCESS)
        {
            retval = err;
        }

        /* Dummy delay */
        R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MICROSECONDS);
    }
    FSP_CRITICAL_SECTION_EXIT;

    R_GPT_StatusGet(g_memory_performance.p_ctrl, &timer_status);
    g_counter= timer_status.counter;

    R_GPT_Close(g_memory_performance.p_ctrl);

    if (retval != FSP_SUCCESS)
    {
        print_to_console("Write failed");
    }

    return (retval);
}
/**********************************************************************************************************************
 End of function write_test_opi
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: read_test_opi
 * Description  : read data from the OSPI flash and use a timer to time the read duration
 * Argument     : block_size - the size of the block to read
 * Return Value : FSP_SUCCESS on success or an error code on failure
 *********************************************************************************************************************/
static fsp_err_t read_test_opi(uint32_t block_size)
{
    fsp_err_t err = FSP_SUCCESS;
    timer_status_t timer_status;
    uint32_t failing_index = 0;
    uint8_t *ptr;

    srand(0);
    volatile uint8_t data = (uint8_t) rand();
    volatile uint32_t errors = 0;

    /* Upscale block_size to bytes by multiply by 64 */
    block_size *= 64;

    s_data_size = TEST_BLOCKS * WRITE_BLOCK_SIZE;

    err = R_GPT_Open(g_memory_performance.p_ctrl, g_memory_performance.p_cfg);

    memset(s_read_block, 0, block_size);

    FSP_CRITICAL_SECTION_DEFINE;
    FSP_CRITICAL_SECTION_ENTER;

    R_GPT_Reset(g_memory_performance.p_ctrl);

    /* Read the data and time the read operation */
    ptr = gp_ospi_cs1;

    err = R_GPT_Start(g_memory_performance.p_ctrl);

    memcpy(s_read_block, ptr, block_size);

    R_GPT_StatusGet(g_memory_performance.p_ctrl, &timer_status);

    g_counter = timer_status.counter;
    FSP_CRITICAL_SECTION_EXIT;

    R_GPT_Close(g_memory_performance.p_ctrl);

    ptr = s_read_block;

    /* Verify data */
    for (uint32_t i = 0; i < block_size; i++)
    {
        data = (uint8_t) rand();

        if (*ptr != data)
        {
            if (errors == 0)
            {
                failing_index = i;
            }

            err = FSP_ERR_INVALID_DATA;
            errors++;
        }

        ptr++;
    }

    if (errors > 0)
    {
        sprintf(s_print_buffer, "Failed to verify %lu bytes. First verification failure index %lu\r\n", errors, failing_index);
        print_to_console(s_print_buffer);
    }

    return (err);
}
/**********************************************************************************************************************
 End of function read_test_opi
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: validate_user_input
 * Description  : parse the user input and convert to integer value
 * Argument     : p_input
 * Return Value : Integer value entered, or 0 if the menu exit character was entered
 *********************************************************************************************************************/
static uint32_t validate_user_input(char_t *p_input)
{
    uint32_t result = INVALID_CHARACTER;
    uint32_t value  = 0;
    uint32_t t      = 0;
    uint32_t c      = 0;

    while (true)
    {
        /* Cast to req type */
        c = (uint32_t) (*p_input);

        p_input++;

        if ((c >= '0') && (c <= '9'))
        {
            /* Cast as compiler will interpret result as int */
            value = (uint32_t) (value * 10);

            /* Cast to req type */
            t = (uint32_t) (c - '0');

            /* Cast as compiler will interpret result as int */
            value = (uint32_t) (value + t);
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
