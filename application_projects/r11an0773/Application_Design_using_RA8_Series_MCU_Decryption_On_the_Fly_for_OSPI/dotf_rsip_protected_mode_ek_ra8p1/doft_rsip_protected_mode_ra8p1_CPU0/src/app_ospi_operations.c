/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "app_common_util.h"
#include "app_ospi_operations.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define FIRST_CALCULATION_ITERATIONS            (30)                /* number of calculation iterations in the first test try */
#define SECOND_CALCULATION_ITERATIONS           (40)                /* number of calculation iterations in the second test try */
#define EXPECTED_RESULT_FIRST_ITERATIONS        (1346269)           /* expected result of Fibonacci calculation fir the first test try */
#define EXPECTED_RESULT_SECOND_ITERATIONS       (165580141)         /* expected result of Fibonacci calculation fir the second test try */

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static fsp_err_t ospi_b_write_enable(void);

static uint32_t fibonacci(uint32_t num);
static uint32_t fibonacci2(uint32_t num);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static volatile uint32_t fibonacci_result;

static uint32_t fibonacci(uint32_t num) __attribute__((noinline)) __attribute__((aligned(4096)))   BSP_PLACE_IN_SECTION(".ospi0_cs1_code");
static uint32_t fibonacci2(uint32_t num) __attribute__((noinline)) __attribute__((aligned(4096)))  BSP_PLACE_IN_SECTION(".plaintext_cpu0_s");

static const spi_flash_direct_transfer_t g_ospi_b_direct_transfer[OSPI_B_MODE_MAX][OSPI_B_TRANSFER_MAX] =
{
     [OSPI_B_MODE_SPI] =
     {
        [OSPI_B_TRANSFER_WRITE_ENABLE] =
        {
            .command = OSPI_B_COMMAND_WRITE_ENABLE_SPI,
            .command_length = OSPI_B_COMMAND_LENGTH_SPI,
            .address = OSPI_B_ADDRESS_DUMMY,
            .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
            .dummy_cycles = OSPI_B_DUMMY_CYCLE_WRITE_SPI,
            .data = OSPI_B_DATA_DUMMY,
            .data_length = OSPI_B_DATA_LENGTH_ZERO
        },

        [OSPI_B_TRANSFER_WRITE_WRCR2_300H] =
        {
            .command = OSPI_B_COMMAND_WCR2_SPI,
            .command_length = OSPI_B_COMMAND_LENGTH_SPI,
            .address = OSPI_B_ADDRESS_CR2_300H_REGISTER,
            .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
            .dummy_cycles = OSPI_B_DUMMY_CYCLE_WRITE_SPI,
            .data = OSPI_B_DATA_DUMMY,
            .data_length = OSPI_B_DATA_LENGTH_ONE
        },

        [OSPI_B_TRANSFER_WRITE_WRCR2_000H] =
        {
            .command = OSPI_B_COMMAND_WCR2_SPI,
            .command_length = OSPI_B_COMMAND_LENGTH_SPI,
            .address = OSPI_B_ADDRESS_CR2_000H_REGISTER,
            .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
            .dummy_cycles = OSPI_B_DUMMY_CYCLE_WRITE_SPI,
            .data = OSPI_B_DATA_DUMMY,
            .data_length = OSPI_B_DATA_LENGTH_ONE
        },

        [OSPI_B_TRANSFER_READ_STATUS] =
        {
            .command = OSPI_B_COMMAND_READ_STATUS_SPI,
            .command_length = OSPI_B_COMMAND_LENGTH_SPI,
            .address = OSPI_B_ADDRESS_DUMMY,
            .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
            .dummy_cycles = OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI,
            .data = OSPI_B_DATA_DUMMY,
            .data_length = OSPI_B_DATA_LENGTH_ONE
        },

        [OSPI_B_TRANSFER_READ_WRCR2_300H] =
        {
            .command = OSPI_B_COMMAND_RCR2_SPI,
            .command_length = OSPI_B_COMMAND_LENGTH_SPI,
            .address = OSPI_B_ADDRESS_CR2_300H_REGISTER,
            .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
            .dummy_cycles = OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI,
            .data = OSPI_B_DATA_DUMMY,
            .data_length = OSPI_B_DATA_LENGTH_ONE
        },

        [OSPI_B_TRANSFER_READ_WRCR2_000H] =
        {
            .command = OSPI_B_COMMAND_RCR2_SPI,
            .command_length = OSPI_B_COMMAND_LENGTH_SPI,
            .address = OSPI_B_ADDRESS_CR2_000H_REGISTER,
            .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
            .dummy_cycles = OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI,
            .data = OSPI_B_DATA_DUMMY,
            .data_length = OSPI_B_DATA_LENGTH_ONE
        }
     },

     [OSPI_B_MODE_OPI] =
     {
        [OSPI_B_TRANSFER_WRITE_ENABLE] =
        {
            .command = OSPI_B_COMMAND_WRITE_ENABLE_OPI,
            .command_length = OSPI_B_COMMAND_LENGTH_OPI,
            .address = OSPI_B_ADDRESS_DUMMY,
            .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
            .dummy_cycles = OSPI_B_DUMMY_CYCLE_WRITE_SPI,
            .data = OSPI_B_DATA_DUMMY,
            .data_length = OSPI_B_DATA_LENGTH_ZERO
        },

        [OSPI_B_TRANSFER_WRITE_WRCR2_300H] =
        {
            .command = OSPI_B_COMMAND_WCR2_OPI,
            .command_length = OSPI_B_COMMAND_LENGTH_OPI,
            .address = OSPI_B_ADDRESS_CR2_300H_REGISTER,
            .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
            .dummy_cycles = OSPI_B_DUMMY_CYCLE_WRITE_OPI,
            .data = OSPI_B_DATA_DUMMY,
            .data_length = OSPI_B_DATA_LENGTH_ONE
        },

        [OSPI_B_TRANSFER_WRITE_WRCR2_000H] =
        {
            .command = OSPI_B_COMMAND_WCR2_OPI,
            .command_length = OSPI_B_COMMAND_LENGTH_OPI,
            .address = OSPI_B_ADDRESS_CR2_000H_REGISTER,
            .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
            .dummy_cycles = OSPI_B_DUMMY_CYCLE_WRITE_OPI,
            .data = OSPI_B_DATA_DUMMY,
            .data_length = OSPI_B_DATA_LENGTH_ONE
        },

        [OSPI_B_TRANSFER_READ_STATUS] =
        {
            .command = OSPI_B_COMMAND_READ_STATUS_OPI,
            .command_length = OSPI_B_COMMAND_LENGTH_OPI,
            .address = OSPI_B_ADDRESS_DUMMY,
            .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
            .dummy_cycles = OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI,
            .data = OSPI_B_DATA_DUMMY,
            .data_length = OSPI_B_DATA_LENGTH_ONE
        },

        [OSPI_B_TRANSFER_READ_WRCR2_300H] =
        {
            .command = OSPI_B_COMMAND_RCR2_OPI,
            .command_length = OSPI_B_COMMAND_LENGTH_OPI,
            .address = OSPI_B_ADDRESS_CR2_300H_REGISTER,
            .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
            .dummy_cycles = OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI,
            .data = OSPI_B_DATA_DUMMY,
            .data_length = OSPI_B_DATA_LENGTH_ONE
        },

        [OSPI_B_TRANSFER_READ_WRCR2_000H] =
        {
            .command = OSPI_B_COMMAND_RCR2_OPI,
            .command_length = OSPI_B_COMMAND_LENGTH_OPI,
            .address = OSPI_B_ADDRESS_CR2_000H_REGISTER,
            .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
            .dummy_cycles = OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI,
            .data = OSPI_B_DATA_DUMMY,
            .data_length = OSPI_B_DATA_LENGTH_ONE
        }
     },
};

