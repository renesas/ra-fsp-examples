/***********************************************************************************************************************
* File Name    : fat_thread_entry.c
* Description  : Implements FAT file system thread, including handling input, output, and media events.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "freertos_plus_fat_internal_data.h"
#include "fat_operation.h"
#include "fat_thread.h"
#include "usb_otg_msc.h"
#include "common_app.h"

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern TaskHandle_t usb_otg_msc_thread;
extern TaskHandle_t fat_thread;

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static time_t g_system_time = 0;
static time_t g_start_time = 0;

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/
fat_disk_t g_internal_fat_disk;
fat_disk_t g_usb_fat_disk;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static void fat_operation(char input);
static void fat_time_init(void);

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief      Retrieves the current FreeRTOS time.
 * @param[out] pxTime  Pointer to store the current time (unused).
 * @retval     Current time calculated as g_start_time + g_system_time.
 **********************************************************************************************************************/
time_t FreeRTOS_time(time_t * pxTime)
{
    FSP_PARAMETER_NOT_USED(pxTime);
    return (g_start_time + g_system_time);
}

/*******************************************************************************************************************//**
 * @brief      Timer callback to increment System time.
 * @param[in]  xTimer  Timer handle.
 * @return     None.
 **********************************************************************************************************************/
void system_timer_callback(TimerHandle_t xTimer)
{
    FSP_PARAMETER_NOT_USED(xTimer);
    /* Increment the System time by one second */
    g_system_time++;
}

/*******************************************************************************************************************//**
 * @brief Callback function for handling USB media events in FreeRTOS+FAT.
 *
 * This function handles media insertion and removal events for USB storage.
 * Depending on the event, it sends a notification to the `fat_thread` task.
 *
 * @param[in] p_args Pointer to callback argument structure containing the event type.
 **********************************************************************************************************************/
