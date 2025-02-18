/***********************************************************************************************************************
 * File Name    : usbx_hmsc_thread_entry.c
 * Description  : Contains data structures and functions used in usbx_hmsc_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "usbx_hmsc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup USBX_hmsc_ep
 * @{
 **********************************************************************************************************************/

/*Global variables */
static uint8_t  g_read_buf[VALUE_32]  = {RESET_VALUE};
static uint8_t  g_write_buf[VALUE_32] = {RESET_VALUE};;
static CHAR     file_name[VALUE_16]   = "usbx_hmsc.txt";
static uint8_t  g_ux_pool_memory[MEMPOOL_SIZE];
static FX_FILE  g_file;
static FX_MEDIA * g_p_media           = UX_NULL;
static ULONG    actual_flags          = RESET_VALUE;

/* Variable to store time*/
static time_format_t g_set_time = {RESET_VALUE};

/* Private functions */
static void assign_month_value(time_format_t * time, char * read_buffer);
static UINT usbx_hmsc_file_directory_creation(void);
static UINT usbx_hmsc_file_write(void);
static UINT usbx_hmsc_file_read(void);
static UINT usbx_safely_eject(void);
static UINT usbx_display_working_directory(void);
static void deinit_media(void);
static UINT usbx_hmsc_operation(uint8_t read_data);
static void time_stamping(void);

/*******************************************************************************************************************//**
 * @brief     In this function, checks the USB device status and notifies to perform operation
 * @param[IN] event       updates usb events on attaching and dettaching device.
 * @param[IN] *host_class  pointer to host class device structure
 * @param[IN] instance
 * @retval    none
 **********************************************************************************************************************/
static UINT apl_change_function (ULONG event, UX_HOST_CLASS * host_class, VOID * instance)
{
    UINT                          status = UX_SUCCESS;
    UX_HOST_CLASS               * class;
    UX_HOST_CLASS_STORAGE       * storage;
    UX_HOST_CLASS_STORAGE_MEDIA * storage_media;
    FSP_PARAMETER_NOT_USED      ((void) instance);

    /* Check the class container if it is for a USBX Host Mass Storage class. */
    if (UX_SUCCESS ==
        _ux_utility_memory_compare(_ux_system_host_class_storage_name, host_class,
                                   _ux_utility_string_length_get(_ux_system_host_class_storage_name)))
    {
        /* Check if there is a device insertion. */
        if (UX_FSP_DEVICE_INSERTION == event)
        {
            status = ux_host_stack_class_get(_ux_system_host_class_storage_name, &class);
            if (UX_SUCCESS != status)
            {
                return status;
            }
            status = ux_host_stack_class_instance_get(class, 0, (void **) &storage);
            if (UX_SUCCESS != status)
            {
                return status;
            }
            if (UX_HOST_CLASS_INSTANCE_LIVE != storage->ux_host_class_storage_state)
            {
                return UX_ERROR;
            }
            storage_media = class->ux_host_class_media;
            g_p_media     = &storage_media->ux_host_class_storage_media;
            tx_event_flags_set(&g_usb_plug_events, EVENT_USB_PLUG_IN, TX_OR);
        }
        else if (UX_FSP_DEVICE_REMOVAL == event) /* Check if there is a device removal. */
        {
            g_p_media = UX_NULL;
            tx_event_flags_set(&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_OR);
        }
        else
        {
        }
    }
    return status;
}

