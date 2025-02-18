/***********************************************************************************************************************
 * File Name    : source_cancel_lpm.c
 * Description  : Contains declarations of data structures and functions used in source_cancel_lpm.c
***********************************************************************************************************************/
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
 * Copyright (C) 2020-2025 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

#include "source_cancel_lpm.h"
#include "lp_mode.h"



/**********************************************************************************************************************
* Private global variables declarations
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function implementations
**********************************************************************************************************************/

static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};



/**********************************************************************************************************************
 * Function Name: source_cancel_lpm_init
 * Description  : This function will initializes modules used to cancel LPM mode
 * Return Value : FSP_SUCCESS    Upon successful operation
 *                Any Other Error code apart from FSP_SUCCES
 *********************************************************************************************************************/
fsp_err_t source_cancel_lpm_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize ICU External IRQ used to cancel LPM */
    err = R_ICU_ExternalIrqOpen(&g_external_irq7_ctrl, &g_external_irq7_cfg);
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
 *                Any Other Error code apart from FSP_SUCCES
 *********************************************************************************************************************/
void source_cancel_lpm_deinit(void)
{
    /* Close ICU External IRQ module used to cancel LPM if it is already open */
    if (g_external_irq7_ctrl.open)
    {
        R_ICU_ExternalIrqClose(&g_external_irq7_ctrl);
    }

}
/**********************************************************************************************************************
 End of function source_cancel_lpm_deinit
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: source_cancel_lpm_enable
 * Description  : This function will enable modules to cancel LPM mode
 * Return Value : FSP_SUCCESS    Upon successful operation
 *                Any Other Error code apart from FSP_SUCCES
 *********************************************************************************************************************/
fsp_err_t source_cancel_lpm_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Enable External IRQ channel interrupt */
    err = R_ICU_ExternalIrqEnable(&g_external_irq7_ctrl);
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
 *                Any Other Error code apart from FSP_SUCCES
 *********************************************************************************************************************/
fsp_err_t source_cancel_lpm_disable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Disable External IRQ channel interrupt */
    err = R_ICU_ExternalIrqDisable(&g_external_irq7_ctrl);
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
