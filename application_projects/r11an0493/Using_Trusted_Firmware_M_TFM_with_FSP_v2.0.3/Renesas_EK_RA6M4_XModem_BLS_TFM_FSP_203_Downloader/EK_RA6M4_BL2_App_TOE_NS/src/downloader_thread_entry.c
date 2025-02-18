#include "downloader_thread.h"
#include "header.h"
#include "./comms/comms.h"
#include "stdio.h"
#include "menu.h"
#include "psa/error.h"
#include "psa/initial_attestation.h"
#include "psa/internal_trusted_storage.h"
#include "tfm_psa/psa/crypto.h"
#include "hal_data.h"
#include "tfm_ns_interface.h"
#include "tfm_platform_api.h"
#include "tfm_ioctl_api.h"

/* Downloader Thread entry function */
/* pvParameters contains TaskHandle_t */
void downloader_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    uint8_t str[100];
    fsp_err_t err;

    tfm_ns_interface_init();

    /* Open the comms driver */
    err = comms_open();
    if (FSP_SUCCESS != err)
    {
        /* Stop as comms open failure */
        while(1)
        {
            ;
        }
    }

#if 0
    /* Open the flash driver */
    err = g_flash0.p_api->open(g_flash0.p_ctrl, g_flash0.p_cfg);
    if (FSP_SUCCESS != err)
    {
        snprintf((char *)str, sizeof(str), "ERROR: Opening flash driver failed!\r\n");
        comms_send(str, strlen((char *)str));
    }
#endif

    uint32_t tfm_err_status;
    uint32_t flash_test_result = 0;
    tfm_err_status = tfm_platform_flash_init((int32_t *)&flash_test_result);
    if (TFM_PLATFORM_ERR_SUCCESS != tfm_err_status)
    {
        snprintf((char *)str, sizeof(str), "ERROR: Opening flash driver failed (tfm_platform_flash_init)!\r\n");
        comms_send(str, strlen((char *)str));
        sprintf((char *)str, "ERROR: tfm_err_status = %d\r\n", (int)tfm_err_status);
        comms_send(str, strlen((char *)str));
        sprintf((char *)str, "ERROR: flash_test_result=%d\r\n", (int)flash_test_result);
        comms_send(str, strlen((char *)str));
    }

    while(1)
    {
        menu();
    }
}

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

    struct image_header * p_img_header;

    /* Primary NS Image Slot */
    p_img_header = (struct image_header *)PRIMARY_NS_IMAGE_START_ADDRESS;
    snprintf((char *)str, sizeof(str), "*************************\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "* Primary NS Image Slot *\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "*************************\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Image version: \t\t%02d.%02d (Rev: %d, Build: %ld)\r\n", p_img_header->ih_ver.iv_major,
                                                                                             p_img_header->ih_ver.iv_minor,
                                                                                             p_img_header->ih_ver.iv_revision,
                                                                                             p_img_header->ih_ver.iv_build_num);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Load address: \t\t0x%08X\r\n", (unsigned int)p_img_header->ih_load_addr);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Header size: \t\t0x%04X (%d bytes)\r\n", p_img_header->ih_hdr_size, p_img_header->ih_hdr_size);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Protected TLV size: \t0x%04X (%d bytes)\r\n", p_img_header->ih_protect_tlv_size, p_img_header->ih_protect_tlv_size);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Image size: \t\t0x%08X (%ld bytes)\r\n", (unsigned int)p_img_header->ih_img_size, p_img_header->ih_img_size);
    comms_send(str, strlen((char *)str));

    /* Secondary S Image Slot */
    p_img_header = (struct image_header *)SECONDARY_S_IMAGE_START_ADDRESS;
    snprintf((char *)str, sizeof(str), "\r\n**************************\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "* Secondary S Image Slot *\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "**************************\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Image version: \t\t%02d.%02d (Rev: %d, Build: %ld)\r\n", p_img_header->ih_ver.iv_major,
                                                                                             p_img_header->ih_ver.iv_minor,
                                                                                             p_img_header->ih_ver.iv_revision,
                                                                                             p_img_header->ih_ver.iv_build_num);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Load address: \t\t0x%08X\r\n", (unsigned int)p_img_header->ih_load_addr);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Header size: \t\t0x%04X (%d bytes)\r\n", p_img_header->ih_hdr_size, p_img_header->ih_hdr_size);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Protected TLV size: \t0x%04X (%d bytes)\r\n", p_img_header->ih_protect_tlv_size, p_img_header->ih_protect_tlv_size);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Image size: \t\t0x%08X (%ld bytes)\r\n", (unsigned int)p_img_header->ih_img_size, p_img_header->ih_img_size);
    comms_send(str, strlen((char *)str));

    /* Secondary NS Image Slot */
    p_img_header = (struct image_header *)SECONDARY_NS_IMAGE_START_ADDRESS;
    snprintf((char *)str, sizeof(str), "\r\n***************************\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "* Secondary NS Image Slot *\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "***************************\r\n");
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Image version: \t\t%02d.%02d (Rev: %d, Build: %ld)\r\n", p_img_header->ih_ver.iv_major,
                                                                                             p_img_header->ih_ver.iv_minor,
                                                                                             p_img_header->ih_ver.iv_revision,
                                                                                             p_img_header->ih_ver.iv_build_num);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Load address: \t\t0x%08X\r\n", (unsigned int)p_img_header->ih_load_addr);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Header size: \t\t0x%04X (%d bytes)\r\n", p_img_header->ih_hdr_size, p_img_header->ih_hdr_size);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Protected TLV size: \t0x%04X (%d bytes)\r\n", p_img_header->ih_protect_tlv_size, p_img_header->ih_protect_tlv_size);
    comms_send(str, strlen((char *)str));
    snprintf((char *)str, sizeof(str), "Image size: \t\t0x%08X (%ld bytes)\r\n", (unsigned int)p_img_header->ih_img_size, p_img_header->ih_img_size);
    comms_send(str, strlen((char *)str));
}
