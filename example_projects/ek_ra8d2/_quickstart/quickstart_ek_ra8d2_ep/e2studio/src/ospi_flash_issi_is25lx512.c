/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : ospi_flash_issi_is25lx512.c
 * Version      : 1.0
 * Description  : OSPI flash ISSI_IS25LX512 device driver file
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.10.2025 1.00     First Release
 *********************************************************************************************************************/

#include <stdint.h>

#include "board_hw_cfg.h"
#include "r_typedefs.h"
#include "common_init.h"
#include "common_data.h"
#include "r_console.h"

#include "jlink_console.h"
#include "ospi_flash_issi_is25lx512.h"

#include "display_thread_entry.h"

#include "ospi_b_ep.h"

#define FLASH_DEVICE_SIZE_BYTES     (1024 * 1024 * 64)          // 64 MB flash device
#define MAX_BLOCK_SIZE              (64 * 1024)                 // maximum block size in bytes

#define WRITE_BLOCK_SIZE            (64)                        // the size of a block when writing to the flash
#define TEST_BLOCKS                 (64)

/* The RA8D2 OSPI flash is ISSI IS25LX512M-JHLE */
/* The device is 512M bit (64M Byte) */
#define OSPI_CLOCK_SOURCE                           (BSP_CLOCKS_SOURCE_CLOCK_PLL2Q)

/* These commands and registers are for the ISSI IS25LX512M and may not match other targets */
#define READ_SFDP_COMMAND                           (0x5AU)
#define READ_SFDP_COMMAND_OPI                       (0x5AU)
#define READ_REGISTER_COMMAND                       (0x71U)
#define READ_REGISTER_COMMAND_OPI                   (0x71U)
#define WRITE_REGISTER_COMMAND                      (0x81U)
#define WRITE_REGISTER_COMMAND_OPI                  (0x81U)
#define WRITE_VOLATILE_CONFIGURATION_REGISTER       (0x81u)
#define WRITE_VOLATILE_CONFIGURATION_REGISTER_OPI   (0x8181u)
#define WRITE_ENABLE_COMMAND                        (0x06U)
#define WRITE_ENABLE_COMMAND_OPI                    (0x0606U)
#define READ_STATUS_COMMAND                         (0x05U)
#define READ_STATUS_COMMAND_OPI                     (0x0505U)
#define READ_FLAG_STATUS_COMMAND_OPI                (0x7070U)
#define READ_VOLATILE_CONFIGURATION_REGISTER        (0x85u)
#define READ_VOLATILE_CONFIGURATION_REGISTER_OPI    (0x8585u)

#define REG_LATENCY_CODE_SPI                        (0x00U)
#define REG_LATENCY_CODE_OPI                        (0x03U)

#define REG_DUMMY_CYCLES_SPI                        (0U)
#define READ_STATUS_DUMMY_CYCLES_OPI                (8U)

#define WRITE_ENABLE_MASK                           (0x02U)

#define READ_SFDP_DUMMY_CYCLES                      (8U)
#define SFDP_SIGNATURE                              (0x50444653U)

#define OSPI_TEST_DATA_LENGTH                       (0x40U)
#define OSPI_SECTOR_SIZE                            (4096U)
#define OSPI_BLOCK_SIZE                             (262144U)

#define OSPI_RESET_PIN                              (BSP_IO_PORT_12_PIN_07)
#define OSPI_RESET_DELAY                            (50U)

#define EXTENDED_SPI_MODE                           (0xFFu)
#define EXTENDED_SPI_MODE_NO_DQS                    (0xDFu)
#define OCTAL_DDR_MODE                              (0xE7u)
#define OCTAL_DDR_MODE_NO_DQS                       (0xC7u)

#define AUTO_CALIBRATION_ADDRESS                    (0x90600000)

#define OSPI_FLASH_ADDRESS                          (AUTO_CALIBRATION_ADDRESS + 0x20000)

static void reset_ospi_device(void);
static void write_en(bool is_dopi);
static void transition_to_dopi(void);
static fsp_err_t write_test_opi(uint32_t num_blocks);
static fsp_err_t read_test_opi(uint32_t num_blocks);
static void wait_for_write(void);
static void ospi_b_change_omclk(bsp_clocks_octaclk_div_t divider);
static void set_clock_speed_66MHz(void);
static void set_clock_speed_133MHz(void);

