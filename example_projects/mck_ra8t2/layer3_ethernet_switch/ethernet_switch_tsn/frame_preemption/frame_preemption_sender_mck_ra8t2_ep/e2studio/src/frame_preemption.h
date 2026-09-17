/***********************************************************************************************************************
 * File Name    : frame_preemption.h
 * Description  : Contains macros, data structures, and function declarations for the Layer 3 Switch TX descriptor
 *                queue used by the Frame Preemption example.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "bsp_api.h"

#define FRAME_PREEMPTION_EXAMPLE_ETHERNET_PORT_NUM   (2)
#define FRAME_PREEMPTION_EXAMPLE_ETHERNET_FRAME_SIZE (1514)

extern bool g_tx_complete_flag;
extern uint32_t g_tx_queue_index_port[FRAME_PREEMPTION_EXAMPLE_ETHERNET_PORT_NUM];
extern uint8_t gp_frame_0b_data[FRAME_PREEMPTION_EXAMPLE_ETHERNET_FRAME_SIZE];
extern uint8_t gp_frame_0a_data[FRAME_PREEMPTION_EXAMPLE_ETHERNET_FRAME_SIZE];

uint32_t frame_preemption_example_transmit_descriptor_initialize(uint8_t port_num);
void frame_preemption_example_transmit_update(uint8_t port, uint8_t ipv, uint8_t * p_trans_buffer, uint32_t trans_buffer_size);
void frame_preemption_handle_error(fsp_err_t err, const char * p_msg);
