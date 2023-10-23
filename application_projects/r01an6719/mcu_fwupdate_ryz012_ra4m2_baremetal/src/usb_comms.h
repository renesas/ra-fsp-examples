/*
 * usb_comms.h
 *
 *  Created on: Jun 27, 2022
 *      Author: a5134013
 */

#ifndef USB_COMMS_H_
#define USB_COMMS_H_

#include "hal_data.h"

typedef enum e_enable_disable
{
    DISABLE,
    RE_ENABLE
}enable_disable_t;

#define LINE_CODING_LENGTH          (0x07U)
#define READ_BUF_SIZE               (8U)

fsp_err_t comms_send(uint8_t * p_src, uint32_t len);
fsp_err_t comms_read(uint8_t * p_dest, uint32_t * len, uint32_t timeout_milliseconds);

void ThreadsAndInterrupts(enable_disable_t EnableDisable);

#endif /* USB_COMMS_H_ */
