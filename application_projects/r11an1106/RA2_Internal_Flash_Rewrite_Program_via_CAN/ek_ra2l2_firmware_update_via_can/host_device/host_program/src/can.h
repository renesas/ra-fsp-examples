/***********************************************************************************************************************
 * File Name    : can.h
 * Description  : Contains can protocol related macro definitions and function prototypes
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#ifndef CAN_H_
#define CAN_H_

#include <menu.h>
#include "hal_data.h"


#define CAN_FRAME_TRANSMIT_DATA_BYTES   (8U)                /* CAN Data length */
#define CAN_MAILBOX_NUMBER_0            (0U)                /* CAN Mail box number */
#define CAN_DESTINATION_MAILBOX_1       (1U)                /* Destination mail box number */
#define CAN_DESTINATION_MAILBOX_2       (2U)                /* Destination mail box number */
#define CAN_FRAME_TRANSMIT_DATA_BYTES   (8U)                /* CAN Data length */

extern can_frame_t g_can_rx_frame;
extern volatile bool b_can_rx;
extern volatile bool b_can_err;
extern volatile bool g_timer0_event;

typedef enum
{
    CAN_MSG_START = 0,
    CAN_MSG_SUCCESS,
    CAN_MSG_FLASH,
    CAN_MSG_DONE,
    CAN_MSG_ERROR,
    CAN_MSG_MAX
} can_msg_t;

extern const uint8_t g_can_msg_payload[CAN_MSG_MAX][CAN_FRAME_TRANSMIT_DATA_BYTES];

#define WAIT_TIME                       (5000U)             /* Wait time value */

/* Function prototypes */
fsp_err_t can_open(void);
fsp_err_t can_send(uint8_t *data, uint32_t can_id);
fsp_err_t can_send_msg(can_msg_t msg, uint32_t can_id);
fsp_err_t can_deinit(void);
fsp_err_t can_receive(void);

#endif /* COMMS_H_ */
