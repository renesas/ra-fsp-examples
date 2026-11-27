/***********************************************************************************************************************
 * File Name    : poeg_ep.c
 * Description  : Contains data structures and functions used in poeg_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "poeg_ep.h"

/* Global variables */
volatile bool g_interrupt_pin_trigger_flag = false;
volatile bool g_interrupt_output_level_flag = false;
volatile bool g_interrupt_acmphs_flag = false;
volatile uint8_t g_mode_flag = RESET_VALUE;

#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
static volatile bool b_comparator_state_flag = false;
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */

/*******************************************************************************************************************//**
 * @brief     This function initializes the GPT timer module.
 * @param[in] p_ctrl                    Pointer to the timer control block.
 * @param[in] p_cfg                     Pointer to the timer configuration structure.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t init_gpt_timer(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Open(p_ctrl, p_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the POEG module.
 * @param[in] p_ctrl                    Pointer to the POEG control block.
 * @param[in] p_cfg                     Pointer to the POEG configuration structure.
 * @param[in] IRQn                      Device specific interrupt number.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t init_poeg(poeg_ctrl_t * const p_ctrl, poeg_cfg_t const * const p_cfg, IRQn_Type IRQn)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the POEG module */
    err = R_POEG_Open(p_ctrl, p_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    NVIC_DisableIRQ(IRQn);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts the GPT timer module.
 * @param[in] p_ctrl                    Pointer to the timer control block.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t start_gpt(timer_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Start(p_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened GPT timer module.
 * @param[in] p_timer_ctl               Pointer to the timer control block.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_gpt_timer(timer_ctrl_t * const p_ctrl)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;

    /* Close the opened GPT timer */
    if (p_instance_ctrl->open)
    {
        R_GPT_Close(p_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened POEG module.
 * @param[in] p_ctrl                    Pointer to the POEG control block.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_poeg(poeg_ctrl_t * const p_ctrl)
{
    poeg_instance_ctrl_t * p_instance_ctrl = (poeg_instance_ctrl_t *) p_ctrl;

    /* De-initialize the opened POEG module */
    if (p_instance_ctrl->open)
    {
        R_POEG_Close(p_ctrl);
    }
}

#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
/*******************************************************************************************************************//**
 * @brief     This function starts conversions on the DAC0, DAC1 modules.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Converted successfully.
 * @retval    Any other error codes     Failed to convert.
 **********************************************************************************************************************/
fsp_err_t enable_acmphs_mode_modules(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open DAC0 module */
    err = DAC_OPEN(&g_dac0_ctrl, &g_dac0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if !(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
    /* Open DAC1 module */
    err = DAC_OPEN(&g_dac1_ctrl, &g_dac1_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */

#if defined (BOARD_RA6T2_MCK)
    /* Initializes the ADC PGA Settings (PGAGEN = 1) */
    err = R_ADC_B_Open(&g_adc0_ctrl, &g_adc0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BOARD_RA6T2_MCK */

    /* Open ACMPHS module */
    err = ACMPHS_OPEN(&g_comparator_ctrl, &g_comparator_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
    /* Write 0 on DAC0 module for providing analog input voltage to comparator and avoid garbage to ACMPHS input */
    err = DAC_WRITE(&g_dac0_ctrl, RESET_VALUE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#else
    /* Write 2048 (≈ 1.65V) on DAC0 module for reference voltage to comparator */
    err = DAC_WRITE(&g_dac0_ctrl, DAC_REF_VAL);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Write 0 on DAC1 module for providing analog input voltage to comparator
     * and avoid garbage to ACMPHS input */
    err = DAC_WRITE(&g_dac1_ctrl, RESET_VALUE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */

    /* Start conversion on DAC0 module */
    err = DAC_START(&g_dac0_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start conversion on DAC1 module */
#if !(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
    err = DAC_START(&g_dac1_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */

    /* Get the minimum stabilization wait time */
    err = ACMPHS_INFOGET(&g_comparator_ctrl, &g_stabilize_time);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Enable the comparator output */
    err = ACMPHS_OUTPUT_ENABLE(&g_comparator_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened ACMPHS module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_acmphs(void)
{
    /* Close the opened ACMPHS module */
    if (g_comparator_ctrl.open)
    {
        ACMPHS_CLOSE(&g_comparator_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened DAC module.
 * @param[in] p_ctrl                    Pointer to the DAC instance control block.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_dac(dac_type_ctrl_t * p_ctrl)
{
    /* Close the opened DAC module */
    if (p_ctrl->channel_opened)
    {
        DAC_CLOSE(p_ctrl);
    }
}

#if defined (BOARD_RA6T2_MCK)
/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened ADC module
 * @param[in] p_ctrl                    Pointer to the ADC instance control block.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_adc(adc_ctrl_t * p_ctrl)
{
    /* Close the opened ADC module */
    if (p_ctrl->opened)
    {
        R_ADC_B_Close(p_ctrl);
    }
}
#endif /* BOARD_RA6T2_MCK */
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT,  BSP_PERIPHERAL_ACMPHS_B_PRESENT */

/*******************************************************************************************************************//**
 * @brief     This function is used for POEG channel0 callback to disable the IRQ and set the mode flag.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void channel0_user_callback(poeg_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Turn ON LED1 */
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[STATE_LED], LED_ON);

    if (POEG_OUTPUT_DISABLE_ON_GTIOC_OUTPUT_LEVEL == g_mode_flag)
    {
        g_interrupt_output_level_flag = true;
    }

    /* If the POEG0 cannot be reset, disable the POEG0 interrupt to prevent it from firing continuously */
    NVIC_DisableIRQ(VECTOR_NUMBER_POEG0_EVENT);
}

/*******************************************************************************************************************//**
 * @brief     This function is used for POEG channel1 callback to disable the IRQ and set the mode flag.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void channel1_user_callback(poeg_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Turn ON LED1 */
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[STATE_LED], LED_ON);

#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
    if (POEG_OUTPUT_DISABLE_ON_ACMPHS_EVENTS == g_mode_flag)
    {
        g_interrupt_acmphs_flag = true;
    }
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */

    if (POEG_TRIGGER_ON_GTETRG_INPUT_PIN_LEVEL == g_mode_flag)
    {
        g_interrupt_pin_trigger_flag = true;
    }

    /* If the POEG1 cannot be reset, disable the POEG1 interrupt to prevent it from firing continuously */
    NVIC_DisableIRQ(VECTOR_NUMBER_POEG1_EVENT);
}

#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
/*******************************************************************************************************************//**
 * @brief     This function is used for ACMPHS callback.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void acmphs_user_callback(comparator_callback_args_t *p_args)
{
    /* Check for the channel 0 of comparator */
    if (0 == p_args->channel)
    {
        /* Toggle the flag */
        b_comparator_state_flag = !b_comparator_state_flag;
    }
}
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */
