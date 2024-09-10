/***********************************************************************************************************************
 * File Name    : comms.h
 * Description  : Contains downloader communication related macro definitions and function prototypes
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMS_H_
#define COMMS_H_

#include "hal_data.h"

#define RAW_COUNT_MS    120000
#define WAIT_FOREVER    0xFFFFFFFF

#define COMMS_UART

#define NO_TIMEOUT  0   //WangJin 2021.04.12

#define MAX_PACKET_SIZE 1


/* Function prototypes */
fsp_err_t comms_open(void);
void comms_send(uint8_t * p_src, uint32_t len);
fsp_err_t comms_read(uint8_t * p_dest, uint32_t * len, uint32_t timeout_milliseconds);

#endif /* COMMS_H_ */
