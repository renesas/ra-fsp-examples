/***********************************************************************************************************************
 * File Name    : xmodem.h
 * Description  : Contains XModem protocol macro definitions and function definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#ifndef _XMODEM_H
#define _XMODEM_H

#define SOH					0x01
#define EOT					0x04
#define ACK					0x06
#define	NAK					0x15
#define CAN					0x18

#define XM_OK				0x00
#define XM_ADDRESS_ERROR	0x01
#define XM_COMMS_ERROR		0x02
#define XM_TIMEOUT			0x04
#define XM_PROG_FAIL		0x08

// status info
#define OK                  0
#define XM_ERROR            1
#define XM_TOUT             2

// function prototypes
unsigned char xmodem_download_and_program_flash(unsigned long flash_address);

#endif
