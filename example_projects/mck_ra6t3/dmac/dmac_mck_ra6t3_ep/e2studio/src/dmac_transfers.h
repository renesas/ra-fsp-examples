/***********************************************************************************************************************
 * File Name    : dmac_transfers.h
 * Description  : Contains data structures and functions used in dmac_transfers.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#ifndef DMAC_TRANSFERS_H_
#define DMAC_TRANSFERS_H_

#define NEW_LINE    '\n'    // new line character
#define NULL_CHAR   '\0'    // end of string character

#define EP_INFO    "\r\nProject initializes 3 DMAC transfer instances. Upon successful initialization, a menu option is \
                     \rdisplayed. On selecting 1 from menu option, AGT generates 100ms interrupt to trigger a transfer \
                     \rfrom the source buffer to port control register for 60 times indicated by LED blinking for 60 times.\
                     \rOn selecting 2 from menu option, 32-bit data is transferred from the GPT counter register to destination\
                     \rarray which gets printed on RTTViewer. On selecting 3 from menu option, 32 bit data is transferred from\
                     \rsource array to port control register and state of LED changes accordingly.\r\n"

#define MENU        "\r\nPress 1 to transfer source array to LED Port in normal mode.\
                     \r\nPress 2 to transfer GPT current timer value.\
					 \r\nPress 3 to transfer source array to LED Port in repeat mode.\n"

/* Perform transfer led blink operation */
void transfer_led_blink_operation(void);

/* Perform transfer gpt timer operation  */
void transfer_gpt_timer_operation(void);

/* Perform transfer write ioport operation */
void transfer_write_ioport_operation(void);



#endif
