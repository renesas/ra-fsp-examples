/***********************************************************************************************************************
 * File Name    : transfer_initialise.h
 * Description  : Contains data structures and functions used in transfer_initialise.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TRANSFER_INITIALISE_H_
#define TRANSFER_INITIALISE_H_

#define SOURCE_DATA_SIZE            (60U)   /* Macro for transfer_led_blink transfer array size */
#define DEST_DATA_SIZE              (16U)   /* Macro for destination array size */

#define EXPECTED_CHAR_END_INDEX     (1U)    /* Buffer index for expected end of RTT input array */

/* Macros used to differentiate between transfer units during initialization and de-initialization */
#define TRANSFER_LED_BLINK          (0U)
#define TRANSFER_GPT_TIMER_VALUE    (1U)
#define TRANSFER_WRITE_IOPORT       (2U)

/* Macros for RTT input to software start DMAC transfer_gpt_value transfer */
#define START_TRANSFER_ON_LED_PORT  (1U)
#define START_TRANSFER_GPT_VALUE    (2U)
#define START_TRANSFER_TOGGLE_LED   (3U)

/* Function initializes the transfer unit module */
fsp_err_t dmac_transfer_init(dmac_instance_ctrl_t *const g_transfer_ctl,
                             transfer_cfg_t const *const g_transfer_cfg,
                             uint8_t transfer_unit);

/* Function de-initializes the transfer unit module */
void dmac_transfer_deinit(dmac_instance_ctrl_t *const p_transfer_ctl, uint8_t transfer_unit);

/* Function performs software start for DMAC transfer instance */
fsp_err_t dmac_transfer_software_start(transfer_ctrl_t * const p_transfer_ctrl);

/* Function prints the data transferred by transfer_write_ioport */
void dmac_transfer_print_data(void);

/* Function sets the source and destination address */
void set_transfer_dst_src_address(transfer_cfg_t const * const p_config,
                                  void const * volatile p_src,
                                  void const * volatile p_dest);

#endif
