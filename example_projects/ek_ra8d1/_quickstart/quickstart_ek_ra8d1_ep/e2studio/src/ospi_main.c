/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : ospi_main.c
 * Version      : .
 * OS           : .
 * H/W Platform : .
 * Description  : This file includes the main tutorial code.
 * Operation    : See doc\readme.txt
 * Limitations  : .
 * xxx          : Make your own other sections as needed. (e.g. describe endian)
 *********************************************************************************************************************/

#include "hal_data.h"
#include "gimp.h"
#include "board_cfg.h"

/* CHange to 1 to enable easier to debug asserts. */
#if 0
 #undef assert
 #define assert(pred)    do {if (!(pred)) {__BKPT(0);}} while (0)
#endif

/* These commands and registers are for the Infineon S28HS256 and may not match other targets. */
#define READ_SFDP_COMMAND             (0x5AU)
#define READ_SFDP_COMMAND_OPI         (0x5A5AU)
#define READ_REGISTER_COMMAND         (0x65U)
#define READ_REGISTER_COMMAND_OPI     (0x6565U)
#define WRITE_REGISTER_COMMAND        (0x71U)
#define WRITE_REGISTER_COMMAND_OPI    (0x7171U)
#define WRITE_ENABLE_COMMAND          (0x06U)
#define WRITE_ENABLE_COMMAND_OPI      (0x0606U)
#define READ_STATUS_COMMAND           (0x05U)
#define READ_STATUS_COMMAND_OPI       (0x0505U)

#define CFR2V_REGISTER_ADDRESS        (0x800003U)
#define CFR3V_REGISTER_ADDRESS        (0x800004U)
#define CFR5V_REGISTER_ADDRESS        (0x800006U)

#define CFR2V_MEMLAT_Pos              (0U)
#define CFR2V_MEMLAT_Msk              (0x0FU)

#define CFR3V_VRGLAT_Pos              (6U)
#define CFR3V_VRGLAT_Msk              (0xC0U)
#define CFR3V_UNHYSA_Pos              (3U)
#define CFR3V_UNHYSA_Msk              (0x08U)

#define CFR5V_SDRDDR_Pos              (1U)
#define CFR5V_SDRDDR_Msk              (0x02U)
#define CFR5V_OPIIT_Pos               (0U)
#define CFR5V_OPIIT_Msk               (0x01U)

#define CFR2V_WRITE_Msk               (0x00U)
#define CFR3V_WRITE_Msk               (0x00U)
#define CFR5V_WRITE_Msk               (0x40U)

#define REG_LATENCY_CODE_SPI          (0x00U)
#define REG_LATENCY_CODE_OPI          (0x03U)

#define MEM_LATENCY_CODE_SPI          (0x08U)
#define MEM_LATENCY_CODE_OPI          (0x0AU)

#define REG_DUMMY_CYCLES_SPI          (0U)
#define REG_DUMMY_CYCLES_OPI          (3U)
#define DDR_SAMPLING_WINDOW_EXTEND    (1U)

// #define REG_DUMMY_CYCLES_OPI   (6U)

#define WRITE_ENABLE_MASK             (0x02U)

#define READ_SFDP_DUMMY_CYCLES        (8U)
#define SFDP_SIGNATURE                (0x50444653U)

#define OSPI_TEST_DATA_LENGTH         (0x40U)
#define OSPI_SECTOR_SIZE              (4096U)
#define OSPI_BLOCK_SIZE               (262144U)

#define OSPI_MODE_SPI                 (CFR5V_WRITE_Msk)
#define OSPI_MODE_DOPI                (CFR5V_WRITE_Msk | CFR5V_OPIIT_Msk | CFR5V_SDRDDR_Msk)

#define OSPI_RESET_PIN                BSP_IO_PORT_01_PIN_06
#define OSPI_RESET_DELAY              (40U)

#define OSPI_GRAPHICS_OFFSET          (0x90040000)

uint32_t get_image_data(st_image_data_t ref);
uint32_t get_sub_image_data(st_image_data_t ref, uint32_t sub_image);

typedef struct st_clk_settings
{
    cgc_pll_cfg_t            pll;
    bsp_clocks_octaclk_div_t div;
} clk_settings_t;

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

