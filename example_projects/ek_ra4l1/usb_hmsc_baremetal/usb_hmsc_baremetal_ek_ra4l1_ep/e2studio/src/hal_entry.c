/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include "common_utils.h"
#include "usb_hmsc_bm_ep.h"
#include "FreeRTOSConfig.h"
#include "r_usb_basic.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_hmsc_baremetal_ep
 * @{
 **********************************************************************************************************************/

 FSP_CPP_HEADER
 void R_BSP_WarmStart(bsp_warm_start_event_t event);
 FSP_CPP_FOOTER

  /* Variables for file operations */
 FF_FILE * gp_px_sourcefile = NULL;
 FF_Disk_t g_disk;
 size_t  g_size_return = RESET_VALUE;

 /* Variables for USB operations */
 rm_freertos_plus_fat_device_t g_device = {
         .sector_count = RESET_VALUE,
         .sector_size_bytes = RESET_VALUE,
 };
 usb_status_t     g_usb_event = USB_STATUS_POWERED;

 usb_event_info_t g_event_info;
 rm_block_media_usb_instance_ctrl_t * gp_instance_ctrl;

 uint8_t g_file_data[RM_FREERTOS_PLUS_FAT_EXAMPLE_BUFFER_SIZE_BYTES];
 uint8_t g_read_buffer[RM_FREERTOS_PLUS_FAT_EXAMPLE_BUFFER_SIZE_BYTES];
 static fsp_err_t freertos_fat_error = FSP_SUCCESS;
 static FF_Error_t ff_err = FSP_SUCCESS;

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
 void hal_entry(void)
 {
     fsp_pack_version_t version = { RESET_VALUE };

     /* Variables to indicate status */
     static state_t usb_state = STATE_DETACH;
     static bool is_usb_ejected = false;

     /* version get API for FLEX pack information */
     R_FSP_VersionGet(&version);

     /* Example Project information printed on the Console */
     APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, \
               version.version_id_b.patch);
     APP_PRINT(EP_INFO);

     /* Fill the write buffer, to be written in a file, with known data. */
     for (uint16_t i = 0; i < RM_FREERTOS_PLUS_FAT_EXAMPLE_BUFFER_SIZE_BYTES; i++)
     {
         g_file_data[i] = (uint8_t) i;
     }

     /* Open media driver. */
     freertos_fat_error = RM_FREERTOS_PLUS_FAT_Open(&g_rm_freertos_plus_fat0_ctrl, &g_rm_freertos_plus_fat0_cfg);
     if (FSP_SUCCESS != freertos_fat_error)
     {
         APP_ERR_PRINT ("\r\nRM_FREERTOS_PLUS_FAT_Open API failed\r\n");
         APP_ERR_TRAP(freertos_fat_error);
     }
     else
     {
         APP_PRINT ("\r\nRM_FREERTOS_PLUS_FAT_Open API success\r\n");
     }

     while (true)
     {
         /* Polling USB event. */
         freertos_fat_error = g_usb_on_usb.eventGet(&g_event_info, &g_usb_event);
         if (FSP_SUCCESS != freertos_fat_error)
         {
             APP_ERR_PRINT ("\r\nR_USB_EventGet API failed\r\n");
             APP_ERR_TRAP(freertos_fat_error);
         }
         /* Process USB events */
         switch (g_usb_event)
         {
             case USB_STATUS_CONFIGURED:
             {
                 /* Mount and configure the USB */
                 usb_media_mount();
                 APP_PRINT("\r\nUSB Media configured Successfully\r\n");
                 usb_state = STATE_USB_CONFIGURED;
                 break;
             }
             case USB_STATUS_DETACH:
             {
                 freertos_fat_error = RM_FREERTOS_PLUS_FAT_DiskDeinit(&g_rm_freertos_plus_fat0_ctrl, &g_disk);
                 if (FSP_SUCCESS != freertos_fat_error)
                 {
                     APP_ERR_PRINT ("\r\nRM_FREERTOS_PLUS_FAT_DiskDeinit API failed\r\n");
                     APP_ERR_TRAP (freertos_fat_error);
                 }
                 else
                 {
                     APP_PRINT ("\r\nRM_FREERTOS_PLUS_FAT_DiskDeinit API success\r\n");
                 }
                 usb_state = STATE_DETACH;
                 is_usb_ejected = true;
                 APP_PRINT("\r\nUSB device Removed\r\n");
                 break;
             }
             default:
             {
                 break;
             }
         }
         /* Process the USB operations based on User inputs */
         process_usb_operation(&usb_state, &is_usb_ejected);
     }
#if BSP_TZ_SECURE_BUILD
     /* Enter non-secure code */
     R_BSP_NonSecureEnter();
#endif
 }

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
 void R_BSP_WarmStart(bsp_warm_start_event_t event)
 {
     if (BSP_WARM_START_RESET == event)
     {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

         /* Enable reading from data flash. */
         R_FACI_LP->DFLCTL = 1U;

         /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
          * C runtime initialization, should negate the need for a delay since the initialization will typically take
		  * more than 6us. */
#endif
     }

     if (BSP_WARM_START_POST_C == event)
     {
         /* C runtime environment and system clocks are setup. */

         /* Configure pins. */
         R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
     }
 }

 void process_usb_operation(state_t * p_usb_state, bool * p_is_usb_ejected)
 {
     /* Check the state and perform action accordingly */
     switch(*p_usb_state)
     {
         case STATE_USB_CONFIGURED:
         {
             /* Print USB HMSC menu */
             APP_PRINT(USB_HMSC_MENU);

             while(true)
             {
                 uint8_t rtt_input_buf[BUFFER_SIZE_DOWN] = {RESET_VALUE};
                 /* Read RTT input from user */
                 if (APP_CHECK_DATA)
                 {
                     APP_READ (rtt_input_buf);
                     state_t current_state = (uint8_t)atoi((char *)rtt_input_buf);
                     if ((STATE_DATA_WRITE > current_state) || (STATE_RECONFIGURE < current_state))
                     {
                         APP_PRINT ("\r\nInvalid input. Provide a valid input\r\n");
                     }
                     else
                     {
                         *p_usb_state = current_state;
                     }
                     break;
                 }
             }
             break;
         }

         case STATE_DATA_WRITE:
         {
             if(false == *p_is_usb_ejected)
             {
                 usb_write_operation();
                 listing_file();
                 APP_PRINT("\r\nWrite operation completed\r\n");
                 *p_usb_state = STATE_USB_CONFIGURED;
             }
             else
             {
                 APP_PRINT("\r\nInsert the USB Drive to perform operations.\r\n");
                 *p_usb_state = STATE_USB_CONFIGURED;
             }
             break;
         }

         case STATE_FILE_READ:
         {
             if(false == *p_is_usb_ejected)
             {
                 usb_read_operation();
                 *p_usb_state = STATE_USB_CONFIGURED;
             }
             else
             {
                 APP_PRINT("\r\nInsert the USB Drive to perform operations.\r\n");
                 *p_usb_state = STATE_USB_CONFIGURED;
             }
             break;
         }

         case STATE_FORMAT:
         {
             if(false == *p_is_usb_ejected)
             {
                 APP_PRINT("\r\nFormatting USB Drive\r\n");
                 format_usb_device();
                 *p_usb_state = STATE_USB_CONFIGURED;
             }
             else
             {
                 APP_PRINT("\r\nInsert the USB Drive to perform operations.\r\n");
                 *p_usb_state = STATE_USB_CONFIGURED;
             }
             break;
         }

         case STATE_REMOVE:
         {
             if(false == *p_is_usb_ejected)
             {
                 APP_PRINT ("\r\nEjecting USB device..\r\n");
                 usb_safely_eject();
                 APP_PRINT ("\r\nUSB Device can be safely removed now\r\n");
                 *p_usb_state = STATE_USB_CONFIGURED;
                 *p_is_usb_ejected = true;
             }
             else
             {
                 APP_PRINT ("\r\nUSB Device not found\r\n");
                 *p_usb_state = STATE_USB_CONFIGURED;
             }
             break;
         }

         case STATE_RECONFIGURE:
         {
             if(true == *p_is_usb_ejected)
             {
                 /* Open media driver. */
                 freertos_fat_error = RM_FREERTOS_PLUS_FAT_Open(&g_rm_freertos_plus_fat0_ctrl, \
				                                                &g_rm_freertos_plus_fat0_cfg);
                 if (FSP_SUCCESS != freertos_fat_error)
                 {
                     APP_ERR_PRINT ("\r\nRM_FREERTOS_PLUS_FAT_Open API failed\r\n");
                     APP_ERR_TRAP(freertos_fat_error);
                 }

                 /* Setting state to default to receive the USB event */
                 *p_is_usb_ejected = false ;
                 *p_usb_state = STATE_DETACH;
             }
             else
             {
                 APP_PRINT ("\r\nUSB Device is already inserted.\r\n");
                 *p_usb_state = STATE_USB_CONFIGURED;
             }
             break;
         }
         default:
         {
             break;
         }
     }
 }

