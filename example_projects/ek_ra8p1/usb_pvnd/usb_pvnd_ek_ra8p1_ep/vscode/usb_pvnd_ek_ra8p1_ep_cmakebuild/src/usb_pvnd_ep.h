/***********************************************************************************************************************
 * File Name    : usb_pvnd_ep.h
 * Description  : Contains macros, variables, and functions used in usb_pvnd_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_PVND_EP_H_
#define USB_PVND_EP_H_

#include <stdbool.h>
#include "usb_peri_vendor_thread.h"

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macro for the data buffer size */
#define BUF_SIZE                    (20U)

/* Macro for the request buffer size */
#define REQ_SIZE                    (20U)

/* Macro for the data mask of one byte */
#define USB_VALUE_FF                (0xFFU)

/* Macro for the first pipe number */
#define START_PIPE                  (USB_PIPE1)

/* Macro for the end of the pipe number range */
#define END_PIPE                    (USB_PIPE9 + 1U)

/* Macros for the vendor class requests */
#define USB_SET_VENDOR_NO_DATA      (0x0000U)
#define USB_SET_VENDOR              (0x0100U)
#define USB_GET_VENDOR              (0x0200U)

/* Macro for the delay after checking the received data in milliseconds */
#define DELAY                       (10U)

/* Macro for the maximum packet size */
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define USB_APL_MXPS                (512U)      /* Max packet size for Hi-Speed */
#else
#define USB_APL_MXPS                (64U)       /* Max packet size for Full-Speed */
#endif /* Specific board */

/* External variables - Defined in usb_pvnd_ep.c */
extern volatile usb_event_info_t * gp_usb_pvnd_event;
extern volatile bool g_b_queue_err;
extern uint8_t g_buf[BUF_SIZE];
extern uint8_t g_bulk_in_pipe;
extern uint8_t g_bulk_out_pipe;
extern uint16_t g_max_packet_size;

/* Function declarations */
fsp_err_t usb_init(void);
fsp_err_t usb_pipes_get(void);
fsp_err_t usb_status_set(void);
fsp_err_t usb_data_get(void);
fsp_err_t usb_data_set(void);
fsp_err_t usb_data_check(void);
fsp_err_t usb_bulk_write(uint32_t size);
fsp_err_t usb_bulk_read(void);
fsp_err_t queue_receive(void);
void usb_deinit(void);

#endif /* USB_PVND_EP_H_ */
