/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : display_mipi_ili9806e.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/*
 * mipi_display_data.c
 *
 *  Created on: Aug 17, 2023
 *      Author: Austin.Hansen
 */
#include "display_mipi_ili9806e.h"
#include "display_mipi_ili9806e_commands.h"

/* Set this to 1 to set the return packet size when requesting information from the panel */
#define ILI9806E_CONFIGURE_RETURN_PACKET_SIZE    (0)

volatile bool g_message_sent = false;

void mipi_dsi0_callback(mipi_dsi_callback_args_t * p_args);

/* Callback function */

/**********************************************************************************************************************
 * Function Name: mipi_dsi0_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void mipi_dsi0_callback (mipi_dsi_callback_args_t * p_args)
{
    if (MIPI_DSI_EVENT_SEQUENCE_0 == p_args->event)
    {
        g_message_sent = (p_args->tx_status == MIPI_DSI_SEQUENCE_STATUS_DESCRIPTORS_FINISHED);
    }
    else
    {
        /* Put a __BKPT(0) here to check for errors. */
    }
}

/**********************************************************************************************************************
 * End of function mipi_dsi0_callback
 *********************************************************************************************************************/

/* Initialize and start display output on ILI9806E */

/**********************************************************************************************************************
 * Function Name: display_mipi_ili9806e_init
 * Description  : .
 * Argument     : p_display
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t display_mipi_ili9806e_init (display_instance_t const * const p_display)
{
    volatile fsp_err_t err;
    dsi_layer_hw_reset();

    vTaskDelay(200);

    /* Initialize GLCDC and MIPI DSI */
    err = R_GLCDC_Open(p_display->p_ctrl, p_display->p_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if ILI9806E_CONFIGURE_RETURN_PACKET_SIZE
    uint8_t        cmd_buffer[]    = {0x02, 0x00};
    mipi_dsi_cmd_t return_size_cmd =
    {
        .channel     = 0,
        .cmd_id      = MIPI_DSI_CMD_ID_SET_MAXIMUM_RETURN_PACKET_SIZE,
        .flags       = MIPI_DSI_CMD_FLAG_LOW_POWER,
        .tx_len      = 2,
        .p_tx_buffer = cmd_buffer,
    };

    /* Set Return packet size */
    g_message_sent = false;
    err            = R_MIPI_DSI_Command(p_mipi_dsi_ctrl, &return_size_cmd);
    if (FSP_SUCCESS != err)
    {
        R_MIPI_DSI_Close(p_mipi_dsi_ctrl);

        return err;
    }

    while (!g_message_sent)
    {
        /* Wait for message to send. */
    }
#endif

    mipi_setting_table_t const * p_entry    = p_command_list_ili9806e;
    mipi_dsi_instance_t        * p_mipi_dsi = ((glcdc_extended_cfg_t const *) (p_display->p_cfg->p_extend))->phy_layer;
    while (p_entry->cmd_id != MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE)
    {
        mipi_dsi_cmd_t cmd =
        {
            .channel     = 0,
            .cmd_id      = p_entry->cmd_id,
            .flags       = p_entry->flags,
            .tx_len      = p_entry->size,
            .p_tx_buffer = p_entry->buffer
        };

        if (MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG == p_entry->cmd_id)
        {
            R_BSP_SoftwareDelay(p_entry->size, BSP_DELAY_UNITS_MILLISECONDS);
        }
        else
        {
            g_message_sent = false;
            err            = R_MIPI_DSI_Command(p_mipi_dsi->p_ctrl, &cmd);
            if (FSP_SUCCESS != err)
            {
                R_GLCDC_Close(p_display->p_ctrl);

                return err;
            }

            while (!g_message_sent)
            {
                /* Wait for message to send. */
            }
        }

        p_entry++;
    }

    /* Start graphics output */
    err = R_GLCDC_Start(p_display->p_ctrl);
    if (FSP_SUCCESS != err)
    {
        R_GLCDC_Close(p_display->p_ctrl);
    }

    return err;
}

/**********************************************************************************************************************
 * End of function display_mipi_ili9806e_init
 *********************************************************************************************************************/
