/***********************************************************************************************************************
 * File Name    : acmplp_ep.c
 * Description  : Contains function definitions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "acmplp_ep.h"
#include "dac_operation.h"

/*******************************************************************************************************************//**
 * @addtogroup acmplp_ep
 * @{
 **********************************************************************************************************************/

/* Private function declaration */
static fsp_err_t check_acmplp_status(void);
static fsp_err_t acmplp_normal_mode_operation(uint16_t *p_value);

/* Led information */
extern bsp_leds_t g_bsp_leds;

/*****************************************************************************************************************
 *  @brief      Reads value from user and perform ACMPLP operation.
 *  @param[in]  None
 *  @retval     FSP_SUCCESS      On successfully ACMPLP operation.
 *  @retval     error            Any Other Error code apart from FSP_SUCCES on Unsuccessful operation.
 ****************************************************************************************************************/
fsp_err_t acmplp_operation(void)
{
    /* Variable to store number of bytes received */
    uint32_t num_bytes                = RESET_VALUE;
    /* Variable to store digital value */
    uint16_t dac_val                  = RESET_VALUE;
    unsigned char dac_buf[BUF_SIZE]   = {INITIAL_VALUE};
    fsp_err_t error                   = FSP_SUCCESS;

    /* Entered DAC value is used as input to comparator which gets
     * compared with the reference voltage and if Input voltage is
     * greater than Reference Voltage than Comparator output is set
     * to High else Low */
    APP_PRINT(" \r\nEnter the DAC Value(0 - 4095) to Compare:");

    while (RESET_VALUE == num_bytes)
    {
        if (APP_CHECK_DATA)
        {
            num_bytes = APP_READ(dac_buf);
            if (RESET_VALUE == num_bytes)
            {
                APP_PRINT(" \r\nInvalid Input\r\n");
            }
        }
    }

    /* Conversion from input string to integer value */
    dac_val =  (uint16_t) (atoi((char *)dac_buf));

    APP_PRINT("Input DAC value:  %d", dac_val);

    if (DAC_MAX_VAL < dac_val)
    {
        APP_ERR_PRINT(" \r\n ** INVALID INPUT, DAC VALUE IS OUT OF RANGE(0 - 4095) ** \r\n");
    }
    else
    {
        /* Start comparator by writing user provided dac value */
        error = acmplp_normal_mode_operation(&dac_val);
        if(FSP_SUCCESS != error)
        {
            APP_ERR_PRINT("\r\n ** AMCPLP FUNCTIONALITY FALIED ** \r\n");
        }

        num_bytes = RESET_VALUE;
        dac_val   = RESET_VALUE;
    }

    /* Reset buffer */
    memset(dac_buf, RESET_VALUE, sizeof(dac_buf));
    return error;
}

/*****************************************************************************************************************
 *  @brief      Using DAC output value enables the comparator output and provides the operating status of the
 *              comparator.
 *  @param[in]  *p_value
 *  @retval     FSP_SUCCESS     On successfully ACMPLP operation.
 *  @retval     err             Any Other Error code apart from FSP_SUCCES on Unsuccessful operation.
 ****************************************************************************************************************/
fsp_err_t acmplp_normal_mode_operation(uint16_t  *p_value)
{
    fsp_err_t err  = FSP_SUCCESS;

    /* Writing to DAC and starting conversion */
    err = dac_write(p_value);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** DAC OPERATION FAILED ** \r\n");
        return err;
    }

    /* Wait for the minimum stabilization wait time before enabling output */
    R_BSP_SoftwareDelay(BSP_FEATURE_ACMPLP_MIN_WAIT_TIME_US, BSP_DELAY_UNITS_MICROSECONDS);

    /* Enable the comparator output */
    err = R_ACMPLP_OutputEnable(&g_comparator_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_ACMPLP_OUTPUTENABLE API FAILED ** \r\n");
        return err;
    }

    /* Check status of comparator */
    err = check_acmplp_status();
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** ACMPLP STATUS CHECK FAILED ** \r\n");
        return err;
    }

    return err;
}

/*****************************************************************************************************************
 *  @brief      Perform ACMPLP status check and sets LED according to status.
 *  @param[in]  None
 *  @retval     FSP_SUCCESS     On successfully ACMPLP status check.
 *  @retval     err             Any Other Error code apart from FSP_SUCCES on Unsuccessful operation.
 ****************************************************************************************************************/
fsp_err_t check_acmplp_status(void)
{
    fsp_err_t err = FSP_SUCCESS;
    comparator_status_t status;

    /* Provides operating status of comparator */
    err = R_ACMPLP_StatusGet(&g_comparator_ctrl, &status);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_ACMPLP_STATUSGET API FAILED ** \r\n");
        return err;
    }

    if (COMPARATOR_STATE_OUTPUT_HIGH == status.state)
    {
        APP_PRINT (" \r\nComparator Output is HIGH and Setting On-board LED HIGH\r\n");

        /* Turn on the Led */
        err = R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], BSP_IO_LEVEL_HIGH);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\n ** R_IOPORT_PINWRITE API FAILED ** \r\n");
            return err;
        }
    }
    else if (COMPARATOR_STATE_OUTPUT_LOW == status.state)
    {
        APP_PRINT(" \r\nComparator Output is LOW and Setting On-board LED LOW\r\n");

        /* Turn off the Led */
        err = R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], BSP_IO_LEVEL_LOW);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\n ** R_IOPORT_PINWRITE API FAILED ** \r\n");
            return err;
        }
    }
    else
    {
        APP_PRINT(" \r\nComparator Output is Disabled \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     Initialize ACMPLP driver.
 * @param[in] None
 * @retval    FSP_SUCCESS         R_ACMPLP opened successfully.
 * @retval    error               Any Other Error code apart from FSP_SUCCES Unsuccessful Open.
 **********************************************************************************************************************/
fsp_err_t init_acmplp(void)
{
    fsp_err_t error = FSP_SUCCESS;

    /* Initialize the ACMPLP module */
    error = R_ACMPLP_Open(&g_comparator_ctrl, &g_comparator_cfg);
    if (FSP_SUCCESS != error)
    {
        APP_ERR_PRINT ("\r\n ** R_ACMPLP_OPEN API FAILED ** \r\n");

    }
    return error;
}

/*****************************************************************************************************************
 * This function is called to do closing of ACMPLP module using its HAL level API.
 * @brief     Close the ACMPLP module. Handle the Error internally with Proper Message.
 * @param[in] None
 * @retval    None
 ****************************************************************************************************************/
void deinit_acmplp(void)
{
    fsp_err_t error = FSP_SUCCESS;

    /* Closing ACMPLP instance */
    error = R_ACMPLP_Close(&g_comparator_ctrl);
    if (FSP_SUCCESS != error)
    {
        APP_ERR_PRINT (" \r\n ** R_ACMPLP_CLOSE API FAILED ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup acmplp_ep)
 **********************************************************************************************************************/
