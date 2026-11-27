/***********************************************************************************************************************
 * File Name    : dmac_transfers.c
 * Description  : Contains data structures and functions used in dmac_transfers.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "dmac_transfers.h"
#include "transfer_initialise.h"
#include "timer_initialise.h"

/* Source and destination arrays for DMAC transfer */
extern uint32_t g_source_data[SOURCE_DATA_SIZE];
extern uint32_t g_dest_data[DEST_DATA_SIZE];

/* Boolean flag to determine if transfer is complete */
volatile bool g_is_transfer_complete = false;

/* LED port pin control register for the specific board */
#if defined (BOARD_RA6M4_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4M3_EK) ||\
    defined (BOARD_RA4M2_EK) || defined (BOARD_RA8E1_FPB)
static volatile uint32_t *p_ioport_pnctrl_register = &R_PORT4->PCNTR1;
#elif defined (BOARD_RA6M5_EK)
static volatile uint32_t *p_ioport_pnctrl_register = &R_PORT0->PCNTR1;
#elif defined (BOARD_RA8T1_MCK) 
static volatile uint32_t *p_ioport_pnctrl_register = &R_PORT10->PCNTR1;
#elif defined (BOARD_RA4L1_EK) || defined (BOARD_RA8P1_EK) || defined (BOARD_RA4C1_EK) ||\
      defined (BOARD_RA8T2_MCK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8M2_EK)
static volatile uint32_t *p_ioport_pnctrl_register = &R_PORT6->PCNTR1;
#elif defined (BOARD_RA8E2_EK)
static volatile uint32_t *p_ioport_pnctrl_register = &R_PORT2->PCNTR1;
#else
static volatile uint32_t *p_ioport_pnctrl_register = &R_PORT1->PCNTR1;
#endif

/* GPT counter register */
static volatile uint32_t *p_gpt_counter_register = &R_GPT0->GTCNT;

/*******************************************************************************************************************//**
 * @brief     This function performs transfer LED blink operation.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
fsp_err_t transfer_led_blink_operation(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Set the Source and Destination Addresses for DMAC g_transfer_led_blink.
     * Note: If the source and destination addresses are not set prior to
     * opening the transfer interface, then the open will fail. */
    set_transfer_dst_src_address(&g_transfer_led_blink_cfg, (void *)g_source_data, (void *)p_ioport_pnctrl_register);

    /* Open and enable DMAC g_transfer_led_blink */
    err = dmac_transfer_init(&g_transfer_led_blink_ctrl, &g_transfer_led_blink_cfg, TRANSFER_LED_BLINK);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Open and start AGT timer */
    err = agt_timer_init();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for g_transfer_led_blink to complete.
     * This boolean flag is set in transfer_agt_timer_callback */
    while (false == g_is_transfer_complete)
    {
        /* Do nothing */
    }

    /* Reset the flag */
    g_is_transfer_complete = false;

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function performs transfer GPT timer operation.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
fsp_err_t transfer_gpt_timer_operation(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Set the Source and Destination Addresses for DMAC transfer_gpt_value.
     * Note: If the source and destination addresses are not set prior to
     * opening the transfer interface, then the open will fail. */
    set_transfer_dst_src_address(&g_transfer_gpt_value_cfg, (void *)p_gpt_counter_register, (void *)&g_dest_data);

    /* Open and enable DMAC g_transfer_gpt_value */
    err = dmac_transfer_init(&g_transfer_gpt_value_ctrl, &g_transfer_gpt_value_cfg, TRANSFER_GPT_TIMER_VALUE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Open GPT timer and start */
    err = gpt_timer_init();

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function performs transfer write IOPORT operation.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
fsp_err_t transfer_write_ioport_operation(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Set the source and destination addresses DMAC, g_transfer_write_ioport
     * is configured to transfer in repeat mode,
     * from g_source_data to R_IOPORTX->PCNTR1. The value X is specific to each board.
     * This is similar to DMAC g_transfer_led_blink
     * However, g_transfer_led_blink was triggered by a peripheral interrupt (AGT0)
     * Here, DMAC g_transfer_write_ioport is configured for Software Activation. */
    set_transfer_dst_src_address(&g_transfer_write_ioport_cfg, (void *)g_source_data, (void *)p_ioport_pnctrl_register);

    /* Open and enable DMAC g_transfer_write_ioport */
    err = dmac_transfer_init(&g_transfer_write_ioport_ctrl, &g_transfer_write_ioport_cfg, TRANSFER_WRITE_IOPORT);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     transfer_agt_timer_callback function.
 * @param[in] p_args    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void transfer_agt_timer_callback(dmac_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Set boolean flag to print data */
    g_is_transfer_complete = true;
}
