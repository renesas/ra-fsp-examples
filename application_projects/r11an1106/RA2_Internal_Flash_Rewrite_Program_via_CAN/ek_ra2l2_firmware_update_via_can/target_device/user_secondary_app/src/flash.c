/***********************************************************************************************************************
 * File Name    : flash.c
 * Description  : Contains can protocol related functions
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "flash.h"
#include "can.h"
/*******************************************************************************************************************//**
 * @brief       Opens the Flash_LP module.
 * @param[IN]   None
 * @retval      FSP_SUCCESS             Upon successful open of Flash_LP module
 * @retval      err                     Upon unsuccessful open
 **********************************************************************************************************************/

//extern uint8_t can_pack_msg[CAN_FRAME_TRANSMIT_DATA_BYTES];
fsp_err_t flash_lp_init(void)
{
    fsp_err_t err = FSP_SUCCESS;    /* Error status */

    /* Open Flash_LP module */
    err = R_FLASH_LP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* Flash_LP failure message */
        APP_PRINT("** R_FLASH_LP_Open API for Flash_LP failed ** \r\n");
    }

    return err;
}

fsp_err_t code_flash_lp_operations()
{
    fsp_err_t err = FSP_SUCCESS;
    flash_result_t blank_check_result = FLASH_RESULT_BLANK;


    uint32_t start_addr = SECONDARY_IMAGE_START_ADDRESS;
    uint32_t length     = SECONDARY_IMAGE_END_ADDRESS - SECONDARY_IMAGE_START_ADDRESS;

    /* Perform blank check */
    err = R_FLASH_LP_BlankCheck(&g_flash0_ctrl, start_addr, length, &blank_check_result);
    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_FLASH_LP_BlankCheck API for Flash_LP failed ** \r\n");
        return err;
    }

    if (FLASH_RESULT_BLANK == blank_check_result)
    {
        return FSP_SUCCESS;
    }

    if (FLASH_RESULT_NOT_BLANK != blank_check_result)
    {
        return FSP_ERR_ABORTED;
    }

    APP_PRINT("Erasing the secondary slot...\r\n");
    err = R_FLASH_LP_Erase(&g_flash0_ctrl, (uint32_t) start_addr, SECONDARY_IMAGE_NUM_BLOCKS);


    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("ERROR: Erasing the secondary slot\r\n");
        return err;
    }

    err = R_FLASH_LP_BlankCheck(&g_flash0_ctrl, start_addr, length, &blank_check_result);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_FLASH_LP_BlankCheck failed after erase **\r\n");
        return err;
    }

    APP_PRINT("Secondary slot erased\r\n");
    return FSP_SUCCESS;
}

fsp_err_t display_image_slot_info(const uint32_t address)
{

    fsp_err_t err = FSP_SUCCESS;                        /* Error status */

    struct image_version {
        uint8_t iv_major;
        uint8_t iv_minor;
        uint16_t iv_revision;
        uint32_t iv_build_num;
    };

    struct image_header {
        uint32_t ih_magic;
        uint32_t ih_load_addr;
        uint16_t ih_hdr_size;            /* Size of image header (bytes). */
        uint16_t ih_protect_tlv_size;    /* Size of protected TLV area (bytes). */
        uint32_t ih_img_size;            /* Does not include header. */
        uint32_t ih_flags;               /* IMAGE_F_[...]. */
        struct image_version ih_ver;
        uint32_t _pad1;
    };

    const struct image_header * p_img_header = (const struct image_header *)address;

    uint8_t data[16] = {0};
    uint32_t add = address;

    memcpy(&data[0],&p_img_header->ih_ver, 8); /* version */
    memcpy(&data[8],&add, 4); /* slot address */
    memcpy(&data[12],&p_img_header->ih_img_size, 4); /* image size */

    for (int i=0; i<2 ; i++)
    {
        err = can_send(&data[8*i], CAN_DESTINATION_MAILBOX_1);

        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Wait ACK (CAN_MSG_SUCCESS) from other side */
        err = can_receive();
        if (FSP_SUCCESS != err)
        {
            return FSP_ERR_ABORTED;
        }
    }
    return err;
}