static uint8_t * const gp_ospi_cs1 = (uint8_t *) OSPI_FLASH_ADDRESS;

static uint8_t read_block[MAX_BLOCK_SIZE];
static uint8_t g_test_data[OSPI_TEST_DATA_LENGTH];
static uint32_t s_test_remain           = WRITE_BLOCK_SIZE;
static uint32_t s_test_current_block    = 0;
static uint32_t s_test_current_sub      = 0;
static size_t   s_data_size             = TEST_BLOCKS * WRITE_BLOCK_SIZE;
static uint32_t g_counter = 0;

/* Function declarations */

static const uint8_t s_auto_calibration_pattern[] = {0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x08, 0x00, 0x00, 0xF7, 0xFF, 0x00, 0x08, 0xF7, 0x00, 0xF7};
static uint8_t * const s_auto_calibration_address = (uint8_t *) AUTO_CALIBRATION_ADDRESS;

/**********************************************************************************************************************
 * Function Name: ospi_b_change_omclk
 * Description  : Changes the OCTACLK divider; assumes PLL2 is being used as the source for OCTACLK
 *              : divider New clock divider to switch to
 * Return Value : None
 *********************************************************************************************************************/
static void ospi_b_change_omclk(bsp_clocks_octaclk_div_t divider)
{
    /* ensure clock source (PLL2 in this example) is running before changing the OCTACLK frequency */
    /* PLL2R should be set up to give a clock speed of 266.67 MHz */
    bsp_octaclk_settings_t octaclk_settings;
    octaclk_settings.source_clock = OSPI_CLOCK_SOURCE;
    octaclk_settings.divider = divider;
    R_BSP_OctaclkUpdate(&octaclk_settings);
}
/**********************************************************************************************************************
 End of function ospi_b_change_omclk
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: set_clock_speed_66MHz
 * Description  : set the OSPI flash bit clock to 66 MHz (133 MHz OCTACLK)
 * Return Value : None
 *********************************************************************************************************************/
static void set_clock_speed_66MHz(void)
{
    /* change the OCTACLK to 133 MHz (to give a device clock of 66 MHz) */
    /* OCTACLK source is configured with 266 MHz input */
    ospi_b_change_omclk(BSP_CLOCKS_OCTACLK_DIV_2);
}
/**********************************************************************************************************************
 End of function set_clock_speed_66MHz
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: set_clock_speed_133MHz
 * Description  : set the OSPI flash bit clock to 133 MHz (266 MHz OCTACLK)
 * Return Value : None
 *********************************************************************************************************************/
static void set_clock_speed_133MHz(void)
{
    /* change the OCTACLK to 266 MHz (to give a device clock of 133 MHz) */
    /* OCTACLK source is configured with 266 MHz input */
    ospi_b_change_omclk(BSP_CLOCKS_OCTACLK_DIV_1);
}
/**********************************************************************************************************************
 End of function set_clock_speed_133MHz
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: ospi_performance_test
 * Description  : write data to the OSPI flash, then read it. Use a timer to measure the durations
 * Argument     : data_size -  the data size to write and read
 * Argument     : ospi_performance_write_result - time taken for the write
 * Argument     : ospi_performance_read_result - time taken for the read
 * Return Value : None
 *********************************************************************************************************************/
