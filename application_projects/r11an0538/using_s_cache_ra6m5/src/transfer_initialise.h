/***********************************************************************************************************************
 * File Name    : transfer_initialise.h
 * Description  : Contains data structures and functions used in transfer_initialise.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2020-2021] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/


#ifndef TRANSFER_INITIALISE_H_
#define TRANSFER_INITIALISE_H_

#define DATA_SIZE (180U)          /* Macros for transfer_led_blink transfer array size */
#define DEST_DATA_SIZE   (16U)          /* Macro for destination array size */

#define EXPECTED_CHAR_END_INDEX     (1u)  /* Buffer index for expected end of RTT input array */

/* Macro used to differentiate between transfer units during initialisation and deinitialisation*/
#define SINE_WAVE (0U)
#define COSINE_WAVE (1U)
#define TRANSFER_WRITE_IOPORT (2U)


/* Macro for RTT input to software start DMAC transfer_gpt_value transfer */
#define START_TRANSFER_ON_LED_PORT (1u)
#define START_TRANSFER_GPT_VALUE   (2u)
#define START_TRANSFER_TOGGLE_LED  (3u)


/* Function initialises the transfer unit module */
fsp_err_t dmac_transfer_init(dmac_instance_ctrl_t *const g_transfer_ctl,
                                transfer_cfg_t const *const g_transfer_cfg,
                                uint8_t transfer_unit);

/* Function deinitialises the transfer unit module */
void dmac_transfer_deinit(dmac_instance_ctrl_t *const p_transfer_ctl, uint8_t transfer_unit);

/* Function performs software start for dmac transfer instance */
fsp_err_t dmac_transfer_software_start(transfer_ctrl_t * const p_transfer_ctrl);


/* Function sets the source and destination address */
void set_transfer_dst_src_address(transfer_cfg_t const * const p_config,
                                    void const * volatile   p_src,
                                    void const * volatile   p_dest );

#endif
