/***********************************************************************************************************************
 * File Name    : frame_preemption.c
 * Description  : Contains data structures and functions for the Layer 3 Switch RX descriptor queue used by the
 *                Frame Preemption example.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <stdio.h>
#include "hal_data.h"
#include "frame_preemption.h"

#define FRAME_PREEMPTION_EXAMPLE_DESCRIPTOR_QUEUE_LENGTH               (5)
#define FRAME_PREEMPTION_EXAMPLE_ETHERNET_FRAME_LENGTH_UPPER           ((uint8_t)((FRAME_PREEMPTION_EXAMPLE_ETHERNET_FRAME_SIZE & 0xFF00) >> 8))
#define FRAME_PREEMPTION_EXAMPLE_ETHERNET_FRAME_LENGTH_LOWER           ((uint8_t)((FRAME_PREEMPTION_EXAMPLE_ETHERNET_FRAME_SIZE & 0x00FF)))

bool g_rx_complete_flag = false;

uint32_t g_rx_queue_index_port[FRAME_PREEMPTION_EXAMPLE_ETHERNET_PORT_NUM] = {0};

BSP_ALIGN_VARIABLE(32) layer3_switch_descriptor_t gp_rx_descriptor_array[FRAME_PREEMPTION_EXAMPLE_DESCRIPTOR_QUEUE_LENGTH];
BSP_ALIGN_VARIABLE(32) uint8_t gpp_rx_buffer[(FRAME_PREEMPTION_EXAMPLE_DESCRIPTOR_QUEUE_LENGTH - 1) * 2][FRAME_PREEMPTION_EXAMPLE_ETHERNET_FRAME_SIZE];

layer3_switch_descriptor_queue_cfg_t g_rx_queue_cfg = {0};

static void frame_preemption_example_reset_rx_descriptor(uint32_t rx_queue_index);

/*******************************************************************************************************************//**
 * @brief      Checks the given error code. On failure, prints the failing call and de-initializes (closes) the
 *             Layer 3 switch module instead of asserting.
 * @param[in]  err      The FSP error code to check.
 * @param[in]  p_msg    Message identifying the failing call, printed on error.
 * @retval     None.
 **********************************************************************************************************************/
void frame_preemption_handle_error(fsp_err_t err, const char * p_msg)
{
    if (FSP_SUCCESS != err)
    {
        printf("ERROR: %s (0x%x)\r\n", p_msg, (unsigned int) err);

        if (0U != g_layer3_switch0_ctrl.open)
        {
            R_LAYER3_SWITCH_Close(&g_layer3_switch0_ctrl);
        }

    }
}

/*******************************************************************************************************************//**
 * @brief      Creates the RX descriptor queue for the given port and resets all its descriptors so they are
 *             ready to receive frames.
 * @param[in]  port_num    Ethernet port number to create the RX descriptor queue for.
 * @retval     Index of the created RX descriptor queue, to be used with frame_preemption_example_reception_update().
 **********************************************************************************************************************/
uint32_t frame_preemption_example_reception_descriptor_initialize(uint8_t port_num)
{
    fsp_err_t err           = FSP_SUCCESS;
    uint32_t rx_queue_index = 0;

    g_rx_queue_cfg.array_length          = FRAME_PREEMPTION_EXAMPLE_DESCRIPTOR_QUEUE_LENGTH;
    g_rx_queue_cfg.descriptor_format     = LAYER3_SWITCH_DISCRIPTOR_FORMTAT_EXTENDED;
    g_rx_queue_cfg.p_descriptor_array    = gp_rx_descriptor_array;
    g_rx_queue_cfg.p_ts_descriptor_array = NULL;
    g_rx_queue_cfg.ports                 = (1 << port_num);
    g_rx_queue_cfg.rx_timestamp_storage  = LAYER3_SWITCH_RX_TIMESTAMP_STORAGE_DISABLE;
    g_rx_queue_cfg.type                  = LAYER3_SWITCH_QUEUE_TYPE_RX;
    g_rx_queue_cfg.write_back_mode       = LAYER3_SWITCH_WRITE_BACK_MODE_FULL;

    err = R_LAYER3_SWITCH_CreateDescriptorQueue(&g_layer3_switch0_ctrl, &rx_queue_index, &g_rx_queue_cfg);
    frame_preemption_handle_error(err, "rx descriptor queue create");

    frame_preemption_example_reset_rx_descriptor(rx_queue_index);

    return rx_queue_index;
}

/*******************************************************************************************************************//**
 * @brief      Reloads every descriptor in the RX queue except the last one with a fixed frame buffer marked
 *             FEMPTY, then restarts the queue so it is ready to receive again.
 * @param[in]  rx_queue_index    RX descriptor queue index to reset.
 * @retval     None.
 **********************************************************************************************************************/
