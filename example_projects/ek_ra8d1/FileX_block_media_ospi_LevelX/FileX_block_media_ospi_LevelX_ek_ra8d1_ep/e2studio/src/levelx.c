/**********************************************************************************************************************
 * File Name    : levelx.c
 * Description  : Contains data structures and functions used in levelx.c and filex.c
 *********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "levelx.h"

/**********************************************************************************************************************
 * Private function prototypes
 *********************************************************************************************************************/
static UINT verify_input_sector(ULONG **select_sector_address, uint16_t *select_sector, _Bool *is_empty);

/**********************************************************************************************************************
 * External global variable
 *********************************************************************************************************************/
extern FX_MEDIA g_fx_media;
extern volatile _Bool g_media_opened;

/**********************************************************************************************************************
 *  Function Name: defragment_flash
 *  Description  : This function defragments the Flash.
 *  Arguments    : None
 *  Return Value : LX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from LX_SUCCESS
 *********************************************************************************************************************/
UINT defragment_flash(void)
{
    UINT lx_err = LX_SUCCESS;

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR ("Media has not been opened. Please open media first!!!\r\n");
        return FX_SUCCESS;
    }

    PRINT_INFO_STR("\r\n\r\nDefragmenting the Flash ...\r\n");

    PRINT_INFO_STR("This operation will take several minutes\r\n");

    /* Defragment sectors of the NOR flash */
    lx_err = lx_nor_flash_defragment(&g_lx_nor0);
    RETURN_ERR_STR(lx_err, "lx_nor_flash_defragment failed\r\n");

    /* Defragment flash successfully */
    PRINT_INFO_STR("\r\nFlash has been defragmented successfully\r\n");

    return LX_SUCCESS;
}
/**********************************************************************************************************************
* End of function defragment_flash
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: sector_write
 *  Description  : This function writes data to selected sector.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
fsp_err_t sector_write(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint16_t selected_sector = RESET_VALUE;
    CHAR sector_write_data[SECTOR_WIRTE_SIZE];
    sector_info_t sector = {.sector_address = NULL};
    _Bool is_empty = true;

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR ("Media has not been opened. Please open media first!!!\r\n");
        return FX_SUCCESS;
    }

    /* Verify the input sector and retrieve its address. */
    verify_input_sector(&sector.sector_address, &selected_sector, &is_empty);

    if (is_empty)
    {
        PRINT_INFO_STR("\r\n\r\nWriting to the selected sector ...\r\n");

        /* Create sector_write_data buffer according to selected sector */
        snprintf(sector_write_data, SECTOR_WIRTE_SIZE, "Sector %u: %s", selected_sector, SECTOR_WIRTE_TEXT);

        /* Write to selected sector */
        err = RM_LEVELX_NOR_SPI_Write(&g_rm_levelx_nor_spi_ctrl, sector.sector_address, (ULONG *)sector_write_data, SECTOR_WIRTE_SIZE / BYTES_PER_WORD);
        RETURN_ERR_STR(err, "RM_LEVELX_NOR_SPI_Write API FAILED \r\n");

        PRINT_INFO_STR("\r\nWrote to a sector successfully");
    }
    else
    {
        PRINT_INFO_STR("\r\nThe selected sector is currently in use and must be erased before writing.\r\n");
    }

    return FSP_SUCCESS;
}
/**********************************************************************************************************************
* End of function sector_write
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: sector_read
 *  Description  : This function reads data from selected sector.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
fsp_err_t sector_read(void)
{
    fsp_err_t err = FSP_SUCCESS;
    CHAR read_data[SECTOR_READ_SIZE] =  {RESET_VALUE};
    sector_info_t sector = {.sector_address = NULL};
    uint16_t selected_sector = RESET_VALUE;
    _Bool is_empty = true;

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR ("Media has not been opened. Please open media first!!!\r\n");
        return FX_SUCCESS;
    }

    /* Verify input sector, convert to address, and check if it's empty */
    verify_input_sector(&sector.sector_address, &selected_sector, &is_empty);

    if (!is_empty)
    {
        PRINT_INFO_STR("\r\n\r\nReading from the selected sector ...\r\n");

        /* Read from selected sector */
        err = RM_LEVELX_NOR_SPI_Read(&g_rm_levelx_nor_spi_ctrl, sector.sector_address, (ULONG *)read_data, SECTOR_READ_SIZE / BYTES_PER_WORD);
        RETURN_ERR_STR(err, "RM_LEVELX_NOR_SPI_Read API FAILED \r\n");

        /* Add NULL char to terminate the read_data */
        read_data[SECTOR_READ_SIZE - 1] = NULL_CHAR;

        PRINT_INFO_STR("\r\nContent of this sector:\r\n");
        PRINT_INFO_STR(read_data);
        PRINT_INFO_STR("\r\nRead from a sector successfully\r\n");
    }
    else
    {
        PRINT_INFO_STR("\r\nThis sector is empty!!!\r\n");
    }

    /* Display sector information */
    PRINT_SECTOR_INFO(sector);

    return FSP_SUCCESS;
}
/**********************************************************************************************************************
* End of function sector_read
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: sector_erase
 *  Description  : This function erases selected sector.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
fsp_err_t sector_erase(void)
{
    fsp_err_t err = FSP_SUCCESS;
    sector_info_t sector = {.sector_address = NULL};
    uint16_t selected_sector = RESET_VALUE;
    ULONG block = RESET_VALUE;
    _Bool is_empty = true;

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR ("Media has not been opened. Please open media first!!!\r\n");
        return FX_SUCCESS;
    }

    /* Verify input sector, convert to address, and check if it's empty */
    verify_input_sector(&sector.sector_address, &selected_sector, &is_empty);

    if (!is_empty)
    {
        PRINT_INFO_STR("\r\n\r\nErasing the selected sector ...\r\n");

        /* Convert the sector address to the block that need to erase */
        block = ((ULONG)sector.sector_address - FILEX_START_ADDRESS) / OSPI_B_SECTOR_256K_SIZE;

        /* Erase selected sector */
        err = RM_LEVELX_NOR_SPI_BlockErase(&g_rm_levelx_nor_spi_ctrl, block, UNUSE_ARGS);
        RETURN_ERR_STR(err, "RM_LEVELX_NOR_SPI_BlockErase API FAILED \r\n");

        /* Ensure the selected sector is erased success  */
        err = RM_LEVELX_NOR_SPI_BlockErasedVerify(&g_rm_levelx_nor_spi_ctrl, block);
        RETURN_ERR_STR(err, "RM_LEVELX_NOR_SPI_BlockErasedVerify API FAILED \r\n");

        PRINT_INFO_STR("\r\nErased a sector successfully");
    }
    else
    {
        PRINT_INFO_STR("\r\nThis sector is empty!!!\r\n");
    }

    return FSP_SUCCESS;
}
/**********************************************************************************************************************
* End of function sector_erase
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: verify_input_sector
 *  Description  : This function verifies input sector and converts it to corresponding address.
 *  Arguments    : select_sector_address     address of selected sector
 *                 select_sector             selected sector
 *                 is_empty                  status of selected sector
 *  Return Value : None
 *********************************************************************************************************************/
