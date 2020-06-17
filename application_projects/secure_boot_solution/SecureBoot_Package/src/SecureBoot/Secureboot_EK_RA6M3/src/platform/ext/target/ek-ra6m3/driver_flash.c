/***********************************************************************************************************************
* File Name    : driver_flash.c
* Description  : This file contains flash driver specific API implementation.
* ***********************************************************************************************************************/

/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

#include <Driver_Flash.h>
#include <string.h>
#include <stdint.h>
#include "hal_data.h"

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif

/* Driver version */
#define ARM_FLASH_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

#define NUM_OF_CF_BLOCKS(x)     ((x) / FLASH0_SECTOR_SIZE)

/* Code Flash */
#define FLASH0_BASE_S         0x0
#define FLASH0_SIZE           0x00200000 /* 2 MB */
#define FLASH0_SECTOR_SIZE    0x00008000 /* 32 kB */
#define FLASH0_PAGE_SIZE      0x00000080 /* 128 bytes */
#define FLASH0_PROGRAM_UNIT   0x80       /* Minimum write size */

/* Data Flash */
#define FLASH1_BASE_S         0x40100000
#define FLASH1_SIZE           0x10000   /* 64 KB */
#define FLASH1_SECTOR_SIZE    0x40      /* 64 bytes */
#define FLASH1_PAGE_SIZE      0x4       /* 4 bytes */
#define FLASH1_PROGRAM_UNIT   0x4       /* 4 bytes */



/*
 * ARM FLASH device structure
 */
struct arm_flash_dev_t {
    const uint32_t memory_base;   /*!< FLASH memory base address */
    ARM_FLASH_INFO *data;         /*!< FLASH data */
};

/* Driver Version */
static ARM_DRIVER_VERSION DriverVersion = {
    ARM_FLASH_API_VERSION,
    ARM_FLASH_DRV_VERSION
};

struct ARM_Flash_FAW_Configure {
    const uint32_t start_addr;
    const uint32_t end_addr;
};

static int32_t is_range_valid(struct arm_flash_dev_t *flash_dev,
                              uint32_t offset)
{
    uint32_t flash_limit = 0;
    int32_t rc = 0;

    flash_limit = (flash_dev->data->sector_count * flash_dev->data->sector_size)
                   - 1;

    if (offset > flash_limit) {
        rc = -1;
    }
    return rc;
}

static int32_t is_write_aligned(struct arm_flash_dev_t *flash_dev,
                                uint32_t param)
{
    int32_t rc = 0;

    if ((param % flash_dev->data->program_unit) != 0) {
        rc = -1;
    }
    return rc;
}

static int32_t is_sector_aligned(struct arm_flash_dev_t *flash_dev,
                                 uint32_t offset)
{
    int32_t rc = 0;

    if ((offset % flash_dev->data->sector_size) != 0) {
        rc = -1;
    }
    return rc;
}


static ARM_FLASH_INFO ARM_FLASH0_DEV_DATA = {
    .sector_info  = NULL,                  /* Uniform sector layout */
    .sector_count = FLASH0_SIZE / FLASH0_SECTOR_SIZE,
    .sector_size  = FLASH0_SECTOR_SIZE,
    .page_size    = FLASH0_PAGE_SIZE,
    .program_unit = FLASH0_PROGRAM_UNIT,
    .erased_value = 0xFF};

static ARM_FLASH_INFO ARM_FLASH1_DEV_DATA = {
     .sector_info  = NULL,                  /* Uniform sector layout */
     .sector_count = FLASH1_SIZE / FLASH1_SECTOR_SIZE,
     .sector_size  = FLASH1_SECTOR_SIZE,
     .page_size    = FLASH1_PAGE_SIZE,
     .program_unit = FLASH1_PROGRAM_UNIT,
     .erased_value = 0xFF
};

static struct arm_flash_dev_t ARM_FLASH0_DEV = {
#if (__DOMAIN_NS == 1)
    .memory_base = FLASH0_BASE_NS,
#else
    .memory_base = FLASH0_BASE_S,
#endif /* __DOMAIN_NS == 1 */
    .data        = &(ARM_FLASH0_DEV_DATA)};

static struct arm_flash_dev_t ARM_FLASH1_DEV = {
    .memory_base = FLASH1_BASE_S,

    .data = &(ARM_FLASH1_DEV_DATA)
};

struct arm_flash_dev_t *FLASH0_DEV = &ARM_FLASH0_DEV;
struct arm_flash_dev_t *FLASH1_DEV = &ARM_FLASH1_DEV;


/*
 * Functions
 */


static ARM_DRIVER_VERSION ARM_Flash_GetVersion(void)
{
    fsp_version_t ver;
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_FLASH_HP_VersionGet(&ver);
    if(FSP_SUCCESS != fsp_err)
    {
        DriverVersion.api = (uint16_t)((uint16_t)(ver.api_version_major << 8) | (ver.api_version_minor));
        DriverVersion.drv = (uint16_t)((uint16_t)(ver.code_version_major << 8) | (ver.code_version_minor));
    }
    else
    {
        memset(&DriverVersion, 0, sizeof(DriverVersion));
    }

    return DriverVersion;
}

