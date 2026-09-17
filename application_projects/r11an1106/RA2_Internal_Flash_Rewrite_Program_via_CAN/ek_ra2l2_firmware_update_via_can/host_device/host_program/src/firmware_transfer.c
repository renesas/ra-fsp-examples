/***********************************************************************************************************************
 * File Name    : firmware_transfer.c
 * Description  : Contains firmware transfer related functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include <common_utils.h>
#include <firmware_transfer.h>
#include <slot_info.h>


static fsp_err_t data_can_transfer(uint8_t *packet_data)
{
    fsp_err_t err = FSP_SUCCESS;

    for (uint8_t i=0; i<16; i++)
    {
        /* Transmit the data from mail box #0 with tx_frame */
        err = can_send((uint8_t*)&packet_data[8U*i], CAN_DESTINATION_MAILBOX_2);
        /* Error trap */
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    return FSP_SUCCESS;
}

fsp_err_t firmware_transfer()
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t read_buff[BLOCK_SIZE] = {RESET_VALUE};      /* Read data buffer */

    /* Read code flash data */
    for (uint32_t block = 0; block < SECONDARY_FIRMWARE_NUM_BLOCKS; block ++)
    {
        /* Read 128-bytes block from define firmware address */
        memcpy(&read_buff, (uint8_t*)(SECONDARY_FIRMWARE_START_ADDRESS + (BLOCK_SIZE * block)) , BLOCK_SIZE);

        /* Send firmware data (16 CAN frames) */
        err = data_can_transfer(read_buff);
        
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Wait for ACK from receiver */
        err = can_receive();
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        memset(read_buff, RESET_VALUE, BLOCK_SIZE);
    }
    
    /* All firmware blocks sent → notify DONE */
    err = can_send_msg(CAN_MSG_DONE, CAN_DESTINATION_MAILBOX_1);

    return err;
}

