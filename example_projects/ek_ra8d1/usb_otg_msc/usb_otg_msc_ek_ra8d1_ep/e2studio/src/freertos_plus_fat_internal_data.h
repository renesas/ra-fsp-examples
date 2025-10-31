/***********************************************************************************************************************
* File Name    : freertos_plus_fat_internal_data.h
* Description  : Contains the configuration and data structures used in freertos_plus_fat_internal_data.c
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FREERTOS_PLUS_FAT_INTERNAL_DATA_H_
#define FREERTOS_PLUS_FAT_INTERNAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "r_dtc.h"
#include "r_transfer_api.h"
#include "r_sdhi.h"
#include "r_sdmmc_api.h"
#include "rm_block_media_sdmmc.h"
#include "rm_block_media_api.h"
#include "ff_headers.h"
#include "rm_freertos_plus_fat_api.h"
#include "rm_freertos_plus_fat.h"
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8P1_EK)
#define SDMMC_CHANNEL                        (1U)
#else
#define SDMMC_CHANNEL                        (0U) 
#endif

#if defined(BOARD_RA8T1_MCK)
#define SDMMC_WRITE_PROTECT                  (SDMMC_WRITE_PROTECT_NONE)
#else
#define SDMMC_WRITE_PROTECT                  (SDMMC_WRITE_PROTECT_WP)
#endif

/* Transfer on DTC Instance */
extern const transfer_instance_t g_transfer_sdmmc_internal;

/* Access the DTC instance using these structures when calling API functions directly (::p_api is not used) */
extern dtc_instance_ctrl_t g_transfer_sdmmc_internal_ctrl;
extern const transfer_cfg_t g_transfer_sdmmc_internal_cfg;
/* SDMMC on SDMMC Instance */
extern const sdmmc_instance_t g_sdmmc_internal;

/* Access the SDMMC instance using these structures when calling API functions directly (::p_api is not used) */
extern sdhi_instance_ctrl_t g_sdmmc_internal_ctrl;
extern sdmmc_cfg_t g_sdmmc_internal_cfg;

#ifndef rm_block_media_sdmmc_callback
void rm_block_media_sdmmc_callback(sdmmc_callback_args_t *p_args);
#endif
extern const rm_block_media_instance_t g_rm_block_media_internal;

/* Access the SDMMC instance using these structures when calling API functions directly (::p_api is not used) */
extern rm_block_media_sdmmc_instance_ctrl_t g_rm_block_media_internal_ctrl;
extern const rm_block_media_cfg_t g_rm_block_media_internal_cfg;

#ifndef rm_freertos_plus_fat_memory_callback
void rm_freertos_plus_fat_memory_callback(rm_block_media_callback_args_t *p_args);
#endif
/* FAT on FreeRTOS Instance */
extern const rm_freertos_plus_fat_instance_t g_rm_freertos_plus_fat_internal;
extern rm_freertos_plus_fat_instance_ctrl_t g_rm_freertos_plus_fat_internal_ctrl;
extern const rm_freertos_plus_fat_cfg_t g_rm_freertos_plus_fat_internal_cfg;
extern rm_freertos_plus_fat_disk_cfg_t g_rm_freertos_plus_fat_internal_disk_cfg;

#ifndef freertos_plus_fat_internal_callback
void freertos_plus_fat_internal_callback(rm_freertos_plus_fat_callback_args_t *p_args);
#endif

FSP_FOOTER
#endif /* FREERTOS_PLUS_FAT_INTERNAL_DATA_H_ */