static void reset_ospi_device(void);
static void wait_for_write(void);
static void write_en(bool is_dopi);
static void oclk_change(clk_settings_t const * const clock_settings);
static void transition_to_dopi(void);
static void transition_to_spi(void);

static uint8_t * const gp_ospi_cs1_img_hdr = (uint8_t *) (void *) 0x90000000 + (1 * 4096);

/* consider placing all of ospi dat into sdram */

// uint8_t unused_screens[64][DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0] BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram");
uint8_t unused_screens[64][854 * 480 * 2] BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram");

static clk_settings_t clk_50MHz =
{
    .pll              =
    {
        .source_clock = CGC_CLOCK_MAIN_OSC,
        .divider      = BSP_CLOCKS_PLL_DIV_1,
        .multiplier   = BSP_CLOCKS_PLL_MUL(40U, 0U),
        .out_div_p    = CGC_PLL_OUT_DIV_16,
        .out_div_q    = CGC_PLL_OUT_DIV_6, // Not used, set to div/6 (default)
        .out_div_r    = CGC_PLL_OUT_DIV_6, // Not used, set to div/6 (default)
    },
    .div              = (bsp_clocks_octaclk_div_t) BSP_CLOCKS_OCTA_CLOCK_DIV_1,
};

static clk_settings_t clk_200MHz =
{
    .pll              =
    {
        .source_clock = CGC_CLOCK_MAIN_OSC,
        .divider      = BSP_CLOCKS_PLL_DIV_1,
        .multiplier   = BSP_CLOCKS_PLL_MUL(40U, 0U),
        .out_div_p    = CGC_PLL_OUT_DIV_4,
        .out_div_q    = CGC_PLL_OUT_DIV_6, // Not used, set to div/6 (default)
        .out_div_r    = CGC_PLL_OUT_DIV_6, // Not used, set to div/6 (default)
    },
    .div              = (bsp_clocks_octaclk_div_t) BSP_CLOCKS_OCTA_CLOCK_DIV_1,
};

/**********************************************************************************************************************
 * Function Name: reset_ospi_device
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void reset_ospi_device (void)
{
    R_BSP_PinAccessEnable();
    R_BSP_PinCfg(OSPI_RESET_PIN,
                 ((uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT |
                  (uint32_t) IOPORT_CFG_DRIVE_HIGH |
                  (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW));

    R_BSP_PinWrite(OSPI_RESET_PIN, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay((bsp_delay_units_t) OSPI_RESET_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    R_BSP_PinWrite(OSPI_RESET_PIN, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay((bsp_delay_units_t) OSPI_RESET_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    R_BSP_PinAccessDisable();
}

/**********************************************************************************************************************
 * End of function reset_ospi_device
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: write_en
 * Description  : .
 * Argument     : is_dopi
 * Return Value : .
 *********************************************************************************************************************/
static void write_en (bool is_dopi)
{
    spi_flash_direct_transfer_t tfr =
    {
        .command        = is_dopi ? WRITE_ENABLE_COMMAND_OPI : WRITE_ENABLE_COMMAND,
        .command_length = is_dopi ? 2U : 1U,
        .address_length = 0U,
        .data_length    = 0U,
        .dummy_cycles   = 0U
    };
    fsp_err_t err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    assert(FSP_SUCCESS == err);

    tfr = (spi_flash_direct_transfer_t) {
        .command        = is_dopi ? READ_STATUS_COMMAND_OPI : READ_STATUS_COMMAND,
        .command_length = is_dopi ? 2U : 1U,
        .address_length = is_dopi ? 4U : 0U, // Address is always sent for any kind of read in DOPI
        .data_length    = 1U,
        .dummy_cycles   = is_dopi ? REG_DUMMY_CYCLES_OPI : REG_DUMMY_CYCLES_SPI,
    };
    err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    assert(FSP_SUCCESS == err);

    if ((tfr.data & WRITE_ENABLE_MASK) == 0)
    {
        __BKPT(0);
    }
}

/**********************************************************************************************************************
 * End of function write_en
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wait_for_write
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void wait_for_write (void)
{
    spi_flash_status_t status = {0};
    do
    {
        assert(FSP_SUCCESS == g_ospi0.p_api->statusGet(g_ospi0.p_ctrl, &status));
    } while (status.write_in_progress);
}

/**********************************************************************************************************************
 * End of function wait_for_write
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: oclk_change
 * Description  : .
 * Argument     : clock_settings
 * Return Value : .
 *********************************************************************************************************************/
