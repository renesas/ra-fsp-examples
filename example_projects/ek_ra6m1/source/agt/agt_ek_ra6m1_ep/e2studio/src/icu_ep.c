/***********************************************************************************************************************
 * File Name    : r_icu_ep.c
 * Description  : Contains data structures and functions used in r_icu_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * This file is part of Renesas RA Flex Software Package (FSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/
#include "common_utils.h"
#include "icu_ep.h"
#include "agt_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup icu_ep
 * @{
 **********************************************************************************************************************/

extern volatile uint8_t g_icu_flag;
extern volatile uint8_t g_one_shot_timer_flag ;
extern volatile uint32_t g_error_flag ;
/*******************************************************************************************************************//**
 * @brief       This functions initializes ICU module.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful open of ICU module
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t ext_irq_init(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Open External IRQ module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_ctrl, &g_external_irq_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nICU driver failed.\r\nRestart the Application\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions enables external interrupt.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  External interrupt for the specified channel is enabled
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t ext_irq_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    if(!g_icu_flag)
    {
        /* Enable Push button */
        err = R_ICU_ExternalIrqEnable(&g_external_irq_ctrl);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\nEnabling the push button failed\r\n");
            return err;
        }
        else
        {
            APP_PRINT( "\r\nPush Button Enabled. Press the push button\r\n");
            g_icu_flag = SET_FLAG;           // Set ICU Flag as Ext_IRQ is enabled
        }
    }
    else
    {
        APP_PRINT("\r\nPush Button is already Enabled.\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions disables external interrupt.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  External interrupt for the specified channel is disabled
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t ext_irq_disable(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Disable the interrupt */
    err = R_ICU_ExternalIrqDisable(&g_external_irq_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nDisabling the interrupt failed\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief This function is callback for r_icu module and starts AGT0 timer in One shot mode.
 *
 * @param[in] (external_irq_callback_args_t *)
 **********************************************************************************************************************/
void ext_irq_callback(external_irq_callback_args_t *p_args)
{
    g_one_shot_timer_flag = RESET_FLAG; //Clear the timer flag since the timer is started in callback only

    fsp_err_t err = FSP_SUCCESS;
    FSP_PARAMETER_NOT_USED(p_args);

    err = agt_start_oneshot_timer();
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        g_error_flag = SET_FLAG;
        return;
    }
    else
    {
        g_one_shot_timer_flag = SET_FLAG;         // Set Timer Flag as timer is started
    }
}
/*******************************************************************************************************************//**
 * @brief This function closes opened ICU module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void ext_irq_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close ICU module */
    err = R_ICU_ExternalIrqClose(&g_external_irq_ctrl);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Close failure message */
        APP_ERR_PRINT("** R_ICU_ExternalIrqClose API failed **  \r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup icu_ep)
 **********************************************************************************************************************/



