/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "slcdc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup slcdc_ep
 * @{
 **********************************************************************************************************************/

/*
 * function declarations
 */
void R_BSP_WarmStart(bsp_warm_start_event_t event);

static fsp_err_t slcd_init(void);
static fsp_err_t set_segments_digits( uint32_t value );
static void set_err_segments(void);
static void deinit_slcdc(void);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err               = FSP_SUCCESS;
    fsp_pack_version_t version  = {RESET_VALUE};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the RTT */
    APP_PRINT (BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
               version.version_id_b.patch);
    APP_PRINT (EP_INFO);

    /* Initialize slcdc driver. */
    err = slcd_init();
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** SLCDC INITIALIZATION FAILED ** \r\n");
        APP_ERR_TRAP (err);
    }

    APP_PRINT ("\r\nLCD initialized and started");
    for (uint32_t value = INITIAL_COUNT; value <= FINAL_COUNT; value += COUNT_INCREMENT_STEP)
    {
        /* Set the segment digits. */
         err = set_segments_digits(value);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\n ** SETTING SEGMENT DIGITS FAILED ** \r\n");
            /* de-initialize the opened slcdc module before trap*/
            deinit_slcdc();
            APP_ERR_TRAP (err);
        }

        /* Add a delay so that user can observe the change in SLCD screen */
        R_BSP_SoftwareDelay (COUNT_DELAY , BSP_DELAY_UNITS_MILLISECONDS);
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

/*******************************************************************************************************************//**
 *  @brief       Performs open, start and set display area operations.
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS     Upon successful SLCDC Initialization.
 *  @retval      Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation .
 **********************************************************************************************************************/
static fsp_err_t slcd_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open SLCDC driver.*/
    err = R_SLCDC_Open(&g_slcdc_ctrl, &g_slcdc_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_SLCDC_Open API FAILED ** \r\n");
        return err;
    }

    /* When using internal voltage boosting method a delay of 5ms is required between calling R_SLCDC_Open and
     * R_SLCDC_Start to allow the boost circuit to charge. See RA4M1 User's Manual (R01UH0887EJ0100)
     * section 45.2.2 Notes. */
    R_BSP_SoftwareDelay (LCD_STABILIZATION_DELAY, BSP_DELAY_UNITS_MILLISECONDS);

    /* Start SLCDC driver.*/
    err = R_SLCDC_Start(&g_slcdc_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_SLCDC_Start API FAILED ** \r\n");
        /* de-initialize the opened slcdc module before trap*/
        deinit_slcdc();
        return err;
    }

    /* Set Display Area of SLCDC driver.*/
    err = R_SLCDC_SetDisplayArea(&g_slcdc_ctrl, SLCDC_DISP_A);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_SLCDC_SetDisplayArea API FAILED ** \r\n");
        /* de-initialize the opened slcdc module before trap*/
        deinit_slcdc();
    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief       Sets SLCDC segment digits.
 *  @param[IN]   Value to be set on display
 *  @retval      FSP_SUCCESS     Upon successful segment digit set operation.
 *  @retval      Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation .
 **********************************************************************************************************************/