void freertos_plus_fat_usb_callback(rm_freertos_plus_fat_callback_args_t *p_args)
{
    /* Check if the event indicates that USB media has been inserted */
    if (p_args->event & RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_INSERTED)
    {
        /* Notify the FAT thread about USB media insertion */
        xTaskNotifyFromISR(fat_thread, FAT_USB_DISK_INSERTED, eSetBits, NULL);
    }
    /* Check if the event indicates that USB media has been removed */
    else if (p_args->event & RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED)
    {
        /* Notify the FAT thread about USB media removal */
        xTaskNotifyFromISR(fat_thread, FAT_USB_DISK_REMOVED, eSetBits, NULL);
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 * @brief Callback function for handling internal media events in FreeRTOS+FAT.
 *
 * This function handles media insertion and removal events for internal storage.
 * Depending on the event, it sends a notification to the `fat_thread` task.
 *
 * @param[in] p_args Pointer to callback argument structure containing the event type.
 **********************************************************************************************************************/
void freertos_plus_fat_internal_callback(rm_freertos_plus_fat_callback_args_t *p_args)
{
    /* Check if the event indicates that internal media has been inserted */
    if (p_args->event & RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_INSERTED)
    {
        /* Notify the FAT thread about internal media insertion */
        xTaskNotifyFromISR(fat_thread, FAT_INTERNAL_DISK_INSERTED, eSetBits, NULL);
    }
    /* Check if the event indicates that internal media has been removed */
    else if (p_args->event & RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED)
    {
        /* Notify the FAT thread about internal media removal */
        xTaskNotifyFromISR(fat_thread, FAT_INTERNAL_DISK_REMOVED, eSetBits, NULL);
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 * @brief      FAT file system thread entry function for handling FAT FS commands.
 *
 * This function initializes Internal and USB disks for the FAT file system and handles commands and events
 * related to disk operations. It listens for notifications about user input and disk events, performs
 * appropriate actions, and continuously processes commands from the terminal.
 *
 * @param[in]  pvParameters Task handle (unused in this implementation).
 * @return     None.
 **********************************************************************************************************************/
void fat_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    fsp_err_t status = FSP_SUCCESS;
    BaseType_t os_status = pdTRUE;
    uint32_t notified;

    uint32_t msg_size = 0;
    char msg_buff[64];

    /* Initialize the file system time */
    fat_time_init();

    /* Wait until the terminal framework is initialized */
    term_framework_init_check();

    /* Create the internal disk for the FAT file system */
    status = fat_disk_create(&g_internal_fat_disk, FAT_INTERNAL_DISK_PATH, true,
                             &g_rm_freertos_plus_fat_internal,
                             &g_rm_freertos_plus_fat_internal_disk_cfg);
    TERM_ERR_TRAP(FSP_SUCCESS != status, status, "Failed to create the internal disk for the FAT file system.\r\n");

    /* Create the USB disk for the FAT file system */
    status = fat_disk_create(&g_usb_fat_disk, FAT_USB_DISK_PATH, false,
                             &g_rm_freertos_plus_fat_usb,
                             &g_rm_freertos_plus_fat_usb_disk_cfg);
    TERM_ERR_TRAP(FSP_SUCCESS != status, status, "Failed to create the USB disk for the FAT file system.\r\n");

    /* Main loop for input and event processing */
    while (true)
    {
        /* Wait for any notification from callback functions or terminal input */
        while (true)
        {
            os_status = xTaskNotifyWait(NOTIFY_NO_CLEAR, NOTIFY_ALL_CLEAR, &notified, portMAX_DELAY);
            if (pdTRUE != os_status)
            {
                PRINT_ERR_STR("Failed to receive a notification for FAT events.");
            }
            else
            {
                break;
            }
        }

        /* Handle user terminal input commands */
        if (FAT_GENERAL_INPUT_COMMAND == (notified & FAT_GENERAL_INPUT_COMMAND))
        {
            memset(msg_buff, 0, sizeof(msg_buff));
            status = term_get_input_queue(msg_buff, &msg_size, 0);
            TERM_ERR_TRAP(FSP_SUCCESS != status, status, "Failed to get the user input.");

            /* Ensure both USB and internal disks are mounted before processing commands */
            if (true == g_usb_fat_disk.is_mount && true == g_internal_fat_disk.is_mount)
            {
                /* Process user input commands based on the first character of the input */
                fat_operation(msg_buff[0]);

                /* Print the menu for further inputs */
                PRINT_EP_MENU;
            }
        }

        /* Handle USB disk insert event */
        if (FAT_USB_DISK_INSERTED == (notified & FAT_USB_DISK_INSERTED))
        {
            PRINT_INFO_STR("USB disk inserted.");
            status = fat_disk_mount(&g_usb_fat_disk);
            TERM_ERR_TRAP(FSP_SUCCESS != status, status, "Failed to mount the USB disk.");
            if (g_usb_fat_disk.is_mount && g_internal_fat_disk.is_mount)
            {
                PRINT_EP_MENU;
            }
        }

        /* Handle USB disk remove event */
        if (FAT_USB_DISK_REMOVED == (notified & FAT_USB_DISK_REMOVED))
        {
            PRINT_INFO_STR("USB disk removed.");
            status = fat_disk_unmount(&g_usb_fat_disk);
            TERM_ERR_TRAP(FSP_SUCCESS != status, status, "Failed to unmount the USB disk.");
        }

        /* Handle internal disk insert event */
        if (FAT_INTERNAL_DISK_INSERTED == (notified & FAT_INTERNAL_DISK_INSERTED))
        {
            PRINT_INFO_STR("Internal disk inserted.");
            status = fat_disk_mount(&g_internal_fat_disk);
            TERM_ERR_TRAP(FSP_SUCCESS != status, status, "Failed to mount the internal disk.");
            if (g_usb_fat_disk.is_mount && g_internal_fat_disk.is_mount)
            {
                PRINT_EP_MENU;
            }
        }

        /* Handle internal disk remove event */
        if (FAT_INTERNAL_DISK_REMOVED == (notified & FAT_INTERNAL_DISK_REMOVED))
        {
            PRINT_INFO_STR("Internal disk removed.");
            status = fat_disk_unmount(&g_internal_fat_disk);
            TERM_ERR_TRAP(FSP_SUCCESS != status, status, "Failed to unmount the internal disk.");
        }

        /* Handle request to open all disks */
        if (FAT_ALL_DISK_REQUEST_OPEN == (notified & FAT_ALL_DISK_REQUEST_OPEN))
        {
            status = fat_disk_open(&g_usb_fat_disk);
            TERM_ERR_TRAP(FSP_SUCCESS != status, status, "Failed to open the USB disk.");

            status = fat_disk_open(&g_internal_fat_disk);
            TERM_ERR_TRAP(FSP_SUCCESS != status, status, "Failed to open the internal disk.");

            status = fat_disk_mount(&g_internal_fat_disk);
            TERM_ERR_TRAP(FSP_SUCCESS != status, status, "Failed to mount the internal disk.");
        }

        /* Handle request to close all disks */
        if (FAT_ALL_DISK_REQUEST_CLOSE == (notified & FAT_ALL_DISK_REQUEST_CLOSE))
        {
            status = fat_disk_unmount(&g_usb_fat_disk);
            TERM_ERR_TRAP(FSP_SUCCESS != status, status, "Failed to unmount the USB disk.");

            status = fat_disk_unmount(&g_internal_fat_disk);
            TERM_ERR_TRAP(FSP_SUCCESS != status, status, "Failed to unmount the internal disk.");

            status = fat_disk_close(&g_usb_fat_disk);
            TERM_ERR_TRAP(FSP_SUCCESS != status, status, "Failed to close the USB disk.");

            status = fat_disk_close(&g_internal_fat_disk);
            TERM_ERR_TRAP(FSP_SUCCESS != status, status, "Failed to close the internal disk.");

            /* Notify USB OTG thread to switch to device mode */
            os_status = xTaskNotify(usb_otg_msc_thread, USB_OTG_CHANGE_TO_DEVICE, eSetBits);
            TERM_ERR_TRAP(pdTRUE != os_status, os_status, "Failed to notify USB OTG thread to switch to device mode.");
        }

        /* Yield the MCU to other tasks */
        vTaskDelay(FAT_THREAD_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief      Initializes the FAT file system time based on build date and time.
 * @return     None.
 **********************************************************************************************************************/
static void fat_time_init(void)
{
    const char *p_mon_str[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    FF_TimeStruct_t build_time = {0};
    char month[4];
    int day, year, hour, min, sec;

    sscanf(__DATE__, "%3s %d %d", month, &day, &year);

    /* Convert month string to integer */
    build_time.tm_mon = 0;
    for (int32_t value = 0; value < (int32_t)(sizeof(p_mon_str) / sizeof(p_mon_str[0])); value++)
    {
        if (0 == strncmp(p_mon_str[value], month, 3))
        {
            build_time.tm_mon = value;
            break;
        }
    }

    build_time.tm_mday = day;
    build_time.tm_year = year - 1900;

    sscanf(__TIME__, "%d:%d:%d", &hour, &min, &sec);
    build_time.tm_hour = hour;
    build_time.tm_min = min;
    build_time.tm_sec = sec;

    /* Initialize start time */
    g_start_time = FreeRTOS_mktime(&build_time);

    /* Start the System timer */
    xTimerStart(g_system_timer, TICK_NO_DELAY);
}

/*******************************************************************************************************************//**
 * @brief      This function performs fat operations based on user requests.
 * @param[in]  input    User input.
 * @return     None.
 **********************************************************************************************************************/
static void fat_operation(char input)
{
    fsp_err_t status = FSP_SUCCESS;
    switch (input)
    {
        case '1':
            PRINT_INFO_STR("Listing the root directory of the USB disk...");
            fat_dir_list(g_usb_fat_disk.p_ff_fs_path);
            break;

        case '2':
            PRINT_INFO_STR("Starting USB disk formatting...");
            status = fat_disk_format(&g_usb_fat_disk);
            if (FSP_SUCCESS != status)
            {
                PRINT_ERR_STR("USB disk formatting failed.");
            }
            else
            {
                PRINT_INFO_STR("USB disk formatting was successful.");
            }
            break;

        case '3':
            PRINT_INFO_STR("Creating and writing data to a file on the USB disk...");
            status = fat_file_create(FAT_USB_FILE_PATH);
            if (FSP_SUCCESS != status)
            {
                PRINT_ERR_STR("Failed to create and write data to the file.");
            }
            else
            {
                PRINT_INFO_STR("File creation and data writing were successful.");
            }
            break;

        case '4':
            PRINT_INFO_STR("Copying a file from the USB disk to the internal disk...");
            status = fat_file_copy(FAT_USB_FILE_PATH, FAT_INTERNAL_FILE_COPY_PATH);
            if (FSP_SUCCESS != status)
            {
                PRINT_ERR_STR("File copy to the internal disk failed.");
            }
            else
            {
                PRINT_INFO_STR("File copy to the internal disk was successful.");
            }
            break;

        case '5':
            PRINT_INFO_STR("Listing the root directory of the internal disk...");
            fat_dir_list(g_internal_fat_disk.p_ff_fs_path);
            break;

        case '6':
            PRINT_INFO_STR("Starting internal disk formatting...");
            status = fat_disk_format(&g_internal_fat_disk);
            if (FSP_SUCCESS != status)
            {
                PRINT_ERR_STR("Internal disk formatting failed.");
            }
            else
            {
                PRINT_INFO_STR("Internal disk formatting was successful.");
            }
            break;

        case '7':
            PRINT_INFO_STR("Creating and writing data to a file on the internal disk...");
            status = fat_file_create(FAT_INTERNAL_FILE_PATH);
            if (FSP_SUCCESS != status)
            {
                PRINT_ERR_STR("Failed to create and write data to the file.");
            }
            else
            {
                PRINT_INFO_STR("File creation and data writing were successful.");
            }
            break;

        case '8':
            PRINT_INFO_STR("Copying a file from the internal disk to the USB disk...");
            status = fat_file_copy(FAT_INTERNAL_FILE_PATH, FAT_USB_FILE_COPY_PATH);
            if (FSP_SUCCESS != status)
            {
                PRINT_ERR_STR("File copy to the USB disk failed.");
            }
            else
            {
                PRINT_INFO_STR("File copy to the USB disk was successful.");
            }
            break;

        default:
            PRINT_INFO_STR("Invalid input. Please select a valid option.");
            break;
    }
}
