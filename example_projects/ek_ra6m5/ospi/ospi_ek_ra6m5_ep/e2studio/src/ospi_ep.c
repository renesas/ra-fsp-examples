/***********************************************************************************************************************
 * File Name    : ospi_ep.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "ospi_commands.h"
#include "ospi_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup ospi_ep.c
 * @{
 **********************************************************************************************************************/

/*Global variables*/
spi_flash_cfg_t     g_loc_ospi_cfg;
ospi_extended_cfg_t g_loc_ospi_extnd_cfg;
extern spi_flash_direct_transfer_t        g_ospi_direct_transfer[OSPI_COMMAND_MAX];
/*Data variable for blank check*/
uint8_t             g_erase_data[DEVICE_SECTOR_SIZE];
/*Data to write on flash*/
const uint8_t       write_data[] =   {0x80, 0xB4, 0x83, 0xB0,
                                      0x00, 0xAF, 0x78, 0x60,
                                      0x7B, 0x68, 0x01, 0x33,
                                      0x18, 0x46, 0x0C, 0x37,
                                      0xBD, 0x46, 0x5D, 0xF8,
                                      0x04, 0x7B, 0x70, 0x47};

/*Private functions */
static fsp_err_t ospi_opi_to_spi_mode_transition (void);
static fsp_err_t ospi_spi_to_opi_mode_transition (ospi_instance_ctrl_t  * p_instance_ctrl,spi_flash_protocol_t    mode);
static fsp_err_t direct_read_write(spi_flash_ctrl_t * p_ctrl, spi_flash_direct_transfer_t * const p_transfer , spi_flash_direct_transfer_dir_t  direction);
static fsp_err_t erase_and_blankcheck(uint8_t * const p_device_address);
static void      wait_operation(void);

