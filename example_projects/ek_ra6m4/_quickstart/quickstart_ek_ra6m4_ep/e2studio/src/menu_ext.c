/**********************************************************************************************************************
 * File Name    : menu_ext.c
 * Version      : .
 * Description  : The Memory device access demo.
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include <stdio.h>
#include <string.h>

#include "bsp_api.h"
#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "menu_ext.h"

#include "r_qspi.h"
#include "qspi_ep.h"


#define CONNECTION_ABORT_CRTL          (0x00)
#define MENU_EXIT_CRTL                 (0x20)
#define MENU_ENTER_RESPONSE_CRTL       (0x09)
#define INPUT_BUFFER                   (0x05)
#define CARRAGE_RETURN                 (0x0D)

#define BLOCK_LIMIT                    (0x40)

#define INVALID_CHARACTER              ((uint32_t) -1)
#define INVALID_BLOCK_SIZE             ((uint32_t) -2)
#define INVALID_BLOCK_BOUNDARY         ((uint32_t) -3)

#define OSPI_TEST_FLASH_DEVICE_CHANNEL (0)
#define OSPI_TEST_FLASH_SIZE           (0x64000000)
#define OSPI_TEST_MEM_MAP_START        (0x68000000U)

#define OSPI_DEVICE_DESTINATION        (OSPI_TEST_MEM_MAP_START + 0)
#define NUM_PAGES                      (512)

#define OSPI_TEST_WRITE_ENABLE_COMMAND_SPI_MODE     (0x06U)
#define OSPI_TEST_WRITE_DISABLE_COMMAND_SPI_MODE    (0x04U)
#define OSPI_TEST_READ_STATUS_COMMAND_SPI_MODE      (0x05U)
#define OSPI_COMMAND_PAGE_PROGRAM_SPI_MODE          (0x12U)
#define OSPI_COMMAND_NORMAL_READ_SPI_MODE           (0x13U)
#define OSPI_COMMAND_NORMAL_FASTREAD_SPI_MODE       (0x0CU)
#define OSPI_COMMAND_SECTOR_ERASE_SPI_MODE          (0x21U)
#define OSPI_COMMAND_READ_ID_SPI_MODE               (0x9FU)

#define OSPI_COMMAND_SECTOR_ERASE_3SPI_MODE         (0x20U)
#define OSPI_COMMAND_BLOCK_ERASE_3SPI_MODE          (0xD8U)
#define OSPI_COMMAND_CHIP_ERASE_3SPI_MODE           (0x60U)

#define OSPI_COMMAND_BURST_MODE                     (0xC0U)

#define OSPI_COMMAND_RCR2_SPI_MODE                  (0x71U)
#define OSPI_COMMAND_WCR2_SPI_MODE                  (0x72U)
#define OSPI_COMMAND_RCR2_OPI_MODE                  (0x718EU)
#define OSPI_TEST_WRITE_ENABLE_COMMAND_OPI_MODE     (0x06F9U)
#define OSPI_TEST_WRITE_DISABLE_COMMAND_OPI_MODE    (0x04FBU)
#define OSPI_TEST_READ_STATUS_COMMAND_OPI_MODE      (0x05FAU)
#define OSPI_COMMAND_PAGE_PROGRAM_OPI_MODE          (0x12EDU)
#define OSPI_COMMAND_NORMAL_READ_OPI_MODE           (0xEC13U)
#define OSPI_COMMAND_READ_DUAL_OPI_MODE             (0xEE11U)
#define OSPI_COMMAND_SECTOR_ERASE_OPI_MODE          (0x21DEU)
#define OSPI_COMMAND_BLOCK_ERASE_OPI_MODE           (0xDC23U)
#define OSPI_COMMAND_CHIP_ERASE_OPI_MODE            (0x609FU)
#define OSPI_COMMAND_WCR2_OPI_MODE                  (0x728DU)

#define OSPI_TEST_READ_COMMAND_SPI_MODE             (0x03)

#define OSPI_TEST_RESET_EN_COMMAND_SPI_MODE         (0x66)
#define OSPI_TEST_RESET_COMMAND_SPI_MODE            (0x99)

#define OSPI_TEST_RESET_EN_COMMAND_OPI_MODE         (0x6600)
#define OSPI_TEST_RESET_COMMAND_OPI_MODE            (0x9966)

#define OSPI_WEN_BIT_MASK                           (0x02U)
#define OSPI_WIP_BIT_MASK                           (0x01U)
#define OSPI_TEST_PAGE_SIZE                         (256U)
#define OSPI_TEST_SECTOR_SIZE                       (4096U)
#define OSPI_TEST_BLOCK_SIZE                        (65536U)
#define OSPI_NUM_BYTES_READ                         (4u)

#define MODULE_NAME     "\r\n%d. QUAD-SPI AND OCTO-SPI SPEED COMPARISON\r\n"

#define SUB_OPTIONS     "\r\nCompares the write and read times to and from external Quad-SPI and Octa-SPI" \
                        "\r\nflash memories\r\n" \
                        "\r\n> Enter the text block size " \
                        "\r\n(in multiples of 2 KB, max 64 KB) and press tab to continue : "

/* sub list of the ospi commands in use for this demo */
typedef enum e_ospi_test_command
{
    OSPI_TEST_COMMAND_WRITE_ENABLE_SPI_MODE = 0,
    OSPI_TEST_COMMAND_WRITE_DISABLE_SPI_MODE,
    OSPI_TEST_COMMAND_READ_STATUS_SPI_MODE,
    OSPI_TEST_COMMAND_SECTOR_ERASE_SPI_MODE,
    OSPI_TEST_COMMAND_READ_SPI_MODE,
    OSPI_TEST_COMMAND_MAX
} ospi_test_command_t;