/*******************************************************************************************************************//**
 * This function initializes and mounts the USB media and disk
 *
 * @param      none
 * @retval     none
 **********************************************************************************************************************/
 void usb_media_mount(void)
 {
     gp_instance_ctrl                 = g_event_info.p_context;
     gp_instance_ctrl->device_address = g_event_info.device_address;
     int32_t file_error = SUCCESS;

     /* Initialize the media and the disk. */
     freertos_fat_error = RM_FREERTOS_PLUS_FAT_MediaInit(&g_rm_freertos_plus_fat0_ctrl, &g_device);
     if (FSP_SUCCESS != freertos_fat_error)
     {
         APP_ERR_PRINT ("\r\nRM_FREERTOS_PLUS_FAT_MediaInit API failed\r\n");
         fat_clean_up();
         APP_ERR_TRAP(freertos_fat_error);
     }

     /* Initialize one disk for each partition used in the application. */
     freertos_fat_error = RM_FREERTOS_PLUS_FAT_DiskInit(&g_rm_freertos_plus_fat0_ctrl, \
	                                                    &g_rm_freertos_plus_fat0_disk_cfg, &g_disk);
     if (FSP_SUCCESS != freertos_fat_error)
     {
         APP_ERR_PRINT ("\r\nRM_FREERTOS_PLUS_FAT_DiskInit API failed\r\n");
         fat_clean_up();
         APP_ERR_TRAP(freertos_fat_error);
     }

     /* Mount each disk. This assumes the disk is already partitioned and formatted. */
     ff_err = FF_Mount(&g_disk, RM_FREERTOS_PLUS_FAT_EXAMPLE_PARTITION_NUMBER);
     if (FSP_SUCCESS != ff_err)
     {
         APP_ERR_PRINT ("\r\nFF_Mount API failed\r\n");
         /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
         fat_clean_up();
         /* As function returns fsp_err_t, ff_err cannot be returned. Hence trapping the error here */
         APP_ERR_TRAP (ff_err);
     }

     /* Add the disk to the file system. */
     file_error = FF_FS_Add(FILE_ENTRY, &g_disk);
     if (SUCCESS == file_error)
     {
         APP_ERR_PRINT ("\r\nFF_FS_Add API failed\r\n");
         /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
         fat_clean_up();
         /* As function returns fsp_err_t, ff_err cannot be returned. Hence trapping the error here */
         APP_ERR_TRAP (file_error);
     }
 }

