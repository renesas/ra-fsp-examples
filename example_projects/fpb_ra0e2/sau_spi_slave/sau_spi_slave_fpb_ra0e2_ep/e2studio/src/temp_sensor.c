/***********************************************************************************************************************
 * File Name    : temp_sensor.c
 * Description  : Contains data structures and functions used in sau_spi_slave_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "temp_sensor.h"

/***********************************************************************************************************************
 *  Function Name: adc_init
 *  Description  : This function opens the ADC, configures and starts the scan operation.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS      Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t adc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open/Initialize ADC module */
    err = R_ADC_D_Open(&g_adc_ctrl, &g_adc_cfg);
    APP_ERR_RET(FSP_SUCCESS != err , err, "** R_ADC_D_Open API failed **\r\n");

    /* Configure the ADC scan parameters */
    err = R_ADC_D_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
    APP_ERR_RET(FSP_SUCCESS != err , err, "** R_ADC_D_ScanCfg API failed **\r\n");

    /* Start the ADC scan */
    err = R_ADC_D_ScanStart(&g_adc_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err , err, "** R_ADC_D_ScanStart API failed **\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function adc_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: adc_read_data
 *  Description  : This function reads the ADC output data from the prescribed channel.
 *  Arguments    : *adc_data        Store ADC data.
 *  Return Value : FSP_SUCCESS      Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t adc_read_data(uint16_t *adc_data)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Read the MCU Temperature from ADC */
    err = R_ADC_D_Read(&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, adc_data);
    APP_ERR_RET(FSP_SUCCESS != err , err, "** R_ADC_D_Read API failed **\r\n");

    /* Wait for conversion time */
    R_BSP_SoftwareDelay(DELAY_ONE_SECOND, BSP_DELAY_UNITS_SECONDS);

    return err;
}
/***********************************************************************************************************************
* End of function adc_read_data
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: adc_deinit
 *  Description  : This function de-initializes opened ADC module.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void adc_deinit(void)
{
    /* Close the opened ADC module */
    if (MODULE_CLOSE != g_adc_ctrl.opened)
    {
        if (FSP_SUCCESS != R_ADC_D_Close(&g_adc_ctrl))
        {
            APP_ERR_PRINT("** R_ADC_D_Close API failed **\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function adc_deinit
***********************************************************************************************************************/
