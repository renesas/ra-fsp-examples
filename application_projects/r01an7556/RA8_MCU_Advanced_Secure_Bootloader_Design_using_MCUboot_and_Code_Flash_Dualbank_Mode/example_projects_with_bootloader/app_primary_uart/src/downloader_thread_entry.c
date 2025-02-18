/***********************************************************************************************************************
 * File Name    : downloader_thread_entry.c
 * Description  : Contains Downloader Thread Implementation
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include "downloader_thread.h"
#include "header.h"
#include "comms.h"
#include "stdio.h"
#include "menu.h"

void display_image_slot_info(void)
{
    uint8_t str[100];

    struct image_version {
        uint8_t iv_major;
        uint8_t iv_minor;
        uint16_t iv_revision;
        uint32_t iv_build_num;
    };

    struct image_header {
        uint32_t ih_magic;
        uint32_t ih_load_addr;
        uint16_t ih_hdr_size;            /* Size of image header (bytes). */
        uint16_t ih_protect_tlv_size;    /* Size of protected TLV area (bytes). */
        uint32_t ih_img_size;            /* Does not include header. */
        uint32_t ih_flags;               /* IMAGE_F_[...]. */
        struct image_version ih_ver;
        uint32_t _pad1;
    };

    struct image_header *p_img_header;

    /* Primary Image Slot */
    p_img_header = (struct image_header *)PRIMARY_IMAGE_START_ADDRESS;
    snprintf((char *)str, sizeof(str), "*************************\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "* Primary Image Slot *\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "*************************\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Image version: \t\t%01d.%01d (Rev: %d, Build: %ld)\r\n", p_img_header->ih_ver.iv_major,
                                                                                                 p_img_header->ih_ver.iv_minor,
                                                                                                 p_img_header->ih_ver.iv_revision,
                                                                                                 p_img_header->ih_ver.iv_build_num);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Primary image start address: \t\t0x%08X\r\n", (unsigned int)PRIMARY_IMAGE_START_ADDRESS);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Header size: \t\t0x%04X (%d bytes)\r\n", p_img_header->ih_hdr_size, p_img_header->ih_hdr_size);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Protected TLV size: \t0x%04X (%d bytes)\r\n", p_img_header->ih_protect_tlv_size, p_img_header->ih_protect_tlv_size);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Image size: \t\t0x%08X (%ld bytes)\r\n", (unsigned int)p_img_header->ih_img_size, p_img_header->ih_img_size);
    comms_send(str, strlen((char *)str));

    /* Secondary S Image Slot */
    p_img_header = (struct image_header *)SECONDARY_IMAGE_START_ADDRESS;
    snprintf((char *)str, sizeof(str), "\r\n**************************\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "* Secondary Image Slot *\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "**************************\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Image version: \t\t%01d.%01d (Rev: %d, Build: %ld)\r\n", p_img_header->ih_ver.iv_major,
                                                                                                 p_img_header->ih_ver.iv_minor,
                                                                                                 p_img_header->ih_ver.iv_revision,
                                                                                                 p_img_header->ih_ver.iv_build_num);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Secondary image start address: \t\t0x%08X\r\n", (unsigned int)SECONDARY_IMAGE_START_ADDRESS);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Header size: \t\t0x%04X (%d bytes)\r\n", p_img_header->ih_hdr_size, p_img_header->ih_hdr_size);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Protected TLV size: \t0x%04X (%d bytes)\r\n", p_img_header->ih_protect_tlv_size, p_img_header->ih_protect_tlv_size);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Image size: \t\t0x%08X (%ld bytes)\r\n", (unsigned int)p_img_header->ih_img_size, p_img_header->ih_img_size);
    comms_send(str, strlen((char *)str));
}

/* Downloader Thread entry function */
/* pvParameters contains TaskHandle_t */
void downloader_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    fsp_err_t err = FSP_SUCCESS;

    /* Open the comms driver */
    err = comms_open();
    if (FSP_SUCCESS != err)
    {
       /* Stop as comms open failure */
       while (true)
       {
           ;
       }
    }
    err = R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    if (FSP_SUCCESS != err)
    {
       /* Stop as Internal flash open failure */
       while (true)
       {
           ;
       }
    }

    while (true)
    {
    	menu();
        vTaskDelay(1);
    }
}