/* USB Thread entry function */
void hmsc_thread_entry(void)
{
    UINT       ux_return = UX_SUCCESS;
    UINT       status    = RESET_VALUE;
    fsp_err_t  err       = FSP_SUCCESS;
    uint8_t    input_data = RESET_VALUE;

    /* Initialize the RTT Thread.*/
    rtt_thread_init_check();

    /* print the banner and EP info. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* fx_system_initialization */
    fx_system_initialize();

    /* ux_system_initialization */
    ux_return = ux_system_initialize((CHAR *)g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, RESET_VALUE);
    if (UX_SUCCESS != ux_return)
    {
        PRINT_ERR_STR("_UX_SYSTEM_INITIALIZE API FAILED..");
        ERROR_TRAP(ux_return);
    }

    PRINT_INFO_STR("ux_system initialized successfully!");

    /* ux host stack initialization */
    ux_return =  ux_host_stack_initialize(apl_change_function);
    if (UX_SUCCESS != ux_return)
    {
        PRINT_ERR_STR("UX_HOST_STACK_INITIALIZE API FAILED..");
        ERROR_TRAP(ux_return);
    }
    PRINT_INFO_STR("ux host stack initialized successfully!");

    /* Open usb driver */
    err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_USB_OPEN API FAILED..");
        ERROR_TRAP(err);
    }

    PRINT_INFO_STR("Init completed");

    /* extracts date and time from complier and add the time stamp to the files on USB Pendrive*/
    time_stamping();

    PRINT_INFO_STR("Date and time is set for filex as per compiler DATE and TIME values");

    PRINT_INFO_STR("Insert the USB Pendrive");

    /*  Wait until device inserted.*/
    tx_event_flags_get (&g_usb_plug_events, EVENT_USB_PLUG_IN, TX_AND_CLEAR, &actual_flags, TX_WAIT_FOREVER);
    if(EVENT_USB_PLUG_IN == actual_flags)
    {
        PRINT_INFO_STR("USB Pendrive is inserted");
    }

    /* Reset the event flag */
    actual_flags = RESET_VALUE;

    /*Print Menu */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);

    while (true)
    {
        /*  Check if USB is plugged out.*/
        tx_event_flags_get (&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_AND_CLEAR, &actual_flags, TX_NO_WAIT);
        if(EVENT_USB_PLUG_OUT == actual_flags)
        {
            PRINT_INFO_STR("Connect USB Pen drive for EP to work.");
            actual_flags = RESET_VALUE;
            /*  Wait until device inserted.*/
            tx_event_flags_get (&g_usb_plug_events, EVENT_USB_PLUG_IN, TX_AND_CLEAR, &actual_flags, TX_WAIT_FOREVER);
            if(EVENT_USB_PLUG_IN == actual_flags)
            {
                PRINT_INFO_STR("USB Pendrive is inserted");
                /*Print Menu */
                app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);
            }
        }
        /* below section of code(handling and processing user input) will execute when USB Pendrive is inserted */
        else
        {
            input_data = RESET_VALUE;
            /* check for user input to process */
            err = check_for_RTT_user_IP(&input_data);
            if (TX_SUCCESS == err)
            {
                /* Perform file operation using usbx hmsc */
                status = usbx_hmsc_operation(input_data);
                if(FX_SUCCESS != status)
                {
                    if (FX_MEDIA_NOT_OPEN == status)
                    {
                        PRINT_ERR_STR("USB Pendrive is plugged out please insert the Pendrive..");
                    }
                    else
                    {
                        PRINT_ERR_STR("usbx_hmsc_operation failed..");
                        deinit_media();
                        ERROR_TRAP(status);
                    }
                }
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initiates the USB operation by calling respective functions.
 * @param[IN]   read_data   Converted RTT input
 * @retval      status
 ***********************************************************************************************************************/
static UINT usbx_hmsc_operation(uint8_t read_data)
{
    UINT status       = RESET_VALUE;

    /* Perform USBX HMSC operation based on user input*/
    switch (read_data)
    {
        case CREATE :
        {
            /* create directory and file*/
            status = usbx_hmsc_file_directory_creation();
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("usbx_hmsc_file_directory_creation failed..");
                return(status);
            }
            break;
        }

        case WRITE :
        {
            /* Write to the file created */
            status = usbx_hmsc_file_write();
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("usbx_hmsc_file_write failed..");
                return status;
            }
            break;
        }

        case READ :
        {
            /* Read to the file created */
            status = usbx_hmsc_file_read();
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("usbx_hmsc_file_read failed..");
                return status;
            }
            break;
        }

        case DISPLAY :
        {
            /* Display the directory details */
            status = usbx_display_working_directory();
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("usbx_display_working_directory failed..");
                return status;
            }
            break;
        }

        case EJECT :
        {
            /* to safely eject the USB Device */
            status = usbx_safely_eject();
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("usbx_safely_eject failed..");
                return status;
            }
            break;
        }

        default :
        {
            PRINT_INFO_STR("Invalid Menu option entered. Please provide valid input");
            break;
        }
    }
    /*Print the banner only if Pendrive is inserted*/
    if (EVENT_USB_PLUG_OUT != actual_flags)
    {
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief       This function performs filex directory and file creation operation.
 * @param[IN]   None
 * @retval      FX_SUCCESS on successful directory and file creation.
 * @retval      FX_NO_MORE_SPACE on no required media space available.
 ***********************************************************************************************************************/
static UINT usbx_hmsc_file_directory_creation(void)
{
    UINT       fx_return        = FX_SUCCESS;
    CHAR       volume[VALUE_32] = {RESET_VALUE};
    ULONG64    available_space  = RESET_VALUE;

    tx_event_flags_get(&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_AND_CLEAR, &actual_flags, TX_NO_WAIT);
    if(EVENT_USB_PLUG_OUT == actual_flags)
    {
        PRINT_INFO_STR("USB Pendrive is removed. Please insert USB Pendrive to proceed.");
    }
    else
    {
        /*Check for available media space */
        fx_return = fx_media_extended_space_available(g_p_media,&available_space);
        if (FX_SUCCESS != fx_return)
        {
            PRINT_ERR_STR("Media space check failed");
            return fx_return;
        }

        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MEDIA_SPACE, sizeof(ULONG *), &available_space);

        if (DIR_SPACE_REQUIRED <= available_space)
        {
            PRINT_INFO_STR("Required space for Directory creation is available");
        }
        else
        {
            PRINT_ERR_STR("Required space for Directory creation is not available");
            return FX_NO_MORE_SPACE;
        }
        available_space = RESET_VALUE;

        /*Check for media volume name */
        fx_return = fx_media_volume_get(g_p_media, volume, FX_DIRECTORY_SECTOR);
        if (FX_SUCCESS != fx_return)
        {
            PRINT_ERR_STR("fx_media_volume_get API FAILED..");
            return fx_return;
        }

        PRINT_INFO_STR("Media Volume Get successfully");

        /* Create file directory */
        fx_return = fx_directory_create(g_p_media, "filex_dir");
        if (FX_ALREADY_CREATED == fx_return)
        {
            PRINT_INFO_STR("Directory already present on disk");
        }
        else if (FX_SUCCESS != fx_return)
        {
            PRINT_ERR_STR("fx_directory_create failed");
            return fx_return;
        }
        else
        {
            PRINT_INFO_STR("Directory created successfully.");
        }

        /* Sets default directory */
        fx_return = fx_directory_default_set(g_p_media, "filex_dir");
        if (FX_SUCCESS != fx_return)
        {
            PRINT_ERR_STR("\nfx_directory_default_set failed");
            return fx_return;
        }

        /* create a new file*/
        fx_return = fx_file_create(g_p_media, (CHAR *)&file_name[0]);
        if (FX_ALREADY_CREATED == fx_return)
        {
            PRINT_INFO_STR("File already present on disk");
            fx_return = FX_SUCCESS;
        }
        else if (FX_SUCCESS != fx_return)
        {
            PRINT_ERR_STR("fx_file_create failed");
            return fx_return;
        }
        else
        {
            PRINT_INFO_STR("File created successfully");
        }
    }
    return fx_return;
}

