/***********************************************************************************************************************
 * File Name    : iwdt_ep.c
 * Description  : Contains data structures and functions used in iwdt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "iwdt_ep.h"

/*******************************************************************************************************************//**
 * @brief     This function opens the IWDT module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t iwdt_open(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open IWDT module */
    err = R_IWDT_Open(&g_iwdt_ctrl, &g_iwdt_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function refreshes the IWDT module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Refreshed successfully.
 * @retval    Any other error codes     Failed to refresh.
 **********************************************************************************************************************/
fsp_err_t iwdt_refresh(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Refresh IWDT module */
    err = R_IWDT_Refresh(&g_iwdt_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enables the IWDT counter to count in debug mode.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void enable_iwdt_count_in_debug_mode(void)
{
    /* As per hardware manual's DBGREG module, section 2.6.4.2:
     * Clear this bit to enable IWDT Reset/NMI in debug mode */
    R_DEBUG->DBGSTOPCR_b.DBGSTOP_IWDT = RESET_VALUE;
}

/*******************************************************************************************************************//**
 * @brief     This function checks if the last reset was caused by IWDT underflow or NMI software reset.
 * @param[in] None.
 * @retval    RESET_IWDT                Reset was caused by IWDT underflow.
 * @retval    RESET_NMI_SW              Reset was caused by NMI software reset.
 * @retval    RESET_NONE                No IWDT or NMI reset detected.
 **********************************************************************************************************************/
reset_cause_t check_reset_status(void)
{
    /* Check if reset was caused by the IWDT */
#if defined BOARD_RA0E1_FPB || defined (BOARD_RA0E2_FPB) || defined (BOARD_RA0L1_FPB) || defined (BOARD_RA0E3_FPB)
    if (SYSTEM_RSTSR1_IWDTRF_DETECT_RESET == R_SYSTEM->RESF_b.IWDTRF)
#else
    if (SYSTEM_RSTSR1_IWDTRF_DETECT_RESET == R_SYSTEM->RSTSR1_b.IWDTRF)
#endif /* BOARD_RA0E1_FPB || BOARD_RA0E2_FPB || BOARD_RA0L1_FPB || BOARD_RA0E3_FPB */
    {
        /* Clear the flag once read the value */
#if defined BOARD_RA0E1_FPB || defined (BOARD_RA0E2_FPB) || defined (BOARD_RA0L1_FPB) || defined (BOARD_RA0E3_FPB)
        /* Do nothing */
#else
        R_SYSTEM->RSTSR1_b.IWDTRF = RESET_VALUE;
#endif /* BOARD_RA0E1_FPB || BOARD_RA0E2_FPB || BOARD_RA0L1_FPB || BOARD_RA0E3_FPB */
        return RESET_IWDT;
    }

    /* Check if reset was caused by NMI software reset */
#if defined BOARD_RA0E1_FPB || defined (BOARD_RA0E2_FPB) || defined (BOARD_RA0L1_FPB) || defined (BOARD_RA0E3_FPB)
    else if (SYSTEM_RSTSR1_SWRF_DETECT_RESET == R_SYSTEM->RESF_b.SWRF)
#else
    else if (SYSTEM_RSTSR1_SWRF_DETECT_RESET == R_SYSTEM->RSTSR1_b.SWRF)
#endif /* BOARD_RA0E1_FPB || BOARD_RA0E2_FPB || BOARD_RA0L1_FPB || BOARD_RA0E3_FPB */
    {
        /* Clear the flag once read the value */
#if defined BOARD_RA0E1_FPB || defined (BOARD_RA0E2_FPB) || defined (BOARD_RA0L1_FPB) || defined (BOARD_RA0E3_FPB)
        /* Do nothing */
#else
        R_SYSTEM->RSTSR1_b.SWRF = RESET_VALUE;
#endif /* BOARD_RA0E1_FPB || BOARD_RA0E2_FPB || BOARD_RA0L1_FPB || BOARD_RA0E3_FPB */
        return RESET_NMI_SW;
    }
    else
    {
        /* Do nothing */
    }

    return RESET_NONE;
}

/*******************************************************************************************************************//**
 * @brief     This function is the NMI callback function.
 * @param[in] p_args                    Callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void NMI_callback(wdt_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Issue a software reset to reset the MCU */
    __NVIC_SystemReset();
}
