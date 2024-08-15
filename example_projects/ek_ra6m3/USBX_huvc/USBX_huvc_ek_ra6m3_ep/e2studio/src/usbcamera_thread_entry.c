/***********************************************************************************************************************
* File Name    : usbcamera_thread_entry.c
* Description  : Contains data structures and functions used in usbcamera_thread_entry.c
**********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @addtogroup USBX_huvc_ep
 * @{
 **********************************************************************************************************************/
#include "usbcamera_thread.h"
#include "uvccamera_cfg.h"
#include "usbx_host_uvc_common.h"
#include "common_utils.h"
#include "dht_table.h"

/* Mem-pool size of 18k is required for USBX host-class pre-built libraries
 * and it is valid only if it with default USBX configurations. */
static uint32_t g_ux_uvc_pool_memory[MEMPOOL_SIZE / BYTE_SIZE];

/*Private global variables*/
static uvc_camera_cfg_t     uvc_cfg;
static fb_data_t            fb_data;
static UINT                 g_video_status                  = STOP;
static UCHAR                completed_transfer_index        = BUFFER_ZERO;
static UCHAR                fb_buffer[JPEG_BUFFER_NUMBER][JPEG_BUFFER_SIZE];
static UCHAR                video_buffer[VIDEO_BUFFER_SIZE];
static uvc_camera_cfg_t     *thread_data                    = &uvc_cfg;
static ULONG                max_buffer_size                 = RESET_VALUE;
static ULONG                actual_flags;

/*Private function prototypes*/
UINT ux_host_usr_event_notification(ULONG event, UX_HOST_CLASS * host_class, VOID * instance);
VOID uvc_transfer_request_done_callback(UX_TRANSFER * transfer_request);

static void usb_deinit(UINT err, const char *err_str);

/*******************************************************************************************************************//**
* @brief      This function is invoked to indicate the transfer completion from the USB camera.
* @param      UX_TRANSFER *transfer_request
* @retval     none
**********************************************************************************************************************/
VOID uvc_transfer_request_done_callback(UX_TRANSFER *transfer_request)
{
    uvc_cfg.completed_transfer_length[uvc_cfg.callback_index] = transfer_request->ux_transfer_request_actual_length;
    uvc_cfg.callback_index++;
    if (uvc_cfg.callback_index == array_count(uvc_cfg.completed_transfer_length))
        uvc_cfg.callback_index = ZERO;
    tx_semaphore_put(&g_payload_semaphore);
}


/*******************************************************************************************************************//**
* @brief      This function is invoked to insert DHT table to the local JPEG Structure and align it.
*             This is needed for e2studio or other image viewers to display the memory window using JPEG format.
* @param      UCHAR **p_mjpeg,  UINT img_size
* @retval     The new image buffer length
**********************************************************************************************************************/
static UINT dht_insert(UCHAR **p_mjpeg, UINT img_size)
{
    UCHAR *temp_str = *p_mjpeg;
    volatile UINT predht_size = RESET_VALUE, temp_img_size = RESET_VALUE, dht_offset = RESET_VALUE;
    USHORT dht_size = DHT_TABLE_SIZE, index = RESET_VALUE;
    USHORT *index_ptr = (unsigned short*)p_mjpeg;
    UINT append_bytes = RESET_VALUE;

    temp_img_size = img_size;
    /* Calculate bytes zero need to make JPEG aligned */
    append_bytes = (EIGHT_BYTES - ((img_size + dht_size) % EIGHT_BYTES));

    do {
        if (*index_ptr == DHT_MARKER)
        {
            return img_size;
        }
        else if (*index_ptr == SOS_MARKER)
        {
            do {
                *(*p_mjpeg + (temp_img_size + dht_size)) = *(*p_mjpeg + temp_img_size);
                *(*p_mjpeg + temp_img_size) = RESET_VALUE;

                temp_img_size--;
            }while (temp_img_size != (predht_size - 1));

            dht_offset = predht_size;

            /* Insert the DHT table */
            do {
                *(*p_mjpeg + dht_offset) = dht_table[index];
                index++;
                dht_offset++;
                dht_size--;
            }while (dht_size > ZERO);

            /* Pad it with zero to make it aligned */
            index = RESET_VALUE;
            while(append_bytes > ZERO)
            {
                *(*p_mjpeg + (img_size + DHT_TABLE_SIZE + index)) = RESET_VALUE;
                append_bytes--;
                index++;
            }
        }
        else
        {
            temp_str = temp_str + JPEG_IMG_BYTESPP;
            index_ptr = (unsigned short*)temp_str;
            predht_size += JPEG_IMG_BYTESPP;
        }
    }while ((predht_size - 1) != temp_img_size);

    return (img_size + DHT_TABLE_SIZE + (EIGHT_BYTES - ((img_size + DHT_TABLE_SIZE) % EIGHT_BYTES)));
}



