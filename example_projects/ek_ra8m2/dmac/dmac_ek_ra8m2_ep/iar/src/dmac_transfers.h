/***********************************************************************************************************************
 * File Name    : dmac_transfers.h
 * Description  : Contains data structures and functions used in dmac_transfers.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef DMAC_TRANSFERS_H_
#define DMAC_TRANSFERS_H_

#define NEW_LINE    ('\n')    /* New line character */
#define NULL_CHAR   ('\0')    /* End of string character */

#define EP_INFO     "\r\nProject initializes 3 DMAC transfer instances. Upon successful initialization, a menu\
                     \roption is displayed. On selecting 1 from the menu option, AGT generates 100ms interrupt to\
                     \rtrigger RA transfer from the source buffer to the port control register for 60 times indicated\
                     \rby LED toggling for 60 times. On selecting 2 from the menu option, 32-bit data is transferred\
                     \rfrom the GPT counter register to the destination array which is printed on RTT Viewer. On\
                     \rselecting 3 from menu option, 32-bit data is transferred from the source array to the port\
                     \rcontrol register and the state of LED changes accordingly.\r\n"

#define MENU        "\r\nPress 1 to transfer source array to LED Port in normal mode.\
                     \r\nPress 2 to transfer the current GPT timer value.\
					 \r\nPress 3 to transfer source array to LED Port in repeat mode.\n"

/* Perform transfer LED blink operation */
void transfer_led_blink_operation(void);

/* Perform transfer GPT timer operation */
void transfer_gpt_timer_operation(void);

/* Perform transfer write IOPORT operation */
void transfer_write_ioport_operation(void);

#endif
