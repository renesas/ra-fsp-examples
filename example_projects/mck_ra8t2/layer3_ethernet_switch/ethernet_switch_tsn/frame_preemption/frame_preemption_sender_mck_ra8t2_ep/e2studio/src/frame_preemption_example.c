/***********************************************************************************************************************
 * File Name    : frame_preemption_example.c
 * Description  : Contains data structures and functions for the Frame Preemption sender example application.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include "frame_preemption.h"
#include "frame_preemption_example.h"
#include "stdio.h"
#include "string.h"
#include "console/uart_console.h"

#define FRAME_PREEMPTION_EXAMPLE_ETHERNET_USED_PORT   (1)
#define FRAME_PREEMPTION_EXAMPLE_HIGH_PRIORITY        (1)
#define FRAME_PREEMPTION_EXAMPLE_LOW_PRIORITY         (0)
#define FRAME_PREEMPTION_EXAMPLE_HIGH_SEND_FRAME_SIZE (1514)
#define FRAME_PREEMPTION_EXAMPLE_LOW_SEND_FRAME_SIZE  (1514)
#define FRAME_PREEMPTION_EXAMPLE_IPV_PAYLOAD_OFFSET   (14U)
#define FRAME_PREEMPTION_EXAMPLE_MAC_ADDRESS_LEN      (6U)

/*******************************************************************************************************************//**
 * @brief      Prints which DST MAC is currently sent 1st (preemptable/low priority) and which is sent 2nd
 *             (express/high priority), based on whether the roles of Frame 0A/0B have been swapped by the user.
 * @param[in]  priority_swapped    true if Frame 0A/0B roles have been swapped by pressing ENTER.
 * @retval     None.
 **********************************************************************************************************************/
static void frame_preemption_example_print_send_order(bool priority_swapped)
{
    uint8_t * p_first_sent  = priority_swapped ? gp_frame_0b_data : gp_frame_0a_data;
    uint8_t * p_second_sent = priority_swapped ? gp_frame_0a_data : gp_frame_0b_data;

    printf("[MODE] %s\n", priority_swapped ? "SWAPPED" : "NORMAL");
    printf("Sent 1st (preemptable): DST %02X:%02X:%02X:%02X:%02X:%02X\n",
           p_first_sent[0],  p_first_sent[1],  p_first_sent[2],  p_first_sent[3],  p_first_sent[4],  p_first_sent[5]);
    printf("Sent 2nd (express): DST %02X:%02X:%02X:%02X:%02X:%02X\n\n",
           p_second_sent[0], p_second_sent[1], p_second_sent[2], p_second_sent[3], p_second_sent[4], p_second_sent[5]);
}

/*******************************************************************************************************************//**
 * @brief      Entry point for the Frame Preemption sender example. Opens the Layer 3 switch, initializes the TX
 *             descriptor queue, waits for link up, then repeatedly transmits a preemptable (low-priority) frame
 *             followed by an express (high-priority) frame, toggling which physical frame buffer (0A/0B) plays
 *             each role whenever the user presses ENTER.
 * @param[in]  None.
 * @retval     None.
 **********************************************************************************************************************/
void frame_preemption_example_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;
    }

    fsp_pack_version_t version = {0};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Print the EP banner on the console */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
           version.version_id_b.patch);

    printf(EP_INFO);

    err = R_LAYER3_SWITCH_Open(&g_layer3_switch0_ctrl, &g_layer3_switch0_cfg);
    frame_preemption_handle_error(err, "layer3 switch open");

    g_tx_queue_index_port[FRAME_PREEMPTION_EXAMPLE_ETHERNET_USED_PORT] = frame_preemption_example_transmit_descriptor_initialize(FRAME_PREEMPTION_EXAMPLE_ETHERNET_USED_PORT);

    /* Indexed by FRAME_PREEMPTION_EXAMPLE_ETHERNET_USED_PORT so the link-wait below always polls the PHY
     * actually feeding the port in use - keeps this file the only place to touch when switching ports. */
    static rmac_phy_instance_ctrl_t * const p_rmac_phy_ctrl[FRAME_PREEMPTION_EXAMPLE_ETHERNET_PORT_NUM] =
    {
        &g_rmac_phy0_ctrl,
        &g_rmac_phy1_ctrl,
    };

    while(FSP_SUCCESS != R_RMAC_PHY_LinkStatusGet(p_rmac_phy_ctrl[FRAME_PREEMPTION_EXAMPLE_ETHERNET_USED_PORT]))
    {
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
    }

    printf("Start SENDER project (Frame preemption enabled)\n");
    printf("Press ENTER to toggle frame priority (0A/0B)\n\n");

    bool priority_swapped = false;
    frame_preemption_example_print_send_order(priority_swapped);

    while(1)
    {
        /* Non-blocking check: toggle priority when Enter is pressed */
        if (uart_console_has_line())
        {
            char tmp[8];
            uart_console_read(tmp, sizeof(tmp));
            priority_swapped = !priority_swapped;

            frame_preemption_example_print_send_order(priority_swapped);
        }

        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);

        /* gp_frame_0a_data/gp_frame_0b_data are the two fixed frame buffers (named after their DST MAC,
         * 0A/0B). Which one currently plays the preemptable vs. express role swaps on Enter. */
        uint8_t * p_preemptable_frame_data = priority_swapped ? gp_frame_0b_data : gp_frame_0a_data;
        uint8_t * p_express_frame_data     = priority_swapped ? gp_frame_0a_data : gp_frame_0b_data;

        /* Encode IPV into payload so receiver can read it */
        p_preemptable_frame_data[FRAME_PREEMPTION_EXAMPLE_IPV_PAYLOAD_OFFSET] = FRAME_PREEMPTION_EXAMPLE_LOW_PRIORITY;
        p_express_frame_data[FRAME_PREEMPTION_EXAMPLE_IPV_PAYLOAD_OFFSET]     = FRAME_PREEMPTION_EXAMPLE_HIGH_PRIORITY;

        /* Send the preemptable (low-priority) frame first */
        g_tx_complete_flag = false;
        frame_preemption_example_transmit_update(FRAME_PREEMPTION_EXAMPLE_ETHERNET_USED_PORT, FRAME_PREEMPTION_EXAMPLE_LOW_PRIORITY, p_preemptable_frame_data, FRAME_PREEMPTION_EXAMPLE_HIGH_SEND_FRAME_SIZE);
        while(!g_tx_complete_flag)
        {
            R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
        }

        /* Send the express (high-priority) frame second */
        g_tx_complete_flag = false;
        frame_preemption_example_transmit_update(FRAME_PREEMPTION_EXAMPLE_ETHERNET_USED_PORT, FRAME_PREEMPTION_EXAMPLE_HIGH_PRIORITY, p_express_frame_data, FRAME_PREEMPTION_EXAMPLE_HIGH_SEND_FRAME_SIZE);
        while(!g_tx_complete_flag)
        {
            R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
        }
    }
}