static fsp_err_t set_segments_digits( uint32_t value)
{
    fsp_err_t err = FSP_SUCCESS;

    uint8_t temp_thousands  = (uint8_t)((value % TEN_THOUSAND) / THOUSANDS);
    uint8_t temp_hundreds   = (uint8_t)((value % THOUSANDS) / HUNDREDS);
    uint8_t temp_tens       = (uint8_t)((value % HUNDREDS) / TENS);
    uint8_t temp_units      = (uint8_t)((value % TENS) / UNITS);

    if (HUNDREDS_DIGIT_MAX_VAL < value)
    {
        /* Modify the digit in thousand place */
        err = R_SLCDC_Modify(&g_slcdc_ctrl, digit_start_addresses[MN_THOUSANDS][ZERO_INDEX],
                       (digit_data[temp_thousands][ZERO_INDEX]), (uint8_t) (~(digit_data[temp_thousands][ZERO_INDEX])));
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\n ** R_SLCDC_Modify API FAILED AT THE THOUSANDS PLACE ** \r\n");
            set_err_segments();
            return err;
        }

        err = R_SLCDC_Modify(&g_slcdc_ctrl, digit_start_addresses[MN_THOUSANDS][FIRST_INDEX],
                     (digit_data[temp_thousands][FIRST_INDEX]), (uint8_t) (~(digit_data[temp_thousands][FIRST_INDEX])));
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\n ** R_SLCDC_Modify API FAILED AT THE THOUSANDS PLACE ** \r\n");
            set_err_segments();
            return err;
        }
    }

    if (TENS_DIGIT_MAX_VAL < value)
    {
        /* Modify the digit in hundred place */
        err = R_SLCDC_Modify(&g_slcdc_ctrl, digit_start_addresses[MN_HUNDREDS][ZERO_INDEX],
                         (digit_data[temp_hundreds][ZERO_INDEX]), (uint8_t) (~(digit_data[temp_hundreds][ZERO_INDEX])));
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\n ** R_SLCDC_Modify API FAILED AT THE HUNDREDS PLACE ** \r\n");
            set_err_segments();
            return err;
        }

        err = R_SLCDC_Modify(&g_slcdc_ctrl, digit_start_addresses[MN_HUNDREDS][FIRST_INDEX],
                       (digit_data[temp_hundreds][FIRST_INDEX]), (uint8_t) (~(digit_data[temp_hundreds][FIRST_INDEX])));
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\n ** R_SLCDC_Modify API FAILED AT THE HUNDREDS PLACE ** \r\n");
            set_err_segments();
            return err;
        }
    }

    if (UNIT_DIGIT_MAX_VAL < value)
    {
        /* Modify the digit in tens place */
        err = R_SLCDC_Modify(&g_slcdc_ctrl, digit_start_addresses[MN_TENS][ZERO_INDEX],
                             (digit_data[temp_tens][ZERO_INDEX]), (uint8_t) (~(digit_data[temp_tens][ZERO_INDEX])));
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\n ** R_SLCDC_Modify API FAILED AT THE TENS PLACE ** \r\n");
            set_err_segments();
            return err;
        }

        err = R_SLCDC_Modify(&g_slcdc_ctrl, digit_start_addresses[MN_TENS][FIRST_INDEX],
                             (digit_data[temp_tens][FIRST_INDEX]), (uint8_t) (~(digit_data[temp_tens][FIRST_INDEX])));
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\n ** R_SLCDC_Modify API FAILED AT THE TENS PLACE ** \r\n");
            set_err_segments();
            return err;
        }
    }

    /* Modify the digit in units place */
    err = R_SLCDC_Modify(&g_slcdc_ctrl, digit_start_addresses[MN_UNITS][ZERO_INDEX],
                         (digit_data[temp_units][ZERO_INDEX]), (uint8_t) (~(digit_data[temp_units][ZERO_INDEX])));
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_SLCDC_Modify API FAILED AT THE UNITS PLACE **");
        set_err_segments();
        return err;
    }

    err = R_SLCDC_Modify(&g_slcdc_ctrl, digit_start_addresses[MN_UNITS][FIRST_INDEX],
                         (digit_data[temp_units][FIRST_INDEX]), (uint8_t) (~(digit_data[temp_units][FIRST_INDEX])));
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_SLCDC_Modify API FAILED AT THE UNITS PLACE ** \r\n");
        set_err_segments();
    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief       Sets "Err" on SLCDC.
 *  @param[IN]   None.
 *  @retval      None.
 **********************************************************************************************************************/
static void set_err_segments(void)
{
    fsp_err_t err = FSP_SUCCESS;

    for (uint8_t seg_base_address = 0; seg_base_address <= NUMBER_OF_SEG_FOR_EACH_DIGIT; seg_base_address++)
    {
        uint8_t data_index = RESET_VALUE;
        data_index = (uint8_t) (seg_base_address == NUMBER_OF_SEG_FOR_EACH_DIGIT ? ZERO_INDEX : FIRST_INDEX);

        /* Printing 'Err' on the LCD screen. */
        err = R_SLCDC_Write(&g_slcdc_ctrl, digit_start_addresses[seg_base_address][ZERO_INDEX],
                            (uint8_t *) &err_data[data_index][ZERO_INDEX], NUMBER_OF_SEGMENT_TO_WRITE);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\n ** R_SLCDC_Write API FAILED ** \r\n");
            return;
        }

        err = R_SLCDC_Write(&g_slcdc_ctrl, digit_start_addresses[seg_base_address][FIRST_INDEX],
                            (uint8_t *) &err_data[data_index][FIRST_INDEX], NUMBER_OF_SEGMENT_TO_WRITE);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\n ** R_SLCDC_Write API FAILED ** \r\n");
            return;
        }
    }
}

/*****************************************************************************************************************
 * @brief      Close the SLCDC HAL driver.
 * @param[IN]  None
 * @retval     None
 ****************************************************************************************************************/
static void deinit_slcdc(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close slcdc module */
    err = R_SLCDC_Close(&g_slcdc_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n** R_SLCDC_Close API FAILED ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup slcdc_ep)
 **********************************************************************************************************************/
