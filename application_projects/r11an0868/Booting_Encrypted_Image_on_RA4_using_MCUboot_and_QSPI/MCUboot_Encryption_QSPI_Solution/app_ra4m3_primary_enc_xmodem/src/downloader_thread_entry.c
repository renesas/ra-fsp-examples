/***********************************************************************************************************************
 * File Name    : downloader_thread_entry.c
 * Description  : Contains Downloader Thread Implementation
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Copyright [2020-2022] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/
#include "downloader_thread.h"
#include "header.h"
#include "comms.h"
#include "stdio.h"
#include "menu.h"
#include "qspi_operations.h"

void display_image_slot_info (void)
{
    uint8_t str[100];

    struct image_version
    {
        uint8_t  iv_major;
        uint8_t  iv_minor;
        uint16_t iv_revision;
        uint32_t iv_build_num;
    };

    struct image_header
    {
        uint32_t             ih_magic;
        uint32_t             ih_load_addr;
        uint16_t             ih_hdr_size;         /* Size of image header (bytes). */
        uint16_t             ih_protect_tlv_size; /* Size of protected TLV area (bytes). */
        uint32_t             ih_img_size;         /* Does not include header. */
        uint32_t             ih_flags;            /* IMAGE_F_[...]. */
        struct image_version ih_ver;
        uint32_t             _pad1;
    };

    struct image_header * p_img_header;

    /* Primary Image Slot */
    p_img_header = (struct image_header *) PRIMARY_IMAGE_START_ADDRESS;
    snprintf((char *) str, sizeof(str), "*************************\r\n");
    comms_send(str, strlen((char *) str));
    snprintf((char *) str, sizeof(str), "* Primary Image Slot *\r\n");
    comms_send(str, strlen((char *) str));
    snprintf((char *) str, sizeof(str), "*************************\r\n");
    comms_send(str, strlen((char *) str));
    snprintf((char *) str,
             sizeof(str),
             "Image version: \t\t%02d.%02d (Rev: %d, Build: %ld)\r\n",
             p_img_header->ih_ver.iv_major,
             p_img_header->ih_ver.iv_minor,
             p_img_header->ih_ver.iv_revision,
             p_img_header->ih_ver.iv_build_num);
    comms_send(str, strlen((char *) str));
    snprintf((char *) str,
             sizeof(str),
             "Primary image start address: \t\t0x%08X\r\n",
             (unsigned int) PRIMARY_IMAGE_START_ADDRESS);
    comms_send(str, strlen((char *) str));
    snprintf((char *) str,
             sizeof(str),
             "Header size: \t\t0x%04X (%d bytes)\r\n",
             p_img_header->ih_hdr_size,
             p_img_header->ih_hdr_size);
    comms_send(str, strlen((char *) str));
    snprintf((char *) str,
             sizeof(str),
             "Protected TLV size: \t0x%04X (%d bytes)\r\n",
             p_img_header->ih_protect_tlv_size,
             p_img_header->ih_protect_tlv_size);
    comms_send(str, strlen((char *) str));
    snprintf((char *) str,
             sizeof(str),
             "Image size: \t\t0x%08X (%ld bytes)\r\n",
             (unsigned int) p_img_header->ih_img_size,
             p_img_header->ih_img_size);
    comms_send(str, strlen((char *) str));

    /* Secondary S Image Slot */
    p_img_header = (struct image_header *) SECONDARY_IMAGE_START_ADDRESS;
    snprintf((char *) str, sizeof(str), "\r\n**************************\r\n");
    comms_send(str, strlen((char *) str));
    snprintf((char *) str, sizeof(str), "* Secondary Image Slot *\r\n");
    comms_send(str, strlen((char *) str));
    snprintf((char *) str, sizeof(str), "**************************\r\n");
    comms_send(str, strlen((char *) str));
    snprintf((char *) str,
             sizeof(str),
             "Image version: \t\t%02d.%02d (Rev: %d, Build: %ld)\r\n",
             p_img_header->ih_ver.iv_major,
             p_img_header->ih_ver.iv_minor,
             p_img_header->ih_ver.iv_revision,
             p_img_header->ih_ver.iv_build_num);
    comms_send(str, strlen((char *) str));
    snprintf((char *) str,
             sizeof(str),
             "Secondary image start address: \t\t0x%08X\r\n",
             (unsigned int) SECONDARY_IMAGE_START_ADDRESS);
    comms_send(str, strlen((char *) str));
    snprintf((char *) str,
             sizeof(str),
             "Header size: \t\t0x%04X (%d bytes)\r\n",
             p_img_header->ih_hdr_size,
             p_img_header->ih_hdr_size);
    comms_send(str, strlen((char *) str));
    snprintf((char *) str,
             sizeof(str),
             "Protected TLV size: \t0x%04X (%d bytes)\r\n",
             p_img_header->ih_protect_tlv_size,
             p_img_header->ih_protect_tlv_size);
    comms_send(str, strlen((char *) str));
    snprintf((char *) str,
             sizeof(str),
             "Image size: \t\t0x%08X (%ld bytes)\r\n",
             (unsigned int) p_img_header->ih_img_size,
             p_img_header->ih_img_size);
    comms_send(str, strlen((char *) str));
}

usb_callback_t g_usb_cb;

/* Downloader Thread entry function */
/* pvParameters contains TaskHandle_t */
void downloader_thread_entry (void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    fsp_err_t err;

    /* Open the comms driver */
    err = comms_open();
    if (FSP_SUCCESS != err)
    {
        /* Stop as comms open failure */
        while (1)
        {
            ;
        }
    }

    // err =   R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    init_qspi();
    while (1)
    {
        menu();
        vTaskDelay(1);
    }
}
