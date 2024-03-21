/***********************************************************************************************************************
 * File Name    : netx_duo_ftp_client_ep.c
 * Description  : Contains functions used in netx_duo_ftp_client_ep.c
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


#include "netx_duo_ftp_client_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup ftp_client_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief     FTP Client operation takes place in this function. First it connects to the server and then open a file
 *            and start write/read operation and then close the opened file.
 * @param[IN] ftp_client_ptr        ftp client instance to be passed for client settings
 * @param[IN] pool_ptr              pool pointer is  required to allocate the data packets to be send in network
 * @param[IN] username              User has to provide their respective "username" in USER_NAME macro in user_app.h file.
 * @param[IN] password              User has to provide their respective "password" in PASSWORD macro in user_app.h file.
 * @param[IN] server_ip_address     User has to provide their own respective server ip in user_app.h in SERVER_ADDRESS macro
 * @retval    None
 **********************************************************************************************************************/
UINT run_ftp_client_session(NX_FTP_CLIENT *ftp_client_ptr, NX_PACKET_POOL *pool_ptr, CHAR *username, CHAR *password, NXD_ADDRESS *server_ip_address, CHAR *filename, UCHAR *file_buffer, UINT file_size, UINT timeout)
{
    NX_PACKET *ftp_packet;
    UINT status = NX_SUCCESS;       //To get the status of each Netx call
    PRINT_INFO_STR("FTP Client connecting to server... ");

    /* Connect with the NetX Duo FTP (IPv4 or IPv6) server. */
    status = nxd_ftp_client_connect (ftp_client_ptr, server_ip_address, username, password,
                                     TX_WAIT_FOREVER);
    /* Check status */
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("FTP Client connection failed.");
        return status;
    }
    PRINT_INFO_STR("FTP Client opening file for writing.... ");

    /* Open a FTP file for writing.  */
    status = nx_ftp_client_file_open (ftp_client_ptr, filename, NX_FTP_OPEN_FOR_WRITE, timeout * NX_IP_PERIODIC_RATE);
    /* Check status */
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("FTP Client file write failed.");
        return status;
    }

    /* Allocate an FTP packet.  */
    status = nx_packet_allocate (pool_ptr, &ftp_packet, NX_TCP_PACKET, timeout * NX_IP_PERIODIC_RATE);
    /* Check status */
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("FTP Client packet allocate failed.");
        return status;
    }
    /* Append the packet to the same file */
    status = nx_packet_data_append (ftp_packet, &file_buffer[0], file_size, pool_ptr, timeout * NX_IP_PERIODIC_RATE);
    /* Check status */
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("FTP Client file buffer copy to packet failed.");
        return status;
    }

    /* Write the packet to the file test.txt.  */
    status = nx_ftp_client_file_write (ftp_client_ptr, ftp_packet, timeout * NX_IP_PERIODIC_RATE);
    /* Check status */
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("FTP Client file write failed.");
        return status;
    }
    else
    {
        PRINT_INFO_STR("FTP Client file write completed successfully! ");
    }

    PRINT_INFO_STR("FTP Client closing the file.... ");

    /* Close the file */
    status = nx_ftp_client_file_close (ftp_client_ptr, timeout * NX_IP_PERIODIC_RATE);
    /* Check status */
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("FTP Client file close after writing failed.");
        return status;
    }

    PRINT_INFO_STR("FTP Client opening the same file for reading.... ");

    /* Now open the same file for reading.  */
    status = nx_ftp_client_file_open (ftp_client_ptr, filename, NX_FTP_OPEN_FOR_READ, timeout * NX_IP_PERIODIC_RATE);
    /* Check status */
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("FTP Client file open for reading failed.");
        return status;
    }
    /* File read until the end of file */
    do
    {
        /* Read the file.  */
        status = nx_ftp_client_file_read (ftp_client_ptr, &ftp_packet, timeout * NX_IP_PERIODIC_RATE);
        /* Check status */
        if ((NX_SUCCESS != status) && (NX_FTP_END_OF_FILE != status))
        {
            PRINT_ERR_STR("FTP Client file read error.");
            return status;
        }
        else
        {
            /* Do something with the packet data, ideally copy to another
             * buffer to release the packet as soon as possible. */
            nx_packet_release(ftp_packet);
        }
    }
    while ((NX_SUCCESS == status) && (NX_FTP_END_OF_FILE != status));

    /* Check if the file read completed without errors. */
    if (NX_FTP_END_OF_FILE != status)
    {
        PRINT_ERR_STR("File read failed till end of file.");
        return status;
    }
    else
    {
        PRINT_INFO_STR("FTP Client file read completed successfully! ");
    }

    PRINT_INFO_STR("FTP Client closing the file again....  ");

    /* Close the file again.  */
    status = nx_ftp_client_file_close (ftp_client_ptr, timeout * NX_IP_PERIODIC_RATE);
    /* Check status */
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("FTP Client file close after reading failed.");
        return status;
    }

    PRINT_INFO_STR("FTP Client disconnecting.... ");

    /* Disconnect from the server.  */
    status = nx_ftp_client_disconnect (ftp_client_ptr, timeout * NX_IP_PERIODIC_RATE);
    /* Check status */
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("FTP Client disconnect failed.");
        return status;
    }

    return status;
}

/*******************************************************************************************************************//**
 * @} (end defgroup ftp_client_ep)
 **********************************************************************************************************************/