/*******************************************************************************************************************//**
* @brief      USBX Host event notification callback function
* @param      ULONG event, UX_HOST_CLASS * host_class, VOID * instance
* @retval     UX_SUCCESS
**********************************************************************************************************************/
UINT ux_host_usr_event_notification(ULONG event, UX_HOST_CLASS * host_class, VOID * instance)
{
    /* Check the class and pass to specific host class event callback handle.  */
    if (UX_SUCCESS == ux_utility_memory_compare ("ux_host_class_video", host_class, ux_utility_string_length_get ((UCHAR *)"ux_host_class_video")))
    {
        if (UX_DEVICE_INSERTION == event) /* Check if there is a device insertion. */
        {
            if (uvc_cfg.video_host_class == UX_NULL)
            {
                /* Retrieving the Video class instance */
                uvc_cfg.video_host_class = instance;
                PRINT_INFO_STR("Camera USB Inserted.");
                /* Set the event flag to let application know the device insertion. */
                tx_event_flags_set (&g_device_insert_eventflag,EVENTFLAG_USB_DEVICE_INSERTED, TX_OR);
            }

        }
        else if(UX_DEVICE_REMOVAL == event)
        {
            if (uvc_cfg.video_host_class == instance)
            {
                /* Update USB Host Video Class instance to NULL*/
                uvc_cfg.video_host_class = NULL;
                PRINT_INFO_STR("Camera USB Removed.");
                /* Clear the event flag in case the camera was removed before the application could clear it. */
                tx_event_flags_set (&g_device_insert_eventflag, ~EVENTFLAG_USB_DEVICE_INSERTED, TX_AND);
            }
        }
    }

    return UX_SUCCESS;
}

