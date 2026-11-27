/***********************************************************************************************************************
 * File Name    : ssi_master_ep.c
 * Description  : Contains data structures and functions used in ssi_master_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "ssi_master_ep.h"

/* Current SSI event, updated in the callback */
volatile i2s_event_t g_i2s_event = I2S_EVENT_TX_EMPTY;

/* Row index (into g_dest_buff/g_src_buff) used for the current read/write operation */
uint32_t g_buf_row_index = INDEX;

/* Operation mode selected by the user: MASTER_WRITE_SLAVE_READ or MASTER_READ_SLAVE_WRITE */
uint8_t g_selection_value = RESET_VALUE;

/* Number of read/write transactions completed so far in the current operation */
uint16_t g_read_counter = 0U;
uint16_t g_write_counter = 0U;

/* Flag to indicate the callback has reloaded the read FIFO and data is ready to be reported */
volatile bool g_read_in_cb = false;

/* Flag to indicate the main loop should issue the next write */
volatile bool g_send_data_in_main_loop = false;

/* Destination buffer to receive the sample audio data */
uint8_t g_dest_buff[BUF_ROW_SIZE][BUF_FIFO_STAGE] BSP_ALIGN_VARIABLE(4) = {{RESET_VALUE}};

/* Source buffer to transmit the sample audio data */
const uint8_t g_src_buff[BUF_ROW_SIZE][BUF_FIFO_STAGE] BSP_ALIGN_VARIABLE(4) =
{
    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
      0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
      0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F },
    { 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
      0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
      0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
      0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F },
    { 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
      0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
      0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
      0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF },
    { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
      0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
      0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
      0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF }
};

const uint8_t g_src_buff1[BUF_ROW_SIZE][BUF_FIFO_STAGE] BSP_ALIGN_VARIABLE(4) =
{
    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
      0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
      0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F },
    { 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
      0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
      0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
      0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F },
    { 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
      0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
      0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
      0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF },
    { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
      0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
      0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
      0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF }
};

/*******************************************************************************************************************//**
 * @brief     This function initializes SSI module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t ssi_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open SSI channel */
    err = R_SSI_Open(&g_i2s_master_ctrl, &g_i2s_master_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened SSI module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void ssi_deinit(void)
{
    /* Close the opened SSI module */
    if (g_i2s_master_ctrl.open)
    {
        R_SSI_Close(&g_i2s_master_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the GPT channel used to generate the SSI audio clock.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t ssi_clock_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open GPT channel */
    err = R_GPT_Open(&ssi_clock_ctrl, &ssi_clock_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts the GPT timer used to generate the SSI audio clock.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t ssi_clock_start(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Start GPT timer */
    err = R_GPT_Start(&ssi_clock_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened GPT module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void ssi_clock_deinit(void)
{
    /* Close the opened GPT module */
    if (ssi_clock_ctrl.open)
    {
        R_GPT_Close(&ssi_clock_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function reads one FIFO stage of data from the SSI channel into g_dest_buff.
 * @param[in] buf_row_index             Row of g_dest_buff to receive the data.
 * @retval    FSP_SUCCESS               Read successfully.
 * @retval    Any other error codes     Failed to read.
 **********************************************************************************************************************/
fsp_err_t ssi_read(uint32_t buf_row_index)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Read the data from the Slave */
    err = R_SSI_Read(&g_i2s_master_ctrl, &g_dest_buff[buf_row_index][0U], (BUF_FIFO_STAGE) * sizeof(int32_t));

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function writes one FIFO stage of data from g_src_buff to the SSI channel.
 * @param[in] buf_row_index             Row of g_src_buff to transmit the data.
 * @retval    FSP_SUCCESS               Written successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t ssi_write(uint32_t buf_row_index)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Write the data to the Slave */
    err = R_SSI_Write(&g_i2s_master_ctrl, &g_src_buff[buf_row_index][0U], (BUF_FIFO_STAGE) * sizeof(int32_t));

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback function for SSI module and reloads the FIFO for the next transfer.
 * @param[in] p_args                    Callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void ssi_example_callback(i2s_callback_args_t * p_args)
{
    g_i2s_event = p_args->event;

    if (((I2S_EVENT_TX_EMPTY == p_args->event) || (I2S_EVENT_IDLE == p_args->event)) &&\
        (MASTER_WRITE_SLAVE_READ == g_selection_value) && (WRITE_COUNTS > g_write_counter))
    {
        /* Reload FIFO */
        R_SSI_Write(&g_i2s_master_ctrl, &g_src_buff1[g_buf_row_index][0U], (BUF_FIFO_STAGE) * sizeof(int32_t));

        /* Data was not ready yet, send it in the main loop */
        g_send_data_in_main_loop = true;
    }

    if (((I2S_EVENT_RX_FULL == p_args->event) || (I2S_EVENT_IDLE == p_args->event)) &&\
        (MASTER_READ_SLAVE_WRITE == g_selection_value) && (READ_COUNTS > g_read_counter))
    {
        /* Reload FIFO */
        R_SSI_Read(&g_i2s_master_ctrl, &g_dest_buff[g_buf_row_index+1][0U], (BUF_FIFO_STAGE) * sizeof(int32_t));

        /* Set the flag to read it in the main loop */
        g_read_in_cb = true;
    }
}
