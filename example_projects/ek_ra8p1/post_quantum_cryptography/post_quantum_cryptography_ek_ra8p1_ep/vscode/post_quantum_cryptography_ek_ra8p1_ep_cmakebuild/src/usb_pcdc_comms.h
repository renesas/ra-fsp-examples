/***********************************************************************************************************************
 * File Name    : usb_pcdc_comms.h
 * Description  : Contains macros and functions used in usb_pcdc_comms.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_PCDC_COMMS_H_
#define USB_PCDC_COMMS_H_

#include "hal_data.h"

/* Macros for USB PCDC communication */
#define CMD_ACK             (1U)
#define RESP_CMD_SIZE       (1U)
#define DATA_LEN_SIZE       (2U)

/* Function declarations */
fsp_err_t comms_init(void);
fsp_err_t comms_read(uint8_t * p_dest, uint32_t len);
fsp_err_t comms_recv_ack(void);
fsp_err_t comms_send_data(const uint8_t * p_data, size_t len);
fsp_err_t comms_recv_data(uint8_t * p_out_buf, size_t len);
void comms_deinit(void);

#endif /* USB_PCDC_COMMS_H_ */