/*******************************************************************************************************************//**
 * @brief       This function performs file Write operation.
 * @param[IN]   None
 * @retval      FX_SUCCESS on successful file write operation.
 ***********************************************************************************************************************/
static UINT usbx_hmsc_file_write(void)
{
    UINT       fx_return = FX_SUCCESS;

    tx_event_flags_get(&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_AND_CLEAR, &actual_flags, TX_NO_WAIT);
    if(EVENT_USB_PLUG_OUT == actual_flags)
    {
        PRINT_INFO_STR("USB Pendrive is removed. Please insert USB Pendrive to proceed.");
    }
    else
    {
        /* Open the file in write mode */
        fx_return = fx_file_open(g_p_media, &g_file, &file_name[RESET_VALUE], FX_OPEN_FOR_WRITE);
        if (FX_NOT_FOUND == fx_return)
        {
            PRINT_INFO_STR("First create the file using Menu Option 1, then perform file write operation(Option 2).");
            return FX_SUCCESS;
        }
        else if (FX_SUCCESS != fx_return)
        {
            PRINT_ERR_STR("fx_file_open failed");
            ERROR_TRAP(fx_return);
        }
        else
        {
            PRINT_INFO_STR("File opened in Write Mode");
        }

        /* Clear file contents */
        fx_return = fx_file_truncate(&g_file,TRUNCATION_VALUE);
        if (FX_SUCCESS != fx_return)
        {
            PRINT_ERR_STR("fx_file_truncate failed");
            return fx_return;
        }

        /* Fill write buffer with data */
        memset(g_write_buf, 0x65, sizeof(g_write_buf));

        /* Write the fixed data to the opened file */
        fx_return = fx_file_write(&g_file, g_write_buf, DATA_LEN);
        if (FX_SUCCESS != fx_return)
        {
            PRINT_ERR_STR("fx_file_write failed");
            return fx_return;
        }

        /* Close already opened file */
        fx_return = fx_file_close(&g_file);
        if (FX_SUCCESS != fx_return)
        {
            PRINT_ERR_STR("fx_close failed");
            return fx_return;
        }

        /* flush the media */
        fx_return = fx_media_flush(g_p_media);
        if (FX_SUCCESS != fx_return)
        {
            PRINT_ERR_STR("fx_media_flush failed");
            return fx_return;
        }

        PRINT_INFO_STR("File write operation completed successfully");
    }
    return fx_return;
}

