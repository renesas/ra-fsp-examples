/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : source_cancel_lpm.c
 * Version      : 1.0
 * Description  : Functions to return from Low Power Mode .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "source_cancel_lpm.h"
#include "lp_mode.h"

#include "jlink_console.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

/**********************************************************************************************************************
 * Function Name: source_cancel_lpm_init
 * Description  : This function will initializes modules used to cancel LPM mode
 * Return Value : FSP_SUCCESS    Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 *********************************************************************************************************************/
fsp_err_t source_cancel_lpm_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize ICU External IRQ used to cancel LPM */
    err = R_ICU_ExternalIrqOpen(&g_external_irq0_ctrl, &g_external_irq0_cfg);
    if (FSP_SUCCESS != err)
    {
        sprintf (s_print_buffer, "R_ICU_ExternalIrqOpen API Failed\r\n");
        print_to_console (s_print_buffer);
    }

    return err;
}
/**********************************************************************************************************************
 End of function source_cancel_lpm_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: source_cancel_lpm_deinit
 * Description  : This function will de-initializes modules used to cancel LPM mode
 * Return Value : FSP_SUCCESS    Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 *********************************************************************************************************************/
void source_cancel_lpm_deinit(void)
{
    /* Close ICU External IRQ module used to cancel LPM if it is already open */
    if (g_external_irq0_ctrl.open)
    {
        R_ICU_ExternalIrqClose(&g_external_irq0_ctrl);
    }
}
/**********************************************************************************************************************
 End of function source_cancel_lpm_deinit
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: source_cancel_lpm_enable
 * Description  : This function will enable modules to cancel LPM mode
 * Return Value : FSP_SUCCESS    Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 *********************************************************************************************************************/
fsp_err_t source_cancel_lpm_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Enable External IRQ channel interrupt */
    err = R_ICU_ExternalIrqEnable(&g_external_irq0_ctrl);

    if (FSP_SUCCESS != err)
    {
        sprintf (s_print_buffer, "R_ICU_ExternalIrqEnable API Failed\r\n");
        print_to_console (s_print_buffer);
    }

    return err;
}
/**********************************************************************************************************************
 End of function source_cancel_lpm_enable
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: source_cancel_lpm_disable
 * Description  : This function will disable modules to cancel LPM mode
 * Return Value : FSP_SUCCESS    Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 *********************************************************************************************************************/
fsp_err_t source_cancel_lpm_disable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Disable External IRQ channel interrupt */
    err = R_ICU_ExternalIrqDisable(&g_external_irq0_ctrl);
    if (FSP_SUCCESS != err)
    {
        sprintf(s_print_buffer, "R_ICU_ExternalIrqEnable API Failed\r\n");
        print_to_console (s_print_buffer);
    }

    return err;
}
/**********************************************************************************************************************
 End of function source_cancel_lpm_disable
 *********************************************************************************************************************/
