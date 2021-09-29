/***********************************************************************************************************************
 * File Name    : dac_operation.c
 * Description  : Contains function definitions.
 **********************************************************************************************************************/
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "dac_operation.h"

/*******************************************************************************************************************//**
 * @addtogroup acmplp_ep
 * @{
 **********************************************************************************************************************/

/*****************************************************************************************************************
 *  @brief     Setting DAC value to output pin
 *  @param[in] *p_val
 *  @retval    FSP_SUCCESS         R_DAC Write and conversion operation performed successfully.
 *  @retval    error               Any Other Error code apart from FSP_SUCCES Unsuccessful write.
 ****************************************************************************************************************/
fsp_err_t dac_write(uint16_t  * p_val)
{
    fsp_err_t error = FSP_SUCCESS;

    /* Write to DAC channel*/
    error = R_DAC_Write(&g_dac_ctrl, *p_val);
    if (FSP_SUCCESS != error)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_WRITE API FAILED ** \r\n");
    }
    return error;
}

/*******************************************************************************************************************//**
 * @brief     Initialize and start DAC.
 * @param[in] None
 * @retval    FSP_SUCCESS         R_DAC opened successfully.
 * @retval    error               Any Other Error code apart from FSP_SUCCES Unsuccessful Open.
 **********************************************************************************************************************/
fsp_err_t init_dac(void)
{
    fsp_err_t error = FSP_SUCCESS;

    /* Initialize the DAC module */
    error = R_DAC_Open(&g_dac_ctrl, &g_dac_cfg);
    if (FSP_SUCCESS != error)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_OPEN API FAILED ** \r\n");
        return error;
    }

    /* Start the conversion */
    error = R_DAC_Start(&g_dac_ctrl);
    if (FSP_SUCCESS != error)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_START API FAILED ** \r\n");
        /* de-initialize opened DAC module since module is in open state now */
        deinit_dac();
    }

    return error;
}

/*****************************************************************************************************************
 * This function is called to do closing of DAC module using its HAL level API.
 * @brief    Close the DAC module. Handle the Error internally with Proper Message.
 * @param[in] None
 * @retval    None
 ****************************************************************************************************************/
void deinit_dac(void)
{
    fsp_err_t error = FSP_SUCCESS;

    /* Closing ACMPLP instance */
    error = R_DAC_Close(&g_dac_ctrl);
    if (FSP_SUCCESS != error)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_ClOSE API FAILED ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup acmplp_ep)
 **********************************************************************************************************************/
