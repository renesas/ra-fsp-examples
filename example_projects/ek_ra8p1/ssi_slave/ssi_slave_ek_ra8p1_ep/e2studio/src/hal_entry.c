/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "audio_data.h"
#include "ssi.h"

/*******************************************************************************************************************//**
 * @addtogroup ssi_slave_ep
 * @{
 **********************************************************************************************************************/

i2s_state_t g_i2s_state;
static unsigned char g_rbyte[BUFFER_SIZE_DOWN] = {RESET_VALUE};
static uint8_t g_selection_value = RESET_VALUE;
volatile bool g_read_data_in_main_loop = false, g_send_data_in_main_loop = false;
static uint16_t g_write_counter = VALUE_0;
static uint16_t g_read_counter = VALUE_0;
static bool g_read_in_cb = false;
uint32_t g_buf_row_index = INDEX;

/* Destination buffer to receive the sample audio data */
uint8_t g_dest_buff[BUF_ROW_SIZE][BUF_FIFO_STAGE] = {{RESET_VALUE}};

/* Source buffer to transmit the sample audio data */
const uint8_t g_src_buff[BUF_ROW_SIZE][BUF_FIFO_STAGE] BSP_ALIGN_VARIABLE(4) = {\
                  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,\
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,\
                    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,\
                    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F },\
                  { 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,\
                    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,\
                    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,\
                    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F},\
                  { 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,\
                    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,\
                    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,\
                    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF },\
                  { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,\
                    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,\
                    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,\
                    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF }
};

const uint8_t g_src_buff1[BUF_ROW_SIZE][BUF_FIFO_STAGE] BSP_ALIGN_VARIABLE(4) = {\
                  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,\
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,\
                    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,\
                    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F },\
                  { 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,\
                    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,\
                    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,\
                    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F},\
                  { 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,\
                    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,\
                    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,\
                    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF },\
                  { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,\
                    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,\
                    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,\
                    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF }
};

/* Private functions */
static void deinit_ssi(void);
static void master_write_slave_read_operation(void);
static void master_read_slave_write_operation(void);

volatile i2s_event_t g_i2s_event = I2S_EVENT_TX_EMPTY;

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used. This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version =   { RESET_VALUE };
    g_read_data_in_main_loop = true;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet (&version);

    APP_PRINT(BANNER_INFO, EP_VERSION,version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);

    /* Open SSI channel */
    err = R_SSI_Open(&g_i2s_slave_ctrl, &g_i2s_slave_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_SSI_Open API failed\r\n");
        /* Trap here */
        APP_ERR_TRAP(err);
    }
    while(true)
    {
        APP_PRINT("\r\nOperating mode: SLAVE");

        /* The user's input selection */
        APP_PRINT("\r\nPlease select the desired operation\n"
                  "1. Press 1 for Master Write - Slave Read\r\n"
                  "2. Press 2 for Master Read  - Slave Write\r\n");
        while(true)
        {
            if (APP_CHECK_DATA)
            {
                /* Cleaning buffer */
                memset(&g_rbyte[INDEX_0], NULL_CHAR, BUFFER_SIZE_DOWN);
                APP_READ(g_rbyte);
                g_selection_value = (uint8_t)atoi((char *)g_rbyte);
                if ((SELECTION_MAX >= g_selection_value ) && (SELECTION_MIN <= g_selection_value))
                {
                    APP_PRINT("\r\nSelected Mode %d\r\n",g_selection_value);
                    if (MASTER_WRITE_SLAVE_READ == g_selection_value)
                    {
                        APP_PRINT("\r\nMASTER WRITE - SLAVE READ Mode Selected\r\n");
                    }
                    if (MASTER_READ_SLAVE_WRITE == g_selection_value)
                    {
                        APP_PRINT("\r\nMASTER READ - SLAVE WRITE Mode Selected\r\n");
                    }
                    break;
                }
                else
                {
                    APP_PRINT("\r\nInvalid input. Enter valid input\r\n");
                }
            }
        }

        if (MASTER_WRITE_SLAVE_READ == g_selection_value)
        {
            /*-------W R I T E-------*/
            master_write_slave_read_operation();
        }
        if (MASTER_READ_SLAVE_WRITE == g_selection_value)
        {
            /*-------R E A D-------*/
            master_read_slave_write_operation();
        }
    }

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process. This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take
         * more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);

#if BSP_CFG_SDRAM_ENABLED

        /* Setup SDRAM and initialize it. Must configure pins first. */
        R_BSP_SdramInit(true);
#endif
    }
}

#if BSP_TZ_SECURE_BUILD