/* Terminal window escape sequences */
static const char * const sp_clear_screen   = "\x1b[2J";
static const char * const sp_cursor_home    = "\x1b[H";

static char print_buffer[BUFFER_LINE_LENGTH] = {};
static char block_sz_str[INPUT_BUFFER] = {};

static const uint8_t * sp_source = (uint8_t *) "1234567891234567896789123456789678912345678967891"                     \
                                               "12345678912345678967891234567896789123456789678912"                    \
                                               "123456789123456789678912345678967891234567896789"                      \
                                               "123456789123456789678912345678967891234567896789123456789678912"       \
                                               "12345678912345678967891234567896789123456789678912345678967891234567"  \
                                               "1234567891234567896789123456789678912345678967891234567896789123456789"\
                                               "1234567891234567896789123456789678912345678967891234567896789123456"   \
                                               "123456789123456789678912345678967891234567896789123456789678912345678" \
                                               "123456789123456789678912345678967891234567896789123456789678912345678";



uint8_t read_buffer[2 * 1024] = {};

/* An altered temporary copy of configuration structure.
 * Must be global, as a pointer to this is written to a ctrl struct */
static spi_flash_cfg_t s_spi_variant_qspi_cfg;

static bool s_qspi_dma_transfer_complete = false;

static void deinit_qspi (const spi_flash_protocol_t spi_protocol_mode);
static fsp_err_t qpi_mode_set (void);
static fsp_err_t get_flash_status (void);
static fsp_err_t qpi_init (void);

extern void ospi_performance_test (uint32_t data_size,
                                    uint32_t * ospi_performance_write_result,
                                    uint32_t * ospi_performance_read_result);

/**********************************************************************************************************************
 * Function Name: qspi_read_test
 * Description  : .
 * Argument     : block_size
 * Return Value : .
 *********************************************************************************************************************/