/*********************************************************************************************************************
 *  fibonacci calculation
 *  @param[IN]   uint32_t num: number of iterations
 *  @retval      calculation result
***********************************************************************************************************************/
static uint32_t fibonacci(uint32_t num)
{
    uint32_t fn1 = 0;
    uint32_t fn2 = 1;
    uint32_t fn3 = 0;

    for (uint32_t i = 0; i< num; i++)
    {
        fn3 = fn2 + fn1;
        fn1 = fn2;
        fn2 = fn3;
    }

    return fn3;
}

/*********************************************************************************************************************
 *  fibonacci calculation
 *  @param[IN]   uint32_t num: number of iterations
 *  @retval      calculation result
***********************************************************************************************************************/
static uint32_t fibonacci2(uint32_t num)
{
    uint32_t fn1 = 0;
    uint32_t fn2 = 1;
    uint32_t fn3 = 0;

    for (uint32_t i = 0; i< num; i++)
    {
        fn3 = fn2 + fn1;
        fn1 = fn2;
        fn2 = fn3;
    }

    return fn3;
}

static fsp_err_t ospi_b_write_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    spi_flash_direct_transfer_t transfer =
    {
        .command        = RESET_VALUE,
        .command_length = RESET_VALUE,
        .address        = RESET_VALUE,
        .address_length = RESET_VALUE,
        .dummy_cycles   = RESET_VALUE,
        .data           = RESET_VALUE,
        .data_length    = RESET_VALUE,
    };

    /* Transfer write enable command */
    if (g_ospi_b_ctrl.spi_protocol == SPI_FLASH_PROTOCOL_EXTENDED_SPI)
    {
        transfer =  g_ospi_b_direct_transfer[OSPI_B_MODE_SPI][OSPI_B_TRANSFER_WRITE_ENABLE];
    }
    else
    {
        transfer =  g_ospi_b_direct_transfer[OSPI_B_MODE_OPI][OSPI_B_TRANSFER_WRITE_ENABLE];
    }

    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    app_handle_error(err, APP_OSPI_DIRECT_TRANSFER);
    OSPI_B_ERR_RETURN(err, return err);

    /* Read Status Register */
    if (g_ospi_b_ctrl.spi_protocol == SPI_FLASH_PROTOCOL_EXTENDED_SPI)
    {
        transfer = g_ospi_b_direct_transfer[OSPI_B_MODE_SPI][OSPI_B_TRANSFER_READ_STATUS];
    }
    else
    {
        transfer = g_ospi_b_direct_transfer[OSPI_B_MODE_OPI][OSPI_B_TRANSFER_READ_STATUS];
    }

    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    app_handle_error(err, APP_OSPI_DIRECT_TRANSFER);
    OSPI_B_ERR_RETURN(err, return err);

    if ((transfer.data & OSPI_B_WEL_BIT_MASK) != OSPI_B_WEL_BIT_MASK)
    {
        app_handle_error(APP_ERR_COMMON, APP_OSPI_WRITE_ENABLE);
        OSPI_B_ERR_RETURN(FSP_ERR_WRITE_FAILED, return FSP_ERR_WRITE_FAILED);
    }

    return err;
}