/*******************************************************************************************************************//**
 * @brief       This function performs file read operation.
 * @param[IN]   None
 * @retval      FX_SUCCESS on successful file read operation.
 ***********************************************************************************************************************/
static UINT usbx_hmsc_file_read(void)
{
    UINT       fx_return     = FX_SUCCESS;
    ULONG      actual_length = RESET_VALUE;

    tx_event_flags_get(&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_AND_CLEAR, &actual_flags, TX_NO_WAIT);
    if(EVENT_USB_PLUG_OUT == actual_flags)
    {
        PRINT_INFO_STR("USB Pendrive is removed. Please insert USB Pendrive to proceed.");
    }
    else
    {
        /* Open the file in read mode */
        fx_return = fx_file_open(g_p_media, &g_file, &file_name[RESET_VALUE], FX_OPEN_FOR_READ);
        if (FX_NOT_FOUND == fx_return)
        {
            PRINT_INFO_STR("First create the file using Menu Option 1, then perform file read operation(Option 3).");
            return FX_SUCCESS;
        }
        else if(FX_SUCCESS != fx_return)
        {
            PRINT_ERR_STR("fx_file_open failed\n");
            return fx_return;
        }
        else
        {
            PRINT_INFO_STR("File opened in Read Mode");
        }

        /*read the file content */
        fx_return = fx_file_read(&g_file, g_read_buf, DATA_LEN, &actual_length);
        if ((FX_SUCCESS == fx_return) || (FX_END_OF_FILE == fx_return))
        {
            /* do nothing */
        }
        else
        {
            PRINT_ERR_STR("fx_file_read failed");
            return fx_return;
        }
        /* Check if the first location of read buffer contain no data */
        if (RESET_VALUE == g_read_buf[RESET_VALUE])
        {
            PRINT_INFO_STR("No data present in file, first perform File write operation");
            return FX_SUCCESS;
        }
        else
        {
            /*Compare read data with write data*/
            if(RESET_VALUE == memcmp(g_read_buf, g_write_buf, DATA_LEN))
            {
                PRINT_INFO_STR("Read data is same as Write data");
            }
            else
            {
                PRINT_ERR_STR("Read data didn't matched with Write data");
                return FX_INVALID_CHECKSUM;
            }
        }

        /* Close already opened file */
        fx_return = fx_file_close(&g_file);
        if (FX_SUCCESS != fx_return)
        {
            PRINT_ERR_STR("fx_file_close failed");
            return fx_return;
        }
        PRINT_INFO_STR("File read operation completed successfully");
    }
    return fx_return;
}

