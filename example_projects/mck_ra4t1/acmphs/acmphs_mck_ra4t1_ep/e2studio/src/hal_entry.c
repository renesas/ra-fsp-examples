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
 * @addtogroup r_acmphs_ep
 * @{
 **********************************************************************************************************************/

/* Macro definition */
#define DAC_MAX_VAL          (4095U)		/* Max count value for 12 bit DAC */
#define DAC_REF_VAL			 (2048U)		/* Mid count value for 12 bit DAC */
#define LOW_VAL				 (2000U)		/* Lower value of fluctuating range*/
#define HIGH_VAL			 (2100U)		/* Higher value of fluctuating range*/
#define CHANNEL_ZERO		 (0U)			/* Channel 0 of ACMPHS */
#define INITIAL_VALUE        ('\0')

/* Board specific Led */
#if defined (BOARD_RA6M2_EK) || defined (BOARD_RA4T1_MCK)
#define LED_PIN			(BSP_IO_PORT_01_PIN_06)
#elif defined (BOARD_RA6M1_EK)
#define LED_PIN			(BSP_IO_PORT_01_PIN_12)
#elif defined (BOARD_RA6T1_RSSK)
#define LED_PIN         (BSP_IO_PORT_04_PIN_08)
#elif defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define LED_PIN 		(BSP_IO_PORT_01_PIN_00)
#endif

/* Global variable */
static volatile bool b_comparator_state_flag = false;

/* Private functions */
static void deinit_acmphs(void);
static void deinit_dac(dac_instance_ctrl_t * p_ctrl);
static void clean_up(void);

void R_BSP_WarmStart(bsp_warm_start_event_t event);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void) {

    fsp_err_t err 					 		= FSP_SUCCESS;
    unsigned char rByte[BUFFER_SIZE_DOWN]	= {RESET_VALUE};
    uint8_t num_bytes 				 		= RESET_VALUE;
    uint16_t dac_val			 	 		= RESET_VALUE;
    fsp_pack_version_t version 		 		= {RESET_VALUE};
    comparator_info_t stabilize_time 		= {RESET_VALUE};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Project information */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT("\r\n In this project DAC0 is used as reference voltage source and DAC1 is used as input"
            "\r\n voltage source for ACMPHS module. DAC0 value is set to 2048(i.e. 1.6V)."
            "\r\n User can enter DAC1 value within permitted range. When DAC1 input value is greater"
            "\r\n than set DAC0 reference voltage, Comparator output status is HIGH and on-board LED "
            "\r\n is turned ON. When DAC1 input is less than reference voltage, Output status is LOW "
            "\r\n and the LED is turned OFF.\r\n");

    /* Open DAC0 module */
    err = R_DAC_Open(&g_dac0_ctrl, &g_dac0_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_Open API FAILED FOR DAC0 ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Open DAC1 module */
    err = R_DAC_Open(&g_dac1_ctrl, &g_dac1_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_Open API FAILED FOR DAC1 ** \r\n");
        deinit_dac(&g_dac0_ctrl);
        APP_ERR_TRAP(err);
    }

    /* Open ACMPHS module */
    err  = R_ACMPHS_Open(&g_comparator_ctrl, &g_comparator_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_ACMPHS_Open API FAILED ** \r\n");
        deinit_dac(&g_dac0_ctrl);
        deinit_dac(&g_dac1_ctrl);
        APP_ERR_TRAP(err);
    }

    /* Write 2048(≈1.65V) on DAC0 module for reference voltage to comparator */
    err = R_DAC_Write(&g_dac0_ctrl, DAC_REF_VAL);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_Write API FAILED FOR DAC0 ** \r\n");
        clean_up();
        APP_ERR_TRAP(err);
    }

    /* Write 0 on DAC1 module for providing analog input voltage to comparator
     * and avoid garbage to ACMPHS input */
    err = R_DAC_Write(&g_dac1_ctrl, RESET_VALUE);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_Write API FAILED FOR DAC1 ** \r\n");
        clean_up();
        APP_ERR_TRAP(err);
    }

    /* Start conversion on DAC0 module */
    err = R_DAC_Start(&g_dac0_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_Start API FAILED FOR DAC0 ** \r\n");
        clean_up();
        APP_ERR_TRAP(err);
    }

    /* Start conversion on DAC1 module  */
    err = R_DAC_Start(&g_dac1_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_Start API FAILED  FOR DAC1 ** \r\n");
        clean_up();
        APP_ERR_TRAP(err);
    }

    /* Get the minimum stabilization wait time */
    err = R_ACMPHS_InfoGet(&g_comparator_ctrl, &stabilize_time);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_ACMPHS_InfoGet API FAILED ** \r\n");
        clean_up();
        APP_ERR_TRAP(err);
    }

    /* Enable the comparator output */
    err = R_ACMPHS_OutputEnable(&g_comparator_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_ACMPHS_OutputEnable API FAILED ** \r\n");
        clean_up();
        APP_ERR_TRAP(err);
    }

    while(true)
    {
        num_bytes = RESET_VALUE;
        dac_val   = RESET_VALUE;
        /* Reset buffer*/
        memset(rByte,INITIAL_VALUE,sizeof(rByte));

        APP_PRINT(" \r\n Enter the DAC1 Value between (0 - 2000) or (2100 - 4095) to compare:");

        while (RESET_VALUE == num_bytes)
        {
            if (APP_CHECK_DATA)
            {
                /* Take User input */
                num_bytes = (uint8_t)APP_READ(rByte);
                if (RESET_VALUE == num_bytes)
                {
                    APP_PRINT(" \r\nInvalid Input\r\n");
                }
            }
        }

        /* Conversion from input string to integer value */
        dac_val =  (uint16_t) (atoi((char *)rByte));

        APP_PRINT("\r\n Input DAC value %d", dac_val);
        /* Check for out of range value  */
        if (DAC_MAX_VAL < dac_val)
        {
            APP_ERR_PRINT(" \r\n ** INVALID INPUT, DAC VALUE IS OUT OF RANGE(0 - 4095) ** \r\n");
        }

        /* Check for DAC input range to avoid fluctuating voltage caused due to
         * electrical noise which may alter the comparator output */
        else if (LOW_VAL < dac_val && dac_val < HIGH_VAL)
        {
            APP_ERR_PRINT(" \r\n ** Fluctuating voltage range ** \r\n");
        }
        else
        {
            /* Write to DAC1 channel*/
            err = R_DAC_Write(&g_dac1_ctrl, dac_val);
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT ("\r\n ** R_DAC_Write API FAILED FOR DAC1 ** \r\n");
                clean_up();
                APP_ERR_TRAP(err);
            }

            /* Wait for the minimum stabilization wait time   */
            R_BSP_SoftwareDelay(stabilize_time.min_stabilization_wait_us, BSP_DELAY_UNITS_MICROSECONDS);

            /* Check if flag is set */
            if (true == b_comparator_state_flag)
            {
                APP_PRINT("\r\n Comparator output high \r\n");
#if defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4T1_MCK)
                err = R_IOPORT_PinWrite(&g_ioport_ctrl, LED_PIN, BSP_IO_LEVEL_LOW);
#else
                err = R_IOPORT_PinWrite(&g_ioport_ctrl, LED_PIN, BSP_IO_LEVEL_HIGH);
#endif

                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT ("\r\n ** R_IOPORT_PinWrite API FAILED ** \r\n");
                    clean_up();
                    APP_ERR_TRAP(err);
                }
            }
            else
            {
                APP_PRINT("\r\n Comparator output low \r\n");
#if defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4T1_MCK)
                err = R_IOPORT_PinWrite(&g_ioport_ctrl, LED_PIN, BSP_IO_LEVEL_HIGH);
#else
                err = R_IOPORT_PinWrite(&g_ioport_ctrl, LED_PIN, BSP_IO_LEVEL_LOW);
#endif
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT ("\r\n ** R_IOPORT_PinWrite API FAILED ** \r\n");
                    clean_up();
                    APP_ERR_TRAP(err);
                }
            }
        }
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

