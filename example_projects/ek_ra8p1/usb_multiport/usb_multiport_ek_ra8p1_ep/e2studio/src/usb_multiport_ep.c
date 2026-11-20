/***********************************************************************************************************************
 * File Name    : usb_multiport_ep.c
 * Description  : Contains data structures and functions used in usb_multiport_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <string.h>
#include "usb_multiport_ep.h"

/* Global variables */
volatile bool g_write_to_usb = false;
uint16_t g_bytes_to_write = RESET_VALUE;
volatile usb_event_info_t * gp_usb_pcdc_event = NULL;

/* Private variables */
static FF_Disk_t s_my_disk;
static usb_pcdc_linecoding_t s_line_coding;
static uint8_t s_writebuf[READ_BUF_SIZE] = {RESET_VALUE};
static uint8_t s_tempbuf[READ_BUF_SIZE] = {RESET_VALUE};
static bool s_overflow = false;
static bool s_usb_pcdc_opened = false;
static bool s_fat_opened = false;
static bool s_disk_initialized = false;

/*******************************************************************************************************************//**
 * @brief     This function initializes the FreeRTOS+FAT instance.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t fat_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = RM_FREERTOS_PLUS_FAT_Open(&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_cfg);
    if (FSP_SUCCESS == err)
    {
        s_fat_opened = true;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the FreeRTOS+FAT instance.
 * @param[in] None.
 * @retval    FSP_SUCCESS               De-initialized successfully.
 * @retval    Any other error codes     Failed to de-initialize.
 **********************************************************************************************************************/