static uint32_t qspi_read_test(uint32_t block_size)
{
    fsp_err_t fsp_err;
    fsp_err_t err = FSP_SUCCESS;

    uint32_t qspi_read_result = 0;
    timer_status_t status = {};
    spi_flash_protocol_t current_spi_mode;

    uint8_t  *p_dma_read_buffer;
    uint32_t  page_read_count;
    uint8_t  *p_mem_addr;

    /* Convert from kB */
    block_size *= 1024;

    p_dma_read_buffer = pvPortMalloc(block_size);

    if (NULL == p_dma_read_buffer)
    {
        HeapStats_t pxHeapStats;
        vPortGetHeapStats(&pxHeapStats);

        sprintf(print_buffer, "\r\nQSPI malloc operation Failed - Max free mem: %dbytes\r\n",
                pxHeapStats.xSizeOfLargestFreeBlockInBytes);

        /* Verification failed, perhaps the ERASE failed */
        err = FSP_ERR_NOT_ERASED;
    }

    /* The comms mode of the FLASH device is EXTENDED_SPI by default */
    current_spi_mode = SPI_FLASH_PROTOCOL_EXTENDED_SPI;

    /* initialise the QSPI, and change mode to that set in FSP */
    err = qpi_init();
    if (FSP_SUCCESS == err)
    {
        /* The comms mode has changed. So if recovering, this new mode required */
        current_spi_mode = g_qspi_cfg.spi_protocol;
    }

    /* Start the test timer */
    fsp_err = R_GPT_Start(g_memory_performance.p_ctrl);

    /* Handle error */
    if (FSP_SUCCESS != fsp_err)
    {
        /* Fatal error */
        SYSTEM_ERROR
    }

    page_read_count = 0;
    p_mem_addr = (uint8_t *)QSPI_DEVICE_START_ADDRESS;

    while (((page_read_count * PAGE_WRITE_SIZE) < block_size)
            && (FSP_SUCCESS == err))
    {
        /* Verify the written data */
        /* Each block begins one character shifted along the source text. To avoid regular striping in memory */
        if ((fsp_err_t) (memcmp (p_mem_addr, &(sp_source[page_read_count]), PAGE_WRITE_SIZE)) != FSP_SUCCESS)
        {
            err = FSP_ERR_NOT_ERASED;

            sprintf(print_buffer, "\r\nQSPI operation Failed -> Data read does not match with written data\r\n");
        }
        p_mem_addr += PAGE_WRITE_SIZE;
        page_read_count++;
    }

    fsp_err = R_GPT_Stop(g_memory_performance.p_ctrl);

    /* close QSPI module */
    deinit_qspi(current_spi_mode);

    /* Handle error */
    if(FSP_SUCCESS != fsp_err)
    {
        /* Fatal error */
        SYSTEM_ERROR
    }

    fsp_err = R_GPT_StatusGet(g_memory_performance.p_ctrl, &status);

    /* Handle error */
    if (FSP_SUCCESS != fsp_err)
    {
        /* Fatal error */
        SYSTEM_ERROR
    }

    fsp_err = R_GPT_Reset(g_memory_performance.p_ctrl);

    /* Handle error */
    if (FSP_SUCCESS != fsp_err)
    {
        /* Fatal error */
        SYSTEM_ERROR
    }

    qspi_read_result = status.counter;

    vPortFree(p_dma_read_buffer);

    return (qspi_read_result);
}
/**********************************************************************************************************************
 End of function qspi_read_test
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: qspi_dma_complete_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void qspi_dma_complete_callback(dmac_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    s_qspi_dma_transfer_complete = true;
}
/**********************************************************************************************************************
 End of function qspi_dma_complete_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: qspi_write_test
 * Description  : .
 * Argument     : block_size
 * Return Value : .
 *********************************************************************************************************************/