/*******************************************************************************************************************//**
 * This function opens and writes known data to the file
 *
 * @param      none
 * @retval     none
 **********************************************************************************************************************/
void usb_write_operation(void)
 {
     size_t bytes_written = RESET_VALUE;
     int ff_ferr = SUCCESS;
     /* Open a source file for writing. */
     APP_PRINT("\r\nOpening %s to write\r\n", RM_FREERTOS_PLUS_FAT_EXAMPLE_FILE_NAME);
     gp_px_sourcefile = ff_fopen((const char *) RM_FREERTOS_PLUS_FAT_EXAMPLE_FILE_NAME, WRITE_MODE);
     if (NULL == gp_px_sourcefile)
     {
         APP_ERR_PRINT ("\r\nFile open failed\r\n");
         APP_ERR_TRAP(FF_ERR_FILE_NOT_OPENED_IN_WRITE_MODE);
     }
     else
     {
         APP_PRINT("\r\nFile Open Success \r\n");
     }

     /* Write file data. */
     APP_PRINT("Writing data to file\r\n");
     bytes_written = ff_fwrite(g_file_data, sizeof(g_file_data), WRITE_ITEM_SIZE, gp_px_sourcefile);
     if (WRITE_ITEM_SIZE != bytes_written)
     {
         APP_ERR_PRINT ("\r\nFile write failed\r\n");
         APP_ERR_TRAP(FSP_ERR_WRITE_FAILED);
     }

     /* Close the file. */
     ff_ferr = ff_fclose(gp_px_sourcefile);
     if (SUCCESS != ff_ferr)
     {
         APP_ERR_PRINT ("\r\nFile close failed\r\n");
         APP_ERR_TRAP (ff_ferr);
     }
 }