void ospi_performance_test(uint32_t data_size, uint32_t *ospi_performance_write_result, uint32_t *ospi_performance_read_result)
{
     fsp_err_t err;
     uint32_t erase_size = OSPI_SECTOR_SIZE;
     uint32_t erase_blocks;

     //board_cfg_switch_funct_on(SW4_OSPI_OE);

     set_clock_speed_66MHz();

     err = ospi_b_init();

     if (FSP_SUCCESS != err)
     {
         printf_colour("\r\nFailed to initialise the OSPI flash\r\n");
         return;
     }

     err = ospi_flash_open();

     if (FSP_SUCCESS != err)
     {
         printf_colour("\r\nFailed to open the OSPI flash driver\r\n");
         return;
     }

     /* calculate the number of blocks to erase */
     erase_blocks = data_size / erase_size;

     if (erase_blocks * erase_size < data_size)
     {
         erase_blocks++;
     }

     /* erase the flash data area before we write to it */
     for (uint32_t block = 0; block < erase_blocks; block++)
     {
         err = g_ospi0.p_api->erase (g_ospi0.p_ctrl, (uint8_t *) (gp_ospi_cs1 + (erase_size * block)), erase_size);
         assert(FSP_SUCCESS == err);
         wait_for_write();
     }

     transition_to_dopi();

     set_clock_speed_133MHz();

     printf_colour("\r\nWriting the text block to external Octo-SPI flash memory ...\r\n");

     write_test_opi(data_size / WRITE_BLOCK_SIZE);

     *ospi_performance_write_result = g_counter;

     printf_colour("\r\nWriting to flash completed\r\n");

     printf_colour("\r\nReading the text block from external Octo-SPI flash memory ...\r\n");

     read_test_opi(data_size / WRITE_BLOCK_SIZE);

     *ospi_performance_read_result = g_counter;

     printf_colour("\r\nReading from flash completed\r\n");

     R_OSPI_B_Close (g_ospi0.p_ctrl);
}
/**********************************************************************************************************************
 End of function ospi_performance_test
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: write_test_opi
 * Description  : write data to the OSPI flash and use a timer to time the write duration
 * Argument     : num_blocks - the number of blocks
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

    /* use critical section so that interrupts don't affect the time measurements */
    FSP_CRITICAL_SECTION_DEFINE;
    FSP_CRITICAL_SECTION_ENTER;

    /* write the test data in OPI mode */
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

        /* only time the write function that includes the wait_for_write */
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

    /* scale block_size to bytes by multiplying by the block size */
    block_size *= WRITE_BLOCK_SIZE;

    s_data_size = TEST_BLOCKS * WRITE_BLOCK_SIZE;

    err = R_GPT_Open(g_memory_performance.p_ctrl, g_memory_performance.p_cfg);

    memset(read_block, 0, block_size);

    FSP_CRITICAL_SECTION_DEFINE;
    FSP_CRITICAL_SECTION_ENTER;

    R_GPT_Reset(g_memory_performance.p_ctrl);

    /* Read the data and time the read operation */
    ptr = gp_ospi_cs1;

    R_GPT_Start(g_memory_performance.p_ctrl);

    memcpy(read_block, ptr, block_size);

    R_GPT_StatusGet(g_memory_performance.p_ctrl, &timer_status);

    g_counter = timer_status.counter;
    FSP_CRITICAL_SECTION_EXIT;

    R_GPT_Close(g_memory_performance.p_ctrl);

    ptr = read_block;

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
        printf_colour("Failed to verify %lu bytes. First verification failure index %lu\r\n", errors, failing_index);
    }

    return (err);
}
/**********************************************************************************************************************
 End of function read_test_opi
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: reset_ospi_device
 * Description  : reset the OSPI flash device
 * Return Value : None
 *********************************************************************************************************************/
