/*
 * fault_handling_system_reset.c
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
#include "hal_data.h"
#include "fault_handling_system_reset.h"


static volatile uint32_t stacked_r0;
static volatile uint32_t stacked_r1;
static volatile uint32_t stacked_r2;
static volatile uint32_t stacked_r3;
static volatile uint32_t stacked_r12;
static volatile uint32_t stacked_lr;
static volatile uint32_t stacked_pc;
static volatile uint32_t stacked_psr;

__NO_RETURN void PerformReset (void);

void hard_fault_handler_c (uint32_t *hardfault_args);

void HardFault_Handler();
// \brief perform a system reset to restart the application
__NO_RETURN void PerformReset (void)    {
  __DSB( ) ;
  SCB->AIRCR = ( SCB->AIRCR & ~SCB_AIRCR_VECTKEY_Msk ) |
               ( 0x05FAUL << SCB_AIRCR_VECTKEY_Pos ) | SCB_AIRCR_SYSRESETREQ_Msk ;

    // code should never reach this (however FVP Model does not reset!)
  while(1)   {
    __NOP() ;
  }
}
// \brief Hardfault handler in C.
// \param[in] hardfault_args address of stack frame
void hard_fault_handler_c (uint32_t *hardfault_args) {

  uint8_t Flags ;

  stacked_r0  = hardfault_args[0];
  stacked_r1  = hardfault_args[1];
  stacked_r2  = hardfault_args[2];
  stacked_r3  = hardfault_args[3];

  stacked_r12 = hardfault_args[4];
  stacked_lr  = hardfault_args[5];
  stacked_pc  = hardfault_args[6];
  stacked_psr = hardfault_args[7];

  /* log the incident */
  Flags = 0 ;
  if (SAU->SFSR & SAU_SFSR_INVEP_Msk)  {
    /* SecureFault, invalid Secure state entry point */

    Flags |= IS_SECURE ;
  }
  else if(SAU->SFSR & SAU_SFSR_AUVIOL_Msk) {
	  /* SecureFault, invalid Secure state entry point */

	Flags |= IS_SECURE ;
  }
  else if (SCB->CFSR & SCB_CFSR_STKOF_Msk)  {
    /* UsageFault, stack overflow */

    Flags |= IS_SECURE ;
  }
  else if (SCB->CFSR & SCB_CFSR_DIVBYZERO_Msk) {
    /* UsageFault, divide by zero when CCR.DIV_0_TRP is 1 */

    Flags |= IS_SECURE ;
  }
  /* AIRCR.BFHFNMINS  not set, so also non-secure faults end here */
  else if (SCB_NS->CFSR & SCB_CFSR_STKOF_Msk)  {
    /* UsageFault, stack overflow */
  }
  else if (SCB_NS->CFSR & SCB_CFSR_DIVBYZERO_Msk)  {
    /* UsageFault, divide by zero when CCR.DIV_0_TRP is 1 */

  }
  else
  {
	  /* add more case if needed */
  }


  PerformReset ();

// code should never reach this
  while (1)  {
    __NOP( ) ;
  }
}




void HardFault_Handler()
{
     __ASM volatile (
            "TST LR, #0x40\n"
            "BEQ from_nonsecure\n"
          "from_secure:\n"
            "TST LR, #0x04\n"
            "ITE EQ\n"
            "MRSEQ R0, MSP\n"
            "MRSNE R0, PSP\n"
            "B hard_fault_handler_c\n"
          "from_nonsecure:\n"
            "MRS R0, CONTROL_NS\n"
            "TST R0, #2\n"
            "ITE EQ\n"
            "MRSEQ R0, MSP_NS\n"
            "MRSNE R0, PSP_NS\n"
            "B hard_fault_handler_c\n"
          );
}