/*******************************************************************************************************************//**
 * This function opens and reads data from file
 *
 * @param      none
 * @retval     none
 **********************************************************************************************************************/
void usb_read_operation(void)
 {
     size_t bytes_written = RESET_VALUE;
     int ff_ferr = SUCCESS;

     APP_PRINT("\r\nOpening %s to read\r\n", RM_FREERTOS_PLUS_FAT_EXAMPLE_FILE_NAME);
     gp_px_sourcefile = ff_fopen((const char *) RM_FREERTOS_PLUS_FAT_EXAMPLE_FILE_NAME, READ_MODE);
     if(NULL != gp_px_sourcefile)
     {
         for (uint16_t k = RESET_VALUE; k < RM_FREERTOS_PLUS_FAT_EXAMPLE_BUFFER_SIZE_BYTES; k++)
         {
             g_read_buffer[k] = (uint8_t) RESET_VALUE;
         }

         /* Read file data. */
         APP_PRINT("\r\nReading data from file\r\n");
         bytes_written = ff_fread(g_read_buffer, sizeof(g_file_data), WRITE_ITEM_SIZE, gp_px_sourcefile);
         if (READ_WRITE_FAILURE == bytes_written)
         {
             APP_ERR_PRINT ("\r\nFile read failed\r\n");
             APP_ERR_TRAP(FSP_ERR_INVALID_DATA);
         }

         /* Close the file. */
         ff_ferr = ff_fclose(gp_px_sourcefile);
         if (SUCCESS != ff_ferr)
         {
             APP_ERR_PRINT ("\r\nFile close failed\r\n");
             APP_ERR_TRAP (ff_ferr);
         }

         APP_PRINT("Read operation completed\r\n");

         if(memcmp(g_read_buffer, g_file_data, sizeof(g_file_data)) == SUCCESS)
         {
             APP_PRINT("\r\nRead and write buffer comparison success\r\n");
         }
         else
         {
             APP_PRINT("\r\nRead and write buffer comparison failed\r\n");
         }
     }
     else
     {
         APP_PRINT("\r\n%s File not found\r\n", RM_FREERTOS_PLUS_FAT_EXAMPLE_FILE_NAME);
     }
 }