/*******************************************************************************************************************//**
 * @brief       This function to display the directory content of current directory.
 * @param[IN]   None
 * @retval      FX_SUCCESS on successful display current operation.
 ***********************************************************************************************************************/
static UINT usbx_display_working_directory(void)
{
    UINT       fx_return  = FX_SUCCESS;
    /* Variable to store file details */
    file_info_t file_info = {RESET_VALUE};


    tx_event_flags_get(&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_AND_CLEAR, &actual_flags, TX_NO_WAIT);
    if(EVENT_USB_PLUG_OUT == actual_flags)
    {
        PRINT_INFO_STR("USB Pendrive is removed. Please insert USB Pendrive to proceed.");
    }
    else
    {
        fx_return = fx_directory_first_full_entry_find(g_p_media,file_info.directory_name, \
                    (UINT *)file_info.attributes, &file_info.size, &file_info.time_stamp.year, \
                    &file_info.time_stamp.month, &file_info.time_stamp.date, &file_info.time_stamp.hour, \
                    &file_info.time_stamp.min, &file_info.time_stamp.sec);
        if (FX_NOT_FOUND == fx_return)
        {
            PRINT_INFO_STR("First create the file using Menu Option 1, then perform file display operation.");
            return FX_SUCCESS;
        }
        else if (FX_SUCCESS != fx_return)
        {
            PRINT_ERR_STR("fx_directory_first_full_entry_find failed");
            return fx_return;
        }
        else
        {
            /*do nothing */
        }
        /* checks and prints the file and directory details until the last entry */
        do
        {
            /* Assign string according to attribute, describing the file */
            if(RESET_VALUE != (*(file_info.attributes) & FX_DIRECTORY))
            {
                strncpy(file_info.attributes, "directory", ARRAY_SIZE );
            }
            else if(*(file_info.attributes) & FX_READ_ONLY)
            {
                strncpy(file_info.attributes, "read only file", ARRAY_SIZE );
            }
            else
            {
                strncpy(file_info.attributes, "writable file", ARRAY_SIZE );
            }

            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_FILE_ENTRIES, sizeof(file_info_t), &file_info);

            /* Extract details of next entry in the current directory */
            fx_return = fx_directory_next_full_entry_find(g_p_media, file_info.directory_name, \
                        (UINT *)file_info.attributes, &file_info.size, &file_info.time_stamp.year, \
                        &file_info.time_stamp.month, &file_info.time_stamp.date, &file_info.time_stamp.hour, \
                        &file_info.time_stamp.min, &file_info.time_stamp.sec);
            if (((uint8_t)FX_SUCCESS != fx_return) && ((uint8_t)FX_NO_MORE_ENTRIES != fx_return))
            {
                PRINT_ERR_STR("fx_directory_next_full_entry_find failed.");
                return fx_return;
            }
        }while(FX_NO_MORE_ENTRIES != fx_return);

    }
    /* Returning FSP_SUCCESS since the error conditions are already handled */
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened Filex media and safely ejects.
 * @param[IN]   None
 * @retval      FX_SUCCESS on successful eject operation.
 ***********************************************************************************************************************/
static UINT usbx_safely_eject(void)
{
    UINT       fx_return = FX_SUCCESS;
    UINT       tx_return = TX_SUCCESS;

    /* flush the media */
    fx_return = fx_media_flush(g_p_media);
    if (FX_SUCCESS != fx_return)
    {
        PRINT_ERR_STR("fx_media_flush failed");
        return fx_return;
    }

     /*close the media*/
    fx_return = fx_media_close(g_p_media);
    if (FX_SUCCESS != fx_return)
    {
        PRINT_ERR_STR("fx_media_close failed");
        return fx_return;
    }

    PRINT_INFO_STR("USB Device can be safely removed now");
    /*waiting for USB Prndrive to be plugged out */
    tx_return = tx_event_flags_get (&g_usb_plug_events, EVENT_USB_PLUG_OUT, \
                                    TX_AND_CLEAR, &actual_flags, TX_WAIT_FOREVER);
    if (TX_SUCCESS != tx_return)
    {
        PRINT_ERR_STR("tx_event_flags_get failed");
        return tx_return;
    }
    return fx_return;
}

