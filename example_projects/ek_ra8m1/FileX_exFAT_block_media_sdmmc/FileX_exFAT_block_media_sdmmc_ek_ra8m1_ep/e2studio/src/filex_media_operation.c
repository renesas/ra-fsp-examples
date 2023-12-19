/***********************************************************************************************************************
 * File Name    : filex_media_operation.c
 * Description  : Contains data structures and functions used in FileX thread.
 ***********************************************************************************************************************/
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "filex_media_operation.h"

/* Private global variable */
static rm_block_media_info_t g_block_media_info = {RESET_VALUE};

/* Functions implementation */

/*******************************************************************************************************************//**
 * @brief       This function verifies the block media state.
 * @param[in]   None
 * @retval      FX_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FX_SUCCESS
 **********************************************************************************************************************/
UINT media_verify(void)
{
    /* Check media inserted */
    if (RESET_VALUE == (g_fx_media_status & MEDIA_INSERTED))
    {
        PRINT_INFO_STR("SD card not inserted, please insert SD card\r\n");
        return FX_MEDIA_INVALID;
    }

    /* Check media open */
    if (RESET_VALUE == (g_fx_media_status & MEDIA_OPEN))
    {
        PRINT_INFO_STR("Media does not open, please open media\r\n");
        return FX_MEDIA_NOT_OPEN;
    }

    /* Check media opened */
    if (RESET_VALUE == (g_fx_media_status & MEDIA_OPENED))
    {
        PRINT_INFO_STR("Media open failed, please format media\r\n");
        return FX_INVALID_STATE;
    }

    return FX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       This function performs media opening.
 * @param[in]   None
 * @retval      FX_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FX_SUCCESS
 **********************************************************************************************************************/
UINT media_open(void)
{
    UINT status = FX_SUCCESS;

    /* Check media inserted */
    if (RESET_VALUE == (g_fx_media_status & MEDIA_INSERTED))
    {
        PRINT_INFO_STR("SD card not inserted, please insert SD card\r\n");
        return FX_SUCCESS;
    }

    /* Check media opened */
    if (RESET_VALUE != (g_fx_media_status & MEDIA_OPENED))
    {
        PRINT_INFO_STR ("Media has already been opened\r\n");
        return FX_SUCCESS;
    }

    /* Set the media open status */
    g_fx_media_status |= MEDIA_OPEN;

    /* Open media using the Azure FileX API */
    status = fx_media_open(&g_fx_media,
                            "g_fx_media",
                            RM_FILEX_BLOCK_MEDIA_BlockDriver,
                            (void *) &g_rm_filex_block_media_instance,
                            g_fx_media_media_memory,
                            G_FX_MEDIA_MEDIA_MEMORY_SIZE);
    if (FX_SUCCESS != status)
    {
        PRINT_INFO_STR ("Media open failed, please format media\r\n");
        return FX_SUCCESS;
    }

    /* Set the media opened status */
    g_fx_media_status |= MEDIA_OPENED;

    /* Print message successfully */
    PRINT_INFO_STR("Media was opened successfully\r\n");

    return FX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       This function retrieves media properties.
 * @param[in]   None
 * @retval      FX_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FX_SUCCESS
 **********************************************************************************************************************/
UINT media_get_property(void)
{
    UINT status = FX_SUCCESS;
    media_property_t media = {RESET_VALUE};

    /* Verify the current state of the media */
    status = media_verify();
    if (FX_SUCCESS != status)
    {
        return FX_SUCCESS;
    }

    /* Get the physical size of the SD card  */
    status = (UINT)RM_BLOCK_MEDIA_SDMMC_InfoGet(&g_rm_block_media_ctrl, &g_block_media_info);
    RETURN_ERR_STR(status, "RM_BLOCK_MEDIA_SDMMC_InfoGet failed\r\n");

    /* Get the free space size of the media */
    status = fx_media_extended_space_available(&g_fx_media, &media.free_size);
    RETURN_ERR_STR(status, "fx_media_extended_space_available failed\r\n");

    /* Calculate the total physical size of the SD card */
    media.phy_size = (ULONG64)g_block_media_info.num_sectors * (ULONG64)g_block_media_info.sector_size_bytes;

    /* Calculate the total size of the media */
    media.total_zise = (ULONG64)g_fx_media.fx_media_total_clusters *
                       (ULONG64)g_fx_media.fx_media_sectors_per_cluster *
                       (ULONG64)g_fx_media.fx_media_bytes_per_sector;

    PRINT_MEDIA_PROPERTY(media);

    return FX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       This function performs media formatting.
 * @param[in]   None
 * @retval      FX_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FX_SUCCESS
 **********************************************************************************************************************/
UINT media_format(void)
{
    UINT status = FX_SUCCESS;

    /* Check media inserted */
    if (RESET_VALUE == (g_fx_media_status & MEDIA_INSERTED))
    {
        PRINT_INFO_STR("SD card not inserted, please insert SD card\r\n");
        return FX_SUCCESS;
    }

    /* Check media open */
    if (RESET_VALUE == (g_fx_media_status & MEDIA_OPEN))
    {
        PRINT_INFO_STR ("Media must be open at least once\r\n");
        return FX_SUCCESS;
    }

    /* Get the physical size of the SD card  */
    status = (UINT)RM_BLOCK_MEDIA_SDMMC_InfoGet(&g_rm_block_media_ctrl, &g_block_media_info);
    RETURN_ERR_STR(status, "RM_BLOCK_MEDIA_SDMMC_InfoGet failed\r\n");

    /* Format the media in exFAT format */
    status = fx_media_exFAT_format(&g_fx_media,                      // Pointer to FileX media control block.
                                    RM_FILEX_BLOCK_MEDIA_BlockDriver,           // Driver entry
                                    (void *) &g_rm_filex_block_media_instance,  // Pointer to Block Media Driver
                                    g_fx_media_media_memory,                    // Media buffer pointer
                                    G_FX_MEDIA_MEDIA_MEMORY_SIZE,               // Media buffer size
                                    (char *) G_FX_MEDIA_VOLUME_NAME,            // Volume Name
                                    G_FX_MEDIA_NUMBER_OF_FATS,                  // Number of FATs
                                    G_FX_MEDIA_HIDDEN_SECTORS,                  // Hidden sectors
                                    g_block_media_info.num_sectors,             // Total sectors
                                    g_block_media_info.sector_size_bytes,       // Sector size
                                    G_FX_MEDIA_SECTORS_PER_CLUSTER,             // Sectors per cluster
                                    G_FX_MEDIA_VOLUME_SERIAL_NUMBER,            // volume serial number
                                    G_FX_MEDIA_BOUNDARY_UNIT);                  // boundary unit
    RETURN_ERR_STR(status, "fx_media_exFAT_format FileX failed\r\n");

    /* Format the media successfully */
    PRINT_INFO_STR("Media has been formatted\r\n");

    return FX_SUCCESS;
}

UINT media_close(void)
{
    UINT status = FX_SUCCESS;

    /* Check media inserted */
    if (RESET_VALUE == (g_fx_media_status & MEDIA_INSERTED))
    {
        PRINT_INFO_STR("SD card not inserted, please insert SD card\r\n");
        return FX_SUCCESS;
    }

    /* Check media opened */
    if (RESET_VALUE == (g_fx_media_status & MEDIA_OPENED))
    {
        PRINT_INFO_STR ("Media has not been opened\r\n");
        return FX_SUCCESS;
    }

    /* Close media using the Azure FileX API */
    status = fx_media_close(&g_fx_media);
    RETURN_ERR_STR(status, "fx_media_close failed\r\n");

    /* Clear the media open and the media opened flags */
    g_fx_media_status &= ~(MEDIA_OPEN |MEDIA_OPENED);

    /* Close the media successfully */
    PRINT_INFO_STR("Media has been closed\r\n");

    return FX_SUCCESS;
}
