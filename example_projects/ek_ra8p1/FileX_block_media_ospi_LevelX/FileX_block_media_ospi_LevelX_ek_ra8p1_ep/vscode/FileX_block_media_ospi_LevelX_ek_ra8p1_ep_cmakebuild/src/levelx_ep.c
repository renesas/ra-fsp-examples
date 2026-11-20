/***********************************************************************************************************************
 * File Name    : levelx_ep.c
 * Description  : Contains data structures and functions used in levelx_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#include "levelx_ep.h"

/***********************************************************************************************************************
 * Function Name: levelx_init
 * Description  : This function initializes the LevelX NOR flash wear-levelling system.
 * Arguments    : None.
 * Return Value : LX_SUCCESS            Initialized successfully.
 *                Any other error codes Failed to initialize.
 **********************************************************************************************************************/
UINT levelx_init(void)
{
    return lx_nor_flash_initialize();
}
/***********************************************************************************************************************
* End of function levelx_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: defragment_flash
 * Description  : This function defragments the Flash.
 * Arguments    : None.
 * Return Value : LX_SUCCESS            Defragmented successfully.
 *                Any other error codes Failed to defragment.
 **********************************************************************************************************************/
UINT defragment_flash(void)
{
    return lx_nor_flash_defragment(&g_lx_nor0);
}
/***********************************************************************************************************************
* End of function defragment_flash
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sector_info_get
 * Description  : This function gets sector information.
 * Arguments    : logical_sector            Logical sector number to locate.
 *                physical_sector_map_entry Pointer to the sector mapping table entry.
 *                physical_sector_address   Pointer to the physical sector address.
 * Return Value : LX_SUCCESS                Retrieved successfully.
 *                Any other error codes     Failed to retrieve.
 **********************************************************************************************************************/
UINT sector_info_get(ULONG logical_sector, ULONG **physical_sector_map_entry, ULONG **physical_sector_address)
{
    return _lx_nor_flash_logical_sector_find(&g_lx_nor0, logical_sector, LX_TRUE,
                                               physical_sector_map_entry, physical_sector_address);
}
/***********************************************************************************************************************
* End of function sector_info_get
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sector_write
 * Description  : This function writes data to selected sector.
 * Arguments    : p_flash_addr          Pointer to the destination flash address.
 *                p_src                 Pointer to the source data buffer.
 * Return Value : FSP_SUCCESS           Wrote successfully.
 *                Any other error codes Failed to write.
 **********************************************************************************************************************/
fsp_err_t sector_write(ULONG * const p_flash_addr, ULONG * const p_src)
{
    return RM_LEVELX_NOR_SPI_Write(&g_rm_levelx_nor_spi_ctrl, p_flash_addr, p_src, SECTOR_WRITE_SIZE / BYTES_PER_WORD);
}
/***********************************************************************************************************************
* End of function sector_write
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sector_read
 * Description  : This function reads data from selected sector.
 * Arguments    : p_flash_addr          Pointer to the source flash address.
 *                p_dest                Pointer to the destination buffer.
 * Return Value : FSP_SUCCESS           Read successfully.
 *                Any other error codes Failed to read.
 **********************************************************************************************************************/
fsp_err_t sector_read(ULONG * const p_flash_addr, ULONG * const p_dest)
{
    return RM_LEVELX_NOR_SPI_Read(&g_rm_levelx_nor_spi_ctrl, p_flash_addr, p_dest, SECTOR_READ_SIZE / BYTES_PER_WORD);
}
/***********************************************************************************************************************
* End of function sector_read
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sector_erase
 * Description  : This function erases selected sector.
 * Arguments    : p_sector_address      Pointer to the sector address erase.
 * Return Value : FSP_SUCCESS           Erased successfully.
 *                Any other error codes Failed to erase.
 **********************************************************************************************************************/
fsp_err_t sector_erase(ULONG * p_sector_address)
{
    fsp_err_t err = FSP_SUCCESS;
    ULONG block = RESET_VALUE;

    /* Convert the sector address to the block that need to erase */
#if defined (BOARD_RA8D1_EK)
    block = ((ULONG)p_sector_address - FILEX_START_ADDRESS) / OSPI_B_SECTOR_256K_SIZE;
#elif defined (BOARD_RA8P1_EK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8M2_EK) || defined (BOARD_RA8T2_EK)
    block = ((ULONG)p_sector_address - FILEX_START_ADDRESS) / OSPI_B_SECTOR_4K_SIZE;
#endif /* Specific board */

    /* Erase selected sector */
    err = RM_LEVELX_NOR_SPI_BlockErase(&g_rm_levelx_nor_spi_ctrl, block, UNUSE_ARGS);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return RM_LEVELX_NOR_SPI_BlockErasedVerify(&g_rm_levelx_nor_spi_ctrl, block);
}
/***********************************************************************************************************************
* End of function sector_erase
***********************************************************************************************************************/