/*******************************************************************************************************************//**
 * @brief       This function closes the opened media and usb instance.
 * @param[IN]   None
 * @retval      None
 ***********************************************************************************************************************/
static void deinit_media(void)
{
    UINT status   = FX_SUCCESS;
    fsp_err_t err = FSP_SUCCESS;

    /* Close the opened media  */
    status = fx_media_close(g_p_media);
    /* Handle error in-case of failure */
    if (FX_SUCCESS != status)
    {
        PRINT_ERR_STR("Media close failed");
    }

    /* Close opened USB interface */
    err = R_USB_Close(&g_basic0_ctrl);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_USB_Close API Failed");
    }

    /* Uninitializes all the host code for USB */
    status =  ux_host_stack_uninitialize();
    /* Handle error in-case of failure */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_host_stack_uninitialize failed");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function assigns month as per the read value.
 * @param[IN]   *time         Pointer to time format structure
 * @param[IN]   read_buffer  buffer to store captured time and data format.
 * @retval      None
 ***********************************************************************************************************************/
static void assign_month_value(time_format_t * time, char * read_buffer)
{

    switch (read_buffer[RESET_VALUE])
    {
        case JAN_JUN_JUL:
        {
            if(ASCII_LOWER_A == read_buffer[ONE])
            {
                time->month = JANUARY;
            }
            else if(ASCII_LOWER_N == read_buffer[TWO])
            {
                time->month = JUNE;
            }
            else
            {
                time->month = JULY;
            }
        }
        break;
        case FEB:
        {
            time->month = FEBRUARY;
        }
        break;
        case MAR_MAY:
        {
            if(ASCII_LOWER_R == read_buffer[TWO])
            {
                time->month = MARCH;
            }
            else
            {
                time->month = MAY;
            }
        }
        break;
        case APR_AUG:
        {
            if(ASCII_LOWER_P == read_buffer[ONE])
            {
                time->month = APRIL;
            }
            else
            {
                time->month = AUGUST;
            }
        }
        break;
        case SEP:
        {
            time->month = SEPTEMBER;
        }
        break;
        case OCT:
        {
            time->month = OCTOBER;
        }
        break;
        case NOV:
        {
            time->month = NOVEMBER;
        }
        break;
        case DEC:
        {
            time->month = DECEMBER;
        }
        break;
        default :
        {

        }
        break;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is used to add time stamp to the files on pendrive.
 * @param[IN]   None
 * @retval      None
 ***********************************************************************************************************************/
static void time_stamping(void)
{
    /* Extract current Date and Time from compiler MACROS */
        char read_time[MAX_BYTES] = {NULL_CHAR};
        strcat(read_time, __DATE__);
        strcat(read_time," ");
        strcat(read_time, __TIME__);

        /* Date and Time extracted from MACROs are converted to time format so that,
         *  this can be used to add time stamping to the files on SD card */
        g_set_time.date = atoi(&read_time[4]);
        g_set_time.year = atoi(&read_time[7]);
        g_set_time.hour = atoi(&read_time[12]);
        g_set_time.min  = atoi(&read_time[15]);
        g_set_time.sec  = atoi(&read_time[18]);

        if(g_set_time.date > DATE_VALUE_CHECK)
        {
            g_set_time.date -= DATE_VALUE_SUB;
        }

        /* Convert returned month name to month value */
        assign_month_value(&g_set_time, &read_time[0]);

        /* Set system date and time */
        fx_system_date_set(g_set_time.year, g_set_time.month, g_set_time.date);
        fx_system_time_set(g_set_time.hour, g_set_time.min, g_set_time.sec);
}
/*******************************************************************************************************************//**
 * @} (end addtogroup usbx_hmsc_ep)
 **********************************************************************************************************************/
