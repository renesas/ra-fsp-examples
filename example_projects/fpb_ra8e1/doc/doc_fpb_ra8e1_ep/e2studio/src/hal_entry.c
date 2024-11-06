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
/*******************************************************************************************************************//**
 * @addtogroup DOC_EP
 * @{
 **********************************************************************************************************************/
/** Macros **/
#define DOC_DATA_SUB                    (0xA5A4)      //To write the first data to doc register for subtraction event
#define DOC_DATA_UNDERFLOW              (0x0002)      //To write the second data to doc register for subtraction event
#define DOC_DATA_ADD                    (0X5A5A)      //To write the first data to doc register for addition event
#define DOC_DATA_OVERFLOW               (0x0001)      //To write the second data to doc register for addition event to overflow
#define DOC_DATA_COMPARISON_MATCH       (0xA5A5)      //To write the data to doc register for comparison match event
#define DOC_DATA_COMPARISON_MISMATCH    (0xB5B5)      //To write the data to doc register for comparison mismatch event


/** Local Functions **/
void R_BSP_WarmStart(bsp_warm_start_event_t event);
static void doc_deinit(void);

/** Global Variables **/
static bool b_doc_event_flag = false;           //Event counter updates for every callback triggers
extern bsp_leds_t g_bsp_leds;                  // LED Structure used to blink on board LED

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;
    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);

    APP_PRINT("\r\nThis project demonstrates the basic functionality of DOC driver."
            "\r\nDOC module performs one of the four operations(Comparison Mismatch, Comparison Match,"
            "\r\nAddition Overflow & Subtraction Underflow) on reference data based on the user selection in RA configurator."
            "\r\nStatus and Result of the operation is printed on RTT Viewer. "
            "\r\nLED turns ON to indicate the success of operation and stays OFF if the operation fails.\r\n");

    /* Initialize the DOC module for Comparison/Addition/Subtraction Operations with Initial value
     * written in DOC Data Setting Register*/
    err = R_DOC_Open(&g_doc_ctrl, &g_doc_cfg);
    /* Handle Error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** DOC Open API Failed **");
        APP_ERR_TRAP(err);
    }

    /* Write the predefined data to doc register, depends on the selected event operations will take place*/

    switch(g_doc_cfg.event)
    {
        case DOC_EVENT_COMPARISON_MISMATCH:
            APP_PRINT("\r\nSelected event is Comparison MisMatch");
            /* write mismatch to the DOC Data Input Register */
            err = R_DOC_Write (&g_doc_ctrl, DOC_DATA_COMPARISON_MISMATCH);
            /* Handle Error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\n ** DOC Write API Failed **");
                doc_deinit ();
                APP_ERR_TRAP(err);
            }
            break;

        case DOC_EVENT_COMPARISON_MATCH:
            APP_PRINT("\r\nSelected event is Comparison Match");
            /* write match data to the DOC Data Input Register */
            err = R_DOC_Write (&g_doc_ctrl, DOC_DATA_COMPARISON_MATCH);
            /* Handle Error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\n ** DOC Write API Failed **");
                doc_deinit ();
                APP_ERR_TRAP(err);
            }
            break;

        case DOC_EVENT_ADDITION:
            APP_PRINT("\r\nSelected event is Addition Overflow");
            /* write first data to the DOC Data Input Register */
            err = R_DOC_Write (&g_doc_ctrl, DOC_DATA_ADD);
            /* Handle Error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\n ** DOC Write API Failed **");
                doc_deinit ();
                APP_ERR_TRAP(err);
            }
            /* write second data to the DOC Data Input Register to occur overflow event */
            err = R_DOC_Write (&g_doc_ctrl, DOC_DATA_OVERFLOW);
            /* Handle Error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\n ** DOC Write API Failed **");
                doc_deinit ();
                APP_ERR_TRAP(err);
            }
            break;

        case DOC_EVENT_SUBTRACTION:
            APP_PRINT("\r\nSelected event is Subtraction Underflow");
            /* write first data to the DOC Data Input Register */
            err = R_DOC_Write(&g_doc_ctrl, DOC_DATA_SUB);
            /* Handle Error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\n ** DOC Write API Failed **");
                doc_deinit();
                APP_ERR_TRAP(err);
            }
            /* write second data to the DOC Data Input Register to occur underflow event */
            err = R_DOC_Write(&g_doc_ctrl, DOC_DATA_UNDERFLOW);
            /* Handle Error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\n ** DOC Write API Failed **");
                doc_deinit();
                APP_ERR_TRAP(err);
            }
            break;
        default: break;
    }

     /* Prints the RTT message when interrupt occurs upon successful event operations.*/
    if (true == b_doc_event_flag)
    {
        b_doc_event_flag = false;   //RESET flag
        APP_PRINT("\r\nDOC operation is successful for the selected event");
		
        /* Turn ON LED to indicate callback triggered, along with output on RTT*/
		#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA8T1_MCK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA6T3_MCK)
		    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], BSP_IO_LEVEL_LOW);
		#else
			R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], BSP_IO_LEVEL_HIGH);
		#endif
    }
    else
    {
        APP_PRINT("\r\nDOC operation failed for the selected event");
    }
    APP_PRINT("\r\nRestart the application to rerun the code");
    while (true)
    {
        ;
    }
}

/*******************************************************************************************************************//**
 * @brief   Based on the selected event in the configurator, The doc call back will occur for the selected
 *          event when operations are performed with the reference data.
  * @param[in]   p_args
 * @return      None
 **********************************************************************************************************************/
void doc_callback(doc_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    /* Set the event flag */
    b_doc_event_flag = true;
}

/*******************************************************************************************************************//**
 * @brief       This function is to de-initializes the DOC module
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
static void doc_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_DOC_Close(&g_doc_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n **DOC Close API failed**");
        APP_ERR_TRAP(err);
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
    if (BSP_WARM_START_POST_C == event) {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}
/*******************************************************************************************************************//**
 * @} (end defgroup DOC_EP)
 **********************************************************************************************************************/
