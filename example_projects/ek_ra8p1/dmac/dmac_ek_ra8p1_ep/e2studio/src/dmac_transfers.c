/***********************************************************************************************************************
 * File Name    : dmac_transfers.c
 * Description  : Contains data structures and functions used in dmac_transfers.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "dmac_transfers.h"
#include "transfer_initialise.h"
#include "timer_initialise.h"

/*******************************************************************************************************************//**
 * @addtogroup dmac_transfers
 * @{
 **********************************************************************************************************************/

/* Source and destination array for DMAC transfer */
extern uint32_t g_source_data[SOURCE_DATA_SIZE];
extern uint32_t g_dest_data[DEST_DATA_SIZE];

/* Boolean flag to determine if transfer is complete */
volatile bool g_is_transfer_complete  = false;

/* LED port pin control register for port */
#if defined (BOARD_RA6M4_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4M3_EK) || \
    defined (BOARD_RA4M2_EK) || defined (BOARD_RA8E1_FPB)
static volatile uint32_t *p_ioport_pnctrl_register = &R_PORT4->PCNTR1;
#elif defined (BOARD_RA6M5_EK)
static volatile uint32_t *p_ioport_pnctrl_register = &R_PORT0->PCNTR1;
#elif defined (BOARD_RA8T1_MCK)
static volatile uint32_t *p_ioport_pnctrl_register = &R_PORT10->PCNTR1;
#elif defined (BOARD_RA4L1_EK) || defined (BOARD_RA8P1_EK)
static volatile uint32_t *p_ioport_pnctrl_register = &R_PORT6->PCNTR1;
#elif defined (BOARD_RA8E2_EK)
static volatile uint32_t *p_ioport_pnctrl_register = &R_PORT2->PCNTR1;
#else
static volatile uint32_t *p_ioport_pnctrl_register = &R_PORT1->PCNTR1;
#endif

/* GPT counter register */
static volatile uint32_t *p_gpt_counter_register = &R_GPT0->GTCNT;

/*******************************************************************************************************************//**
 *  @brief       Perform transfer led blink operation
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void transfer_led_blink_operation(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;    /* Variable to help handle error codes from functions */

    /* Set the Source and Destination Addresses for DMAC g_transfer_led_blink.
     * NOTE: If the source and destination addresses are not set prior to
     * opening the transfer interface, then the open will fail. */
    set_transfer_dst_src_address(&g_transfer_led_blink_cfg, (void *)g_source_data , (void *)p_ioport_pnctrl_register);

    /* Open and enable DMAC g_transfer_led_blink */
    fsp_err = dmac_transfer_init(&g_transfer_led_blink_ctrl, &g_transfer_led_blink_cfg, TRANSFER_LED_BLINK);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        /* Necessary cleanup is already taken care in initialize function hence no cleanup is required here */
        APP_ERR_PRINT("** DMAC TRANSFER_LED_BLINK INIT failed **\r\n");
        APP_ERR_TRAP(fsp_err);
    }

    /* Open and start AGT timer */
    fsp_err = agt_timer_init();
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        APP_ERR_PRINT("** AGT Timer INIT failed **\r\n");
        dmac_transfer_deinit(&g_transfer_led_blink_ctrl, TRANSFER_LED_BLINK);
        APP_ERR_TRAP(fsp_err);
    }
    /* Display information about g_transfer_led_blink transfer on RTT display and view on-board LED */
    APP_PRINT("\r\nDMAC g_transfer_led_blink transfer in progress. Wait for 6 second\r\n");

    /* Wait for g_transfer_led_blink to complete.
     * This boolean flag is set in transfer_agt_timer_callback */
    while(false == g_is_transfer_complete)
    {
        ;
    }
    /* Reset the flag */
    g_is_transfer_complete = false;
    APP_PRINT("\r\nDMAC g_transfer_led_blink transfer completed\r\n");
}

/*******************************************************************************************************************//**
 *  @brief       Perform transfer GPT timer operation
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void transfer_gpt_timer_operation(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;    /* Variable to help handle error codes from functions */

    /* Set the Source and Destination Addresses for DMAC transfer_gpt_value.
     * NOTE: If the source and destination addresses are not set prior to
     * opening the transfer interface, then the open will fail. */
    set_transfer_dst_src_address(&g_transfer_gpt_value_cfg, (void *)p_gpt_counter_register , (void *)&g_dest_data);

    /* Open and enable DMAC g_transfer_gpt_value */
    fsp_err = dmac_transfer_init(&g_transfer_gpt_value_ctrl, &g_transfer_gpt_value_cfg, TRANSFER_GPT_TIMER_VALUE);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        APP_ERR_PRINT("** DMAC TRANSFER_GPT_TIMER_VALUE INIT failed **\r\n");
        agt_timer_deinit();
        dmac_transfer_deinit(&g_transfer_led_blink_ctrl, TRANSFER_LED_BLINK);
        APP_ERR_TRAP(fsp_err);
    }

    /* Open GPT timer and start */
    fsp_err = gpt_timer_init();
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        APP_ERR_PRINT("** GPT timer INIT failed **\r\n");
        dmac_transfer_deinit(&g_transfer_gpt_value_ctrl, TRANSFER_GPT_TIMER_VALUE);
        agt_timer_deinit();
        dmac_transfer_deinit(&g_transfer_led_blink_ctrl, TRANSFER_LED_BLINK);
        APP_ERR_TRAP(fsp_err);
    }
}

/*******************************************************************************************************************//**
 *  @brief       Perform transfer write IOPORT operation
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void transfer_write_ioport_operation(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;    /* Variable to help handle error codes from functions */

    /* Set the source and destination addresses DMAC, g_transfer_write_ioport
     * is configured to transfer in repeat mode,
     * from g_source_data to R_IOPORT1->PCNTR1
     * This is similar to DMAC g_transfer_led_blink
     * However, g_transfer_led_blink  was triggered by a peripheral interrupt (AGT0)
     * Here, DMAC g_transfer_write_ioport is configured for Software Activation. */
    set_transfer_dst_src_address(&g_transfer_write_ioport_cfg, (void *)g_source_data,
                                 (void *)p_ioport_pnctrl_register);

    /* Open and enable DMAC g_transfer_write_ioport */
    fsp_err = dmac_transfer_init(&g_transfer_write_ioport_ctrl, &g_transfer_write_ioport_cfg, TRANSFER_WRITE_IOPORT);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        APP_ERR_PRINT("** DMAC TRANSFER_WRITE_IOPORT INIT failed **\r\n");
        gpt_timer_deinit();
        dmac_transfer_deinit(&g_transfer_gpt_value_ctrl, TRANSFER_GPT_TIMER_VALUE);
        agt_timer_deinit();
        dmac_transfer_deinit(&g_transfer_led_blink_ctrl, TRANSFER_LED_BLINK);
        APP_ERR_TRAP(fsp_err);
    }
}

/*******************************************************************************************************************//**
 *  @brief      transfer_agt_timer_callback function
 *  @param[in]  callback arguments
 **********************************************************************************************************************/
void transfer_agt_timer_callback(dmac_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Set boolean flag to print data */
    g_is_transfer_complete  = true;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup dmac_transfers)
 **********************************************************************************************************************/
