/*
 * fault_handling_system_reset.h
 */

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

#ifndef FAULT_HANDLING_SYSTEM_RESET_H_
#define FAULT_HANDLING_SYSTEM_RESET_H_

#define IS_SECURE    (1UL << 0)    /* incident happen in secure state */

typedef enum  {
  IR_UNKNOW,  /* unknown reason                                        */
  IR_DIVBY0,  /* UsageFault, divide by zero when CCR.DIV_0_TRP is 1    */
  IR_STKOF,   /* UsageFault, stack overflow                            */
  IR_INVEP,   /* SecureFault, invalid Secure state entry point         */
  IR_WDTEXP,  /* secure systick watchdog timeout                       */
  IR_SECDAT,   /* pointer pointing to secure instead non-secure memory  */
  IR_AUVIOL
} IncidentReason_t;

#endif /* FAULT_HANDLING_SYSTEM_RESET_H_ */
