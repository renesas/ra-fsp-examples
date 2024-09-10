/***********************************************************************************************************************
 * File Name    : flash.c
 * Description  : flash handling subroutines.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "flash.h"
#include "common_util.h"

/*****************************************************************************************************************
 *  @brief      roundup: round up data
 *  @param[in]  numToRound: the number to be rounded
 *  @param[in]  multiple: round up to multiples of this integer
 *  @retval     the number after rounded up
 *  ****************************************************************************************************************/
static int roundup(int numToRound, int multiple)
{
    if (0 == multiple)
        return numToRound;

    int remainder = numToRound % multiple;
    if (0 == remainder )
        return numToRound;

    return (numToRound + multiple - remainder);
}

/*****************************************************************************************************************
 *  @brief      flash_init: flash initialization
 *  @param[in]  none
 *  @retval     flash open API return status
 *  ****************************************************************************************************************/
fsp_err_t flash_init (void)
{
    fsp_err_t status = FSP_SUCCESS;

    status = R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);

    if (FSP_SUCCESS != status )
    {
#ifdef SWO_PRINTF
        printf("Initializing data flash: failed\r\n");
#endif
        APP_ERR_TRAP(status);
    }

    return status;
}

/*****************************************************************************************************************
 *  @brief      flash_read: read and return the flash data. This function is not used in this application.
 *  @param[in]  data_buffer: stores the flash data read
 *  @param[in]  data_size: size of the flash data to read
 *  @param[in]  info_type: type of the flash data
 *  @retval     status of the data copy to the destination buffer
 *  ****************************************************************************************************************/
fsp_err_t flash_read(uint8_t *data_buffer, unsigned int data_size, unsigned int info_type)
{
    unsigned int _dataflash_addr = 0;

    switch(info_type)
    {
        case DEV_CERT_CFG:
            _dataflash_addr = DEVICE_CERT_BLOCK_ADDR;
            break;

        default:
            return FSP_ERR_INVALID_ARGUMENT;
    }

    /*Read code flash data */
    memcpy(data_buffer, (uint8_t *) _dataflash_addr, data_size);

    return ((fsp_err_t)(memcpy(data_buffer, (uint8_t *) _dataflash_addr, data_size)));

}

/*****************************************************************************************************************
 *  @brief      flash_write: write the data_buffer of size data_size
 *  @param[in]  data_buffer: data to be written to flash
 *  @param[in]  data_size: size of the data to write
 *  @param[in]  info_type: type of the flash data
 *  @retval     flash write result
 *  ****************************************************************************************************************/

fsp_err_t flash_write (uint8_t *data_buffer , unsigned int data_size, unsigned int info_type)
{
    fsp_err_t status = FSP_SUCCESS;
    unsigned int flash_addr = 0;
    unsigned int _num_of_blocks = 1;
    uint32_t copy_size = 0;
    int tx_size = 0;
    flash_result_t blank_check_result = FLASH_RESULT_NOT_BLANK;

    switch(info_type)
    {
        case DEV_CERT_CFG:
            flash_addr = DEVICE_CERT_BLOCK_ADDR;
            _num_of_blocks = (unsigned int)NUM_OF_BLOCKS(DEVICE_CERT_BLOCK_LEN);
            break;

        default:
            return FSP_ERR_INVALID_ARGUMENT;
    }

    if( data_size > (_num_of_blocks * FLASH_BLOCK_SIZE))
    {
#ifdef SWO_PRINTF
        printf("Invalid data size in flash_write() API call!!!!\r\n");
#endif
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Erase flash memory location first before write */
    status = R_FLASH_HP_Erase(&g_flash0_ctrl, flash_addr, _num_of_blocks);
    if(FSP_SUCCESS == status)
    {
        status = R_FLASH_HP_BlankCheck(&g_flash0_ctrl, flash_addr, FLASH_HP_CF_BLOCK_SIZE_32KB, &blank_check_result);
        /* Error Handle */
        if (FSP_SUCCESS != status)
        {
#ifdef SWO_PRINTF
        printf("flash blank check in flash_write() API call failed\r\n");
#endif
            return status;
        }

#ifdef SWO_PRINTF
        printf("Successfully erased code flash!!! \r");
#endif
        while (data_size > 0)
        {
            if (data_size > FLASH_BLOCK_SIZE)
            {
                copy_size = FLASH_BLOCK_SIZE;
            }
            else
            {
                copy_size = data_size;
            }

            /* Check if the size is multiple of 128 bytes in case of internal code flash.*/
            tx_size = roundup((int)copy_size, CODE_FLASH_WRITE_SIZE);

            /* Write code flash data*/
            status = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t)data_buffer, flash_addr, (uint32_t)tx_size);

            if ( FSP_SUCCESS == status )
            {

                data_size -= (copy_size);
                data_buffer += (copy_size);
                flash_addr += FLASH_BLOCK_SIZE;
            }
            else
            {
#ifdef SWO_PRINTF
                printf("Failed to write the given buffer content to internal flash memory \r\n");
#endif
                status = FSP_ERR_WRITE_FAILED;
                break;
            }
        }
    }
    else
    {
#ifdef SWO_PRINTF
       printf("Failed to erase given block of internal flash memory, status = %x \r\n", status);
#endif
        status = FSP_ERR_ERASE_FAILED;
    }

    return status;
}

/*****************************************************************************************************************
 *  @brief      flash_FAW_Set: set up flash access window
 *  @param[in]  start_addr: start address of flash access window
 *  @param[in]  end_addr: end address of flash access window
 *  @retval     flash access window set result
 *  ****************************************************************************************************************/

fsp_err_t flash_FAW_Set(uint32_t start_addr, uint32_t end_addr)
{
    fsp_err_t status = FSP_SUCCESS;

    status = R_FLASH_HP_AccessWindowSet(&g_flash0_ctrl, start_addr, end_addr);

    if( FSP_SUCCESS != status )
    {
#ifdef SWO_PRINTF
      printf("Failed to set Access Window, status = %d\r\n", status);
#endif
    }

    return status;
}

/*****************************************************************************************************************
 *  @brief      flash_deinit: close the flash driver
 *  @param[in]  none
 *  @retval     close the flash driver
 *  ****************************************************************************************************************/

fsp_err_t flash_deinit(void)
{
    fsp_err_t status = FSP_SUCCESS;
    status = R_FLASH_HP_Close(&g_flash0_ctrl);
    if( FSP_SUCCESS != status )
    {
#ifdef SWO_PRINTF
      printf("Failed to set Access Window, status = %d\r\n", status);
#endif
    }

    return status;
}

/*****************************************************************************************************************
 *  @brief      flash_FAW_Clear: clear the flash access window setting
 *  @param[in]  none
 *  @retval     status of access window clear opertion
 *  ****************************************************************************************************************/

fsp_err_t flash_FAW_Clear(void)
{
    fsp_err_t status = FSP_SUCCESS;
    status = R_FLASH_HP_AccessWindowClear(&g_flash0_ctrl);

    if(FSP_SUCCESS != status)
    {
#ifdef SWO_PRINTF
      printf("Failed to clear Access Window, status = %d\r\n", status);
#endif
    }
    return status;
}
