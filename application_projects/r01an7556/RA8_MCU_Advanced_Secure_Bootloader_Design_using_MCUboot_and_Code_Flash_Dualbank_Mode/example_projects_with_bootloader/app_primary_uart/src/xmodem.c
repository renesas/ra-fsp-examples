/***********************************************************************************************************************
 * File Name    : xmodem.c
 * Description  : Contains XModem protocol implementations
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include <string.h>
#include "xmodem.h"
#include "comms.h"
#include "hal_data.h"
#include "downloader_thread.h"
#include "header.h"
#include "menu.h"

static bool start_condition;

/*******************************************************************************************************************//**
 * @brief This function implements the XModem protocol and handles the flash operation
 * @param[IN]   flash_address            Start address of the flash where the new image will be downloaded
 * @retval      FSP_SUCCESS             Upon successful FLash_HP data flash operations.
 * @retval      Any Other Error code    Upon unsuccessful Flash_HP data flash operations.
 **********************************************************************************************************************/
unsigned char xmodem_download_and_program_flash(unsigned long flash_address)
{
    uint8_t tx_str[80];

/*
XmodemDownloadAndProgramFlash() takes a memory address as the base address to
which data downloaded is programmed.  The data is downloaded using the XModem
protocol developed in 1977 by Ward Christensen.
The routine detects XM_ERRORs due to XM_TIMEOUTs, comms XM_ERRORs or invalid checksums.
The routine reports the following to the caller:
-Success
-Invalid address	(not implemented here as the address is checked prior to this
					function being called)
-Comms XM_ERROR
-XM_TIMEOUT XM_ERROR
-Failure to program flash


Expects:    
--------
flash_address:
32-bit address located in Flash memory space starting on a 128-byte boundary

Returns:
--------
XM_OK				-	Download and Flash programming performed ok
XM_address_XM_ERROR 	-	address was either not on a 128-bit boundary or not in valid Flash
XM_COMMS_XM_ERROR	 	-	Comms parity, framing or overrun XM_ERROR
XM_XM_TIMEOUT			-	Transmitter did not respond to this receiver
XM_PROG_FAIL		-	Failed to program one or more bytes of the Flash memory
*/
	unsigned char   xm_ret;
	unsigned char   expected_blk_num;
	unsigned char   retry_counter;
	unsigned char   rx_byte_buffer_index;
	unsigned char   checksum;

	unsigned long   address;
	unsigned char   rx_byte_buffer[132] BSP_ALIGN_VARIABLE(4);
	uint8_t         tx_byte BSP_ALIGN_VARIABLE(4);
	uint32_t        rx_len;

	// first xmodem block number is 1
	expected_blk_num = 1;
	
	start_condition = true;
	
	address = flash_address;
	
	while (1)
	{
		//	initialise RX attempts
		retry_counter = 10;
	
		//	decrement RX attempts counter & get Rx byte with a 10 sec TIMEOUT repeat until Rx attempts is 0
		xm_ret = XM_TOUT;
		while ((retry_counter > 0) && (xm_ret == XM_TOUT))
		{
			//if (start_condition == 0)
		    if (true == start_condition)
			{
				//	if this is the start of the xmodem frame
				//	send a NAK to the transmitter
			    tx_byte = NAK;
			    comms_send(&tx_byte, 1);    // Kick off the XModem transfer

				/* Request 132 bytes from host with a delay of 10 seconds */
				fsp_err_t err;
				rx_len = 132;
				memset((void *)&rx_byte_buffer[0], 0, 132);
				err = comms_read((uint8_t *)&rx_byte_buffer[0], &rx_len, 10000);
				if (FSP_SUCCESS == err)
				{
				    xm_ret = OK;
				}
				else if (FSP_ERR_TIMEOUT == err)
				{
				    xm_ret = XM_TOUT;
				}
				else
				{
				    xm_ret = XM_ERROR;
				}
			}                             
			else
			{
			    /* Request 132 bytes from host with a delay of 1 second */
                fsp_err_t err;

                /* Request 132 bytes from host with a delay of 1 second */
                rx_len = 1;
                memset((void *)&rx_byte_buffer[0], 0, 132);
                err = comms_read((uint8_t *)&rx_byte_buffer[0], &rx_len, 1000);
                if (FSP_SUCCESS == err)
                {
                    if (SOH == rx_byte_buffer[0])
                    {
                        rx_len = 131;
                        err = comms_read((uint8_t *)&rx_byte_buffer[1], &rx_len, 10000);
                        if (FSP_SUCCESS == err)
                        {
                            xm_ret = OK;
                        }
                        else if (FSP_ERR_TIMEOUT == err)
                        {
                            xm_ret = XM_TOUT;
                        }
                        else
                        {
                            xm_ret = XM_ERROR;
                        }
                    }
                    else if (EOT == rx_byte_buffer[0])
                    {
                        xm_ret = OK;
                    }
                    else
                    {
                        xm_ret = XM_ERROR;
                    }
                }
                else if (FSP_ERR_TIMEOUT == err)
                {
                    xm_ret = XM_TOUT;
                }
                else
                {
                    xm_ret = XM_ERROR;
                }
			}
			retry_counter--;
		}
			
		start_condition = false;

		if (xm_ret == XM_ERROR)
        {
            return (XM_ERROR);
        }
		else if (xm_ret == XM_TOUT)
		{
		    //  if timed out after 10 attempts
			return (XM_TIMEOUT);
		}
		else			
		{
			// if first received byte is 'end of frame'
			// return ACK to sender
		    if (rx_byte_buffer[0] == EOT)
			{
		    	tx_byte = ACK;
		        comms_send(&tx_byte, 1);
		        sprintf((char *)tx_str, "Resetting the system\r\n");
		        comms_send(tx_str, strlen((char *)tx_str));

		        NVIC_SystemReset();

		        return (XM_OK);
			}
			else
			{				
                // data Rx ok
                // calculate the checksum of the data bytes only
                checksum = 0;
                int cs = 0;

                /* check if the data make sense */
                for (rx_byte_buffer_index = 0; rx_byte_buffer_index < 128; rx_byte_buffer_index++)
                {
                    cs += rx_byte_buffer[rx_byte_buffer_index + 3];
                }

                /* This int to unsigned char conversion needed to eliminate conversion warning with checksum calculation. */
                checksum = (unsigned char)cs;

                //	if SOH, BLK#, 255-BLK# or checksum not valid
                //	(BLK# is valid if the same as expected blk counter or is 1 less
                if (!((rx_byte_buffer[0] == SOH) && ((rx_byte_buffer[1] == expected_blk_num) || (rx_byte_buffer[1] == expected_blk_num - 1)) && (rx_byte_buffer[2] + rx_byte_buffer[1] == 255 ) && (rx_byte_buffer[131] == checksum)))
                {
                    //	send NAK and loop back to (1)
                    tx_byte = NAK;
                    comms_send(&tx_byte, 1);
                }
                else
                {
                    //	if blk# is expected blk num
                    if (rx_byte_buffer[1] == expected_blk_num)
                    {
                        //	Program the received data into flash
                        // Assume flash area is erased

                        int32_t status = 0;
                        threads_and_interrupts(DISABLE);
                        status = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t)&rx_byte_buffer[3], (uint32_t)address, 128);
                        threads_and_interrupts(RE_ENABLE);
                        if (FSP_SUCCESS == status)
                        {
                        	address += 128;
                        	expected_blk_num++;
                            tx_byte = ACK;
                            comms_send(&tx_byte, 1);
                        }
                        else
                        {
                            // prog fail
                            tx_byte = NAK;
                            comms_send(&tx_byte, 1);
                            // cancel xmodem download
                            tx_byte = CAN;
                            comms_send(&tx_byte, 1);

                            return (XM_PROG_FAIL);
                        }
                    }
                    else
                    {
                        // 	block number is valid but this data block has already been received
                        //	send ACK and loop to (1)
                        tx_byte = ACK;
                        comms_send(&tx_byte, 1);
                    }
                }
            }
		}
	}		
}
