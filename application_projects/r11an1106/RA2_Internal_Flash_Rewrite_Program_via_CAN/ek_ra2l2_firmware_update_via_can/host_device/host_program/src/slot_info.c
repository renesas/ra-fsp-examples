/***********************************************************************************************************************
 * File Name    : slot_info.c
 * Description  : Contains image slots implementations
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/


#include <common_utils.h>
#include <menu.h>
#include <slot_info.h>
#include "stdio.h"
#include "hal_data.h"

static fsp_err_t recv_and_print_image_slot(const char * slot_name);
fsp_err_t display_image_slot_info(void)
{
    volatile fsp_err_t err = FSP_SUCCESS;

    err = can_send_msg(CAN_MSG_FLASH, CAN_DESTINATION_MAILBOX_1);
    /* Error trap */
    if (FSP_SUCCESS == err)
    {

        err = recv_and_print_image_slot("Primary");
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        err = recv_and_print_image_slot("Secondary");
        if (FSP_SUCCESS != err)
        {
            return err;
        }

    }
    return err;

}

static fsp_err_t recv_and_print_image_slot(const char * slot_name)
{
    fsp_err_t err;
    uint8_t data[16] = {0};
    uint32_t adr = 0;

    for (int frame_count = 0; frame_count < 2; frame_count++)
    {
        err = R_GPT_Start( &g_can_timer_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
        /* Wait for CAN RX */
        while (true != b_can_rx)
        {
            if (g_timer0_event == true)
            {
                if (b_can_err == true)
                {
                    b_can_err = false;
                    return FSP_ERR_ASSERTION;
                }
                else
                {
                    g_timer0_event = false;
                    return FSP_ERR_TIMEOUT;
                }
            }
        }

        err = R_GPT_Stop(&g_can_timer_ctrl);
        err = R_GPT_Reset( &g_can_timer_ctrl);

        b_can_rx = false;

        memcpy(&data[frame_count * 8], &g_can_rx_frame.data, CAN_FRAME_TRANSMIT_DATA_BYTES);

        memset((uint8_t *)g_can_rx_frame.data, RESET_VALUE, CAN_FRAME_TRANSMIT_DATA_BYTES);

        if (frame_count == 1)
        {
            memcpy(&adr, &data[12], 4);

            APP_PRINT("\r\n******************************\r\n");
            APP_PRINT("**** %s Image Slot ****\r\n", slot_name);
            APP_PRINT("******************************\r\n");
            APP_PRINT("Image version: %d.%d.%d\r\n",
                      data[0], data[1], data[2]);
            APP_PRINT("Load address: 0x%02X%02X%02X%02X\r\n",
                      data[11], data[10], data[9], data[8]);
            APP_PRINT("Image size: %d bytes\r\n", adr);

            memset(data, RESET_VALUE, sizeof(data));
        }

        err = can_send_msg(CAN_MSG_SUCCESS, CAN_DESTINATION_MAILBOX_1);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    return FSP_SUCCESS;
}

fsp_err_t code_flash_lp_operations(void)
{
    fsp_err_t err = FSP_SUCCESS;                        /* Error status */

    
    flash_result_t blank_check_result = FLASH_RESULT_BLANK;

    uint32_t start_addr = SECONDARY_FIRMWARE_START_ADDRESS;
    uint32_t length     = SECONDARY_FIRMWARE_END_ADDRESS - SECONDARY_FIRMWARE_START_ADDRESS;

    /* Perform blank check */
    err = R_FLASH_LP_BlankCheck(&g_flash0_ctrl, start_addr, length, &blank_check_result);
    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_PRINT("** R_FLASH_LP_BlankCheck API for Flash_LP failed ** \r\n");
        return err;
    }

    if (FLASH_RESULT_BLANK == blank_check_result)
    {
        return FSP_ERR_ABORTED;
    }

    if (FLASH_RESULT_NOT_BLANK != blank_check_result)
    {
        return FSP_SUCCESS;
    }
    return err;
}
