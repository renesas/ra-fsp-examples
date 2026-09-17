/***********************************************************************************************************************
 * File Name    : frame_preemption_example.c
 * Description  : Contains data structures and functions for the Frame Preemption receiver example application.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include "frame_preemption.h"
#include "frame_preemption_example.h"
#include "console/uart_console.h"
#include "stdio.h"
#include "string.h"

#define FRAME_PREEMPTION_EXAMPLE_ETHERNET_USED_PORT  (1)
#define FRAME_PREEMPTION_EXAMPLE_IPV_PAYLOAD_OFFSET  (14U)

static const uint8_t s_target_mac_prefix[] = {0x44, 0x44, 0x44, 0x00, 0x00};

/*******************************************************************************************************************//**
 * @brief      Entry point for the Frame Preemption receiver example. Opens the Layer 3 switch, initializes the
 *             RX descriptor queue, waits for link up, then repeatedly polls for received frames and prints each
 *             preemptable/express pair matched by destination MAC prefix.
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

    g_rx_queue_index_port[FRAME_PREEMPTION_EXAMPLE_ETHERNET_USED_PORT] = frame_preemption_example_reception_descriptor_initialize(FRAME_PREEMPTION_EXAMPLE_ETHERNET_USED_PORT);

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

    printf("Start RECEIVER project (Frame preemption enabled)\n");

    uint8_t  p_recv_buffer[FRAME_PREEMPTION_EXAMPLE_ETHERNET_FRAME_SIZE] = {0};
    uint32_t recv_length = 0;
    uint32_t recv_count  = 0;

    while(1)
    {
        recv_length = frame_preemption_example_reception_update(FRAME_PREEMPTION_EXAMPLE_ETHERNET_USED_PORT, p_recv_buffer, sizeof(p_recv_buffer));
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);

        if(0 < recv_length)
        {
            if(0 == memcmp(s_target_mac_prefix, p_recv_buffer, sizeof(s_target_mac_prefix)))
            {
                recv_count++;
                uint8_t ipv = p_recv_buffer[FRAME_PREEMPTION_EXAMPLE_IPV_PAYLOAD_OFFSET];

                /* First frame of the pair opens the block, regardless of which MAC arrives first */
                bool     is_first_of_pair = (1 == (recv_count % 2));
                uint32_t pair_id          = (recv_count + 1) / 2;

                if(is_first_of_pair) printf("--------------------- RX ID: %u ---------------------\n", pair_id);

                printf("%s: IPV=%u | %4u bytes | DST: %02X:%02X:%02X:%02X:%02X:%02X\n",
                       is_first_of_pair ? "1st" : "2nd",
                       ipv, recv_length,
                       p_recv_buffer[0], p_recv_buffer[1], p_recv_buffer[2],
                       p_recv_buffer[3], p_recv_buffer[4], p_recv_buffer[5]);

                if(0 == (recv_count % 2)) printf("-----------------------------------------------------\n\n");
            }
        }
    }
}