/*******************************************************************************************************************//**
 * User defined function for format operation
 *
 * @param      none
 * @retval     none
 **********************************************************************************************************************/
void format_usb_device(void)
 {
     /* Formatting time varies with USB Device capacity. Might take longer time for higher capacity USB Device */
     FF_Error_t ff_error = FF_Format (&g_disk, g_disk.xStatus.bPartitionNumber, pdFALSE , pdFALSE);
     if (FF_ERR_NONE != ff_error)
     {
         APP_ERR_PRINT ("\r\nFF_Format API failed  %d. Check the USB Device.\r\n", FF_GetErrMessage(ff_error));
         APP_ERR_TRAP (ff_error);
     }
     else
     {
         APP_PRINT ("\r\nUSB Device Formatted successfully \r\n");
     }
 }

/*******************************************************************************************************************//**
 * This function de-initializes and safely ejects the USB device
 *
 * @param      none
 * @retval     none
 **********************************************************************************************************************/
void usb_safely_eject(void)
 {
     /* de-initialize disk structure */
     freertos_fat_error = RM_FREERTOS_PLUS_FAT_DiskDeinit(&g_rm_freertos_plus_fat0_ctrl, &g_disk);
     if (FSP_SUCCESS != freertos_fat_error)
     {
         APP_ERR_PRINT ("\r\nRM_FREERTOS_PLUS_FAT_DiskDeinit API failed\r\n");
         APP_ERR_TRAP (freertos_fat_error);
     }

     /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
     freertos_fat_error = RM_FREERTOS_PLUS_FAT_Close (&g_rm_freertos_plus_fat0_ctrl);
     if (FSP_SUCCESS != freertos_fat_error)
     {
         APP_ERR_PRINT ("\r\nRM_FREERTOS_PLUS_FAT_Close API failed\r\n");
         APP_ERR_TRAP (freertos_fat_error);
     }
 }

/*******************************************************************************************************************//**
 * User defined function to close FREERTOS+FAT instance
 *
 * @param      none
 * @retval     none
 **********************************************************************************************************************/
 void fat_clean_up(void)
 {
     /* Close the FREERTOS_PLUS_FAT_Close instance on any failure */
     freertos_fat_error = RM_FREERTOS_PLUS_FAT_Close (&g_rm_freertos_plus_fat0_ctrl);
     if (FSP_SUCCESS != freertos_fat_error)
     {
         APP_PRINT ("\r\nRM_FREERTOS_PLUS_FAT_Close API failed\r\n");
     }
     else
     {
         APP_PRINT ("\r\nFREERTOS PLUS FAT instance Closed successfully.\r\n");
     }
 }

/*******************************************************************************************************************//**
 * User defined function for listing all current file
 *
 * @param      none
 * @retval     none
 **********************************************************************************************************************/
void listing_file(void)
 {
     /* List all the existing files. */
     FF_FindData_t px_find_struct;
     
     /* Clear FF_FindData_t to 0. */
     memset(&px_find_struct, 0x00, sizeof(FF_FindData_t));

     /* Find the first file in CWD. */
     if (RESET_VALUE == ff_findfirst((const char *)FILE_ENTRY, &px_find_struct))
     {
         APP_PRINT("\r\nList of existing files:\r\n");
         do
         {
             if (RESET_VALUE != (px_find_struct.ucAttributes & FF_FAT_ATTR_DIR))
             {
                 /* Do nothing. */
             }
             else
             {
                 APP_PRINT("\n  %s%s\r\n", FILE_ENTRY, px_find_struct.pcFileName);
             }
         } while (RESET_VALUE == ff_findnext(&px_find_struct));
     }
 }

#if BSP_TZ_SECURE_BUILD

 BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

 /* Trustzone Secure Projects require at least one nonsecure callable function in order to build
  * (Remove this if it is not required to build). */
 BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
 {

 }
#endif

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_hmsc_baremetal_ep)
 **********************************************************************************************************************/
