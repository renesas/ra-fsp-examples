/***********************************************************************************************************************
 * File Name    : dmac_transfers.c
 * Description  : Contains data structures and functions used in dmac_transfers.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "dmac_transfers.h"
#include "transfer_initialise.h"
#include "timer_initialise.h"
#include "s_cache.h"

/*******************************************************************************************************************//**
 * @addtogroup dmac_transfers
 * @{
 **********************************************************************************************************************/

/* Source and destination array for DMAC transfer */



/* Boolean flag to determine if transfer is complete */
volatile bool b_is_transfer_complete  = false;
volatile bool b_is_transfer_complete2  = false;
bool invalidate = false;
bool invalidate_app = false;




int32_t g_dest_sine_cosine_data[DATA_SIZE]=
{
 0
};

int32_t g_source_sine_cosine_data[DATA_SIZE]=
{
 0
};


const int32_t g_source_sine_cosine_data_code_flah[DATA_SIZE] =
{

 0x00450F9F,0x008B0F9D,0x00D10F9A,0x01170F96,0x015C0F90,0x01A20F8A,0x01E70F82,0x022C0F79,0x02710F6E,0x02B60F63,0x02FB0F56,0x033F0F48,0x03830F39,0x03BD0F29,0x040B0F17,0x044E0F05,
 0x04910EF1,0x04D40EDC,0x05160EC6,0x05580EAE,0x05990E96,0x05DA0E7C,0x061A0E62,0x065A0E46,0x069A0E29,0x06D90E0B,0x07170DEC,0x07550DCB,0x07930DAA,0x07D00D88,0x080C0D64,0x08470D40,
 0x08820D1A,0x08BC0CF4,0x08F60CCC,0x092F0CA4,0x09670C7A,0x099E0C50,0x09D50C24,0x0A0B0BF8,0x0A400BCA,0x0A740B97,0x0AA70B6D,0x0ADA0B3D,0x0B0C0B0C,0x0B3D0ADA,0x0B6D0AA7,0x0B9C0A74,
 0x0BCA0A40,0x0BF80A0B,0x0C2409D5,0x0C50099E,0x0C7A0967,0x0CA4092F,0x0CCC08F6,0x0CF408BC,0x0D1A0882,0x0D400847,0x0D64080C,0x0D8807D0,0x0DAA0793,0x0DCB0755,0x0DEC0717,0x0E0B06D9,
 0x0E29069A,0x0E46065A,0x0E62061A,0x0E7C05DA,0x0E960599,0x0EAE0558,0x0EC60516,0x0EDC04D4,0x0EF10491,0x0F05044E,0x0F17040B,0x0F2903C7,0x0F390383,0x0F48033F,0x0F5602FB,0x0F6302B6,
 0x0F6E0271,0x0F79022C,0x0F8201E7,0x0F8A01A2,0x0F90015C,0x0F960117,0x0F9A00D1,0x0F9D008B,0x0F9F0045,0x0FA00000,0x0F9F0045,0x0F9D008B,0x0F9A00D1,0x0F960117,0x0F90015C,0x0F8A01A2,
 0x0F8201E7,0x0F79022C,0x0F6E0271,0x0F6302B6,0x0F5602FB,0x0F48033F,0x0F390383,0x0F2903C7,0x0F17040B,0x0F05044E,0x0EF10491,0x0EDC04D4,0x0EC60516,0x0EAE0558,0x0E960599,0x0E7C05DA,
 0x0E62061A,0x0E46065A,0x0E29069A,0x0E0B06D9,0x0DEC0717,0x0DCB0755,0x0DAA0793,0x0D8807D0,0x0D64080C,0x0D400847,0x0D1A0882,0x0CF408BC,0x0CCC08F6,0x0CA4092F,0x0C7A0967,0x0C50099E,
 0x0C2409D5,0x0BF80A0B,0x0BCA0A40,0x0B9C0A74,0x0B6D0AA7,0x0B3D0ADA,0x0B0C0B0C,0x0ADA0B3D,0x0AA70B6D,0x0A740B9C,0x0A400BCA,0x0A0B0BF8,0x09D50C24,0x099E0C50,0x09670C7A,0x092F0CA4,
 0x08F60CCC,0x08BC0CF4,0x08820D1A,0x08470D40,0x080C0D64,0x07D00D88,0x07930DAA,0x07550DCB,0x07170DEC,0x06D90E0B,0x069A0E29,0x065A0E46,0x061A0E62,0x05DA0E7C,0x05990E96,0x05580EAE,
 0x05160EC6,0x04D40EDC,0x04910EF1,0x044E0F05,0x040B0F17,0x03C70F29,0x03830F39,0x033F0F48,0x02FB0F56,0x02B60F63,0x02710F6E,0x022C0F79,0x01E70F82,0x01A20F8A,0x015C0F90,0x01170F96,
 0x00D10F9A,0x008B0F9D,0x00450F9F,0x00000FA0,
};


/*******************************************************************************************************************//**
 *  @brief       Perform transfer led blink operation
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void dma_transfer_sine_cosine_operation(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;    // Variable to help handle error codes from functions

    /* Set the Source and Destination Addresses for DMAC g_transfer_led_blink .
     * NOTE: If the source and destination addresses are not set prior to
     * opening the transfer interface, then the open will fail.
     */
    set_transfer_dst_src_address(&g_transfer_sine_wave_cfg, (void *)g_source_sine_cosine_data_code_flah , (void *) g_dest_sine_cosine_data);

    /* Open and enable dmac g_transfer_led_blink  */
    fsp_err = dmac_transfer_init( &g_transfer_sine_wave_ctrl, &g_transfer_sine_wave_cfg, SINE_WAVE);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        /* Necessary cleanup is already taken care in init function hence no cleanup is required here*/
        APP_ERR_PRINT("** DMAC TRANSFER_LED_BLINK INIT failed ** \r\n");
        APP_ERR_TRAP(fsp_err);
    }

    /* Open and start AGT timer */
    fsp_err = agt_timer_init();
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        APP_ERR_PRINT("** AGT Timer INIT failed ** \r\n");
        dmac_transfer_deinit(&g_transfer_sine_wave_ctrl, SINE_WAVE);
        APP_ERR_TRAP(fsp_err);
    }
    /* Display information about g_transfer_led_blink  transfer on RTT display and view on-board LED */
    APP_PRINT("\r\nDMAC dma_transfer_sine_cosine_operation in progress.  \n");

    /* Wait for g_transfer_led_blink to complete.
     * This boolean flag is set in transfer_agt_timer_callback
     */
    while(false == b_is_transfer_complete)
    {
        ;
    }
    /* Reset the flag */
    b_is_transfer_complete = false;
    APP_PRINT("\r\nDMAC dma_transfer_sine_cosine_operation transfer completed.\n");
}


/*******************************************************************************************************************//**
 *  @brief      transfer_agt_timer_callback function
 *  @param[in]  callback arguments
 **********************************************************************************************************************/
void transfer_complete_callback (dmac_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    if(true==invalidate)
    {
        flush_s_cache();
    }
    /* Set boolean flag to print data */
    b_is_transfer_complete  = true;
}



/*******************************************************************************************************************//**
 * @} (end addtogroup dmac_transfers)
 **********************************************************************************************************************/
