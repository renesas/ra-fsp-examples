/***********************************************************************************************************************
 * File Name    : frame_preemption.h
 * Description  : Contains macros, data structures, and function declarations for the Layer 3 Switch RX descriptor
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

extern bool g_rx_complete_flag;
extern uint32_t g_rx_queue_index_port[FRAME_PREEMPTION_EXAMPLE_ETHERNET_PORT_NUM];

uint32_t frame_preemption_example_reception_descriptor_initialize(uint8_t port_num);
uint32_t frame_preemption_example_reception_update(uint8_t port_num, uint8_t * p_recv_buffer, uint32_t recv_buffer_size);
void frame_preemption_handle_error(fsp_err_t err, const char * p_msg);
