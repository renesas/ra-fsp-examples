/***********************************************************************************************************************
 * File Name    : usb_pcdc_comms.h
 * Description  : Contains data structures and macros used in usb_pcdc_comms.c
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_PCDC_COMMS_H_
#define USB_PCDC_COMMS_H_

#include "common_utils.h"

/* Macros for USB PCDC Communication */
#define CMD_ACK             (1U)
#define CMD_NAK             (0U)
#define RESP_CMD_SIZE       (1U)
#define DATA_LEN_SIZE       (2U)

/* Public functions declarations */
fsp_err_t comms_read(uint8_t * p_dest, uint32_t len);
fsp_err_t comms_recv_ack(void);
fsp_err_t comms_send_data(const uint8_t *p_data, size_t len);
fsp_err_t comms_recv_data (uint8_t *p_out_buf, size_t len);

#endif /* USB_PCDC_COMMS_H_ */