static int32_t ARM_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);
    fsp_err_t  fsp_err = FSP_SUCCESS;

    fsp_err = R_FLASH_HP_Open(&g_flash_ctrl, &g_flash_cfg);
    if(FSP_SUCCESS != fsp_err)
        return ARM_DRIVER_ERROR;
    else
        return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_Uninitialize(void)
{
    fsp_err_t  fsp_err = FSP_SUCCESS;

    fsp_err = R_FLASH_HP_Close(&g_flash_ctrl);
    if(FSP_SUCCESS != fsp_err)
        return ARM_DRIVER_ERROR;
    else
        return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_PowerControl(ARM_POWER_STATE state)
{
    int32_t ret = ARM_DRIVER_OK;

    switch (state) {
    case ARM_POWER_FULL:
        /* Nothing to be done */
        ret = ARM_DRIVER_OK;
        break;

    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
    default:
        ret = ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ret;
}

static int32_t ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    int32_t rc = 0;

    if(data == NULL || cnt == 0)
        return ARM_DRIVER_ERROR_PARAMETER;

    if(addr >= FLASH1_BASE_S)
    {
        /* Check flash memory boundaries */
        rc = is_range_valid(FLASH1_DEV, (addr - FLASH1_DEV->memory_base)  + cnt);
        if (rc != 0) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }
    }
    else
    {
        /* Check flash memory boundaries */
        rc = is_range_valid(FLASH0_DEV, addr + cnt);
        if (rc != 0) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }
    }

    memcpy(data, (void *)addr, cnt);

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ProgramData(uint32_t addr, const void *data,
                                     uint32_t cnt)
{
    int32_t rc = 0;
    fsp_err_t fsp_err = FSP_SUCCESS;
    unsigned int copy_size = 0;

    if(data == NULL || cnt == 0)
        return ARM_DRIVER_ERROR_PARAMETER;

    uint8_t *buf = (uint8_t*)data;

    if(addr >= FLASH1_BASE_S)
    {
         /* Check flash memory boundaries and alignment with minimal write size */
        rc = is_range_valid(FLASH1_DEV, (addr - FLASH1_DEV->memory_base) + cnt);
        rc |= is_write_aligned(FLASH1_DEV, addr);
        rc |= is_write_aligned(FLASH1_DEV, cnt);
        if (rc != 0) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }
    }
    else
    {
         /* Check flash memory boundaries and alignment with minimal write size */
        rc = is_range_valid(FLASH0_DEV, addr + cnt);
        rc |= is_write_aligned(FLASH0_DEV, addr);
        rc |= is_write_aligned(FLASH0_DEV, cnt);
        if (rc != 0) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }
    }

    while(cnt > 0)
    {
        if(cnt > FLASH0_DEV->data->page_size)
        {
            copy_size = FLASH0_DEV->data->page_size;
        }
        else
        {
            copy_size = cnt;
        }

        FSP_CRITICAL_SECTION_DEFINE;

        FSP_CRITICAL_SECTION_ENTER;

        fsp_err = R_FLASH_HP_Write(&g_flash_ctrl, (uint32_t)buf, addr, copy_size);

        FSP_CRITICAL_SECTION_EXIT;

        if(FSP_SUCCESS == fsp_err)
        {
            if(cnt >= copy_size)
                cnt -= copy_size;
            else
                cnt = 0;

            buf += (copy_size);
            addr += (copy_size);
        }
        else
        {
            return ARM_DRIVER_ERROR;
        }
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseSector(uint32_t addr)
{
    int32_t rc = 0;
    fsp_err_t fsp_err = FSP_SUCCESS;

    if(addr >= FLASH1_BASE_S)
    {
        rc  = is_range_valid(FLASH1_DEV, (addr - FLASH1_DEV->memory_base));
        rc |= is_sector_aligned(FLASH1_DEV, (addr - FLASH1_DEV->memory_base));
        if (rc != 0) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }
    }
    else
    {
        rc  = is_range_valid(FLASH0_DEV, addr);
        rc |= is_sector_aligned(FLASH0_DEV, addr);
        if (rc != 0) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }
    }

    FSP_CRITICAL_SECTION_DEFINE;

    FSP_CRITICAL_SECTION_ENTER;

    fsp_err = R_FLASH_HP_Erase(&g_flash_ctrl, addr, 1);

    FSP_CRITICAL_SECTION_EXIT;

    if(FSP_SUCCESS == fsp_err)
        return ARM_DRIVER_OK;
    else
        return ARM_DRIVER_ERROR;
}

static ARM_FLASH_INFO *ARM_Flash_GetInfo(void)
{
    return FLASH0_DEV->data;
}

ARM_DRIVER_FLASH Driver_FLASH0 = {
    ARM_Flash_GetVersion,
    NULL,
    ARM_Flash_Initialize,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    ARM_Flash_ReadData,
    ARM_Flash_ProgramData,
    ARM_Flash_EraseSector,
    NULL,
    NULL,
    ARM_Flash_GetInfo
};

