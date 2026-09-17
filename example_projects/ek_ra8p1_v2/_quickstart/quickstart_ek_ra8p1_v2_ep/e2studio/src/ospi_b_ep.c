/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : ospi_b_ep.c
 * Version      : 1.0
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2018 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "common_init.h"
#include "common_utils.h"
#include "ospi_b_commands.h"
#include "ospi_b_ep.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define OSPI_PERIPHERAL_COMMAND_DELAY (1000)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static fsp_err_t ospi_b_write_enable ();

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: ospi_b_write_enable
 * Description  : This functions enables write and verify the read data.
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t ospi_b_write_enable ()
{
    fsp_err_t                   err             = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer        = {RESET_VALUE};

    /* Transfer write enable command */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi0_ctrl.spi_protocol)
                    ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_SPI]
                                               : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
//    APP_ERR_RETURN(err, "R_OSPI_B_DirectTransfer API FAILED \r\n");

    /* Read Status CFG */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi0_ctrl.spi_protocol)
                    ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_SPI]
                                               : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
//    APP_ERR_RETURN(err, "R_OSPI_B_DirectTransfer API FAILED \r\n");

    /* Check Write Enable bit in Status CFG */
    if(!(0x02 & transfer.data))
    {
//        APP_ERR_RETURN(FSP_ERR_ABORTED, "Write enable FAILED\r\n");
    }
    return err;
}


/**********************************************************************************************************************
 * Function Name: ospi_b_init
 * Description  : This functions initializes OSPI module and Flash device.
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t ospi_b_init ()
{
    volatile fsp_err_t          err             = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer        = {RESET_VALUE};

    if (0 == g_ospi0_ctrl.open)
    {
        err = R_OSPI_B_Open(&g_ospi0_ctrl, &g_ospi0_cfg);
        if (FSP_SUCCESS != err)
            print_to_console ("R_OSPI_B_Open API FAILED \r\n");
    }

    err = R_OSPI_B_SpiProtocolSet(&g_ospi0_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    if (FSP_SUCCESS != err)
        print_to_console ("R_OSPI_B_SpiProtocolSet API FAILED \r\n");

    /*
       Reset flash device
       Power reset using p106 driven by peripheral
    */
    R_BSP_PinAccessEnable();
    R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_01_PIN_06, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(200, BSP_DELAY_UNITS_MICROSECONDS);
    R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_01_PIN_06, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay(200, BSP_DELAY_UNITS_MICROSECONDS);
    R_BSP_PinAccessDisable();

    R_XSPI0->LIOCTL_b.RSTCS0 = 0;
//  R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MICROSECONDS);
    vTaskDelay(100);
    R_XSPI0->LIOCTL_b.RSTCS0 = 1;
//    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MICROSECONDS);
    vTaskDelay(100);

    err = ospi_b_write_enable();
    if (FSP_SUCCESS != err)
        print_to_console ("ospi_b_write_enable FAILED \r\n");

    /* Write to CFG2 to configure SPI mode */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CFG2_SPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
        print_to_console ("R_OSPI_B_DirectTransfer API FAILED \r\n");

    err = ospi_b_write_enable();
    if (FSP_SUCCESS != err)
        print_to_console ("ospi_b_write_enable FAILED \r\n");

    /* Write to CFG2 to configure dummy cycle */
    transfer.address = 0x300;
    transfer.data = 0x7U;  // 6 dummy cycle
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
        print_to_console ("R_OSPI_B_DirectTransfer API FAILED \r\n");

    /* Read from CFG2 */
    transfer.command = OSPI_B_COMMAND_READ_CFG2_SPI;
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
        print_to_console ("R_OSPI_B_DirectTransfer API FAILED \r\n");


    if(!(2U & transfer.data))
    {
        print_to_console("Data read while reading configuration CFG failed \r\n");
    }
    return err;
}
/**********************************************************************************************************************
 End of function ospi_b_init
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: ospi_b_set_protocol_to_spi
 * Description  : This function configures ospi to extended spi mode.
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t ospi_b_set_protocol_to_spi ()
{
    fsp_err_t                   err      = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer = {RESET_VALUE};

    /* Transfer write enable command */
    err = ospi_b_write_enable();
//    APP_ERR_RETURN(err, "ospi_b_write_enable FAILED \r\n");

    /* Write to CFG to Configure SPI mode */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CFG2_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
//    APP_ERR_RETURN(err, "R_OSPI_B_DirectTransfer API FAILED \r\n");

    /* Switch OSPI module mode to SPI mode */
    err = R_OSPI_B_SpiProtocolSet(&g_ospi0_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
//    APP_ERR_RETURN(err, "R_OSPI_SpiProtocolSet API FAILED \r\n");

    return err;
}
/**********************************************************************************************************************
 End of function ospi_b_set_protocol_to_spi
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: ospi_b_set_protocol_to_opi
 * Description  : This function configures ospi to opi mode.
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t ospi_b_set_protocol_to_opi ()
{
    fsp_err_t                   err      = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer = {RESET_VALUE};

    /* Transfer write enable command */
    err = ospi_b_write_enable();
 //   APP_ERR_RETURN(err, "ospi_b_write_enable FAILED \r\n");

    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CFG2_SPI];
    transfer.address = 0x200U;
    transfer.data = 0x2U; /* DQS on STR mode */
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
 //   APP_ERR_RETURN(err, "R_OSPI_B_DirectTransfer API FAILED \r\n");

    err = ospi_b_write_enable();
 //   APP_ERR_RETURN(err, "ospi_b_write_enable FAILED \r\n");

    transfer.address = 0x300U;
    transfer.data = 0x7U;           /* 6 dummy cycles for 66M */
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
  //  APP_ERR_RETURN(err, "R_OSPI_B_DirectTransfer API FAILED \r\n");

    err = ospi_b_write_enable();
//    APP_ERR_RETURN(err, "ospi_b_write_enable FAILED \r\n");

    transfer.address = 0x00U;
#if DTR_MODE
    transfer.data = 0x2U;   /* DTR */
#else
    transfer.data = 0x1U;   /* STR */
#endif

    transfer.data = 0x2U;   /* DTR */

    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
//    APP_ERR_RETURN(err, "R_OSPI_B_DirectTransfer API FAILED \r\n");

    err = R_OSPI_B_SpiProtocolSet(&g_ospi0_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
//    APP_ERR_RETURN(err, "R_OSPI_SpiProtocolSet API FAILED \r\n");

    return err;
}
/**********************************************************************************************************************
 End of function ospi_b_set_protocol_to_opi
 *********************************************************************************************************************/
/**********************************************************************************************************************
 Private (static) functions
 *********************************************************************************************************************/
