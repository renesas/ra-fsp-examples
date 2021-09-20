/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : user_main.h
 * Version      : .
 * OS           : .
 * H/W Platform : FPB-RA6E1
 * Description  : This file includes the set up for the irq pins
 * Operation    : See readme.txt
 * Limitations  : .
 *********************************************************************************************************************/

#ifndef USER_MAIN_H_
#define USER_MAIN_H_


/**********************************************************************************************************************
 * Function Name: st_irq_pins_t;
 * Description  : declares structure irq_pins used to store which pins are used for irqs
 * Argument     : st_irq_pins_t
 * Return Value : .
 *********************************************************************************************************************/
typedef struct irq_pins
{
    const external_irq_instance_t * const p_irq;
} st_irq_pins_t;
/**********************************************************************************************************************
 End of function st_irq_pins_t
 *********************************************************************************************************************/



extern void user_main ();
#endif /* USER_MAIN_H_ */
