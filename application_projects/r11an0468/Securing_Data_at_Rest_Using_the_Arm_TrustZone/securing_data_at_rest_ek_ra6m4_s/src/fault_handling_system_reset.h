/*
 * fault_handling_system_reset.h
 */

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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
