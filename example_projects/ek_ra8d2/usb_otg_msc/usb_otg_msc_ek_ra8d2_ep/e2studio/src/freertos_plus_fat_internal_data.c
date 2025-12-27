/***********************************************************************************************************************
* File Name    : freertos_plus_fat_internal_data.c
* Description  : Contains the configuration and data structures for FreeRTOS Plus FAT file system and SD/MMC driver.
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

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
dtc_instance_ctrl_t g_transfer_sdmmc_internal_ctrl;

#if (1 == 1)
transfer_info_t g_transfer_sdmmc_internal_info DTC_TRANSFER_INFO_ALIGNMENT =
{
    .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
    .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
    .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
    .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
    .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
    .transfer_settings_word_b.size = TRANSFER_SIZE_4_BYTE,
    .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
    .p_dest = (void*) NULL,
    .p_src = (void const*) NULL,
    .num_blocks = 0,
    .length = 128,
};
#elif (1 > 1)
/* User is responsible to initialize the array */
transfer_info_t g_transfer_sdmmc_internal_info[1] DTC_TRANSFER_INFO_ALIGNMENT;
#else
/* User must call api::reconfigure before enabling DTC transfer */
#endif

const dtc_extended_cfg_t g_transfer_sdmmc_internal_cfg_extend =
{
#if defined(VECTOR_NUMBER_SDHIMMC1_DMA_REQ)
    .activation_source = VECTOR_NUMBER_SDHIMMC1_DMA_REQ,
#elif defined(VECTOR_NUMBER_SDHIMMC0_DMA_REQ)
    .activation_source = VECTOR_NUMBER_SDHIMMC0_DMA_REQ,
#endif
};

const transfer_cfg_t g_transfer_sdmmc_internal_cfg =
{
#if (1 == 1)
    .p_info = &g_transfer_sdmmc_internal_info,
#elif (1 > 1)
    .p_info = g_transfer_sdmmc_internal_info,
#else
    .p_info = NULL,
#endif
    .p_extend = &g_transfer_sdmmc_internal_cfg_extend,
};

/* Instance structure to use this module */
const transfer_instance_t g_transfer_sdmmc_internal =
{
    .p_ctrl = &g_transfer_sdmmc_internal_ctrl,
    .p_cfg = &g_transfer_sdmmc_internal_cfg,
    .p_api = &g_transfer_on_dtc,
};

#define RA_NOT_DEFINED (UINT32_MAX)

#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)
/* If the transfer module is DMAC, define a DMAC transfer callback */
#include "r_dmac.h"
extern void r_sdhi_transfer_callback(sdhi_instance_ctrl_t * p_ctrl);

void g_sdmmc_internal_dmac_callback(dmac_callback_args_t * p_args)
{
    r_sdhi_transfer_callback((sdhi_instance_ctrl_t *) p_args->p_context);
}
#endif
#undef RA_NOT_DEFINED

sdhi_instance_ctrl_t g_sdmmc_internal_ctrl;
sdmmc_cfg_t g_sdmmc_internal_cfg =
{
    .bus_width = SDMMC_BUS_WIDTH_4_BITS,
    .channel = SDMMC_CHANNEL,
    .p_callback = rm_block_media_sdmmc_callback,
    .p_context = &g_rm_block_media_internal_ctrl,
    .block_size = 512,
    .card_detect = SDMMC_CARD_DETECT_CD,
    .write_protect = SDMMC_WRITE_PROTECT,
    .p_extend = NULL,
    .p_lower_lvl_transfer = &g_transfer_sdmmc_internal,
    .access_ipl = 12,
    .sdio_ipl = BSP_IRQ_DISABLED,
    .card_ipl = 12,
    .dma_req_ipl = 12,
#if defined(VECTOR_NUMBER_SDHIMMC1_ACCS)
    .access_irq = VECTOR_NUMBER_SDHIMMC1_ACCS,
#elif defined(VECTOR_NUMBER_SDHIMMC0_ACCS)
    .access_irq = VECTOR_NUMBER_SDHIMMC0_ACCS,
#else
    .access_irq = FSP_INVALID_VECTOR,
#endif

#if defined(VECTOR_NUMBER_SDHIMMC1_CARD)
    .card_irq = VECTOR_NUMBER_SDHIMMC1_CARD,
#elif defined(VECTOR_NUMBER_SDHIMMC0_CARD)
    .card_irq = VECTOR_NUMBER_SDHIMMC0_CARD,
#else
    .card_irq = FSP_INVALID_VECTOR,
#endif

    .sdio_irq = FSP_INVALID_VECTOR,

#if defined(VECTOR_NUMBER_SDHIMMC1_DMA_REQ)
    .dma_req_irq = VECTOR_NUMBER_SDHIMMC1_DMA_REQ,
#elif defined(VECTOR_NUMBER_SDHIMMC0_DMA_REQ)
    .dma_req_irq = VECTOR_NUMBER_SDHIMMC0_DMA_REQ,
#else
    .dma_req_irq = FSP_INVALID_VECTOR,
#endif
};

/* Instance structure to use this module */
const sdmmc_instance_t g_sdmmc_internal =
{
    .p_ctrl = &g_sdmmc_internal_ctrl,
    .p_cfg = &g_sdmmc_internal_cfg,
    .p_api = &g_sdmmc_on_sdhi,
};

const rm_block_media_sdmmc_extended_cfg_t g_rm_block_media_internal_cfg_extend =
{
    .p_sdmmc = &g_sdmmc_internal,
};

const rm_block_media_cfg_t g_rm_block_media_internal_cfg =
{
    .p_extend = &g_rm_block_media_internal_cfg_extend,
    .p_callback = rm_freertos_plus_fat_memory_callback,
    .p_context = &g_rm_freertos_plus_fat_internal_ctrl,
};

rm_block_media_sdmmc_instance_ctrl_t g_rm_block_media_internal_ctrl;

const rm_block_media_instance_t g_rm_block_media_internal =
{
    .p_api = &g_rm_block_media_on_sdmmc,
    .p_ctrl = &g_rm_block_media_internal_ctrl,
    .p_cfg = &g_rm_block_media_internal_cfg,
};

rm_freertos_plus_fat_instance_ctrl_t g_rm_freertos_plus_fat_internal_ctrl;

const rm_freertos_plus_fat_cfg_t g_rm_freertos_plus_fat_internal_cfg =
{
    .p_block_media = &g_rm_block_media_internal,
    .p_callback = freertos_plus_fat_internal_callback,
};

static uint8_t g_rm_freertos_plus_fat_internal_cache[1024];
rm_freertos_plus_fat_disk_cfg_t g_rm_freertos_plus_fat_internal_disk_cfg =
{
    .device.sector_count = 31293440,
    .device.sector_size_bytes = 512,
    .cache_size_bytes = 1024,
    .p_cache = &g_rm_freertos_plus_fat_internal_cache[0],
    .partition_number = 0,
};

/* Instance structure to use this module */
const rm_freertos_plus_fat_instance_t g_rm_freertos_plus_fat_internal =
{
    .p_ctrl = &g_rm_freertos_plus_fat_internal_ctrl,
    .p_cfg = &g_rm_freertos_plus_fat_internal_cfg,
    .p_api = &g_fat_on_freertos,
};