static void oclk_change (clk_settings_t const * const clock_settings)
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
 * End of function oclk_change
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: transition_to_dopi
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void transition_to_dopi (void)
{
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_direct_transfer_t tfr =
    {
        .command        = WRITE_REGISTER_COMMAND,
        .command_length = 1U,
        .address_length = 3U,
        .data_length    = 1U,
        .dummy_cycles   = 0U
    };

    write_en(false);

    /* The chip should be modified for the new clock speed, but that doesn't appear to work correctly. */
#if 0

    /* Modify the registers for the new clock speed. */
    tfr.address = CFR2V_REGISTER_ADDRESS;
    tfr.data    = CFR2V_WRITE_Msk | ((MEM_LATENCY_CODE_OPI << CFR2V_MEMLAT_Pos) & CFR2V_MEMLAT_Msk);
    err         = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    assert(FSP_SUCCESS == err);

    tfr.address = CFR3V_REGISTER_ADDRESS;
    tfr.data    = CFR3V_WRITE_Msk | ((REG_LATENCY_CODE_OPI << CFR3V_VRGLAT_Pos) & CFR3V_VRGLAT_Msk);
    err         = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    assert(FSP_SUCCESS == err);
#endif

    /* Transition to DOPI. */
    tfr.address = CFR5V_REGISTER_ADDRESS;
    tfr.data    = OSPI_MODE_DOPI;
    err         = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    assert(FSP_SUCCESS == err);

    /* Change clock speed. */
    oclk_change(&clk_200MHz);

    /* Change the protocol mode of the driver. */
    /* DS will auto-calibrate in this call. */
    err = g_ospi0.p_api->spiProtocolSet(g_ospi0.p_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
    assert(FSP_SUCCESS == err);

    /* Read the mode register to verify it changed to DOPI */
    tfr = (spi_flash_direct_transfer_t) {
        .command        = READ_REGISTER_COMMAND_OPI,
        .command_length = 2U,
        .address        = CFR5V_REGISTER_ADDRESS,
        .address_length = 4U,
        .data           = 0U,
        .data_length    = 1U,
        .dummy_cycles   = REG_DUMMY_CYCLES_OPI
    };
    err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    assert(FSP_SUCCESS == err);
    assert(OSPI_MODE_DOPI == (tfr.data & 0xFF)); // Need to mask here because DOPI always reads 2 bytes at a time.
}

/**********************************************************************************************************************
 * End of function transition_to_dopi
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: transition_to_spi
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void transition_to_spi (void)
{
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_direct_transfer_t tfr =
    {
        .command        = WRITE_REGISTER_COMMAND_OPI,
        .command_length = 2U,
        .address_length = 4U,
        .data_length    = 1,
        .dummy_cycles   = 0U
    };;

    write_en(true);

    /* The chip should be modified for the new clock speed, but that doesn't appear to work correctly. */
#if 0

    /* Modify the registers for the new clock speed. */
    tfr.address = CFR2V_REGISTER_ADDRESS;
    tfr.data    = CFR2V_WRITE_Msk | ((MEM_LATENCY_CODE_SPI << CFR2V_MEMLAT_Pos) & CFR2V_MEMLAT_Msk);
    err         = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    assert(FSP_SUCCESS == err);

    tfr.address = CFR3V_REGISTER_ADDRESS;
    tfr.data    = CFR3V_WRITE_Msk | ((REG_LATENCY_CODE_SPI << CFR3V_VRGLAT_Pos) & CFR3V_VRGLAT_Msk);
    err         = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    assert(FSP_SUCCESS == err);
#endif

    /* Transition the flash device to SPI. */
    tfr.address = CFR5V_REGISTER_ADDRESS;
    tfr.data    = OSPI_MODE_SPI;
    err         = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    assert(FSP_SUCCESS == err);

    /* Change clock speed. */
    oclk_change(&clk_50MHz);

    /* Change the protocol mode of the driver. */
    err = g_ospi0.p_api->spiProtocolSet(g_ospi0.p_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    assert(FSP_SUCCESS == err);

    /* Read the mode register to verify it changed to SPI */
    tfr = (spi_flash_direct_transfer_t) {
        .command        = READ_REGISTER_COMMAND,
        .command_length = 1U,
        .address        = CFR5V_REGISTER_ADDRESS,
        .address_length = 3U,
        .data           = 0U,
        .data_length    = 1U,
        .dummy_cycles   = REG_DUMMY_CYCLES_SPI
    };
    err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    assert(FSP_SUCCESS == err);
    assert(OSPI_MODE_SPI == (tfr.data & 0xFF));
}

/**********************************************************************************************************************
 * End of function transition_to_spi
 *********************************************************************************************************************/

#define WRITE_BLOCK_SIZE    (64)

typedef unsigned int bool_t;

typedef struct
{
    bsp_unique_id_t uid;
    uint32_t        files;
    uint32_t        links[64];
} st_ospi_hdr_t;

static st_ospi_hdr_t g_hdr = {};

/* No security check on ospi data */

/**********************************************************************************************************************
 * Function Name: read_hdr
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
static bool_t read_hdr ()
{
    bool_t status = true;
    memcpy(&g_hdr, gp_ospi_cs1_img_hdr, sizeof(g_hdr));

    return status;
}

/**********************************************************************************************************************
 * End of function read_hdr
 *********************************************************************************************************************/

void init_ospi(void);

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: init_ospi
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void init_ospi (void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* This is here to prevent the code from running if you need to power cycle to restore the default state of the flash device.
     * The reset doesn't always work as expected. Comment out if you don't need this. */

// __BKPT(0);
    reset_ospi_device();

    err = g_ospi0.p_api->open(g_ospi0.p_ctrl, g_ospi0.p_cfg);
    assert(FSP_SUCCESS == err);

    R_XSPI->LIOCFGCS_b[g_ospi0_ctrl.channel].DDRSMPEX = DDR_SAMPLING_WINDOW_EXTEND;

    spi_flash_direct_transfer_t test_tfr =
    {
        .command        = READ_SFDP_COMMAND,
        .command_length = 1,
        .address        = 0,
        .address_length = 3,
        .data           = 0,
        .data_length    = 4,
        .dummy_cycles   = READ_SFDP_DUMMY_CYCLES
    };
    err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &test_tfr, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    assert(FSP_SUCCESS == err);

    if (SFDP_SIGNATURE != test_tfr.data)
    {
        while (true)
        {
            TURN_RED_ON;
            R_BSP_SoftwareDelay((bsp_delay_units_t) OSPI_RESET_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
            TURN_RED_OFF;
            R_BSP_SoftwareDelay((bsp_delay_units_t) OSPI_RESET_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
        }
    }

    R_BSP_SoftwareDelay((bsp_delay_units_t) 200, BSP_DELAY_UNITS_MILLISECONDS);
    transition_to_dopi();
    R_BSP_SoftwareDelay((bsp_delay_units_t) 200, BSP_DELAY_UNITS_MILLISECONDS);

    read_hdr();
}

/**********************************************************************************************************************
 * End of function init_ospi
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_sub_image_data
 * Description  : .
 * Arguments    : ref
 *              : sub_image
 * Return Value : .
 *********************************************************************************************************************/
uint32_t get_sub_image_data (st_image_data_t ref, uint32_t sub_image)
{
    uint32_t   chosen_one = OSPI_GRAPHICS_OFFSET + g_hdr.links[ref];                                  // part 1 index into ospi
    uint32_t * p_co       = (uint32_t *) (OSPI_GRAPHICS_OFFSET + g_hdr.links[ref] + (8 * sub_image)); // part 2 offset to sub_image

    chosen_one += (*p_co);                                                                            // part 1 + 2

    return chosen_one;
}

/**********************************************************************************************************************
 * End of function get_sub_image_data
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_image_data
 * Description  : .
 * Argument     : ref
 * Return Value : .
 *********************************************************************************************************************/
uint32_t get_image_data (st_image_data_t ref)
{
    uint32_t chosen_one = OSPI_GRAPHICS_OFFSET + g_hdr.links[ref];

    return chosen_one;
}

/**********************************************************************************************************************
 * End of function get_image_data
 *********************************************************************************************************************/
