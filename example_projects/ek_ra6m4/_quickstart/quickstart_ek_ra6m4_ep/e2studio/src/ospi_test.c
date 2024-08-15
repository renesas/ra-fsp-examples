/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "ospi_commands.h"
#include "ospi_test.h"

#define OSPI_WEN_BIT_MASK                           (0x02U)
#define OSPI_WIP_BIT_MASK                           (0x01U)
#define OSPI_TEST_PAGES_IN_BLOCK                    (0x100U)
#define OSPI_TEST_PAGE_SIZE                         (256U)
#define OSPI_TEST_BLOCK_SIZE                        (65536U)
/* Key code for writing PRCR register. */
#define BSP_PRV_PRCR_KEY                          (0xA500U)
#define BSP_PRV_PRCR_UNLOCK                       ((BSP_PRV_PRCR_KEY) | 0x3U)
#define BSP_PRV_PRCR_LOCK                         ((BSP_PRV_PRCR_KEY) | 0x0U)

const uint8_t page[OSPI_TEST_PAGE_SIZE] =
{
    0x33, 0x33, 0x33, 0x33, 0xBB, 0xFE, 0xEE, 0xF1, 0x34, 0x34, 0x34, 0x34, 0xBC, 0xF6, 0xE0, 0xF2,
    0x35, 0x35, 0x35, 0x35, 0xBD, 0xFF, 0xEE, 0xF1, 0x36, 0x36, 0x16, 0x36, 0xCC, 0xF5, 0xEF, 0xF2,
    0x37, 0x37, 0x37, 0x37, 0xBF, 0x01, 0xEE, 0xF1, 0x38, 0x38, 0x38, 0x38, 0xBA, 0x1F, 0x6F, 0x93,
    0x38, 0x38, 0x38, 0x38, 0xC0, 0x02, 0xEE, 0xF1, 0x39, 0x39, 0x19, 0x39, 0x0C, 0xF1, 0x4F, 0xF4,
    0x33, 0x33, 0x33, 0x33, 0xBB, 0xF2, 0xEE, 0xF2, 0x34, 0x34, 0x14, 0x34, 0xBC, 0xFF, 0xEF, 0x92,
    0x35, 0x35, 0x35, 0x35, 0xB1, 0xFF, 0x3E, 0xF3, 0x36, 0x36, 0x36, 0x96, 0x0C, 0xF4, 0xEF, 0xF2,
    0x37, 0x37, 0x37, 0xA7, 0xBF, 0x01, 0x4E, 0xF4, 0x38, 0x38, 0x38, 0x38, 0xCA, 0x1F, 0xDF, 0x93,
    0x38, 0x38, 0x38, 0x38, 0xC0, 0x02, 0xE1, 0xF5, 0x39, 0x39, 0x09, 0x39, 0xBC, 0xF1, 0x4F, 0xF4,
    0x33, 0x33, 0x33, 0x33, 0xBB, 0xFE, 0xEE, 0xF1, 0x34, 0x34, 0x34, 0x34, 0xBC, 0xF3, 0xEF, 0xF2,
    0x35, 0x35, 0x35, 0x35, 0xBD, 0xFF, 0xEE, 0x41, 0x36, 0x36, 0x36, 0x36, 0x1C, 0xFF, 0xEF, 0xF2,
    0x37, 0x37, 0x37, 0x37, 0xBF, 0x01, 0xEE, 0x51, 0x38, 0x38, 0x38, 0x38, 0x2A, 0x1F, 0x6F, 0x93,
    0x38, 0x38, 0x38, 0x38, 0xC0, 0x02, 0xE1, 0x71, 0x39, 0x39, 0x09, 0x39, 0xBC, 0xF1, 0x4F, 0xF4,
    0x33, 0x33, 0x33, 0x33, 0xBB, 0xFE, 0xEE, 0x81, 0x34, 0x34, 0x34, 0x34, 0xBC, 0xFF, 0xEF, 0xF2,
    0x35, 0x35, 0x35, 0x35, 0xBD, 0xFF, 0xEE, 0xF1, 0x36, 0x36, 0x36, 0x36, 0xBC, 0xFF, 0xEF, 0xF2,
    0x37, 0x37, 0x37, 0x37, 0xBF, 0x01, 0xEE, 0x81, 0x38, 0x38, 0x38, 0x38, 0xBA, 0x1F, 0x6F, 0x93,
    0x38, 0x38, 0x38, 0x38, 0xC0, 0x02, 0xEE, 0x01, 0x39, 0x39, 0x09, 0x39, 0x0C, 0xF1, 0x4F, 0xF4,
};