fsp_err_t fat_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    if (s_fat_opened)
    {
        err = RM_FREERTOS_PLUS_FAT_Close(&g_rm_freertos_plus_fat_ctrl);
        if (FSP_SUCCESS == err)
        {
            s_fat_opened = false;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the FreeRTOS+FAT disk and adds it to the file system.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t fat_disk_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    FF_Error_t ff_err = FF_ERR_NONE;
    rm_freertos_plus_fat_device_t device;

    memset(&device, RESET_VALUE, sizeof(device));

    /* Initialize the mass storage device. This should not be done until the device is plugged in and initialized */
    err = RM_FREERTOS_PLUS_FAT_MediaInit(&g_rm_freertos_plus_fat_ctrl, &device);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Initialize one disk for each partition used in the application */
    err = RM_FREERTOS_PLUS_FAT_DiskInit(&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_disk_cfg,
                                        &s_my_disk);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    s_disk_initialized = true;

    /* Mount each disk. This assumes the disk is already partitioned and formatted */
    ff_err = FF_Mount(&s_my_disk, s_my_disk.xStatus.bPartitionNumber);
    if (FF_ERR_NONE != ff_err)
    {
        return FSP_ERR_ABORTED;
    }

    /* Add the disk to the file system. FF_FS_Add() returns 0 on failure */
    ff_err = FF_FS_Add("/", &s_my_disk);
    if (FF_FS_ADD_FAILURE == ff_err)
    {
        return FSP_ERR_ABORTED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the FreeRTOS+FAT disk.
 * @param[in] None.
 * @retval    FSP_SUCCESS               De-initialized successfully.
 * @retval    Any other error codes     Failed to de-initialize.
 **********************************************************************************************************************/
fsp_err_t fat_disk_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (s_disk_initialized)
    {
        err = RM_FREERTOS_PLUS_FAT_DiskDeinit(&g_rm_freertos_plus_fat_ctrl, &s_my_disk);
        if (FSP_SUCCESS == err)
        {
            s_disk_initialized = false;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the USB PCDC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t usb_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_Open(&g_basic1_ctrl, &g_basic1_cfg);
    if (FSP_SUCCESS == err)
    {
        s_usb_pcdc_opened = true;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function reads data from the USB PCDC module.
 * @param[in]  p_api_ctrl               Pointer to the USB control block.
 * @param[out] p_buf                    Pointer to the buffer used to store the received data.
 * @param[in]  size                     Number of bytes to read.
 * @param[in]  destination              USB device class used for the read.
 * @retval     FSP_SUCCESS              Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
fsp_err_t usb_read(usb_ctrl_t * const p_api_ctrl, uint8_t * p_buf, uint32_t size, uint8_t destination)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_Read(p_api_ctrl, p_buf, size, destination);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function writes data to the USB PCDC module.
 * @param[in] p_api_ctrl                Pointer to the USB control block.
 * @param[in] p_buf                     Pointer to the buffer containing data to write.
 * @param[in] size                      Number of bytes to write.
 * @param[in] destination               USB device class used for the write.
 * @retval    FSP_SUCCESS               Wrote successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t usb_write(usb_ctrl_t * const p_api_ctrl, uint8_t const * const p_buf, uint32_t size, uint8_t destination)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_Write(p_api_ctrl, p_buf, size, destination);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function processes the USB PCDC class requests.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Processed successfully.
 * @retval    Any other error codes     Failed to process.
 **********************************************************************************************************************/
fsp_err_t usb_pcdc_class_request(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check for the specific CDC class request IDs */
    if (USB_PCDC_SET_LINE_CODING == (gp_usb_pcdc_event->setup.request_type & USB_BREQUEST))
    {
        err = R_USB_PeriControlDataGet(&g_basic1_ctrl, (uint8_t *)&s_line_coding, LINE_CODING_LENGTH);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    else if (USB_PCDC_GET_LINE_CODING == (gp_usb_pcdc_event->setup.request_type & USB_BREQUEST))
    {
        err = R_USB_PeriControlDataSet(&g_basic1_ctrl, (uint8_t *)&s_line_coding, LINE_CODING_LENGTH);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    else if (USB_PCDC_SET_CONTROL_LINE_STATE == (gp_usb_pcdc_event->setup.request_type & USB_BREQUEST))
    {
        err = R_USB_PeriControlStatusSet(&g_basic1_ctrl, USB_SETUP_STATUS_ACK);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    else
    {
        /* Do nothing */
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened USB PCDC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_deinit(void)
{
    /* De-initialize the opened USB PCDC module */
    if (s_usb_pcdc_opened)
    {
        R_USB_Close(&g_basic1_ctrl);
        s_usb_pcdc_opened = false;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the USB device to be detected, up to the configured wait time.
 * @param[in] None.
 * @retval    true                      USB device is detected.
 * @retval    false                     USB device is not detected within the wait time.
 **********************************************************************************************************************/
bool wait_till_usb_detected(void)
{
    bool status = false;
    EventBits_t ux_bits = xEventGroupWaitBits(g_event_group, MEDIA_INSERTED | MEDIA_REMOVED, pdFALSE, pdFALSE,
                                              (TickType_t)WAIT_TIME / portTICK_PERIOD_MS);

    if (ux_bits & MEDIA_INSERTED)
    {
        status = true;
    }
    else if (ux_bits & MEDIA_REMOVED)
    {
        status = false;
    }
    else
    {
        /* Do nothing */
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function checks the USB HMSC connection status.
 * @param[in] None.
 * @retval    true                      USB device is connected.
 * @retval    false                     USB device is not connected.
 **********************************************************************************************************************/
bool check_usb_connected(void)
{
    bool status = false;
    EventBits_t ux_bits = xEventGroupGetBits(g_event_group);

    if (ux_bits & MEDIA_INSERTED)
    {
        status = true;
    }
    else if (ux_bits & MEDIA_REMOVED)
    {
        status = false;
    }
    else
    {
        /* Do nothing */
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function buffers the received character and sends the buffer to the HMSC thread on Enter.
 * @param[in] p_buf                     Pointer to the buffer containing the received data.
 * @param[in] size                      Number of bytes received.
 * @retval    None.
 **********************************************************************************************************************/
void fill_data_buffer_and_write_to_file(uint8_t const * const p_buf, uint32_t size)
{
    static uint16_t write_buf_index = RESET_VALUE;

    if (RESET_VALUE < size)
    {
        /* Check for carriage return */
        if (CARRIAGE_RETURN == p_buf[RESET_VALUE])
        {
            /* Check for overflow flag */
            if (s_overflow)
            {
                g_bytes_to_write = WRITE_ITEM_SIZE;

                /* Arrange the circular buffer data from oldest to latest before writing to the file */
                uint16_t temp_buf_index = RESET_VALUE;

                /* Copy data from write_buf_index to end of buffer into temp buffer */
                for (uint16_t cnt = write_buf_index; cnt < READ_BUF_SIZE; cnt++)
                {
                    s_tempbuf[temp_buf_index++] = s_writebuf[cnt];
                }

                /* Copy data from 0 to write_buf_index into temp buffer */
                for (uint16_t cnt = RESET_VALUE; cnt < write_buf_index; cnt++)
                {
                    s_tempbuf[temp_buf_index++] = s_writebuf[cnt];
                }

                /* Copy data back to buffer */
                memcpy(s_writebuf, s_tempbuf, READ_BUF_SIZE);
            }

            /* Send data to HMSC thread by queue */
            if (pdTRUE == xQueueSend(g_queue_data_from_pcdc, s_writebuf, (TickType_t)(RESET_VALUE)))
            {
                /* Reset index and flags */
                write_buf_index = RESET_VALUE;
                s_overflow = false;
                g_write_to_usb = false;
            }
        }
        else
        {
            /* Check for buffer size index */
            if (READ_BUF_SIZE == write_buf_index)
            {
                /* Reset the index */
                write_buf_index = RESET_VALUE;

                /* Set the flag */
                s_overflow = true;
            }

            s_writebuf[write_buf_index++] = p_buf[RESET_VALUE];
            g_bytes_to_write++;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for the FreeRTOS+FAT module, triggered when the USB flash drive is removed or
 *            inserted.
 * @param[in] p_args                    Callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void free_rtos_fat_callback(rm_freertos_plus_fat_callback_args_t * p_args)
{
    if (MEDIA_INSERTED == p_args->event)
    {
        xEventGroupClearBits(g_event_group, MEDIA_REMOVED);
    }
    else if (MEDIA_REMOVED == p_args->event)
    {
        xEventGroupClearBits(g_event_group, MEDIA_INSERTED);
    }
    else
    {
        /* Do nothing */
    }

    xEventGroupSetBits(g_event_group, p_args->event);
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for the USB PCDC module.
 * @param[in] p_usb_event               Pointer to the USB event information.
 * @param[in] task                      Handle to the calling task (unused).
 * @param[in] state                     USB on/off state (unused).
 * @retval    None.
 **********************************************************************************************************************/
void usb_pcdc_callback(usb_event_info_t * p_usb_event, usb_hdl_t task, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED(task);
    FSP_PARAMETER_NOT_USED(state);

    /* Queue the USB event information */
    (void)xQueueSendFromISR(g_event_queue, (const void *)&p_usb_event, NULL);
}