/*******************************************************************************************************************//**
 * @brief       This functions initializes OSPI module.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful open of OSPI module
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t ospi_init(void)
{
    fsp_err_t err                = FSP_SUCCESS;

    /*Configuration setup of Extended SPI configuration into local config*/
    ospi_extended_cfg_t* textd = NULL;
    textd = (void *)g_ospi_cfg.p_extend;
    g_loc_ospi_extnd_cfg = *textd;
    g_loc_ospi_cfg =  g_ospi_cfg;
    g_loc_ospi_cfg.p_erase_command_list = &spi_erase_command_list[INITIAL_INDEX];
    g_loc_ospi_cfg.p_extend = &g_loc_ospi_extnd_cfg;

    /*Open Octa Flash device in Extended SPI Mode */
    err = R_OSPI_Open(&g_ospi_ctrl, &g_loc_ospi_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n** R_OSPI_Open API FAILED **\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions enables write and verify the read data.
 * @param[IN]   p_instance_ctrl              Control instance
 * @param[IN]   mode                         SPI mode
 * @retval      FSP_SUCCESS                  Upon successful read operation
 * @retval      FSP_ERR_ABORTED              Upon incorrect read data.
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t ospi_write_enable_and_verify (ospi_instance_ctrl_t * p_instance_ctrl, uint8_t mode)
{
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_direct_transfer_t g_ospi_direct_transfer_stack;

    /*check for OSPI mode*/
    g_ospi_direct_transfer_stack = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == mode) ? g_ospi_direct_transfer[OSPI_COMMAND_WRITE_ENABLE_SPI_MODE]
                                                                                                  : g_ospi_direct_transfer[OSPI_COMMAND_WRITE_ENABLE_OPI_MODE];

    /* Write Enable */
    err = direct_read_write(p_instance_ctrl, &g_ospi_direct_transfer_stack, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n** R_OSPI_DirectTransfer API FAILED while Write Enable**\r\n");
        return err;
    }

    /* check for OSPI mode */
    g_ospi_direct_transfer_stack = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == mode) ? g_ospi_direct_transfer[OSPI_COMMAND_READ_STATUS_SPI_MODE]
                                                                                                  : g_ospi_direct_transfer[OSPI_COMMAND_READ_STATUS_OPI_MODE];

    /* Read Status Register */
    err = direct_read_write(p_instance_ctrl, &g_ospi_direct_transfer_stack, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n** R_OSPI_DirectTransfer API FAILED while Reading back status register**\r\n");
        return err;
    }

    /* Check for data read by ospi */
    if(OSPI_WEN_BIT_MASK != (g_ospi_direct_transfer_stack.data & OSPI_WEN_BIT_MASK))
    {
        APP_ERR_PRINT ("\r\n** Data read while write enable is incorrect**\r\n");
        return FSP_ERR_ABORTED;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function configures ospi from opi to extended spi mode.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful transition from opi to spi operating mode.
 * @retval      FSP_ERR_ABORTED              Upon incorrect read data.
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t ospi_opi_to_spi_mode_transition (void)
{
    fsp_err_t    err                  = FSP_SUCCESS;
    g_loc_ospi_cfg.p_erase_command_list = &spi_erase_command_list[RESET_VALUE];
    g_loc_ospi_cfg.spi_protocol         = SPI_FLASH_PROTOCOL_EXTENDED_SPI;

    /* Write Enable */
    err = ospi_write_enable_and_verify(&g_ospi_ctrl, OSPI_MODE_OPI);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** OSPI Write Enable and Verify failed in opi mode**\r\n");
        return err;
    }

    /*Write configuration register in OPI mode*/
    g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_OPI_MODE].address = OSPI_CR2_ADDRESS_HEX_0;
    g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_OPI_MODE].data    = OSPI_CR2_DATA_HEX_00;

    err = direct_read_write(&g_ospi_ctrl, &g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_OPI_MODE],
                            SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_OSPI_DirectTransfer failed while writing Write configuration register in OPI mode **\r\n");
        return err;
    }

    /* Transition the OPI peripheral to SPI mode */
    err = R_OSPI_SpiProtocolSet(&g_ospi_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_OSPI_SpiProtocolSet API failed **\r\n");
        return err;
    }

    /*Close the device*/
    err = R_OSPI_Close(&g_ospi_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_OSPI_Close API failed **\r\n");
        return err;
    }

    /* Open with new configuration */
    err = R_OSPI_Open(&g_ospi_ctrl, &g_loc_ospi_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_OSPI_Open API failed **\r\n");
        return err;
    }

    /*Set the OSPI in extended SPI mode */
    err = R_OSPI_SpiProtocolSet(&g_ospi_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** OSPI Write Enable and Verify failed **\r\n");
        return err;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function configures ospi from spi to opi operating mode.
 * @param[IN]   p_instance_ctrl              Control instance
 * @param[IN]   mode                         SPI mode
 * @retval      FSP_SUCCESS                  Upon successful transition from spi to opi operating mode
 * @retval      FSP_ERR_ABORTED              Upon incorrect read data.
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t ospi_spi_to_opi_mode_transition (ospi_instance_ctrl_t  * p_instance_ctrl, spi_flash_protocol_t    mode)
{
    fsp_err_t    err                  = FSP_SUCCESS;
    g_loc_ospi_cfg.p_erase_command_list = &opi_erase_command_list[RESET_VALUE];

    /*Check for mode selected in OPI mode */
    if (mode == SPI_FLASH_PROTOCOL_SOPI)
    {
        /*copy mode to local config*/
        g_loc_ospi_cfg.spi_protocol         = mode;

        /* Write Enable */
        err = ospi_write_enable_and_verify(p_instance_ctrl, OSPI_MODE_SPI);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** OSPI Write Enable and Verify failed in spi mode**\r\n");
            return err;
        }

        /* Write CR2 and enable DQS */
        g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE].address = OSPI_CR2_ADDRESS_HEX_200;
        g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE].data    = OSPI_CR2_DATA_HEX_02; /* DOS: DQS on STR mode */

        err = direct_read_write(p_instance_ctrl, &g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** R_OSPI_DirectTransfer failed while writing Write configuration register in SPI mode **\r\n");
            return err;
        }

        /* Read CR2 and check if DQS is enabled */
        g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_SPI_MODE].address = OSPI_CR2_ADDRESS_HEX_200;

        err = direct_read_write(p_instance_ctrl, &g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_SPI_MODE], SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** R_OSPI_DirectTransfer failed while reading Write configuration register in SPI mode **\r\n");
            return err;
        }

        /*check read data */
        if(OSPI_CR2_DQS_ON_SOPI_MODE_BIT_MASK != (g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_SPI_MODE].data & OSPI_CR2_DQS_ON_SOPI_MODE_BIT_MASK))
        {
            APP_ERR_PRINT("\r\n** Data read is incorrect in SPI mode **\r\n");
            return err;
        }
    }
    else
    {
        /*Copy mode into local config */
        g_loc_ospi_cfg.spi_protocol         = mode;

        /* Write Enable */
        err = ospi_write_enable_and_verify(p_instance_ctrl, OSPI_MODE_SPI);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** OSPI Write Enable and Verify failed in SPI mode  **\r\n");
            return err;
        }

        /* Write CR2 and enable DQS */
        g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE].address = OSPI_CR2_ADDRESS_HEX_200;
        g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE].data    = OSPI_CR2_DATA_HEX_00; /* DOS: DQS on STR mode */

        err = direct_read_write(p_instance_ctrl, &g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE],
                                SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** R_OSPI_DirectTransfer failed while writing Write configuration register in spi mode**\r\n");
            return err;
        }

        /* Read CR2 and check if DQS is enabled */
        g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_SPI_MODE].address = OSPI_CR2_ADDRESS_HEX_200;
        err = direct_read_write(p_instance_ctrl, &g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_SPI_MODE], SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** R_OSPI_DirectTransfer failed while reading Write configuration register in spi mode**\r\n");
            return err;
        }
        /*check for data read */
        if(RESET_VALUE != (g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_SPI_MODE].data & OSPI_CR2_DQS_ON_SOPI_MODE_BIT_MASK))
        {
            APP_ERR_PRINT("\r\n** Data mismatched in SPI mode **\r\n");
            return err;
        }
    }

    /* Write Enable */
    err = ospi_write_enable_and_verify(p_instance_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** OSPI Write Enable and Verify failed in extended SPI mode **\r\n");
        return err;
    }

    /* Change dummy cycles according to requested frequency */
    g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE].address = OSPI_CR2_ADDRESS_HEX_300;
    g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE].data    = OSPI_CR2_DATA_HEX_05;

    /*Write configuration register*/
    err = direct_read_write(p_instance_ctrl, &g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE], SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_OSPI_DirectTransfer failed while writing Write configuration register in spi mode **\r\n");
        return err;
    }

    /* Read CR2 and check if DQS is enabled */
    g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_SPI_MODE].address = OSPI_CR2_ADDRESS_HEX_300;

    /*Read configuration register*/
    err = direct_read_write(p_instance_ctrl, &g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_SPI_MODE], SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_OSPI_DirectTransfer failed while reading Write configuration register in spi mode **\r\n");
        return err;
    }

    /*check read data */
    if(OSPI_CR2_DATA_HEX_05 != (g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_SPI_MODE].data & OSPI_CR2_DATA_HEX_05))
    {
        APP_ERR_PRINT("\r\n** Data mismatched in SPI mode **\r\n");
        return err;
    }

    /* Write Enable in extended spi mode*/
    err = ospi_write_enable_and_verify(p_instance_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** OSPI Write Enable and Verify failed in extended spi mode **\r\n");
        return err;
    }

    /* The OctaFlash chip is in SPI mode. Change DOPI mode */
    g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE].address = OSPI_CR2_ADDRESS_HEX_0;
    g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE].data    = mode - TWO; /* Requested OPI mode */

    /*Write configuration register*/
    err = direct_read_write(p_instance_ctrl, &g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE], SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_OSPI_DirectTransfer failed while writing Write configuration register in opi mode **\r\n");
        return err;
    }

    /* Transition the OSPI peripheral to SOPI mode. This will initiate auto calibration as MDTR is 0 */
    err = R_OSPI_SpiProtocolSet(p_instance_ctrl, mode);
    if (FSP_SUCCESS == err)
    {
        /* Verify that the chip is in requested OPI mode */
        g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_OPI_MODE].address = OSPI_CR2_ADDRESS_HEX_0;
        err = direct_read_write(p_instance_ctrl, &g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_OPI_MODE], SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** R_OSPI_DirectTransfer failed while reading configuration register in opi mode **\r\n");
            return err;
        }
        /*check for read data*/
        if((mode - TWO) != (g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_OPI_MODE].data & (mode - TWO)))
        {
            APP_ERR_PRINT("\r\n** Data mismatched while reading mode  **\r\n");
            return err;
        }
    }

    /*Close Octa SPI driver*/
    err = R_OSPI_Close(&g_ospi_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_OSPI_Close API failed **\r\n");
        return err;
    }

    /*Open with new configuration*/
    err = R_OSPI_Open(&g_ospi_ctrl, &g_loc_ospi_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_OSPI_OPEN API failed **\r\n");
        return err;
    }

    /*Set OSPI as per mode provided*/
    err = R_OSPI_SpiProtocolSet(&g_ospi_ctrl, mode);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_OSPI_SpiProtocolSet API failed **\r\n");
        return err;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       The function perform extended spi operation
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful spi mode operation
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t spi_operation(void)
{
    uint8_t               read_data          = RESET_VALUE;
    fsp_err_t             err                = FSP_SUCCESS;
    uint8_t               *ospi_ref_addr     = (uint8_t *) (SPI_REFERENCE_ADDRESS);

    /* Check if Flash is in Extended SPI mode*/
    if(SPI_FLASH_PROTOCOL_EXTENDED_SPI != g_loc_ospi_cfg.spi_protocol)
    {
        /*Change Flash mode from OPI to SPI*/
        err = ospi_opi_to_spi_mode_transition();
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** OSPI transition from opi mode to spi mode failed **\r\n");
                    return err;
        }
    }

    /* Print the Sub menu option */
    APP_PRINT(OSPI_SUB_MENU);

    while(EXIT != read_data)
    {
        /* read user input */
        read_data = process_input_data();

        switch (read_data)
        {
            case WRITE:
            {
                /* Erase and Blank check the ospi flash */
                err = erase_and_blankcheck(ospi_ref_addr);
                if(FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\n** OSPI erase and blank check operation failed **\r\n");
                            return err;
                }
                /*Write data on flash and wait till operation completes*/
                err = R_OSPI_Write(&g_ospi_ctrl, write_data, ospi_ref_addr, (uint32_t)(OSPI_DATA_SIZE));
                wait_operation();
                if(FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\n** R_OSPI_Write API failed in SPI Operation **\r\n");
                            return err;
                }

                APP_PRINT("\nWrite operation on Flash Completed Successfully\r\n");
            }
            break;
            case READ:
            {
                volatile int compare_ret_value = memcmp(ospi_ref_addr, write_data, (size_t)(OSPI_DATA_SIZE));
                if(RESET_VALUE == compare_ret_value)
                {
                    APP_PRINT("\r\nRead operation completed successfully and read data is equal to write data\r\n");
                }
                else
                {
                    APP_PRINT("\nRead operation Failed due to data mismatch or no data was written previously\r\n");
                }
            }
            break;
            case ERASE:
            {
                /* Erase and Blank check the ospi flash */
                err = erase_and_blankcheck(ospi_ref_addr);
                if(FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\n** OSPI erase and blank check operation failed **\r\n");
                            return err;
                }
            }
            break;
            case EXIT:
            {
                APP_PRINT ("\nGO back to Main Menu");
            }
            break;
            default:
            {
                APP_PRINT ("\r\n Invalid input. Provide a valid input\r\n");
            }
            break;
        }

        if(EXIT != read_data)
        {
            APP_PRINT(OSPI_SUB_MENU);
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       The function perform single opi or double opi operation as per selected mode.
 * @param[IN]   mode                         SOPI or DOPI mode
 * @retval      FSP_SUCCESS                  Upon successful spi mode operation
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t opi_operation(uint8_t mode)
{
    uint8_t                 read_data          = RESET_VALUE;
    fsp_err_t               err                = FSP_SUCCESS;
    uint8_t                 *ospi_ref_addr     = NULL;

    /*Check if Flash id in OPI mode*/
    if(mode != g_loc_ospi_cfg.spi_protocol)
    {
        /*Change flash mode from spi to sopi*/
        err = ospi_spi_to_opi_mode_transition(&g_ospi_ctrl, (spi_flash_protocol_t)mode);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** OSPI transition from spi to opi mode failed **\r\n");
                    return err;
        }
    }

    /*Update the reference address to write */
    if(SPI_FLASH_PROTOCOL_SOPI == mode)
    {
        ospi_ref_addr = (uint8_t *) (SOPI_REFERNCE_ADDRESS);
        APP_PRINT("\nMode selected is SPI_FLASH_PROTOCOL_SOPI\r\n");
    }
    else
    {
        ospi_ref_addr = (uint8_t *) (DOPI_REFERNECE_ADDRESS);
        APP_PRINT("\nMode selected is SPI_FLASH_PROTOCOL_DOPI\r\n");
    }

    /*Print the Sub menu option*/
    APP_PRINT(OSPI_SUB_MENU);

    while(EXIT != read_data)
    {
        /* read user input */
        read_data = process_input_data();

        switch (read_data)
        {
            case WRITE:
            {
                /* Erase and Blank check the ospi flash */
                err = erase_and_blankcheck(ospi_ref_addr);
                if(FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\n** OSPI erase and blank check operation failed **\r\n");
                            return err;
                }
                /*Write data on flash and wait till operation completes*/
                err = R_OSPI_Write(&g_ospi_ctrl, write_data, ospi_ref_addr, (uint32_t)(OSPI_DATA_SIZE));
                wait_operation();
                if(FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\n** R_OSPI_Write API failed **\r\n");
                            return err;
                }
                APP_PRINT("\nWrite operation on Flash Completed Successfully\r\n");
            }
            break;
            case READ:
            {
                volatile int compare_ret_value = memcmp(ospi_ref_addr, write_data, (size_t)(OSPI_DATA_SIZE));
                if(RESET_VALUE == compare_ret_value)
                {
                    APP_PRINT("\nRead operation completed successfully and read data is equal to write data \r\n");
                }
                else
                {
                    APP_PRINT("\nRead operation Failed due to data mismatch or no data was written previously\r\n");
                }
            }
            break;
            case ERASE:
            {
                /* Erase and Blank check the ospi flash */
                err = erase_and_blankcheck(ospi_ref_addr);
                if(FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\n** OSPI erase and blank check operation failed **\r\n");
                            return err;
                }
            }
            break;
            case EXIT:
            {
                /*Change mode from opi to spi*/
                err = ospi_opi_to_spi_mode_transition();
                if(FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\n** OSPI transition from opi to spi mode failed **\r\n");
                            return err;
                }

                APP_PRINT ("\nMode changed to SPI\r\n");
                APP_PRINT ("Go back to Main Menu\r\n");
            }
            break;
            default:
            {
                APP_PRINT ("\r\n Invalid input. Provide a valid input\r\n");
            }
            break;
        }
        if(EXIT != read_data)
        {
            APP_PRINT(OSPI_SUB_MENU);
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Wait until operation completes.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void wait_operation(void)
{
    uint32_t timeout         = UINT32_MAX;
    spi_flash_status_t status;
    status.write_in_progress = true;

    while ((status.write_in_progress) && (--timeout > RESET_VALUE))
    {
        R_OSPI_StatusGet(&g_ospi_ctrl, &status);
    }
    /*check if timeout occured*/
    if(timeout == RESET_VALUE)
    {
        APP_PRINT("Timeout Occurred");
    }
}

/*******************************************************************************************************************//**
 *  @brief       Closes the OSPI, Print and traps error.
 *  @param[IN]   status    error status
 *  @param[IN]   err_str   error string
 *  @retval      None
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, char * err_str)
{
    if(FSP_SUCCESS != err)
    {
        if (FSP_SUCCESS != R_OSPI_Close(&g_ospi_ctrl))
        {
            APP_ERR_PRINT ("\r\n** R_OSPI_Close API Failed ** \r\n ");
        }
        APP_PRINT(err_str);
        APP_ERR_TRAP(err);
    }
}

/*****************************************************************************************************************
 *  @brief      Process input string to integer value
 *  @param[in]  None
 *  @retval     integer value of input string.
 ****************************************************************************************************************/
uint8_t process_input_data(void)
{
    unsigned char buf[BUFFER_SIZE_DOWN] = {INITIAL_VALUE};
    uint32_t num_bytes                  = RESET_VALUE;
    uint8_t  value                      = RESET_VALUE;

    while (RESET_VALUE == num_bytes)
    {
        if (APP_CHECK_DATA)
        {
            num_bytes = APP_READ(buf);
            if (RESET_VALUE == num_bytes)
            {
                APP_PRINT("\r\nInvalid Input\r\n");
            }
        }
    }
    /* Conversion from input string to integer value */
    value =  (uint8_t) (atoi((char *)buf));
    return value;
}

/*******************************************************************************************************************//**
 * @brief       The function perform direct transfer operation as per direction set on flash.
 * @param[IN]   p_ctrl                         control instance
 * @param[IN]   p_transfer                     transfer instance
 * @param[IN]   direction                      Direct Read and Write direction
 * @retval      FSP_SUCCESS                    Upon successful direct transfer operation
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t direct_read_write(spi_flash_ctrl_t * p_ctrl, spi_flash_direct_transfer_t * const p_transfer,
        spi_flash_direct_transfer_dir_t  direction)
{
    fsp_err_t err;
    err = R_OSPI_DirectTransfer(p_ctrl, p_transfer, direction);
    /* The error is handle and print statement is captured in function where it is called */
    return err;
}

/**********************************************************************************************************************
 * @brief   The function Reads device id
 * @param[OUT]  *dev_id            Pointer will be used to store deviceid
 * @retval      FSP_SUCCESS        Upon successful direct transfer operation
 * @retval      FSP_ERR_ABORTED    On incorrect device id read.
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t read_device_id (uint32_t *dev_id)
{
    fsp_err_t err = FSP_SUCCESS ;
    /* ospi command structure for device id */
    spi_flash_direct_transfer_t ospi_read_id =
    {
     .command        = OSPI_SPI_CMD_READ_DEVICE_ID,
     .address        = OSPI_CR2_ADDRESS_HEX_0,
     .data           = OSPI_CR2_DATA_HEX_00,
     .command_length = LENGTH_ONE,
     .address_length = LENGTH_ZERO,
     .data_length    = LENGTH_THREE,
     .dummy_cycles   = LENGTH_ZERO
    };
    /* direct transfer read command and read device id*/
    err = R_OSPI_DirectTransfer(&g_ospi_ctrl, &ospi_read_id, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\n R_OSPI_DirectTransfer API failed \r\n");
        return err;
    }

    *dev_id = ospi_read_id.data & DEVICE_ID_MASK;

    /* Check for device id read by ospi */
    if(OSPI_DEVICE_ID != *dev_id)
    {
        err = FSP_ERR_ABORTED;
        APP_ERR_PRINT("\nDevice ID is incorrect\r\n");
    }
    return err;
}

/**********************************************************************************************************************
 * @brief   The function perform reconfiguration of device in spi mode
 * @param[IN]   None
 * @retval      FSP_SUCCESS        Upon successful direct transfer operation
 * @retval      FSP_ERR_ABORTED    Incorrect data read
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t reconfigure_device(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* OSPI Configuration */
    spi_flash_direct_transfer_t ospi_write_configuration2 =
    {
     .command        = OSPI_COMMAND_WCR2_SPI_MODE,
     .address        = OSPI_CR2_ADDRESS_HEX_0,
     .data           = OSPI_CR2_DATA_HEX_00,
     .command_length = LENGTH_ONE,
     .address_length = LENGTH_FOUR,
     .data_length    = LENGTH_ONE,
     .dummy_cycles   = LENGTH_ZERO
    };

    /* Direct transfer to enable SPI in Flash*/
    err = direct_read_write(&g_ospi_ctrl, &ospi_write_configuration2, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_OSPI_DirectTransfer API Failed while setting write configuration register **\r\n");
        return err;
    }

    /*Enable Write */
    err = ospi_write_enable_and_verify(&g_ospi_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** OSPI write enable and read operation failed **\r\n");
        return err;
    }

    /* Change dummy cycles according to requested frequency */
    ospi_write_configuration2.address = OSPI_CR2_ADDRESS_HEX_300;
    ospi_write_configuration2.data    = OSPI_CR2_DATA_HEX_07;

    /* Direct transfer to enable SPI in Flash*/
    err = direct_read_write(&g_ospi_ctrl, &ospi_write_configuration2, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_OSPI_DirectTransfer while setting dummy cycles failed  **\r\n");
        return err;
    }

    /*Read configuration register*/
    ospi_write_configuration2.command = OSPI_COMMAND_RCR2_SPI_MODE;

    /*Read Status Register */
    err = direct_read_write(&g_ospi_ctrl, &ospi_write_configuration2, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_OSPI_DirectTransfer while reading configuration register failed **\r\n");
        return err;
    }

    /* Check for data read by ospi */
    if(OSPI_WEN_BIT_MASK != (ospi_write_configuration2.data & OSPI_WEN_BIT_MASK))
    {
        err = FSP_ERR_ABORTED;
        APP_ERR_PRINT ("\r\n** Data read while reading configuration register failed **\r\n");
        return err;
    }

    /*Set OSPI in extended spi mode */
    err = R_OSPI_SpiProtocolSet(&g_ospi_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n** R_OSPI_SpiProtocolSet API failed **\r\n");
        return err;
    }
    return err;
}

/**********************************************************************************************************************
 * @brief   The function performs erase and blank check operation on ospi device
 * @param[IN]  p_device_address    Pointer to flash device address.
 * @retval     FSP_SUCCESS         Upon successful direct transfer operation
 * @retval     FSP_ERR_NOT_ERASED  On incorrect blank check operation
 * @retval     Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t erase_and_blankcheck(uint8_t * const p_device_address)
{
    fsp_err_t err = FSP_SUCCESS;

    /*reset the data*/
    memset (g_erase_data,ERASE_INITIAL_VAL,DEVICE_SECTOR_SIZE);

    /*Erase block data and wait till operation completes*/
    err = R_OSPI_Erase(&g_ospi_ctrl, p_device_address, DEVICE_SECTOR_SIZE);
    wait_operation();
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_OSPI_Erase API failed **\r\n");
                return err;
    }
    /*perform blank check*/
    volatile int ret_value = memcmp(p_device_address, g_erase_data, DEVICE_SECTOR_SIZE);
    if(RESET_VALUE == ret_value)
    {
        APP_PRINT("\r\nErase and blank check operation completed successfully\r\n");
    }
    else
    {
        /* The error is handle and print statement is captured in function where it is called */
        err = FSP_ERR_NOT_ERASED;
        return err;
    }
    return err;
}
/*******************************************************************************************************************//**
 * @} (end addtogroup ospi_ep.c)
 **********************************************************************************************************************/