/* Large block in RAM for performance measure  64K */
uint8_t perf_read[ OSPI_TEST_PAGE_SIZE * OSPI_TEST_PAGES_IN_BLOCK];

static void configure_dopi_ospi(void);
static void configure_ospi_clocks_dopi(void);
static uint32_t write_dopi_ospi(uint32_t data_size);
static uint32_t read_dopi_ospi(uint32_t data_size);
static void erase_dopi_ospi(void);
static void ospi_test_wait_until_wip (void);
static void configure_spi_ospi(void);
static void configure_ospi_clocks_spi(void);


/* Define the units to be used with the software delay function */
const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;

/* Set the blink frequency (must be <= bsp_delay_units */
const uint32_t freq_in_hz = 2;

/* Calculate the delay in terms of bsp_delay_units */
const uint32_t delay = bsp_delay_units / freq_in_hz;

#define HIGH_SPEED_MODE (1)

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
    fsp_err_t err;
    uint32_t i =  1;

    if (R_CGC_Open (g_cgc.p_ctrl, g_cgc.p_cfg) != FSP_SUCCESS)
    {
        __asm("bkpt");
    }
    
    while (i)
    {

        err = R_OSPI_Open(g_ospi.p_ctrl, g_ospi.p_cfg);
        if (FSP_SUCCESS != err)
        {
            __asm("bkpt");
        }


#if HIGH_SPEED_MODE
        configure_dopi_ospi();
        ospi_test_wait_until_wip();
#endif

        *ospi_performance_write_result = write_dopi_ospi(data_size);
        ospi_test_wait_until_wip();

        *ospi_performance_read_result = read_dopi_ospi(data_size);
        ospi_test_wait_until_wip();

        erase_dopi_ospi();
        ospi_test_wait_until_wip();

#if HIGH_SPEED_MODE
        configure_spi_ospi();
        ospi_test_wait_until_wip();
#endif
        err = R_OSPI_Close(g_ospi.p_ctrl);
        if (FSP_SUCCESS != err)
        {
            __asm("bkpt");
        }

        i--;
    }
    
}
/**********************************************************************************************************************
 End of function ospi_performance_test
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: configure_dopi_ospi
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void configure_dopi_ospi(void)
{
    fsp_err_t err;

    /* Write Enable */
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_ENABLE_SPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }

    /* Read Status Register */
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_STATUS_SPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }
    if (2 != (ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_STATUS_SPI_MODE].data & 2U))
    {
        __asm("bkpt");
    }

    /* Write CR2 and disable DQS */
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_SPI_MODE].address = 0x200;
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_SPI_MODE].data    = 0x0U; /* DOS: DQS on STR mode */
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_SPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }

    /* Read CR2 and check if DQS is disabled */
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_SPI_MODE].address = 0x200;
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_SPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }
    if (0U != (ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_SPI_MODE].data & 0x2U))
    {
        __asm("bkpt");
    }

    /* Write Enable */
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_ENABLE_SPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }

    /* Read Status Register */
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_STATUS_SPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }
    if (2U != (ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_STATUS_SPI_MODE].data & 2U))
    {
        __asm("bkpt");
    }

    /* Change dummy cycles according to requested frequency */
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_SPI_MODE].address = 0x300;
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_SPI_MODE].data    = 5U; /* 10 dummy cycles */
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_SPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }

    /* Read CR2 and check if dummy cycles are correctly updated */
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_SPI_MODE].address = 0x300;
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_SPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }
    if (5U !=  ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_SPI_MODE].data)
    {
        __asm("bkpt");
    }

    /* Write Enable */
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_ENABLE_SPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }

    /* Read Status Register */
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_STATUS_SPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }
    if (2 != (ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_STATUS_SPI_MODE].data))
    {
        __asm("bkpt");
    }

#ifdef BURST_MODE_32
    /* Burst mode Register */
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_BURST_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }
#endif

    /* The OctaFlash chip is in SPI mode. Change DOPI mode */
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_SPI_MODE].address = 0x0U;

    /* Requested OPI mode */
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_SPI_MODE].data    = SPI_FLASH_PROTOCOL_DOPI - 2U;
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_SPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }

    configure_ospi_clocks_dopi();

    /* Transition the OSPI peripheral to DOPI mode. */
    if (R_OSPI_SpiProtocolSet (g_ospi.p_ctrl, SPI_FLASH_PROTOCOL_DOPI) != FSP_SUCCESS)
    {
        __asm("bkpt");
    }

    /* Verify that the chip is in requested OPI mode */
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_OPI_MODE].address = 0x0U;
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_OPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }
#if 0
    if((SPI_FLASH_PROTOCOL_DOPI - 2U) != ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_OPI_MODE].data)
    {
        __asm("bkpt");
    }
