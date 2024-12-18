
/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/

#include "common_util.h"
#include "ospi_operations.h"
#include "runtime_encryption.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define OSPI_B_TIME_ERASE_4K                (100000U)        //  Type 4KB sector is 95 KBps -> Type 0.042s
#define OSPI_B_TIME_WRITE                   (10000U)
#define OSPI_B_TIME_UNIT                    (BSP_DELAY_UNITS_MICROSECONDS)
#define OSPI_B_TIME_RESET_PULSE             (1000U)          //  Type 500us
#define OSPI_B_TIME_RESET_SETUP             (2U)             //  Type 50ns
#define OSPI_WRITE_UNIT                     (64)

/***********************************************************************************************************************
 * Private global variables and functions
 ************************************************************************************************************************/

static spi_flash_direct_transfer_t direct_command_preset[5] =
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

static void ospi_b_wait_operation (uint32_t timeout);


/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/

/*********************************************************************************************************************
 *  @brief       This function initialize the OSPI to operates in 8D-8D-8D mode
 *  @param[in]   none
 *  @retval      none
 **********************************************************************************************************************/
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
    if((FSP_SUCCESS != err) || ((direct_command_preset[1].data & 0x00000002) == 0x0)){
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

}

/*********************************************************************************************************************
 *  @brief       This function wait for the OSPI operation to finish
 *  @param[in]   timeout value
 *  @retval      none
 **********************************************************************************************************************/
static void ospi_b_wait_operation (uint32_t timeout)
{
    fsp_err_t          err    = FSP_SUCCESS;
    spi_flash_status_t status = {RESET_VALUE};

    status.write_in_progress = true;
    while (status.write_in_progress)
    {
        /* Get device status */
        err = R_OSPI_B_StatusGet(&g_ospi_b_ctrl, &status);
        handle_error(err, APP_OSPI_STATUSGET);

        if(RESET_VALUE == timeout)
        {
            handle_error(err, APP_OSPI_TIMEOUT);
        }
        R_BSP_SoftwareDelay(1, OSPI_B_TIME_UNIT);
        timeout --;
    }

}
/*********************************************************************************************************************
 *  @brief       This function erase the OSPI flash and then write the encryption result to the OSPI area
 *  @param[in]   none
 *  @retval      none
 **********************************************************************************************************************/
void write_encrypted_data_to_ospi(void)
{
    fsp_err_t          err    = FSP_SUCCESS;
    volatile uint32_t temp =(uint32_t) &(__ospi_device_1_start__);

    err = R_OSPI_B_Erase(&g_ospi_b_ctrl, (uint8_t *)temp, 0x1000);
    handle_error(err, APP_OSPI_ERASE);

    /* Wait till operation completes */
    ospi_b_wait_operation(OSPI_B_TIME_ERASE_4K);

    /* Write data to flash device, write unit is 64 bytes */
    for (uint32_t i=0; i < (sizeof(encrypted_data)/OSPI_WRITE_UNIT); i++)
    {
        err = R_OSPI_B_Write(&g_ospi_b_ctrl, (uint8_t *)encrypted_data + i*OSPI_WRITE_UNIT, (uint8_t *)temp + i*OSPI_WRITE_UNIT, sizeof(encrypted_data));
        handle_error(err, APP_OSPI_WRITE);
        /* Wait until write operation completes */
        ospi_b_wait_operation(OSPI_B_TIME_WRITE);
    }

}