FSP_CPP_HEADER
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build
 * (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
FSP_CPP_FOOTER

#endif

/*******************************************************************************************************************//**
* @brief       Close SSI module
* @param[in]   None
* @return      None
**********************************************************************************************************************/
static void deinit_ssi(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close SSI Module */
    err = R_SSI_Close(&g_i2s_slave_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_SSI_Close API Failed\r\n");
    }
}

 /******************************************************************************************************************//**
 * @brief       master_write_slave_read_operation
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
static void master_write_slave_read_operation()
{
    fsp_err_t err = FSP_SUCCESS;
    g_buf_row_index = INDEX_0;
    g_read_in_cb = false;
    g_read_data_in_main_loop = false;
    g_read_counter = VALUE_0;

    /* Wait for incoming data */
    APP_PRINT("Reading data from Master...\r\n");

    /* Clear the buffer before reading it */
    memset(g_dest_buff, VALUE_0, sizeof(g_dest_buff));

    /* Read data from Master */
    err = R_SSI_Read(&g_i2s_slave_ctrl, &g_dest_buff[g_buf_row_index][INDEX_0], (BUF_FIFO_STAGE)* sizeof(int32_t));
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_SSI_Read API failed. Closing SSI\r\n");
        deinit_ssi();
        /* Trap here */
        APP_ERR_TRAP(err);
    }

    while(true)
    {
        if(true == g_read_in_cb)
        {
            /* Printing read data */
            APP_PRINT("\r\n");
            for(uint16_t index = VALUE_0; index < BUF_FIFO_STAGE; index++)
            {
                APP_PRINT("0x%02x, ", g_dest_buff[g_buf_row_index + INDEX_1][index]);
            }

            /* Data was not ready yet, send it in the main loop */
            g_read_data_in_main_loop = true;

            /* Reset the read flag */
            g_read_in_cb = false;
        }
        if (true == g_read_data_in_main_loop)
        {
            /* Read data back to Slave */
            if((I2S_EVENT_IDLE == g_i2s_event) || (I2S_EVENT_RX_FULL == g_i2s_event))
            {
                g_read_data_in_main_loop = false;
                err = R_SSI_Read(&g_i2s_slave_ctrl, &g_dest_buff[g_buf_row_index][INDEX_0],\
                                 (BUF_FIFO_STAGE)* sizeof(int32_t));
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_SSI_Read API failed. Closing SSI\r\n");
                    deinit_ssi();
                    /* Trap here */
                    APP_ERR_TRAP(err);
                }
                else
                {
                    /* Increment Read counter */
                    g_read_counter++;

                    APP_PRINT("\r\n");
                    for(uint16_t index = VALUE_0; index < BUF_FIFO_STAGE; index++)
                    {
                        APP_PRINT("0x%02x, ", g_dest_buff[g_buf_row_index][index]);
                    }
                }
            }
        }
        R_BSP_SoftwareDelay (WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);

        /* Check if the counter reaches READ_COUNTS */
        if(READ_COUNTS == g_read_counter)
        {
            APP_PRINT("\r\nReading data from Master Completed...\r\n");
            APP_PRINT("To Terminate the read process Press any key...\r\n");
            while(true)
            {
                if (APP_CHECK_DATA)
                {
                    /* Cleaning buffer */
                    memset(&g_rbyte[INDEX_0], NULL_CHAR, BUFFER_SIZE_DOWN);
                    APP_READ(g_rbyte);
                    APP_PRINT("\r\nTerminating...\r\n");
                    break;
                }
            }
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       master_read_slave_write_operation
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
static void master_read_slave_write_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    g_buf_row_index = INDEX_0;
    g_write_counter = VALUE_0 ;
    g_send_data_in_main_loop = false;

    APP_PRINT("Writing data to Master...\r\n");
    /* Write data to Slave */
    err = R_SSI_Write(&g_i2s_slave_ctrl, &g_src_buff[g_buf_row_index][INDEX_0], (BUF_FIFO_STAGE)* sizeof(int32_t));
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_SSI_Write API failed. Closing SSI\r\n");
        deinit_ssi();
        /* Trap here */
        APP_ERR_TRAP(err);
    }

    while(WRITE_COUNTS > g_write_counter)
    {
        if ((true == g_send_data_in_main_loop))
        {
            /* Write data back to Slave */
            if((I2S_EVENT_IDLE == g_i2s_event)||(I2S_EVENT_TX_EMPTY == g_i2s_event))
            {
                g_send_data_in_main_loop = false;
                err = R_SSI_Write(&g_i2s_slave_ctrl, &g_src_buff[g_buf_row_index][INDEX_0],\
                                  (BUF_FIFO_STAGE)* sizeof(int32_t));
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_SSI_Write API failed. Closing SSI\r\n");
                    deinit_ssi();
                    /* Trap here */
                    APP_ERR_TRAP(err);
                }
                else
                {
                    g_write_counter++;
                }
            }
        }
        R_BSP_SoftwareDelay (WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
    }
    APP_PRINT("Write complete\r\n");
    APP_PRINT("Turn the Master OFF and Press any key to Terminate the process\r\n");
    while(true)
    {
        if (APP_CHECK_DATA)
        {
            /* Cleaning buffer */
            memset(&g_rbyte[INDEX_0], NULL_CHAR, BUFFER_SIZE_DOWN);
            APP_READ(g_rbyte);
            APP_PRINT("\r\nTerminating...\r\n");
            break;
        }
    }
}

/*******************************************************************************************************************//**
* @brief       SSI callback function
* @param[in]   p_args
* @return      None
**********************************************************************************************************************/
void ssi_example_callback (i2s_callback_args_t * p_args)
{
    g_i2s_event = p_args->event;

    if (((I2S_EVENT_RX_FULL == p_args->event) || (I2S_EVENT_IDLE == p_args->event)) && \
        (MASTER_WRITE_SLAVE_READ == g_selection_value) && (READ_COUNTS > g_read_counter))
    {
        /* Reload FIFO and handle errors */
        R_SSI_Read(&g_i2s_slave_ctrl, &g_dest_buff[g_buf_row_index+1][INDEX_0], (BUF_FIFO_STAGE)* sizeof(int32_t));

        /* Set the flag to read it in the main loop */
        g_read_in_cb = true;
    }

    if (((I2S_EVENT_TX_EMPTY == p_args->event) || (I2S_EVENT_IDLE == p_args->event)) && \
        (MASTER_READ_SLAVE_WRITE == g_selection_value) && (WRITE_COUNTS > g_write_counter))
    {
        /* Reload FIFO and handle errors */
        R_SSI_Write(&g_i2s_slave_ctrl, &g_src_buff1[g_buf_row_index][INDEX_0], (BUF_FIFO_STAGE)* sizeof(int32_t));

        /* Data was not ready yet, send it in the main loop */
        g_send_data_in_main_loop = true;
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup ssi_slave_ep)
 **********************************************************************************************************************/
