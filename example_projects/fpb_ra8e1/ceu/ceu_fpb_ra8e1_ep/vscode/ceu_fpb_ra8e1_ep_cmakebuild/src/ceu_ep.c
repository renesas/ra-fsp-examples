/***********************************************************************************************************************
 * File Name    : ceu_ep.c
 * Description  : Contains data structures and functions used in ceu_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "ceu_ep.h"

/* Capture completion flag */
volatile bool g_capture_ready = false;

/*******************************************************************************************************************//**
 * @brief     This function opens the CEU module.
 * @param[in] p_ctrl                    Pointer to the CEU control block.
 * @param[in] p_cfg                     Pointer to the CEU configuration structure.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t ceu_init(capture_ctrl_t * const p_ctrl, capture_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_CEU_Open(p_ctrl, p_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function starts a capture.
 * @param[in]  p_ctrl                   Pointer to the CEU control block.
 * @param[out] p_buffer                 Pointer to the capture buffer.
 * @retval     FSP_SUCCESS              Started successfully.
 * @retval     Any other error codes    Failed to start.
 **********************************************************************************************************************/
fsp_err_t ceu_capture_start(capture_ctrl_t * const p_ctrl, uint8_t * const p_buffer)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_CEU_CaptureStart(p_ctrl, p_buffer);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened CEU module.
 * @param[in] p_ctrl                    Pointer to the CEU control block.
 * @retval    None.
 **********************************************************************************************************************/
void ceu_deinit(capture_ctrl_t * const p_ctrl)
{
    ceu_instance_ctrl_t * p_instance_ctrl = (ceu_instance_ctrl_t *) p_ctrl;
    if (p_instance_ctrl -> open != 0U)
    {
        R_CEU_Close(p_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function opens the IIC Master module.
 * @param[in] p_api_ctrl                Pointer to the IIC Master control block.
 * @param[in] p_cfg                     Pointer to the IIC Master configuration structure.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t iic_master_init(i2c_master_ctrl_t * const p_api_ctrl, i2c_master_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_IIC_MASTER_Open(p_api_ctrl, p_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened IIC Master module.
 * @param[in] p_api_ctrl                Pointer to the IIC Master control block.
 * @retval    None.
 **********************************************************************************************************************/
void iic_master_deinit(i2c_master_ctrl_t * const p_api_ctrl)
{
    iic_master_instance_ctrl_t * p_ctrl = (iic_master_instance_ctrl_t *) p_api_ctrl;
    if (p_ctrl -> open != 0U)
    {
        R_IIC_MASTER_Close(p_api_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function opens the GPT module.
 * @param[in] p_ctrl                    Pointer to the timer control block.
 * @param[in] p_cfg                     Pointer to the timer configuration structure.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t gpt_init(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Open(p_ctrl, p_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts the GPT module.
 * @param[in] p_ctrl                    Pointer to the timer control block.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t gpt_start(timer_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Start(p_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened GPT module.
 * @param[in] p_ctrl                    Pointer to the timer control block.
 * @retval    None.
 **********************************************************************************************************************/
void gpt_deinit(timer_ctrl_t * const p_ctrl)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
    if (p_instance_ctrl -> open != 0U)
    {
        R_GPT_Close(p_ctrl);
    }
}

#if defined (BOARD_RA8E1_FPB)
/*******************************************************************************************************************//**
 * @brief     This function is a callback for QVGA CEU module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void ceu_qvga_callback(capture_callback_args_t * p_args)
{
    if (CEU_EVENT_FRAME_END == p_args->event)
    {
        g_capture_ready = true;
    }
}
#else
/*******************************************************************************************************************//**
 * @brief     This function is a callback for SXGA CEU module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void ceu_sxga_callback(capture_callback_args_t * p_args)
{
    if (CEU_EVENT_FRAME_END == p_args->event)
    {
        g_capture_ready = true;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for VGA CEU module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void ceu_vga_callback(capture_callback_args_t * p_args)
{
    if (CEU_EVENT_FRAME_END == p_args->event)
    {
        g_capture_ready = true;
    }
}
#endif /* BOARD_RA8E1_FPB */