fsp_err_t ospi_b_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer =
    {
        .command        = RESET_VALUE,
        .command_length = RESET_VALUE,
        .address        = RESET_VALUE,
        .address_length = RESET_VALUE,
        .dummy_cycles   = RESET_VALUE,
        .data           = RESET_VALUE,
        .data_length    = RESET_VALUE,
    };

    err = R_OSPI_B_Open(&g_ospi_b_ctrl, &g_ospi_b_cfg);
    app_handle_error(err, APP_OSPI_OPEN);
    OSPI_B_ERR_RETURN(err, return err);

    err = R_OSPI_B_SpiProtocolSet(&g_ospi_b_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    app_handle_error(err, APP_OSPI_SPI_PROTOCOL_SET);
    OSPI_B_ERR_RETURN(err, return err);

    /* Reset flash device by driving OM_RESET pin */
    R_XSPI0->LIOCTL_b.RSTCS0 = 0;
    R_BSP_SoftwareDelay(OSPI_B_TIME_RESET_PULSE, OSPI_B_TIME_UNIT);
    R_XSPI0->LIOCTL_b.RSTCS0 = 1;
    R_BSP_SoftwareDelay(OSPI_B_TIME_RESET_SETUP, OSPI_B_TIME_UNIT);

    err = ospi_b_write_enable();
    app_handle_error(err, APP_OSPI_WRITE_ENABLE);
    OSPI_B_ERR_RETURN(err, return err);

    transfer = g_ospi_b_direct_transfer[OSPI_B_MODE_SPI][OSPI_B_TRANSFER_WRITE_WRCR2_300H];
    transfer.data = OSPI_B_DATA_SET_WRCR2_300H;
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    app_handle_error(err, APP_OSPI_DIRECT_TRANSFER);
    OSPI_B_ERR_RETURN(err, return err);

    /* Read back and verify CR2 register data */
    transfer = g_ospi_b_direct_transfer[OSPI_B_MODE_SPI][OSPI_B_TRANSFER_READ_WRCR2_300H];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    app_handle_error(err, APP_OSPI_DIRECT_TRANSFER);
    OSPI_B_ERR_RETURN(err, return err);

    if ((uint8_t)transfer.data != OSPI_B_DATA_SET_WRCR2_300H)
    {
        app_handle_error(APP_ERR_COMMON, APP_OSPI_READ);
        OSPI_B_ERR_RETURN(FSP_ERR_WRITE_FAILED, return err);
    }

    return err;
}

/*********************************************************************************************************************
 *  Execute Encrypted Function from OSPI
 *  @param[IN]   none
 *  @retval      none
***********************************************************************************************************************/
void execute_encrypted_func_ospi(void)
{
    /* Run encrypted code from external memory */
   fibonacci_result = fibonacci(FIRST_CALCULATION_ITERATIONS);
   if (fibonacci_result != EXPECTED_RESULT_FIRST_ITERATIONS)
   {
      app_handle_error(APP_FIBONACCI_CALCULATION, APP_FIBONACCI_CALCULATION);
   }

   fibonacci_result = fibonacci(SECOND_CALCULATION_ITERATIONS);
   if (fibonacci_result != EXPECTED_RESULT_SECOND_ITERATIONS)
   {
       app_handle_error(APP_FIBONACCI_CALCULATION, APP_FIBONACCI_CALCULATION);
   }
}

/*********************************************************************************************************************
 *  Execute Plaintext Function from OSPI
 *  @param[IN]   none
 *  @retval      none
***********************************************************************************************************************/
void execute_plaintext_fun_ospi(void)
{
    /* Run plaintext code from external memory */
    fibonacci_result = fibonacci2(FIRST_CALCULATION_ITERATIONS);
    if (fibonacci_result != EXPECTED_RESULT_FIRST_ITERATIONS)
    {
       app_handle_error(APP_FIBONACCI_CALCULATION, APP_FIBONACCI_CALCULATION);
    }

    fibonacci_result = fibonacci2(SECOND_CALCULATION_ITERATIONS);
    if (fibonacci_result != EXPECTED_RESULT_SECOND_ITERATIONS)
    {
        app_handle_error(APP_FIBONACCI_CALCULATION, APP_FIBONACCI_CALCULATION);
    }
}