#endif
}

static void configure_spi_ospi(void)
{
    fsp_err_t err;

    /* Write Enable */
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_ENABLE_OPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }

    /* Read Status Register */
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_STATUS_OPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }
    if (2U != (ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_STATUS_OPI_MODE].data & 2U))
    {
        __asm("bkpt");
    }

    /* Change dummy cycles to 0 for SPI */
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_OPI_MODE].address = 0x300;
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_OPI_MODE].data    = 0U;
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_OPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }

    /* Read CR2 and check if dummy cycles are correctly updated */
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_OPI_MODE].address = 0x300;
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_OPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }
    if (0U !=  (ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_OPI_MODE].data & 0xFFU))
    {
        __asm("bkpt");
    }

    /* Write Enable */
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_ENABLE_OPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }

    /* Read Status Register */
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_STATUS_OPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }
    if (2 != (ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_STATUS_OPI_MODE].data & 2U))
    {
        __asm("bkpt");
    }

    /* The OctaFlash chip is in SPI mode. Change DOPI mode */
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_OPI_MODE].address = 0x0U;
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_OPI_MODE].data    = SPI_FLASH_PROTOCOL_EXTENDED_SPI; /* SPI */
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_WRITE_CR2_OPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }

    configure_ospi_clocks_spi();

    /* Transition the OSPI peripheral to SPI mode. */
    if (R_OSPI_SpiProtocolSet (g_ospi.p_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI) != FSP_SUCCESS)
    {
        __asm("bkpt");
    }

    /* Need to add this as a part of R_OSPI_SpiProtocolSet eventually.
     * This is needed to revert to SPI as dummy cycles = 0 */
    R_OSPI->MDLR = 0;

    /* Verify that the chip is in SPI mode */
    ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_SPI_MODE].address = 0x0U;
    err = R_OSPI_DirectTransfer(g_ospi.p_ctrl,
                                &ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_SPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        __asm("bkpt");
    }
    if (SPI_FLASH_PROTOCOL_EXTENDED_SPI != ospi_test_direct_transfer[OSPI_TEST_COMMAND_READ_CR2_SPI_MODE].data)
    {
        __asm("bkpt");
    }
}
/**********************************************************************************************************************
 End of function configure_spi_ospi
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: configure_ospi_clocks_spi
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void configure_ospi_clocks_spi(void)
{
    /* Stop PLL2 */
    if (R_CGC_ClockStop (g_cgc.p_ctrl, CGC_CLOCK_PLL2) != FSP_SUCCESS)
    {
        __asm("bkpt");
    }
    cgc_pll_cfg_t pll2_cfg =
    {
        .source_clock = CGC_CLOCK_MAIN_OSC,
        .divider      = BSP_CLOCKS_PLL_DIV_4,
        .multiplier   = BSP_CLOCKS_PLL_MUL(20,0),
    };

    /* Start PLL2 with appropriate configurations */
    if (R_CGC_ClockStart (g_cgc.p_ctrl, CGC_CLOCK_PLL2, &pll2_cfg) != FSP_SUCCESS)
    {
        __asm("bkpt");
    }

    /* Main oscillator and PLL2 are running. */
    if (0U !=  R_SYSTEM->MOSCCR_b.MOSTP)
    {
        __asm("bkpt");
    }
    if (0U != R_SYSTEM->PLL2CR_b.PLL2STP)
    {
        __asm("bkpt");
    }

    /* Unlock CGC and LPM protection registers. */
    R_SYSTEM->PRCR = (uint16_t) BSP_PRV_PRCR_UNLOCK;

    /* Request to change the OCTASPI Clock. */
    R_SYSTEM->OCTACKCR_b.OCTACKSREQ = 1;

    /* Wait for the clock to be stopped. */
    FSP_HARDWARE_REGISTER_WAIT(R_SYSTEM->OCTACKCR_b.OCTACKSRDY, 1U);

    /* Write the settings. */
    R_SYSTEM->OCTACKDIVCR = BSP_CLOCKS_OCTA_CLOCK_DIV_4;       /* /2 */
    R_SYSTEM->OCTACKCR    = BSP_CLOCKS_SOURCE_CLOCK_PLL2 | R_SYSTEM_OCTACKCR_OCTACKSREQ_Msk; /* PLL2 */

    /* Start the OCTASPI Clock. */
    R_SYSTEM->OCTACKCR = BSP_CLOCKS_SOURCE_CLOCK_PLL2;

    /* Wait for the OCTASPI Clock to be started. */
    FSP_HARDWARE_REGISTER_WAIT(R_SYSTEM->OCTACKCR_b.OCTACKSRDY, 0U);

    /* Lock CGC and LPM protection registers. */
    R_SYSTEM->PRCR = (uint16_t) BSP_PRV_PRCR_LOCK;
}
/**********************************************************************************************************************
 End of function configure_ospi_clocks_spi
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: configure_ospi_clocks_dopi
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void configure_ospi_clocks_dopi(void)
{
    /* Stop PLL2 */
    if (R_CGC_ClockStop (g_cgc.p_ctrl, CGC_CLOCK_PLL2) != FSP_SUCCESS)
    {
        __asm("bkpt");
    }
    cgc_pll_cfg_t pll2_cfg =
    {
        .source_clock = CGC_CLOCK_MAIN_OSC,
        .divider      = BSP_CLOCKS_PLL_DIV_2,
        .multiplier   = BSP_CLOCKS_PLL_MUL(20,0),
    };

    /* Start PLL2 with appropriate configurations */
    if (R_CGC_ClockStart (g_cgc.p_ctrl, CGC_CLOCK_PLL2, &pll2_cfg) != FSP_SUCCESS)
    {
        __asm("bkpt");
    }

    /* Main oscillator and PLL2 are running. */
    if (0U !=  R_SYSTEM->MOSCCR_b.MOSTP)
    {
        __asm("bkpt");
    }
    if (0U != R_SYSTEM->PLL2CR_b.PLL2STP)
    {
        __asm("bkpt");
    }

    /* Unlock CGC and LPM protection registers. */
    R_SYSTEM->PRCR = (uint16_t) BSP_PRV_PRCR_UNLOCK;

    /* Request to change the OCTASPI Clock. */
    R_SYSTEM->OCTACKCR_b.OCTACKSREQ = 1;

    /* Wait for the clock to be stopped. */
    FSP_HARDWARE_REGISTER_WAIT(R_SYSTEM->OCTACKCR_b.OCTACKSRDY, 1U);

    /* Write the settings. */
    R_SYSTEM->OCTACKDIVCR = BSP_CLOCKS_OCTA_CLOCK_DIV_2;       /* /1 */
    R_SYSTEM->OCTACKCR    = BSP_CLOCKS_SOURCE_CLOCK_PLL2 | R_SYSTEM_OCTACKCR_OCTACKSREQ_Msk; /* PLL2 */

    /* Start the OCTASPI Clock. */
    R_SYSTEM->OCTACKCR = BSP_CLOCKS_SOURCE_CLOCK_PLL2;

    /* Wait for the OCTASPI Clock to be started. */
    FSP_HARDWARE_REGISTER_WAIT(R_SYSTEM->OCTACKCR_b.OCTACKSRDY, 0U);

    /* Lock CGC and LPM protection registers. */
    R_SYSTEM->PRCR = (uint16_t) BSP_PRV_PRCR_LOCK;
}
/**********************************************************************************************************************
 End of function configure_ospi_clocks_dopi
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: write_dopi_ospi
 * Description  : .
 * Argument     : data_size
 * Return Value : .
 *********************************************************************************************************************/