static UINT verify_input_sector(ULONG **select_sector_address, uint16_t *select_sector, _Bool *is_empty)
{
    UINT status = LX_SUCCESS;
    CHAR msg_buf[MSG_SIZE];

    /* Assume sector is empty initially */
    *is_empty = true;

    do
    {
        /* Print the valid sector range used in LevelX operation. */
        snprintf(msg_buf, MSG_SIZE, "\r\nSelect the sector for operation (from %d to %d): ",
                 LEVELX_SECTOR_RANGE_START, LEVELX_SECTOR_RANGE_END);
        PRINT_INFO_STR(msg_buf);

        /* Suspend until there is an input queue */
        status = terminal_get_input_queue(select_sector);
        if (TX_SUCCESS != status)
        {
            RETURN_ERR_STR(status, "**terminal_get_input_queue failed**\r\n");
        }

        /* Verify user input */
        if (*select_sector >= LEVELX_SECTOR_RANGE_START && *select_sector <= LEVELX_SECTOR_RANGE_END)
        {
            /* Convert input to sector address */
            *select_sector_address = (ULONG *)(LEVELX_START_ADDRESS +
                                               ((*select_sector - LEVELX_SECTOR_RANGE_START) * OSPI_B_SECTOR_256K_SIZE));

            /* Check if sector is empty */
            for (uint32_t n = 0; n < OSPI_B_SECTOR_256K_SIZE; n++)
            {
                UCHAR *byte_address = (UCHAR *)(*select_sector_address) + n;

                if (*byte_address != NOR_SPI_FLASH_CLEARED)
                {
                    *is_empty = false;
                    break;
                }
            }
        }
        else
        {
            *select_sector_address = NULL;
            PRINT_INFO_STR("\r\n\r\nInvalid input. Please select another sector.\r\n");
        }
    } while (*select_sector_address == NULL);

    return LX_SUCCESS;
}
/**********************************************************************************************************************
* End of function verify_input_sector
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: sector_info_get
 *  Description  : This function gets sector information.
 *  Arguments    : None
 *  Return Value : LX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from LX_SUCCESS
 *********************************************************************************************************************/
UINT sector_info_get(void)
{
    UINT lx_err = LX_SUCCESS;
    sector_info_t sector = {RESET_VALUE};

    sector.logical_sector = g_fx_media.fx_media_driver_logical_sector;

    /* Retrieve the sector address */
    lx_err = _lx_nor_flash_logical_sector_find(&g_lx_nor0, sector.logical_sector ,LX_TRUE, &sector.sector_map_entry, &sector.sector_address);
    if (lx_err != LX_SUCCESS)
    {
        RETURN_ERR_STR(lx_err, "_lx_nor_flash_logical_sector_find failed\r\n");
    }

    /* Display sector information */
    PRINT_SECTOR_INFO(sector);

    return LX_SUCCESS;
}
/**********************************************************************************************************************
* End of function sector_info_get
**********************************************************************************************************************/