/*******************************************************************************************************************//**
* @brief      This function is invoked to start and stop the video capture based on the RTT viewer user input.
* @param      none
* @retval     none
**********************************************************************************************************************/
static void usbx_huvc_operation(void)
{
    rtt_msg_t        *r_data1 = NULL;
    UINT             status = UX_SUCCESS;
    uint8_t          read_data = RESET_VALUE;

    /* Receive user input from RTT viewer */
    tx_queue_receive (&g_rtt_ip_data_queue, (VOID*) &r_data1, TX_NO_WAIT);
    if ((NULL != r_data1) && (RTT_INPUT_MESSAGE_TYPE_STR_QUEUED == r_data1->msg_id))
    {
        /* Conversion from input string to integer value */
        read_data = (uint8_t) (atoi ((char*) r_data1->p_msg));
        switch (read_data)
        {
            case START_VIDEO:
            {
                if (START != g_video_status)
                {
                    /* Start video transfer. */
                    status = ux_host_class_video_start (thread_data->video_host_class);
                    if (UX_SUCCESS != status)
                    {
                        PRINT_ERR_STR("failed to start streaming video");
                        /* Stop video transfer. */
                        status = ux_host_class_video_stop (thread_data->video_host_class);
                        usb_deinit (status, "failed to stop video");
                    }
                    else
                    {
                        /* Set g_video_status to start video channel */
                        g_video_status = START;
                        PRINT_INFO_STR("Start streaming video successfully.");
                    }
                }
                else
                {
                    PRINT_INFO_STR("Video channel has started");
                }

                break;
            }
            case STOP_VIDEO_AND_CAPTURE_IMAGE:
            {

                if (START == g_video_status)
                {
                    /* Set g_video_status flag to capture image */
                    g_video_status = CAPTURE_IMAGE;
                    PRINT_INFO_STR("Stop video and capture image.");
                }
                else
                {
                    PRINT_INFO_STR("Video channel has stopped");
                }
                break;
            }
            default:
            {
                PRINT_INFO_STR("Invalid Option Selected.");
                app_rtt_print_data (RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);
                break;
            }
        }

    }
    /* release print message allocated block */
    tx_byte_release (r_data1);
}
/*******************************************************************************************************************//**
* @brief      This function is invoked to wait for camera insertion and setup video channel.
* @param      none
* @retval     none
**********************************************************************************************************************/
static void wait_for_camera_insertion(void)
{
    UINT    status = UX_SUCCESS;

    /* Suspend here until a USBX Host Class Instance gets ready. */
    tx_event_flags_get (&g_device_insert_eventflag, thread_data->device_inserted_eventflag, TX_OR,(ULONG*) &actual_flags, TX_WAIT_FOREVER);
    /* This delay is required for now to get valid ISO IN UX_ENDPOINT instance. */
    tx_thread_sleep (WAIT_100_TICKS);

    /* Set camera parameters. */
     status = ux_host_class_video_frame_parameters_set (thread_data->video_host_class, UX_HOST_CLASS_VIDEO_VS_FORMAT_MJPEG, VIDEO_RESOLUTION_X, VIDEO_RESOLUTION_Y, VIDEO_FRAME_INTERVAL_30FPS);
     usb_deinit(status, "failed to set video parameters");
    /* Set the transfer callback function of video class. */
    ux_host_class_video_transfer_callback_set (thread_data->video_host_class, uvc_transfer_request_done_callback);
    /* Clear semaphore to zero; this ensures that while reading transfer requests, we don't trick ourselves into thinking we read one. */
    tx_semaphore_get (&g_payload_semaphore, WAIT_10_TICKS);
    /* Allocate space for video buffer. */
    for (int index = RESET_VALUE; index < (int) array_count(thread_data->video_data_ptrs); index++)
    {
        thread_data->video_data_ptrs[index] = &video_buffer[index];
    }
    /*  Get the maximum memory buffer size.*/
    max_buffer_size = ux_host_class_video_max_payload_get (thread_data->video_host_class);
}
/*******************************************************************************************************************//**
* @brief      Usb camera thread entry function
* @param      none
* @retval     none
**********************************************************************************************************************/
void usbcamera_thread_entry(void)
{
    fsp_err_t               err         = FSP_SUCCESS;
    UINT                    status      = UX_SUCCESS;
    UINT                    frame_index = RESET_VALUE;
    _Bool                   is_eof      = false;
    UINT                    fb_index    = BUFFER_ZERO;
    volatile jpeg_stream_t  jpeg_stream;
    uvc_stream_header_t     *p_uvc_stream_header;

    /* Initialize the RTT Thread.*/
    rtt_thread_init_check ();
    /* Print the banner and EP info. */
    app_rtt_print_data (RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);
    /* Print the main menu. */
    app_rtt_print_data (RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);

    /* ux_system_initialization. */
    status = ux_system_initialize(g_ux_uvc_pool_memory, MEMPOOL_SIZE, UX_NULL, UX_ZERO);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_system_initialize API Failed..");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("UX system initialized successfully!");
    /* Host stack initialization. */
    status = ux_host_stack_initialize(ux_host_usr_event_notification);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_host_stack_initialize API Failed..");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("UX host stack initialized successfully!");

    /* Start the USB module by calling open function. */
    err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_USB_Open API Failed..");
        /* Uninitializes the host stack for USB */
        status =  ux_host_stack_uninitialize();
        /* Handle error in-case of failure */
        if (UX_SUCCESS != status)
        {
            PRINT_ERR_STR("ux_host_stack_uninitialize failed");
        }
        ERROR_TRAP(err);
    }

    PRINT_INFO_STR("RA USBX Host UVC Initialization Completed.");
    memset(&fb_buffer,RESET_VALUE,sizeof(fb_buffer));
    thread_data->device_inserted_eventflag = EVENTFLAG_USB_DEVICE_INSERTED;
    thread_data->uvc_transfer_request_done_callback = uvc_transfer_request_done_callback;
    /*  Wait until usb video camera inserted.*/
    wait_for_camera_insertion();
    while (true)
    {
        /* Wait until device inserted if USB is unplugged.*/
        tx_event_flags_get (&g_device_insert_eventflag, thread_data->device_inserted_eventflag, TX_OR, (ULONG*) &actual_flags, TX_NO_WAIT);
        if (!(actual_flags & thread_data->device_inserted_eventflag))
        {
            wait_for_camera_insertion ();
        }
        /* Below section of code will execute when USB video camera is inserted */
        /* Performs the USBX huvc operation .*/
        usbx_huvc_operation();
        if (STOP != g_video_status)
        {
            /* Add the buffer for video transfer.*/
            status = ux_host_class_video_transfer_buffer_add (thread_data->video_host_class,
                                                              thread_data->video_data_ptrs[completed_transfer_index]);
            if (UX_SUCCESS != status)
            {
                /* Stop video transfer. */
                if (UX_SUCCESS != ux_host_class_video_stop (thread_data->video_host_class))
                {
                    PRINT_ERR_STR("failed to stop video channel");
                }
                usb_deinit(status, "failed to add the buffer for video transfer");
            }
            /* Suspend here until a transfer callback is called . */
            tx_semaphore_get (&g_payload_semaphore, TX_WAIT_FOREVER);
            /* Was there any data received in this transfer? */
            if (ZERO != thread_data->completed_transfer_length[completed_transfer_index])
            {
                /* Save the video stream header. */
                p_uvc_stream_header = (uvc_stream_header_t*) (thread_data->video_data_ptrs[completed_transfer_index]);

                if (p_uvc_stream_header->bmHeaderInfo & UVC_STREAM_HEADER_HEADERINFO_ERR)
                {
                    /* It may have only been this packet that had an error, so continue. */
                    PRINT_ERR_STR("Error in packet header");
                }
                /* Get JPEG data address and the size. */
                jpeg_stream.p_pointer = thread_data->video_data_ptrs[completed_transfer_index] + UVC_VIDEO_PAYLOAD_HEADER_SIZE;
                jpeg_stream.size = thread_data->completed_transfer_length[completed_transfer_index] - UVC_VIDEO_PAYLOAD_HEADER_SIZE;
                if (ZERO != jpeg_stream.size)
                {
                    memcpy((ULONG*) &fb_buffer[fb_index][frame_index], (ULONG*) jpeg_stream.p_pointer,max_buffer_size);
                    frame_index += jpeg_stream.size;
                    /* Search for the Start of Frame. */
                    if (SOF_MARKER == *(USHORT*) jpeg_stream.p_pointer)
                    {
                        jpeg_stream.flag = UVC_JPEG_DATA_FLAG_SOF;
                    }
                    /* Check the end of UVC packet for a JPEG frame. */
                    else if (p_uvc_stream_header->bmHeaderInfo & UVC_STREAM_HEADER_HEADERINFO_EOF)
                    {
                        jpeg_stream.flag = UVC_JPEG_DATA_FLAG_EOF;
                    }
                    else
                    {
                        jpeg_stream.flag = UVC_JPEG_DATA_FLAG_MID;
                    }

                    is_eof = !!(jpeg_stream.flag == UVC_JPEG_DATA_FLAG_EOF);
                    if (is_eof)
                    {
                        /* Get fb_data address and the size. */
                        fb_data.p_pointer = &fb_buffer[fb_index][0];
                        fb_data.size = frame_index;
                        /* Append DHT table to the local JPEG Structure. */
                        dht_insert (&fb_data.p_pointer, fb_data.size);
                        if (CAPTURE_IMAGE == g_video_status)
                        {
                            /* Stop video after inserted DHT table. */
                            status = ux_host_class_video_stop (thread_data->video_host_class);
                            usb_deinit(status, "failed to stop video channel");
                            /* Set g_video_status to stop video. */
                            g_video_status = STOP;
                        }
                        /* Swap the active frame buffer */
                        fb_index = (fb_index == BUFFER_ZERO) ? BUFFER_ONE : BUFFER_ZERO;
                        frame_index = RESET_VALUE;
                    }
                }
        }
        completed_transfer_index++;
        if (completed_transfer_index == array_count(thread_data->video_data_ptrs))
            completed_transfer_index = BUFFER_ZERO;
        }
    }
}

/******************************************************************************************************************
 * @brief       This function closes the opened usb instance and uninitializes the host stack for USB.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void usb_deinit(UINT err, const char *err_str)
{
    if (UX_SUCCESS != err)
    {
        APP_ERR_PRINT(err_str);
        /*Close opened USB interface*/
        if (FSP_SUCCESS != R_USB_Close (&g_basic0_ctrl))
        {
            PRINT_ERR_STR("R_USB_Close API Failed");
        }

        /* Uninitializes the host stack for USB */
        if (UX_SUCCESS != ux_host_stack_uninitialize ())
        {
            PRINT_ERR_STR("ux_host_stack_uninitialize failed");
        }
        APP_ERR_TRAP(err);
    }
}
/*******************************************************************************************************************//**
 * @} (end addtogroup USBX_huvc_ep)
 **********************************************************************************************************************/
