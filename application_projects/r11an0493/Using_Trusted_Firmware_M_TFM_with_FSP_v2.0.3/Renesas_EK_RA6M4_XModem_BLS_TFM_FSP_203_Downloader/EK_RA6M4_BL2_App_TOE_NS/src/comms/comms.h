/*
 * comms.h
 *
 *  Created on: 2 Oct 2019
 *      Author: b3800274
 */

#ifndef COMMS_H_
#define COMMS_H_

#include "hal_data.h"

#define RAW_COUNT_MS    120000
#define WAIT_FOREVER    0xFFFFFFFF

//#define COMMS_UART
#define COMMS_USB

#ifdef COMMS_USB
#define MAX_PACKET_SIZE 64
#elif defined COMMS_UART
#define MAX_PACKET_SIZE 1
#endif

/* Function prototypes */
fsp_err_t comms_open(void);
void comms_send(uint8_t * p_src, uint32_t len);
fsp_err_t comms_read(uint8_t * p_dest, uint32_t * len, uint32_t timeout_milliseconds);

#endif /* COMMS_H_ */