static uint32_t qspi_write_test(uint32_t block_size)
{
    fsp_err_t fsp_err;
    uint32_t qspi_write_result = 0;
    timer_status_t status = {};
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_protocol_t current_spi_mode;

    /* Convert from kB */
    block_size *= 1024;

    /* The comms mode is EXTENDED_SPI by default */
    current_spi_mode = SPI_FLASH_PROTOCOL_EXTENDED_SPI;

    /* initialise the QSPI, and change mode to that set in FSP */
    err = qpi_init();
    if (FSP_SUCCESS == err)
    {
        /* The comms mode has changed. So if recovering, this new mode required */
        current_spi_mode = g_qspi_cfg.spi_protocol;
    }

    uint32_t page_write_count = 0;
    uint8_t *p_mem_addr;

    p_mem_addr = (uint8_t *)QSPI_DEVICE_START_ADDRESS;

    while (((page_write_count * SECTOR_SIZE) < block_size)
        && ( FSP_SUCCESS == err ) )
    {

        /* Erase Flash for one sector */
        err = R_QSPI_Erase(&g_qspi_ctrl, p_mem_addr, SECTOR_SIZE);
        if (FSP_SUCCESS != err)
        {
            sprintf(print_buffer, "R_QSPI_Erase Failed\r\n");
        }
        else
        {
            err = get_flash_status();
            if (FSP_SUCCESS != err)
            {
                sprintf(print_buffer, "Failed to get status for QSPI operation\r\n");
            }

            /* Verify the erased block data */
            uint32_t count;
            for (count = 0 ; count < SECTOR_SIZE ; count++ )
            {
                if (DEFAULT_MEM_VAL != p_mem_addr[count])
                {
                    /* Verification failed, perhaps the ERASE failed */
                    err = FSP_ERR_NOT_ERASED;
                }
            }
        }

        p_mem_addr += SECTOR_SIZE;
        page_write_count++;
    }

    /* Start the test timer */
    fsp_err = R_GPT_Start(g_memory_performance.p_ctrl);

    /* Handle error */
    if (FSP_SUCCESS != fsp_err)
    {
        /* Fatal error */
        SYSTEM_ERROR
    }

    p_mem_addr = (uint8_t *)QSPI_DEVICE_START_ADDRESS;
    page_write_count = 0;

    while (((page_write_count * PAGE_WRITE_SIZE) < block_size)
        && (FSP_SUCCESS == err))
    {
        if (FSP_SUCCESS == err)
        {
            /* Write data to QSPI Flash */
            /* Each block begins one character shifted along the source text. To avoid regular striping in memory */
            err = R_QSPI_Write(&g_qspi_ctrl, &(sp_source[page_write_count]), p_mem_addr, PAGE_WRITE_SIZE);
            if (FSP_SUCCESS != err)
            {
                sprintf(print_buffer, "R_QSPI_Write Failed\r\n");
            }
            else
            {
                err = get_flash_status();
                if (FSP_SUCCESS != err)
                {
                    sprintf(print_buffer, "Failed to get status for QSPI operation\r\n");
                }
            }
        }
        p_mem_addr += PAGE_WRITE_SIZE;
        page_write_count++;
    }

    /* close QSPI module */
    deinit_qspi(current_spi_mode);

    fsp_err = R_GPT_Stop(g_memory_performance.p_ctrl);

    /* Handle error */
    if (FSP_SUCCESS != fsp_err)
    {
        /* Fatal error */
        SYSTEM_ERROR
    }

    fsp_err = R_GPT_StatusGet(g_memory_performance.p_ctrl, &status);

    /* Handle error */
    if (FSP_SUCCESS != fsp_err)
    {
        /* Fatal error */
        SYSTEM_ERROR
    }

    fsp_err = R_GPT_Reset(g_memory_performance.p_ctrl);

    /* Handle error */
    if (FSP_SUCCESS != fsp_err)
    {
        /* Fatal error */
        SYSTEM_ERROR
    }

    qspi_write_result = status.counter;

    return (qspi_write_result);
}
/**********************************************************************************************************************
 End of function qspi_write_test
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: validate_user_input
 * Description  : .
 * Argument     : p_input
 * Return Value : .
 *********************************************************************************************************************/