static void reset_ospi_device(void)
{
    /* configure the pin as GPIO */
    R_IOPORT_PinCfg(&g_ioport_ctrl, OSPI_RESET_PIN, (uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW);

    /* generate reset pulse */
    R_IOPORT_PinWrite(g_ioport.p_ctrl, OSPI_RESET_PIN, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(OSPI_RESET_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    R_IOPORT_PinWrite(g_ioport.p_ctrl, OSPI_RESET_PIN, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay(OSPI_RESET_DELAY, BSP_DELAY_UNITS_MILLISECONDS);

    /* set to OSPI peripheral mode */
    R_IOPORT_PinCfg(&g_ioport_ctrl, OSPI_RESET_PIN, (uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_OSPI);

    /* Reset flash device */
    R_XSPI0->LIOCTL_b.RSTCS0 = 0;
    R_BSP_SoftwareDelay(OSPI_RESET_DELAY, BSP_DELAY_UNITS_MICROSECONDS);
    R_XSPI0->LIOCTL_b.RSTCS0 = 1;
    R_BSP_SoftwareDelay(OSPI_RESET_DELAY, BSP_DELAY_UNITS_MICROSECONDS);
}
/**********************************************************************************************************************
 End of function reset_ospi_device
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: write_en
 * Description  : send a write enable commend to the OSPI flash device
 * Return Value : None
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
        .address_length = is_dopi ? 0U : 0U,    // Address is always sent for any kind of read in DOPI
        .data_length = 1U,
        .dummy_cycles = is_dopi ? READ_STATUS_DUMMY_CYCLES_OPI : REG_DUMMY_CYCLES_SPI,
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
 * Function Name: transition_to_dopi
 * Description  : transition the device from SPI mode to DOPI mode
 * Return Value : None
 *********************************************************************************************************************/
static void transition_to_dopi(void)
{
    volatile fsp_err_t err = FSP_SUCCESS;

    spi_flash_direct_transfer_t tfr;
    spi_flash_direct_transfer_t tfr_read;

    write_en(false);

    /* Write to Volatile Configuration Register 0 */
    tfr.command = WRITE_VOLATILE_CONFIGURATION_REGISTER;
    tfr.command_length = 1U;
    tfr.address = 0;
    tfr.address_length = 3U;
    tfr.data_length = 1U;
    tfr.data = OCTAL_DDR_MODE;
    tfr.dummy_cycles = 0U;
    err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    assert(FSP_SUCCESS == err);

    /* Device should now be in DOPI mode */

    /* Change the protocol mode of the driver */
    /* DS will auto-calibrate in this call */
    err = g_ospi0.p_api->spiProtocolSet(g_ospi0.p_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
    assert(FSP_SUCCESS == err);

    tfr_read.command = READ_VOLATILE_CONFIGURATION_REGISTER;
    tfr_read.command_length = 1U;
    tfr_read.address = 0;
    tfr_read.address_length = 4U;
    tfr_read.data = 0U;
    tfr_read.data_length = 1U;
    tfr_read.dummy_cycles = 8u;

    err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr_read, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    assert(FSP_SUCCESS == err);
    //printf_colour("DOPI volatile configuration register 0 read result = 0x%02x\r\n", tfr_read.data & 0xFF);

    /* Need to mask here because DOPI always reads 2 bytes at a time */
    if ((tfr_read.data & 0xFF) == OCTAL_DDR_MODE)
    {
        //printf_colour("[GREEN]Transition to DOPI mode verified[WHITE]\r\n");
    }
}
/**********************************************************************************************************************
 End of function transition_to_dopi
 *********************************************************************************************************************/

fsp_err_t ospi_flash_open_in_dopi(void);

fsp_err_t ospi_flash_open_in_dopi(void)
{
    fsp_err_t err = FSP_ERR_NOT_OPEN;

    err = ospi_flash_open();

    if (err == FSP_SUCCESS)
    {
        transition_to_dopi();
        if (err == FSP_SUCCESS)
        {
            set_clock_speed_133MHz();
        }
    }

    return(err);
}

/**********************************************************************************************************************
 * Function Name: ospi_flash_open
 * Description  : open the OSPI driver and read the OSPI flash SFDP signature
 * Return Value : FSP_SUCCESS on success or an error code on failure
 *********************************************************************************************************************/
/* static */ fsp_err_t ospi_flash_open(void)
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

    err = g_ospi0.p_api->open(g_ospi0.p_ctrl, g_ospi0.p_cfg);

    err = ospi_b_init();

    if (err == FSP_SUCCESS)
    {
        err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &test_tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);

        if (SFDP_SIGNATURE != test_tfr.data)
        {
            err = FSP_ERR_NOT_INITIALIZED;
        }
    }

    /* Check for auto-calibration pattern */
    if (memcmp(s_auto_calibration_pattern, s_auto_calibration_address, sizeof(s_auto_calibration_pattern)) == 0)
    {
        /* Erase the first block/sector */
        err = g_ospi0.p_api->erase(g_ospi0.p_ctrl, s_auto_calibration_address, OSPI_SECTOR_SIZE);
        wait_for_write();

        /* write the auto calibration data */
        err = g_ospi0.p_api->write(g_ospi0.p_ctrl, s_auto_calibration_pattern, (uint8_t *) s_auto_calibration_address, sizeof(s_auto_calibration_pattern));

        if (FSP_SUCCESS == err)
        {
            wait_for_write();
        }

    }

    return (err);
}
/**********************************************************************************************************************
 End of function ospi_flash_open
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wait_for_write
 * Description  : Read the status register until it indicates that the write has completed
 * Return Value : None
 *********************************************************************************************************************/
static void wait_for_write(void)
{
    spi_flash_status_t status = { 0 };

    do
    {
        assert(FSP_SUCCESS == g_ospi0.p_api->statusGet (g_ospi0.p_ctrl, &status));
    }
    while (status.write_in_progress);
}
/**********************************************************************************************************************
 End of function wait_for_write
 *********************************************************************************************************************/
