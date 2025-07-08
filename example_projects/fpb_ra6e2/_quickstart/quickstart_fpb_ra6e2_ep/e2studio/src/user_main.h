/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : user_main.h
 * Version      : .
 * OS           : .
 * H/W Platform : FPB-RA6E2
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