static uint32_t validate_user_input(char *p_input)
{
    uint32_t result = INVALID_CHARACTER;
    uint32_t value = 0;
    uint32_t t = 0;
    char c = 0;

    bool_t processing = true;

    while (true == processing)
    {
        c = *p_input++;

        if((c >= '0') && (c <= '9'))
        {
            value = (uint32_t)(value * 10);
            t     = ((uint32_t)atoi(&c));
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
    int c = -1;
    uint32_t block_size_actual = 0;
    int block_sz_ndx = 0;
    int block_sz_limit = (INPUT_BUFFER - 2); /* Allowing for TAB and End of Message */
    timer_info_t timer_info;

    bool_t valid_block_size = false;

    uint32_t value = 0;

    sprintf(print_buffer, "%s%s", sp_clear_screen, sp_cursor_home);
    print_to_console(print_buffer);

    sprintf(print_buffer, MODULE_NAME, g_selected_menu);
    print_to_console(print_buffer);

    sprintf(print_buffer, SUB_OPTIONS);
    print_to_console(print_buffer);

    /* Keep trying to read a vaild text block size
     * complete the loop in one of two ways:
     * [1] Valid block size is entered (2K boundary range 2-64K) followed by TAB
     * [2] Space Bar is pressed at any stage
     */

    while (false == valid_block_size)
    {
        /* Reset input state */
        c = -1;
        block_sz_ndx = 0;
        memset(&block_sz_str, 0, INPUT_BUFFER);

        while ((CONNECTION_ABORT_CRTL != c))
        {
            c = input_from_console();

            if (block_sz_ndx < block_sz_limit)
            {
                block_sz_str[block_sz_ndx++ ] = (char)c;
            }
            else
            {
                /* maximum bloclk size exceeded (4 digits / characters entered) */
                block_sz_str[block_sz_ndx] = MENU_ENTER_RESPONSE_CRTL;
                break;
            }

            if (MENU_EXIT_CRTL == c)
            {
                /* Abort the test */
                valid_block_size = true;
                block_size_actual = 0;
                break;
            }

            if (MENU_ENTER_RESPONSE_CRTL == c)
            {
                break;
            }

            if (CARRAGE_RETURN != c)
            {
                sprintf(print_buffer, "%c", c);
                print_to_console(print_buffer);
            }
        }

        /* If the input was terminated with a TAB thern attempt to process it */
        if (MENU_ENTER_RESPONSE_CRTL == c)
        {
            value = validate_user_input(&block_sz_str[0]);
        }

        vTaskDelay(10);

        if ((value > 0) && (value < 2))
        {
            value = INVALID_BLOCK_SIZE;
        }

        if (value > BLOCK_LIMIT)
        {
            value = INVALID_BLOCK_SIZE;
        }

        if ((value > 0) && ((value % 2) != 0))
        {
            value = INVALID_BLOCK_BOUNDARY;
        }

        switch (value)
        {
            case INVALID_CHARACTER:
            {
                sprintf(print_buffer,
                        "\r\nInvalid character in entry please enter the text block size\r\n specifying a 2K boundary (eg 24 and press <TAB>) :");
                print_to_console(print_buffer);
                break;
            }

            case INVALID_BLOCK_SIZE:
            {
                sprintf(print_buffer, "\r\nInvalid size please enter the text block size (eg 24 and press <TAB>) :");
                print_to_console(print_buffer);
                break;
            }

            case INVALID_BLOCK_BOUNDARY:
            {
                sprintf(print_buffer,
                        "\r\nInvalid boundary please enter the text block size specifying\r\n a 2K boundary (eg 24 and press <TAB>) :");
                print_to_console(print_buffer);
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
        fsp_err_t      fsp_err;
        uint32_t       ospi_read_result  = 0;
        uint32_t       ospi_write_result = 0;
        uint32_t       qspi_read_result  = 0;
        uint32_t       qspi_write_result = 0;

        fsp_err = R_GPT_Open(g_memory_performance.p_ctrl, g_memory_performance.p_cfg);

        /* Handle error */
        if (FSP_SUCCESS != fsp_err)
        {
            /* Fatal error */
            SYSTEM_ERROR
        }

        sprintf(print_buffer, "\r\n\r\nGenerated a text block of %2lu KB in SRAM\r\n", block_size_actual);
        print_to_console(print_buffer);

        print_to_console("\r\nWriting the text block to external Quad-SPI and Octo-SPI flash memories...\r\n");

        uint32_t ospi_performance_write_result = 0;
        uint32_t ospi_performance_read_result = 0;
        uint32_t timer_frequency;


        R_GPT_InfoGet(g_memory_performance.p_ctrl, &timer_info);
        timer_frequency = timer_info.clock_frequency;

        ospi_performance_test (block_size_actual, &ospi_performance_write_result, &ospi_performance_read_result);

        /* Multiply uSec calcs by 100, to avoid losses due to small results in integer maths
         * Scaled to fit within uint32_t */
        ospi_write_result = ((100000000 / timer_frequency) * ospi_performance_write_result) / 100;
        qspi_write_result  = ((100000000 / timer_frequency) * qspi_write_test(block_size_actual)) / 100;

        print_to_console("Writing to flash completed\r\n");

        print_to_console("\r\nReading the text block from external Quad-SPI and Octo-SPI flash memories...\r\n");

        ospi_read_result  = ((100000000 / timer_frequency) * ospi_performance_read_result) / 100;
        qspi_read_result  = ((100000000 / timer_frequency) * qspi_read_test(block_size_actual)) / 100;
        print_to_console("Reading from flash completed\r\n");

        R_GPT_Close(g_memory_performance.p_ctrl);

        /* Handle error */
        if (FSP_SUCCESS != fsp_err)
        {
            /* Fatal error */
            SYSTEM_ERROR
        }

        print_to_console("\r\n-------------------------------------------------");
        print_to_console("\r\nOperation/Flash     Quad-SPI       Octa-SPI");
        print_to_console("\r\n-------------------------------------------------");

        sprintf(print_buffer, "\r\nWrite                %6ld         %6ld", qspi_write_result , ospi_write_result);
        print_to_console(print_buffer);
        sprintf(print_buffer, "\r\nRead                 %6ld         %6ld", qspi_read_result , ospi_read_result);
        print_to_console(print_buffer);

        print_to_console("\r\n-------------------------------------------------");
        print_to_console("\r\nNote: Times are in microseconds");

        sprintf(print_buffer, MENU_RETURN_INFO);
        print_to_console(print_buffer);
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

/*******************************************************************************************************************//**
 *  @brief       wait for QSPI flash device status register to get idle till operation is in progress
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS or any other possible error codes
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: get_flash_status
 *********************************************************************************************************************/
static fsp_err_t get_flash_status(void)
{
    spi_flash_status_t status = {.write_in_progress = true};
    int32_t time_out          = (INT32_MAX);
    fsp_err_t err             = FSP_SUCCESS;

    do
    {
        /* Get status from QSPI flash device */
        err = R_QSPI_StatusGet(&g_qspi_ctrl, &status);
        if (FSP_SUCCESS != err)
        {
            sprintf(print_buffer, "R_QSPI_StatusGet Failed\r\n");
            return err;
        }

        /* Decrement time out to avoid infinite loop in case of consistent failure */
        --time_out;

        if (RESET_VALUE >= time_out)
        {
            sprintf(print_buffer, "** Timeout : No result from QSPI flash status register ** \r\n");
            return FSP_ERR_TIMEOUT;
        }

    }while (false != status.write_in_progress);

    return err;
}
/**********************************************************************************************************************
 End of function get_flash_status
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 *  @brief       Close QSPI module
 *  @param[IN]   spi_protocol mode
 *  @retval      None
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: deinit_qspi
 *********************************************************************************************************************/
static void deinit_qspi(const spi_flash_protocol_t spi_protocol_mode)
{
    fsp_err_t error = FSP_SUCCESS;

    /* if QPI is active mode then Exit QPI mode from flash device before QSPI close */
    if (SPI_FLASH_PROTOCOL_QPI == spi_protocol_mode)
    {
        uint8_t data_exit_qpi = QSPI_MX25L_CMD_EXIT_QPI_MODE;

        sprintf(print_buffer, "Exit QPI mode\r\n");

        error = R_QSPI_DirectWrite(&g_qspi_ctrl, &data_exit_qpi, ONE_BYTE, false);
        if (FSP_SUCCESS != error)
        {
            sprintf(print_buffer, "R_QSPI_DirectWrite Failed\r\n");
        }
    }

    /* close QSPI module */
    error = R_QSPI_Close(&g_qspi_ctrl);
    if (FSP_SUCCESS != error)
    {
        sprintf(print_buffer, "R_QSPI_Close Failed\r\n");
    }
}
/**********************************************************************************************************************
 End of function deinit_qspi
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 *  @brief       set QPI Mode in flash device and MCU
 *  @param[IN]   none
 *  @retval      FSP_SUCCESS or any other possible error codes
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: qpi_mode_set
 *********************************************************************************************************************/
static fsp_err_t qpi_mode_set(void)
{
    fsp_err_t err = FSP_SUCCESS;

    sprintf(print_buffer, "Setting QPI mode: sending QPI enabling command byte to flash\r\n");

    /* write enable once again section 9-1 states that
     * we should do it before sending 0x35 to flash device
     */
    err = R_QSPI_DirectWrite(&g_qspi_ctrl, &(g_qspi_cfg.write_enable_command), ONE_BYTE, false);
    if (FSP_SUCCESS != err)
    {
        sprintf(print_buffer, "R_QSPI_DirectWrite Failed\r\n");
        return err;
    }
    else
    {
        err = get_flash_status();
        if (FSP_SUCCESS != err)
        {
            sprintf(print_buffer, "Failed to get status for QSPI operation\r\n");
            return err;
        }
    }

    /* send QPI mode enable command in flash device
     * Note - no status register read after this operation
     * because flash device has gone in QPI mode
     * and MCU at this point is in extended SPI mode only.
     * vice versa same is applicable while exiting QPI mode too.
     */
    uint8_t data_qpi_en = QSPI_MX25L_CMD_ENTER_QPI_MODE;  // EQIO command

    err = R_QSPI_DirectWrite(&g_qspi_ctrl, &data_qpi_en, ONE_BYTE, false);
    if (FSP_SUCCESS != err)
    {
        sprintf(print_buffer, "R_QSPI_DirectWrite Failed\r\n");
        return err;
    }

    sprintf(print_buffer, "Set QSPI driver to QSPI mode\r\n");

    /* Command byte transferred to flash-> NOW  set the QPI protocol in MCU run time */
    err = R_QSPI_SpiProtocolSet(&g_qspi_ctrl, SPI_FLASH_PROTOCOL_QPI);
    if (FSP_SUCCESS != err)
    {
        sprintf(print_buffer, "R_QSPI_SpiProtocolSet Failed\r\n");
    }

    return err;
}
/**********************************************************************************************************************
 End of function qpi_mode_set
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: qpi_init
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t qpi_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (SPI_FLASH_PROTOCOL_QPI == g_qspi_cfg.spi_protocol)
    {
        /*
         * this needs to be done since QPI is set by user in configuration
         * and it sets QPI only in MCU but not in flash device
         * so as a system (MCU + QSPI flash device) QPI mode does not get set by
         * simply calling only R_QSPI_Open in QPI mode.
         * Rather QPI mode enabling has to be done in Flash device as well
         * So opening the driver in extended SPI mode only
         * and QPI mode is enabled when qpi_mode_set sub-function is called
         */

        memcpy((spi_flash_cfg_t *)&s_spi_variant_qspi_cfg, (spi_flash_cfg_t *)&g_qspi_cfg, sizeof (spi_flash_cfg_t));

        s_spi_variant_qspi_cfg.spi_protocol = SPI_FLASH_PROTOCOL_EXTENDED_SPI;

        sprintf(print_buffer, "Initialise QSPI driver in Extended SPI mode\r\n");

        /* open QSPI with local configuration  */
        err = R_QSPI_Open(&g_qspi_ctrl, &s_spi_variant_qspi_cfg);
        if (FSP_SUCCESS != err)
        {
            sprintf(print_buffer, "R_QSPI_Open Failed\r\n");
        }
    }
    else
    {
        /* open QSPI in extended SPI mode */
        err = R_QSPI_Open(&g_qspi_ctrl, &g_qspi_cfg);
        if (FSP_SUCCESS != err)
        {
            sprintf(print_buffer, "R_QSPI_Open Failed\r\n");
        }
    }

    if (FSP_SUCCESS == err)
    {
        /* write enable for further operations */
        err = R_QSPI_DirectWrite(&g_qspi_ctrl, &(g_qspi_cfg.write_enable_command), ONE_BYTE, false);
        if (FSP_SUCCESS != err)
        {
            sprintf(print_buffer, "R_QSPI_DirectWrite Failed\r\n");
        }
        else
        {
            err = get_flash_status();
            if (FSP_SUCCESS != err)
            {
                sprintf(print_buffer, "Failed to get status for QSPI operation\r\n");
            }
        }
    }

    if (FSP_SUCCESS == err)
    {
        /*
         * write QSPI flash status register
         * This is required to make sure the device is ready for general
         * read write operation
         * This performs settings such as physical reset,WP hardware pin disable,
         * block protection lock bits clearing and enable Quad mode.
         * for more details please refer Mx25L data sheet.
         */
        uint8_t   data_sreg[SREG_SIZE]  = STATUS_REG_PAYLOAD;

        err = R_QSPI_DirectWrite(&g_qspi_ctrl, &(data_sreg[0]), SREG_SIZE, false);
        if (FSP_SUCCESS != err)
        {
            sprintf(print_buffer, "R_QSPI_DirectWrite Failed\r\n");
        }
        else
        {
            err = get_flash_status();
            if (FSP_SUCCESS != err)
            {
                sprintf(print_buffer, "Failed to get status for QSPI operation\r\n");
            }
        }
    }

    if (FSP_SUCCESS == err)
    {
        /*
         * Verifying data written to QSPI flash status register
         * Step 1: - send command byte - 0x05
         * through R_QSPI_DirectWrite with last argument set as true (to enable subsequent read)
         * Step 2 - read data through R_QSPI_DirectRead
         */
        err = R_QSPI_DirectWrite(&g_qspi_ctrl, &(g_qspi_cfg.status_command), ONE_BYTE, true);
        if (FSP_SUCCESS != err)
        {
            sprintf(print_buffer, "Failed to write the read status command\r\n");
        }
    }

    if (FSP_SUCCESS == err)
    {
        /*
         *  we should not call function get_flash_status here
         * because the CS line should not get interrupted between write read
         *
         * Also MCU <SFMCD register> is set as 0 when status register is read
         * to resume in ROM access mode hence API direct read returns error as part
         * of parameter check itself
         */
        uint8_t sreg_data = RESET_VALUE;

        err = R_QSPI_DirectRead(&g_qspi_ctrl, &sreg_data, ONE_BYTE);
        if (FSP_SUCCESS != err)
        {
            sprintf(print_buffer, "R_QSPI_DirectRead Failed\r\n");
        }
        else
        {
            /* check for status check operation here */
            err = get_flash_status();
            if (FSP_SUCCESS != err)
            {
                sprintf(print_buffer, "Failed to get status for QSPI operation\r\n");
            }
        }

        /* verify read status register data.
         * Following test will pass only for Quad mode. Edit if using Extended-SPI in FSP.*/
        if (SET_SREG_VALUE != sreg_data)
        {
            sprintf(print_buffer, "Failed to get value set in the status register \r\n");
            err = FSP_ERR_WRITE_FAILED;
        }
    }

    if (FSP_SUCCESS == err)
    {
        if (SPI_FLASH_PROTOCOL_QPI == g_qspi_cfg.spi_protocol)
        {
            /* set QPI mode in flash and MCU device */
            err = qpi_mode_set();
            if (FSP_SUCCESS != err)
            {
                sprintf(print_buffer, "qpi_mode_set failed\r\n");
            }
            else
            {
                /* Test that we can still talk to the FLASH in the selected mode */
                err = get_flash_status();
                if (FSP_SUCCESS != err)
                {
                    sprintf(print_buffer, "Failed to get status for QSPI operation\r\n");
                }
            }
        }
    }

    return err;
}
/**********************************************************************************************************************
 End of function qpi_init
 *********************************************************************************************************************/


