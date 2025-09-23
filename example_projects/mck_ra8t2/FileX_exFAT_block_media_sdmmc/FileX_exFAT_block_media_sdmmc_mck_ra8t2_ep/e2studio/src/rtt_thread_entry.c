/***********************************************************************************************************************
 * File Name    : rtt_thread_entry.c
 * Description  : Contains macros, data structures and functions commonly used in rtt_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "rtt_thread.h"
#include "rtt.h"

TX_BYTE_POOL g_rtt_byte_pool;
static uint8_t g_byte_pool_memory [RTT_BYTE_POOL_SIZE] = {RESET_VALUE};
static CHAR *gp_menu_content [MENU_MAX] =
{
     [MENU_MAIN] = MAIN_OPTION,
     [MENU_MEDIA] = MEDIA_OPTION,
     [MENU_DIR] = DIR_OPTION,
     [MENU_FILE] = FILE_OPTION
};

static void conv_ul64_to_str(ULONG64 value, CHAR * p_buf, UINT len);

/* RTT Thread entry function */
void rtt_thread_entry(void)
{
    UINT tx_status = TX_SUCCESS;

    /* Initialize the RTT framework */
    tx_status = rtt_framework_init();
    if(TX_SUCCESS != tx_status)
    {
        APP_PRINT("\r\nERROR: rtt_framework_init failed\r\n");
        APP_ERR_TRAP(tx_status);
    }

    /* Send a start event to the Console thread */
    tx_status = (ULONG)tx_event_flags_set(&g_request_event, REQUEST_START_CONSOLE, TX_OR);
    if(TX_SUCCESS != tx_status)
    {
        APP_PRINT("\r\nERROR: tx_event_flags_set failed\r\n");
        APP_ERR_TRAP(tx_status);
    }

    tx_thread_sleep(RTT_THREAD_SLEEP_TICK);

    while(true)
    {
        /* Handle user input from RTT */
        tx_status = rtt_input_handle();
        if(TX_SUCCESS != tx_status)
        {
            APP_PRINT("\r\nERROR: rtt_input_handle Failed\r\n");
            APP_ERR_TRAP(tx_status);
        }

        /* Handle other threads that need output to the RTT Viewer */
        tx_status = rtt_output_handle();
        if(TX_SUCCESS != tx_status)
        {
            APP_PRINT("\r\nERROR: rtt_output_handle Failed\r\n");
            APP_ERR_TRAP(tx_status);
        }

        tx_thread_sleep(RTT_THREAD_SLEEP_TICK);
    }
}

UINT rtt_framework_init(void)
{
    UINT tx_status = TX_SUCCESS;

    /* Creates a memory byte pool for the RTT frame */
    tx_status = tx_byte_pool_create(&g_rtt_byte_pool, RTT_BYTE_POOL_NAME, (VOID *)g_byte_pool_memory,\
                                    RTT_BYTE_POOL_SIZE);
    if(TX_SUCCESS != tx_status)
    {
        return tx_status;
    }

    return tx_status;
}

UINT rtt_input_handle(void)
{
    UINT tx_status = TX_SUCCESS;
    CHAR rtt_buffer [BUFFER_SIZE_DOWN] = {NULL_CHAR};
    UINT rtt_len = RESET_VALUE;
    rtt_msg_t * p_rtt_msg = NULL;

    if(APP_CHECK_DATA)
    {
        rtt_len = APP_READ(rtt_buffer);

        /* Allocate memory for RTT frame and input message */
        tx_status = tx_byte_allocate(&g_rtt_byte_pool, (VOID **)&p_rtt_msg,\
                                     sizeof(rtt_msg_t) + rtt_len + RTT_INPUT_ALIGN, TX_WAIT_FOREVER);
        if(TX_SUCCESS != tx_status)
        {
            APP_PRINT("\r\nERROR: tx_byte_allocate failed\r\n");
            return tx_status;
        }

        /* Prepares the RTT frame */
        p_rtt_msg->id = RTT_INPUT_MESSAGE;
        p_rtt_msg->time = tx_time_get();
        p_rtt_msg->size = rtt_len + RTT_INPUT_ALIGN;
        memcpy(p_rtt_msg->msg, rtt_buffer, p_rtt_msg->size);

        /* Send the RTT frame pointer to input queue */
        tx_status = tx_queue_send(&g_rtt_input_queue, (VOID *)&p_rtt_msg, TX_WAIT_FOREVER);
        if(TX_SUCCESS != tx_status)
        {
            APP_PRINT("\r\nERROR: tx_queue_send failed\r\n");
            return tx_status;
        }

        return TX_SUCCESS;
    }

    return tx_status;
}