static uint32_t write_dopi_ospi(uint32_t data_size)
{

    fsp_err_t err;
    uint32_t test_var;
    uint8_t * dest = (uint8_t *) 0x68000000;
    timer_status_t status = {};
    int32_t number_of_pages;

    R_GPT_Open(g_memory_performance.p_ctrl, g_memory_performance.p_cfg);


    number_of_pages = data_size * ( 1024 / OSPI_TEST_PAGE_SIZE);

    /* Write 32 blocks worth of data starting at Block 0
     * Block size = 64K, i.e. 2 blocks = 128K of data
     * check this comment....... */


    for (test_var = 0; test_var < number_of_pages; test_var++)
    {
    /* Performance measured around this loop will be slightly lower due to branches and test write-in-progress
         * The actual throughput should be measured with direct debugger downloads (not supported by SEGGER yet)*/

        R_GPT_Start(g_memory_performance.p_ctrl);
        err = R_OSPI_Write(g_ospi.p_ctrl, page, dest, OSPI_TEST_PAGE_SIZE);
        if (FSP_SUCCESS != err)
        {
            __asm("bkpt");
        }
        ospi_test_wait_until_wip();
        dest += OSPI_TEST_PAGE_SIZE;
        R_GPT_Stop(g_memory_performance.p_ctrl);
        vTaskDelay(1U);
    }

    R_GPT_StatusGet(g_memory_performance.p_ctrl, &status);
    R_GPT_Reset(g_memory_performance.p_ctrl);
    R_GPT_Close(g_memory_performance.p_ctrl);

    return (status.counter);
}
/**********************************************************************************************************************
 End of function write_dopi_ospi
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: read_dopi_ospi
 * Description  : .
 * Argument     : data_size
 * Return Value : .
 *********************************************************************************************************************/
