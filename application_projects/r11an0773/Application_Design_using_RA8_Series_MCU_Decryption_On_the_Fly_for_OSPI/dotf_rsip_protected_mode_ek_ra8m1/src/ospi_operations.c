/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <ospi_operations.h>
#include "hal_data.h"
#include "common_util.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define FIRST_CALCULATION_ITERATIONS            (30)                /* number of calculation iterations in the first test try */
#define SECOND_CALCULATION_ITERATIONS           (40)                /* number of calculation iterations in the second test try */
#define EXPECTED_RESULT_FIRST_ITERATIONS        (1346269)           /* expected result of Fibonacci calculation fir the first test try */
#define EXPECTED_RESULT_SECOND_ITERATIONS       (165580141)         /* expected result of Fibonacci calculation fir the second test try */

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

spi_flash_direct_transfer_t direct_command_preset[5] =
{
 [0] = {
  .command        = 0x06, // 1S1S1S - Write enable command
  .command_length = 1U,
 },
 [1] = {
  .command        = 0x05, // 1S1S1S - Read status register 1 (including write enable bit) command
  .command_length = 1U,
  .data_length    = 1U
 },
 [2] = {
  .command        = 0x71, // 1S1S1S - Write status register n command
  .command_length = 1U,
  .address        = 0x00800003, // CFR2V: Configuration Register 2 (volatile)
  .address_length = 3U,
  .data           = 0x84,
  .data_length    = 1U,
  .dummy_cycles   = 0U
 },
 [3] = {
  .command        = 0x71, // 1S1S1S - Write status register n command
  .command_length = 1U,
  .address        = 0x00800006, // CFR5V: Configuration Register 5 (volatile)
  .address_length = 3U,
  .data           = 0x43,
  .data_length    = 1U,
 },
 [4] = {
  .command        = 0x6565, // 8D8D8D - Write status register n command
  .command_length = 2U,
  .address        = 0x00800006, // CFR5V: Configuration Register 5 (volatile)
  .address_length = 4U,
  .data_length    = 1U,
 }
};

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static void ospi_test_wait_until_wip(void);
static uint32_t fibonacci(uint32_t num);
static uint32_t fibonacci2(uint32_t num);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

static volatile uint32_t fibonacci_result;


static uint32_t fibonacci(uint32_t num) __attribute__((noinline)) __attribute__((aligned(4096))) BSP_PLACE_IN_SECTION(".ospi_device_1");
static uint32_t fibonacci2(uint32_t num) __attribute__((noinline)) __attribute__((aligned(4096))) BSP_PLACE_IN_SECTION(".code_in_ospi_device_1");

/*********************************************************************************************************************
 *  fibonacci calculation
 *  @param[IN]   uint32_t num: number of iterations
 *  @retval      calculation result
***********************************************************************************************************************/
uint32_t fibonacci(uint32_t num)
{
    uint32_t fn1 = 0;
    uint32_t fn2 = 1;
    uint32_t fn3 = 0;

    for(uint32_t i=0; i<num; i++)
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
uint32_t fibonacci2(uint32_t num)
{
    uint32_t fn1 = 0;
    uint32_t fn2 = 1;
    uint32_t fn3 = 0;

    for(uint32_t i=0; i<num; i++)
    {
        fn3 = fn2 + fn1;
        fn1 = fn2;
        fn2 = fn3;
    }

    return fn3;
}



/*********************************************************************************************************************
 *  wait function for the OSPI operation
 *  @param[IN]   none
 *  @retval      none
***********************************************************************************************************************/
static void ospi_test_wait_until_wip(void)
{
    spi_flash_status_t status;
    status.write_in_progress = true;
    uint32_t timeout = UINT32_MAX;
    while ((status.write_in_progress) && (--timeout > 0))
    {
        R_OSPI_B_StatusGet (&g_ospi_b_ctrl, &status);
    }

    if (0 == timeout)
    {
        while(1);//handle_error(APP_OSPI_OPERATION_TIMEOUT, APP_OSPI_OPERATION_TIMEOUT);
    }
}
/*********************************************************************************************************************
 *  OSPI Initialization
 *  @param[IN]   none
 *  @retval      none
***********************************************************************************************************************/
void ospi_init(void)
{
    fsp_err_t err = 0;
    err = R_OSPI_B_Open(&g_ospi_b_ctrl, &g_ospi_b_cfg);
      handle_error(err, APP_OSPI_OPEN);


      /* Transfer write enable command */
      err = R_OSPI_B_DirectTransfer (&g_ospi_b_ctrl, &direct_command_preset[0], SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
      handle_error(err, APP_OSPI_DIRECT_TRANSFER);

      /* Transfer write enable register read command */
      err = R_OSPI_B_DirectTransfer (&g_ospi_b_ctrl, &direct_command_preset[1], SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
      if((FSP_SUCCESS != err) || ((direct_command_preset[1].data & 0x00000002) == 0x0))
      {
          handle_error(err, APP_OSPI_DIRECT_TRANSFER);
      }

      /* Write CFR5V register (volatile) to switch the protocol to 8D-8D-8D */
      err = R_OSPI_B_DirectTransfer (&g_ospi_b_ctrl, &direct_command_preset[3], SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
      handle_error(err, APP_OSPI_DIRECT_TRANSFER);

      /* Change the protocol to 8D-8D-8D in MCU side */
      err = R_OSPI_B_SpiProtocolSet(&g_ospi_b_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
      handle_error(err, APP_OSPI_SPI_PROTOCOL_SET);

      /* Read CFR5V register to confirm if the protocol is switched to 8D-8D-8D */
      err = R_OSPI_B_DirectTransfer (&g_ospi_b_ctrl, &direct_command_preset[4], SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
      if((FSP_SUCCESS != err) || ((direct_command_preset[4].data != 0x43) != 0x0))
      {
          handle_error(err, APP_OSPI_DIRECT_TRANSFER);
      }


      ospi_test_wait_until_wip();
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
       if(fibonacci_result != EXPECTED_RESULT_FIRST_ITERATIONS)
       {
          handle_error(APP_FIBONACCI_CALCULATION, APP_FIBONACCI_CALCULATION);
       }

       fibonacci_result = fibonacci(SECOND_CALCULATION_ITERATIONS);
       if(fibonacci_result != EXPECTED_RESULT_SECOND_ITERATIONS)
       {
          handle_error(APP_FIBONACCI_CALCULATION, APP_FIBONACCI_CALCULATION);
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
       if(fibonacci_result != EXPECTED_RESULT_FIRST_ITERATIONS)
       {
           handle_error(APP_FIBONACCI_CALCULATION, APP_FIBONACCI_CALCULATION);
       }

       fibonacci_result = fibonacci2(SECOND_CALCULATION_ITERATIONS);
       if(fibonacci_result != EXPECTED_RESULT_SECOND_ITERATIONS)
       {
           handle_error(APP_FIBONACCI_CALCULATION, APP_FIBONACCI_CALCULATION);
       }

}