/*******************************************************************************************************************//**
 * @brief     This function prints data to the RTT Viewer based on the RTT frame taken from the RTT out queue.
 * @param[in] None
 * @retval    Any other error code apart from TX_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
UINT rtt_output_handle(void)
{
    UINT tx_status = TX_SUCCESS;
    UINT err = RESET_VALUE;
    UINT menu = RESET_VALUE;
    CHAR str_size [STR_UL64_MAX_LEN] = {RESET_VALUE};
    CHAR str_size_align [STR_UL64_MAX_LEN] = {RESET_VALUE};
    fsp_pack_version_t version = {RESET_VALUE};
    media_property_t media =
    {
     .phy_size   = RESET_VALUE,
     .total_size = RESET_VALUE,
     .free_size  = RESET_VALUE
    };

    dir_property_t dir =
    {
     .file      = RESET_VALUE,
     .subdir    = RESET_VALUE,
     .file_size = RESET_VALUE
    };

    volume_info_t volume =
    {
     .lable   = {NULL_CHAR},
     .number = RESET_VALUE
    };

    entry_info_t entry =
    {
     .name = {NULL_CHAR},
     .attr = RESET_VALUE,
     .size = RESET_VALUE,
     .time = {
              .month = RESET_VALUE,
              .date  = RESET_VALUE,
              .year  = RESET_VALUE,
              .hour  = RESET_VALUE,
              .min   = RESET_VALUE,
              .sec   = RESET_VALUE,
             }
    };
    rtt_msg_t * p_rtt_msg = NULL;

    while(true)
    {
        /* Get output queue from FileX thread */
        tx_status = tx_queue_receive(&g_rtt_output_queue, (VOID *)&p_rtt_msg, TX_NO_WAIT);
        if(TX_SUCCESS == tx_status)
        {
            switch(p_rtt_msg->id)
            {
                case RTT_INPUT_MESSAGE:
                    break;

                case RTT_OUTPUT_EP_BANNER:
                    R_FSP_VersionGet(&version);
                    APP_PRINT(BANNER_INFO,EP_VERSION,version.version_id_b.major,\
                              version.version_id_b.minor, version.version_id_b.patch );
                    break;

                case RTT_OUTPUT_EP_INFO:
                    APP_PRINT(EP_INFO);
                    break;

                case RTT_OUTPUT_APP_INFO_STR:
                    APP_PRINT("%s", p_rtt_msg->msg);
                    break;

                case RTT_OUTPUT_APP_ERR_STR:
                    APP_PRINT("\r\nERROR: %s\r\n", p_rtt_msg->msg);
                    break;

                case RTT_OUTPUT_APP_ERR_TRAP:
                    err = *(UINT*)p_rtt_msg->msg;
                    APP_ERR_TRAP(err);
                    break;

                case RTT_OUTPUT_APP_MENU:
                    menu = *(menu_t*)p_rtt_msg->msg;
                    APP_PRINT(gp_menu_content[menu]);

                    break;

                    case RTT_OUTPUT_APP_MEDIA_PROPERTY:
                        media = *(media_property_t*)p_rtt_msg->msg;
                        conv_ul64_to_str(media.phy_size, str_size, STR_UL64_MAX_LEN);
                        APP_PRINT("SD card total size is: %15s bytes\r\n", str_size);

                        conv_ul64_to_str(media.total_size, str_size, STR_UL64_MAX_LEN);
                        APP_PRINT("Media total size is:   %15s bytes\r\n", str_size);

                        conv_ul64_to_str(media.free_size, str_size, STR_UL64_MAX_LEN);
                        APP_PRINT("Media free size is:    %15s bytes\r\n", str_size);
                        break;

                    case RTT_OUTPUT_APP_MEDIA_VOLUME_INFO:
                        volume = *(volume_info_t*)p_rtt_msg->msg;
                        /* Print media volume information */
                        APP_PRINT("Volume label is %s\r\n", volume.lable);
                        APP_PRINT("Volume Serial Number is %d\r\n", volume.number);
                        APP_PRINT("\r\nDirectory of: %s \r\n\r\n", DIR_ROOT_NAME);
                        break;

                    case RTT_OUTPUT_APP_ENTRY_INFO:
                        entry = *(entry_info_t*)p_rtt_msg->msg;
                        if(FX_DIRECTORY == (entry.attr & FX_DIRECTORY))
                        {
                            /* Print directory information */
                            APP_PRINT("%.2u/%.2u/%.4u  %.2u:%.2u:%.2u    %s %s\r\n",
                                      entry.time.month, entry.time.date, entry.time.year,
                                      entry.time.hour, entry.time.min, entry.time.sec,
                                      "<DIR>          ", entry.name);
                        }
                        else if(FX_ARCHIVE == (entry.attr & FX_ARCHIVE))
                        {
                            /* Print file information */
                            conv_ul64_to_str(entry.size, str_size, STR_UL64_MAX_LEN);
                            snprintf(str_size_align, STR_UL64_MAX_LEN, "%15s", str_size);
                            APP_PRINT("%.2u/%.2u/%.4u  %.2u:%.2u:%.2u    %s %s\r\n",
                                      entry.time.month, entry.time.date, entry.time.year,
                                      entry.time.hour, entry.time.min, entry.time.sec,
                                      str_size_align, entry.name);
                        }
                        else
                        {
                            /* Do nothing */
                        }
                        break;

                    case RTT_OUTPUT_APP_DIR_PROPERTY:
                        dir = *(dir_property_t*)p_rtt_msg->msg;
                        /* Print directory property */
                        conv_ul64_to_str(dir.file_size, str_size, STR_UL64_MAX_LEN);
                        snprintf(str_size_align, STR_UL64_MAX_LEN, "%15s", str_size);
                        APP_PRINT("         %6u File(s) %s bytes\r\n", dir.file, str_size_align);
                        APP_PRINT("         %6u Dir(s)\r\n\r\n", dir.subdir);
                        break;

                default :
                    break;
            }

            /* Release print message allocated block */
            tx_status = tx_byte_release((void *)p_rtt_msg);
            if(TX_SUCCESS != tx_status)
            {
                return tx_status;
            }
        }

        /* In case queue is empty */
        else if(TX_QUEUE_EMPTY == tx_status)
        {
            return TX_SUCCESS;
        }

        /* In case the API is error */
        else
        {
            return tx_status;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function prepares the RTT frame and sends it to the RTT output queue.
 *            To be called by the User/Application Thread.
 * @param[in] id        Message id.
 * @param[in] size      Message size.
 * @param[in] p_data    Pointer to the message string.
 * @retval    Any other error code apart from TX_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
UINT send_data_to_rtt(rtt_event_id_t id, uint32_t size, void * const p_data)
{
    UINT tx_status = TX_SUCCESS;
    rtt_msg_t * p_rtt_msg = NULL;

    /* Allocate memory for RTT frame and output message */
    tx_status = tx_byte_allocate(&g_rtt_byte_pool, (VOID **)&p_rtt_msg, sizeof(rtt_msg_t) + size, TX_WAIT_FOREVER);
    if(TX_SUCCESS != tx_status)
    {
        APP_PRINT("\r\nERROR: tx_byte_allocate failed\r\n");
        return tx_status;
    }

    /* Prepare the RTT frame */
    p_rtt_msg->id = id;
    p_rtt_msg->time = tx_time_get();
    p_rtt_msg->size = size;
    memcpy(p_rtt_msg->msg, p_data, size);

    /* Send the RTT frame pointer to output queue */
    tx_status = tx_queue_send(&g_rtt_output_queue, (VOID *)&p_rtt_msg, TX_WAIT_FOREVER);

    return tx_status;
}

/*******************************************************************************************************************//**
 * @brief      This function gets and converts data to an integer from the RTT input queue.
 *             To be called by the User/Application Thread.
 * @param[out] p_data   Pointer to data buffer.
 * @retval     Any other error code apart from TX_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
UINT get_data_from_rtt(uint8_t * p_data)
{
    UINT tx_status = TX_SUCCESS;
    rtt_msg_t * p_rtt_msg = NULL;

    /* Wait for an input queue from the RTT thread */
    tx_status = tx_queue_receive(&g_rtt_input_queue, (VOID *)&p_rtt_msg , TX_WAIT_FOREVER);
    if(TX_SUCCESS != tx_status)
    {
        return tx_status;
    }

    if(RTT_INPUT_MESSAGE == p_rtt_msg->id)
    {
        if(NULL != p_rtt_msg)
        {
            /* Conversion from input string to integer value */
            *p_data = (uint8_t)(atoi(p_rtt_msg->msg));
        }
    }

    /* Free the memory allocated for this RTT frame */
    tx_status = tx_byte_release(p_rtt_msg);
    if(TX_SUCCESS != tx_status)
    {
        return tx_status;
    }

    return TX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       This function converts a ULONG 64 value to a string.
 * @param[in]   value   Value is of type ULONG 64 bits.
 * @param[out]  p_buf   Pointer to string buffer.
 * @param[in]   len     Length of input buffer.
 * @retval      None
 **********************************************************************************************************************/
static void conv_ul64_to_str(ULONG64 value, CHAR * p_buf, UINT len)
{
    UINT units     = RESET_VALUE;
    UINT thousands = RESET_VALUE;
    UINT millions  = RESET_VALUE;
    UINT billions  = RESET_VALUE;

    /* Clean input buffer */
    memset(p_buf, NULL_CHAR, len);

    /* Check the value range */
    if(BILLION <= value)
    {
        billions = (UINT)(value / BILLION);
        millions = (UINT)((value / MILLION) % THOUSAND);
        thousands = (UINT)((value / THOUSAND) % THOUSAND);
        units = (UINT)(value % THOUSAND);
        snprintf(p_buf, len, "%u,%.3u,%.3u,%.3u", billions, millions, thousands, units);
    }
    else if(MILLION <= value)
    {
        millions = (UINT)(value / MILLION);
        thousands = (UINT)((value / THOUSAND) % THOUSAND);
        units = (UINT)(value % THOUSAND);
        snprintf(p_buf, len, "%u,%.3u,%.3u", millions, thousands, units);
    }
    else if(THOUSAND <= value)
    {
        thousands = (UINT)(value / THOUSAND);
        units = (UINT)(value % THOUSAND);
        snprintf(p_buf, len, "%u,%.3u", thousands, units);
    }
    else
    {
        units = (UINT)value;
        snprintf(p_buf, len, "%u", units);
    }
}