static uint32_t read_dopi_ospi(uint32_t data_size)
{
    uint32_t * src = (uint32_t *)0x68000000;
    uint32_t * dest = (uint32_t *) perf_read;

    uint32_t  number_of_blocks;
    volatile uint32_t data;

    /* Full bus access, transfer 4 bytes at a time */
    timer_status_t status = {};

    R_GPT_Open(g_memory_performance.p_ctrl, g_memory_performance.p_cfg);

#ifdef READ_PAGE_BY_PAGE

    /* convert to number of mem pages */
    number_of_blocks = data_size * ( 1024 / OSPI_TEST_PAGE_SIZE);

    while(number_of_blocks--)
    {
        /* convert page size to number of 32bit reads */
        data = (OSPI_TEST_PAGE_SIZE) / 4U;

        /* Start timer */
        R_GPT_Start(g_memory_performance.p_ctrl);

#ifdef MANUAL_READ
        while (data)
        {
            *dest = *src;
            dest++;
            src++;
            data--;
        }
#endif

#define READ_MEMCPY
#ifdef READ_MEMCPY
        memcpy ( dest, src, OSPI_TEST_PAGE_SIZE / 4);
        src += OSPI_TEST_PAGE_SIZE / 4;
        dest += OSPI_TEST_PAGE_SIZE / 4;

        /* Stop timer */
        R_GPT_Stop(g_memory_performance.p_ctrl);
        vTaskDelay(1U);
#endif

    }
#endif /* READ_PAGE_BY_PAGE */

#define READ_AS_SINGLE_BLOCK
#ifdef READ_AS_SINGLE_BLOCK
    /* Start timer */
    R_GPT_Start(g_memory_performance.p_ctrl);

    R_OSPI_XipEnter(g_ospi.p_ctrl);
        memcpy ( dest, src, (data_size * 1024) / 4);
    R_OSPI_XipExit(g_ospi.p_ctrl);

    /* Stop timer */
    R_GPT_Stop(g_memory_performance.p_ctrl);
#endif

    R_GPT_StatusGet(g_memory_performance.p_ctrl, &status);
    R_GPT_Reset(g_memory_performance.p_ctrl);
    R_GPT_Close(g_memory_performance.p_ctrl);

    return (status.counter);
}
/**********************************************************************************************************************
 End of function read_dopi_ospi
 *********************************************************************************************************************/


#if 0
static void read_dopi_ospi(void)
{
    uint32_t * src = (uint32_t *)0x68000000;
    uint32_t * dest = (uint32_t *) perf_read;
    uint32_t data = (2 * OSPI_TEST_PAGES_IN_BLOCK * OSPI_TEST_PAGE_SIZE) / 4U;
    /* Full bus access, transfer 4 bytes at a time */
    while (data)
    {
        *dest = *src;
        dest++;
        src++;
        data--;
    }
}
#endif

/**********************************************************************************************************************
 * Function Name: erase_dopi_ospi
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void erase_dopi_ospi(void)
{
    uint8_t * dest = (uint8_t *) 0x68000000;

    /* Erase block 0 */
    if(FSP_SUCCESS != R_OSPI_Erase(g_ospi.p_ctrl, dest, OSPI_TEST_BLOCK_SIZE))
    {
        __asm("bkpt");
    }
    ospi_test_wait_until_wip();

    /* Erase Block 1 */
    dest = (uint8_t *) 0x68010000;
    if (FSP_SUCCESS != R_OSPI_Erase(g_ospi.p_ctrl, dest, OSPI_TEST_BLOCK_SIZE))
    {
        __asm("bkpt");
    }
    ospi_test_wait_until_wip();
}
/**********************************************************************************************************************
 End of function erase_dopi_ospi
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ospi_test_wait_until_wip
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void ospi_test_wait_until_wip (void)
{
    spi_flash_status_t status;
    status.write_in_progress = true;
    uint32_t timeout = UINT32_MAX;
    while ((status.write_in_progress) && ( --timeout > 0))
    {
        if (R_OSPI_StatusGet (g_ospi.p_ctrl, &status) != FSP_SUCCESS)
        {
            __asm("bkpt");
        }
    }

    if (!(timeout > 0))
    {
        __asm("bkpt");
    }
}
/**********************************************************************************************************************
 End of function ospi_test_wait_until_wip
 *********************************************************************************************************************/