/*****************************************************************************************************************
 *  @brief      Close ACMPHS module.
 *  @param[in]  None
 *  @retval     None
 ****************************************************************************************************************/
static void deinit_acmphs(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close ACMPHS module */
    err = R_ACMPHS_Close(&g_comparator_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** R_ACMPHS_Close API FAILED ** \r\n");
    }
}

/*****************************************************************************************************************
 *  @brief      Close corresponding DAC module.
 *  @param[in]  p_ctrl		Pointer to dac_instance_ctrl_t
 *  @retval     None
 ****************************************************************************************************************/
static void deinit_dac(dac_instance_ctrl_t * p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close DAC */
    err = R_DAC_Close(p_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** R_DAC_Close API FAILED ** \r\n");
    }
}

/*****************************************************************************************************************
 *  @brief      Close all modules.
 *  @param[in] 	None
 *  @retval     None
 ****************************************************************************************************************/
static void clean_up(void)
{
    deinit_dac(&g_dac0_ctrl);
    deinit_dac(&g_dac1_ctrl);
    deinit_acmphs();
}

/*****************************************************************************************************************
 *  @brief      User defined callback
 *  @param[in] 	p_args
 *  @retval     None
 ****************************************************************************************************************/
void acmphs_user_callback(comparator_callback_args_t *p_args)
{
    /* Check for the channel 0 of comparator */
    if(CHANNEL_ZERO == p_args->channel)
    {
        /* Toggle the flag */
        b_comparator_state_flag = !b_comparator_state_flag;
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup acmphs_ep)
 **********************************************************************************************************************/