static void frame_preemption_example_reset_rx_descriptor(uint32_t rx_queue_index)
{
    fsp_err_t err = FSP_SUCCESS;

    for(uint8_t rx_descriptor_index = 0; rx_descriptor_index < (FRAME_PREEMPTION_EXAMPLE_DESCRIPTOR_QUEUE_LENGTH - 1); rx_descriptor_index++)
    {
        layer3_switch_descriptor_t rx_descriptor = {0};
        rx_descriptor.basic.ds_l  = FRAME_PREEMPTION_EXAMPLE_ETHERNET_FRAME_LENGTH_LOWER;
        rx_descriptor.basic.ds_h  = FRAME_PREEMPTION_EXAMPLE_ETHERNET_FRAME_LENGTH_UPPER;
        rx_descriptor.basic.die   = 1;
        rx_descriptor.basic.ptr_l = (uintptr_t)gpp_rx_buffer[rx_descriptor_index];
        rx_descriptor.basic.ptr_h = 0;
        rx_descriptor.basic.dt    = LAYER3_SWITCH_DESCRIPTOR_TYPE_FEMPTY;

        err = R_LAYER3_SWITCH_SetDescriptor(&g_layer3_switch0_ctrl, rx_queue_index, &rx_descriptor);
        frame_preemption_handle_error(err, "rx set descriptor");
    }

    err = R_LAYER3_SWITCH_StartDescriptorQueue(&g_layer3_switch0_ctrl, rx_queue_index);
    frame_preemption_handle_error(err, "rx start descriptor queue");
}

/*******************************************************************************************************************//**
 * @brief      Polls the RX descriptor queue for the given port for a newly received frame. If one is available
 *             and free of errors, copies it into p_recv_buffer. Automatically resets the RX descriptor queue
 *             every 4 received frames to keep it running.
 * @param[in]  port_num           Ethernet port number to poll for received frames.
 * @param[out] p_recv_buffer      Buffer to copy the received frame into.
 * @param[in]  recv_buffer_size   Size of p_recv_buffer, in bytes.
 * @retval     Number of bytes copied into p_recv_buffer (0 if no frame was received).
 **********************************************************************************************************************/
uint32_t frame_preemption_example_reception_update(uint8_t port_num, uint8_t * p_recv_buffer, uint32_t recv_buffer_size)
{
    fsp_err_t err                                                  = FSP_SUCCESS;
    uint32_t recv_length                                           = 0;
    layer3_switch_descriptor_t rx_descriptor                       = {0};
    static uint8_t s_reception_descriptor_index[FRAME_PREEMPTION_EXAMPLE_ETHERNET_PORT_NUM] = {0};

    uint32_t rx_queue_index = 0;

    if(port_num < FRAME_PREEMPTION_EXAMPLE_ETHERNET_PORT_NUM)
    {
        rx_queue_index = g_rx_queue_index_port[port_num];
    }
    else
    {
        /* Invalid port number. */
        return recv_length;
    }

    err = R_LAYER3_SWITCH_GetDescriptor(&g_layer3_switch0_ctrl, rx_queue_index, &rx_descriptor);
    if ((FSP_SUCCESS != err) && (FSP_ERR_IN_USE != err))
    {
        frame_preemption_handle_error(err, "rx get descriptor");
    }

    if((FSP_ERR_IN_USE == err) || (LAYER3_SWITCH_DESCRIPTOR_TYPE_FEMPTY == rx_descriptor.basic.dt))
    {
        /* No received. Nothing to do. */
        return recv_length;
    }

    if(0 == rx_descriptor.basic.err)
    {
        recv_length  = (uint32_t) (rx_descriptor.basic.ds_l);
        recv_length |= (uint32_t)(rx_descriptor.basic.ds_h << 8);

        if(recv_buffer_size < recv_length)
        {
            /* Buffer over flow. */
            recv_length = recv_buffer_size;
        }

        memcpy(p_recv_buffer, (uint8_t *)rx_descriptor.basic.ptr_l, recv_length);
    }

    s_reception_descriptor_index[port_num]++;
    if( s_reception_descriptor_index[port_num] == 4)
    {
        s_reception_descriptor_index[port_num] = 0;
        frame_preemption_example_reset_rx_descriptor(rx_queue_index);
    }

    return recv_length;
}

/*******************************************************************************************************************//**
 * @brief      Layer 3 switch event callback. Sets g_rx_complete_flag when an RX queue full, RX message lost, or
 *             RX complete event is received.
 * @param[in]  p_args    Pointer to the Layer 3 switch callback arguments.
 * @retval     None.
 **********************************************************************************************************************/
void layer3_switch_callback (ether_switch_callback_args_t * p_args)
{
    switch (p_args->event)
    {
        case ETHER_SWITCH_EVENT_RX_QUEUE_FULL:
        case ETHER_SWITCH_EVENT_RX_MESSAGE_LOST:
        case ETHER_SWITCH_EVENT_RX_COMPLETE:
        {
            g_rx_complete_flag = true;
            break;
        }
        case ETHER_SWITCH_EVENT_TAS_ERROR:
        {
            break;
        }

        default:
        {
            break;
        }
    }
}
