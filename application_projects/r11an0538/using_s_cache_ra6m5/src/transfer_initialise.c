/***********************************************************************************************************************
 * File Name    : transfer_initialise.c
 * Description  : Contains data structures and functions used in transfer_initialise.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
#include "common_utils.h"
#include "transfer_initialise.h"

/*******************************************************************************************************************//**
 *  @brief      Initializing dmac transfer instance based on transfer unit and enable for transfer
 *  @param[in]  p_transfer_ctl       Transfer instance control structure
 *  @param[in]  p_transfer_cfg       Transfer instance configuration structure
 *  @param[in]  transfer_unit        DMAC transfer unit
 *  @retval     FSP_SUCCESS          On successful initialization of dmac_tranfer
 *  @retval     Any Other Error code apart from FSP_SUCCESS if Unsuccessful open or enable
 **********************************************************************************************************************/
fsp_err_t dmac_transfer_init(dmac_instance_ctrl_t *const p_transfer_ctl,
        transfer_cfg_t const *const p_transfer_cfg, uint8_t transfer_unit)
{
    /* Variable to help handle error codes */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Open the transfer instance */
    fsp_err = R_DMAC_Open(p_transfer_ctl, p_transfer_cfg);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        APP_ERR_PRINT("**R_DMAC_Open API for transfer%d failed ** \r\n",transfer_unit);
        return fsp_err;
    }

    /* Enable transfer for the configured activation source*/
    fsp_err = R_DMAC_Enable(p_transfer_ctl);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        fsp_err_t err = FSP_SUCCESS;
        /* In case of DMAC_open is successful and enabling fails, requires a immediate cleanup.
         * Since, cleanup for dmac open is done here, cleanup in dmac_transfer_deinit for open is not required */
        err = R_DMAC_Close(p_transfer_ctl);
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* DMAC Close failure message */
            APP_ERR_PRINT("** R_DMAC_Close for transfer%d API failed **  \r\n", transfer_unit);
        }
    }
    return fsp_err;
}

/*******************************************************************************************************************//**
 *  @brief     Deinitializing dmac transfer instance based on transfer unit and enable for transfer
 *  @param[in] p_transfer_ctl       Transfer instance control structure
 *  @param[in] transfer_unit        DMAC transfer unit
 *  @retval    None
 **********************************************************************************************************************/
void dmac_transfer_deinit(dmac_instance_ctrl_t *const p_transfer_ctl, uint8_t transfer_unit)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_DMAC_Close(p_transfer_ctl);
    /* handle error */
    if (FSP_SUCCESS != fsp_err)
    {
        /* DMAC Close failure message */
        APP_ERR_PRINT("** R_DMAC_Close for transfer%d API failed **  \r\n", transfer_unit);
    }
}


/*******************************************************************************************************************//**
 *  @brief          start dmac transfer
 *  @param[in]      transfer_ctrl_t
 *  @retval         FSP_SUCCESS      Upon successful start of dmac
 *  @retval         Any Other Error code apart from FSP_SUCCESS is  Unsuccessful  start
 **********************************************************************************************************************/
fsp_err_t dmac_transfer_software_start(transfer_ctrl_t * const p_transfer_ctrl)
{

    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Start DMAC transfer by software */
    fsp_err = R_DMAC_SoftwareStart(p_transfer_ctrl, TRANSFER_START_MODE_SINGLE);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        APP_ERR_PRINT("\r\nDMAC dmac transfer software start failed.\n");
    }
    return fsp_err;
}

/*******************************************************************************************************************//**
 *  @brief       Set the source and destination address for the DMAC transfer instance
 *  @param[IN]   p_config       Transfer instance configuration structure
 *  @param[IN]   p_src          Source address
 *  @param[IN]   p_dest         Destination address
 *  @retval      None
 **********************************************************************************************************************/
void set_transfer_dst_src_address(transfer_cfg_t const * const p_config,
        void const * volatile p_src, void const * volatile p_dest )
{
    p_config->p_info->p_src = (void *) p_src;
    p_config->p_info->p_dest = (void *) p_dest;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup transfer_initialise)
 **********************************************************************************************************************/
