/*
 * FreeRTOS Kernel V10.3.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 */

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for RA MCUs
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_api.h"

#ifndef configSYSTICK_CLOCK_HZ
 #define configSYSTICK_CLOCK_HZ      configCPU_CLOCK_HZ

/* Ensure the SysTick is clocked at the same frequency as the core. */
 #define portNVIC_SYSTICK_CLK_BIT    (SysTick_CTRL_CLKSOURCE_Msk)
#else

/* The way the SysTick is clocked is not modified in case it is not the same
 * as the core. */
 #define portNVIC_SYSTICK_CLK_BIT    (0)
#endif

/* configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to 0.
 * See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html */
#if 0U == configMAX_SYSCALL_INTERRUPT_PRIORITY
 #error "configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to 0"
#endif

/* Constants required to check the validity of an interrupt priority. */
#define portFIRST_USER_INTERRUPT_NUMBER    (16)
#define portTOP_BIT_OF_BYTE                ((uint8_t) 0x80)
#define portMAX_PRIGROUP_BITS              ((uint8_t) 7)
#define portPRIORITY_GROUP_MASK            (0x07UL << 8UL)
#define portPRIGROUP_SHIFT                 (8UL)

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR                   (0x01000000)
#define portINITIAL_EXC_RETURN             (0xfffffffd)

/* The systick is a 24-bit counter. */
#define portMAX_24_BIT_NUMBER              (0xffffffUL)

/* For strict compliance with the Cortex-M spec the task start address should
 * have bit-0 clear, as it is loaded into the PC on exit from an ISR. */
#define portSTART_ADDRESS_MASK             ((StackType_t) 0xfffffffeUL)

/* Let the user override the pre-loading of the initial LR with the address of
 * prvTaskExitError() in case it messes up unwinding of the stack in the
 * debugger. */
#ifdef configTASK_RETURN_ADDRESS
 #define portTASK_RETURN_ADDRESS           configTASK_RETURN_ADDRESS
#else
 #define portTASK_RETURN_ADDRESS           prvTaskExitError
#endif

/* Used to convert an evaluated macro into a string. */
#define RM_FREERTOS_PORT_STRINGIFY_EXPANDED(s)    RM_FREERTOS_PORT_STRINGIFY(s)

/* Used to convert text into a string. */
#define RM_FREERTOS_PORT_STRINGIFY(s)             #s

/* The maximum allowed end address of the process stack monitor. */
#define RM_FREERTOS_PORT_MAX_SPMON_EA                  (0x200FFFFFU)

/* Value to write to OAD register of MPU stack monitor to enable NMI when a stack overflow is detected. */
#define RM_FREERTOS_PORT_SPMON_NMI_ON_DETECTION        (0xA500U)

#define RM_FREERTOS_PORT_UNLOCK_LPM_REGISTER_ACCESS    (0xA502U)
#define RM_FREERTOS_PORT_LOCK_LPM_REGISTER_ACCESS      (0xA500U)

#if defined(__GNUC__) && !defined(__ARMCC_VERSION) && !defined(__clang__)
 #define RM_FREERTOS_PORT_NAKED_FUNCTION               __attribute__((naked, no_instrument_function, \
                                                                      no_profile_instrument_function))
#else
 #define RM_FREERTOS_PORT_NAKED_FUNCTION               __attribute__((naked))
#endif

/*-----------------------------------------------------------*/

extern volatile uint32_t pxCurrentTCB;

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt(void);

/*
 * Exception handlers.
 */
void PendSV_Handler(void) RM_FREERTOS_PORT_NAKED_FUNCTION;
void SysTick_Handler(void);
void SVC_Handler(void) RM_FREERTOS_PORT_NAKED_FUNCTION;

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
static void prvPortStartFirstTask(void);

#ifndef configTASK_RETURN_ADDRESS

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError(void);

#endif

/*
 * Subroutines used by the RA port. See comment header above implementation
 * for details. These functions cannot be static because they are called
 * from inline assembly.
 */
void rm_freertos_port_restore_task_stackless(void) RM_FREERTOS_PORT_NAKED_FUNCTION;
void rm_freertos_port_stack_monitor_cfg(uint32_t psp);
void rm_freertos_port_sleep_preserving_lpm(uint32_t xExpectedIdleTime);

/*-----------------------------------------------------------*/

/* Each task maintains its own interrupt status in the critical nesting
 * variable. */
static UBaseType_t uxCriticalNesting = 0U;

/* The mask level is saved before disabling interrupts so it can be restored
 * when the critical section exits. */
static uint32_t g_mask_level_before_disable = 0U;

/* This stores the reload value calculated for the SysTick when tickless idle
 * is used. */
#if (configUSE_TICKLESS_IDLE == 1)
static uint32_t g_reset_systick = 0U;
#endif

/*
 * The number of SysTick increments that make up one tick period.
 */
#if (configUSE_TICKLESS_IDLE == 1)
static uint32_t ulTimerCountsForOneTick = 0;
#endif                                 /* configUSE_TICKLESS_IDLE */

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
#if (configUSE_TICKLESS_IDLE == 1)
static uint32_t xMaximumPossibleSuppressedTicks = 0;
#endif                                 /* configUSE_TICKLESS_IDLE */

/*
 * Used by the portASSERT_IF_INTERRUPT_PRIORITY_INVALID() macro to ensure
 * FreeRTOS API functions are not called from interrupts that have been assigned
 * a priority above configMAX_SYSCALL_INTERRUPT_PRIORITY.
 */
#if (configASSERT_DEFINED == 1)
static uint8_t ucMaxSysCallPriority = 0;
 #ifdef __ARM_ARCH_7EM__
static uint32_t ulMaxPRIGROUPValue = 0;
 #endif
#endif                                 /* configASSERT_DEFINED */

/*-----------------------------------------------------------*/

/***********************************************************************************************************************
 * This interrupt is called to start the first task.
 **********************************************************************************************************************/
void SVC_Handler (void)
{
    /* This is a naked/stackless function. Do not pass arguments to the inline assembly when the GCC compiler is
     * used. */
    __asm volatile (
#if defined(__GNUC__)
        ".syntax unified                         \n"
#endif

        /* Restore the first task from the saved state created during pxPortInitialiseStack(). */
        "B       rm_freertos_port_restore_task_stackless  \n"
        );
}

/***********************************************************************************************************************
 * Resets the FPU lazy stacking, configures the hardware stack monitor, enables interrupts, and requests an SVC
 * interrupt. The first task is started in the SVC interrupt.
 **********************************************************************************************************************/
static void prvPortStartFirstTask (void)
{
    /* Start the first task.  This also clears the bit that indicates the FPU is in use in case the FPU was used before
     * the scheduler was started - which would otherwise result in the unnecessary leaving of space in the main stack
     * for saving FPU registers. */

#if RM_FREERTOS_PORT_CFG_HW_STACK_MONITOR_ENABLE

    /* Configure the PSP stack monitor.  */

    /* Disable PSP monitoring  */
    R_MPU_SPMON->SP[1].CTL = 0;

    /* Setup NMI interrupt  */
    R_MPU_SPMON->SP[1].OAD = RM_FREERTOS_PORT_SPMON_NMI_ON_DETECTION;

    /* Set SPEEN bit to enable NMI on stack monitor exception. NMIER bits cannot be cleared after reset, so no need
     * to read-modify-write. */
    R_ICU->NMIER = R_ICU_NMIER_SPEEN_Msk;

 #if !configRECORD_STACK_HIGH_ADDRESS

    /* If the end address is not stored in the TCB, the PSP stack monitor end address is set to the maximum value.
     * A stack underflow cannot be detected by the hardware stack monitor in this case. */
    R_MPU_SPMON->SP[1].EA = RM_FREERTOS_PORT_MAX_SPMON_EA;
 #endif
#endif

    /** Enable interrupts */

    /* CPSIE that performs interrupt enabling is not guaranteed to be self synchronizing, with the architecture
     * guaranteeing the recognition of the change only following an ISB. The same is true for MSR to CONTROL for
     * stack-pointer selection and privilege level.
     * Source: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHHFHJB.html */
    __enable_irq();
    __ISB();

    __asm volatile (

#if __FPU_USED

        /* Reset lazy stacking if the MCU has an FPU by clearing the FPCA bit in CONTROL. */

        /* The ISB instruction flushes the pipeline in Cortex-M processors and ensures effects of all context altering
         * operations prior to the ISB are recognized by subsequent operations. It should be used after the CONTROL
         * register is updated. Source:
         * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/index.html&_ga=2.198390767.1711886109.1566228944-1408114765.1566228570 */
        "MRS    R12, CONTROL                     \n"
        "BIC    R12, R12, #4                     \n"
        "MSR    CONTROL, R12                     \n"
        "ISB                                     \n"
#endif

        /* Supervisor call to start first task. This pends the SVC_Handler. */
        "SVC    0                                \n"
        );

    /* FreeRTOS never exits, so this should be unreachable.  */
    while (1)
    {
        __BKPT(0);
    }
}

/***********************************************************************************************************************
 * This interrupt is called to execute a context switch.
 **********************************************************************************************************************/
void PendSV_Handler (void)
{
    /* This function is assembly only to meet the requirements for naked/stackless functions. For GCC, only basic
     * assembly is allowed. */

    /* Before entry into this exception handler, the hardware stacks XPSR, PC, LR, R12, R3, R2, R1, and R0 onto the
     * stack (typically the process stack of the executing task). When the FPU is in use, the FPSCR and S0-S15
     * registers are also stored on the stack. All other registers are stored by software in this function. */

    /* Only R0-R3 and R12 can be used before stack preservation is complete. */

    /* R0-R3 and R12 are not guaranteed to retain their values after C functions are called. The main stack is used
     * when C functions are called. */

    __asm volatile (
#if defined(__GNUC__)
        ".syntax unified                         \n"
#endif

        /* Store the address of pxCurrentTCB in R0. */
        /* R0 = &pxCurrentTCB */
        /* R1 = pxCurrentTCB */
        "LDR     R0, =pxCurrentTCB               \n"
        "LDR     R1, [R0]                        \n"

        /* Store PSP in R2 so it can be used with STM instructions.  */
        /* R2 = PSP */
        "MRS     R2,  PSP                        \n"
#if __FPU_USED

        /* Check to see if the current thread is using the FPU. If so, stack S16-S31. */
        "TST     LR, #0x10                       \n"
        "BNE     _skip_vfp_save                  \n"
        "VSTMDB  R2!,{S16-S31}                   \n"
        "_skip_vfp_save:                         \n"

        /* Stack R4-R11 on the process stack. Also stack LR since the FPU is supported. */
        "STMDB   R2!, {R4-R11, LR}               \n"

        /* Save the new top of the stack into the first member of the TCB. */
        "STR R2, [R1]                            \n"

 #if RM_FREERTOS_PORT_CFG_HW_STACK_MONITOR_ENABLE

        /* If the stack monitor is used, also update PSP  to the new top of the stack. If a stack overflow has
         * occurred during this context switch, it is detected by the hardware stack monitor here. */
        "MSR     PSP, R2                         \n"
 #endif
#else                                  /* __FPU_USED */

        /* Currently, all RA MCUs that do not have an FPU also do not support the DB (decrement before) instruction suffix
         * of STM. Because the register pointing to the top of the stack is incremented after each store in STMIA, we
         * must allocate space prior to saving registers. */

        /* Allocate 32 bytes of stack space to stack R4-R11. */
        "SUBS    R2, R2, #32                     \n"

        /* Save the new top of the stack into the first member of the TCB. */
        "STR R2, [R1]                            \n"
 #if RM_FREERTOS_PORT_CFG_HW_STACK_MONITOR_ENABLE

        /* If the stack monitor is used, also update PSP  to the new top of the stack. If a stack overflow has
         * occurred during this context switch, it is detected by the hardware stack monitor here. */
        "MSR     PSP, R2                         \n"
 #endif

        /* CM23 stack frame (higher addresses on top):
         * Task saved R2/PSP ->  R11
         *                       R10
         *                       R9
         *                       R8
         *                       R7
         *                       R6
         *                       R5
         *                       R4
         * Task return R2/PSP ->
         *//* Stack R4-R7, then move R8-R11 into R4-R7 and stack them again. */
        "STMIA   R2!, {R4-R7}                    \n"
        "MOV     R4, R8                          \n"
        "MOV     R5, R9                          \n"
        "MOV     R6, R10                         \n"
        "MOV     R7, R11                         \n"
        "STMIA   R2!, {R4-R7}                    \n"
#endif                                 /* __FPU_USED */

        /* Stack preservation is complete. */

        /* Disable interrupts that could interfere with FreeRTOS during vTaskSwitchContext(). This is done by
         * setting BASEPRI to configMAX_SYSCALL_INTERRUPT_PRIORITY on MCUs that have BASEPRI to mask interrupts
         * of equal or lower priority than configMAX_SYSCALL_INTERRUPT_PRIORITY. On MCUs that do not have BASEPRI,
         * this is done by disabling interrupts globally. *//* If BASEPRI is set in the application code we won't end up here because PendSV is the lowest priority. */
#if RM_FREERTOS_PORT_HAS_BASEPRI
        "LDR     R0, = "RM_FREERTOS_PORT_STRINGIFY_EXPANDED (configMAX_SYSCALL_INTERRUPT_PRIORITY)
        "                                        \n"
        "MSR     BASEPRI, R0                     \n"
        "DSB                                     \n"
        "ISB                                     \n"
#else                                  /* RM_FREERTOS_PORT_HAS_BASEPRI */
        "CPSID   i                               \n"
#endif /* RM_FREERTOS_PORT_HAS_BASEPRI */

        /* Call vTaskSwitchContext(). In this function, pxCurrentTCB is updated to the next task to run. */
        "BL      vTaskSwitchContext              \n"

        /* Restore interrupts. This is done by unmasking all interrupts in BASEPRI if the MCU has BASEPRI,
         * or by reenabling interrupts globally if the MCU does not have BASEPRI. */
#if RM_FREERTOS_PORT_HAS_BASEPRI
        "MOV     R0, #0                          \n"
        "MSR     BASEPRI, R0                     \n"
#else
        "CPSIE   i                               \n"
#endif

        /* Restore the task. This function does not return. */
        "B rm_freertos_port_restore_task_stackless \n"
        );
}

#if RM_FREERTOS_PORT_CFG_HW_STACK_MONITOR_ENABLE

/***********************************************************************************************************************
 * Disables the process stack monitor, configures the stack start and end address for the new task and sets the PSP for
 * the new task, then reenables the process stack monitor.
 *
 * @param[in]  psp  PSP for the new task
 **********************************************************************************************************************/
void rm_freertos_port_stack_monitor_cfg (uint32_t psp)
{
    /* Setup the Hardware Stack Monitor for next task.  */

    /* Disable PSP monitoring  */
    R_MPU_SPMON->SP[1].CTL = 0;

    /* Setup start address  */

    /* Bits 0 and 1 of pxCurrentTCB->pxStack are always 0 since the stack must be aligned on an 8 byte boundary. */
    StaticTask_t * p_current_task = (StaticTask_t *) pxCurrentTCB;
 #define pxStack    pxDummy6
    R_MPU_SPMON->SP[1].SA = (uint32_t) p_current_task->pxStack;
 #undef pxStack

 #if configRECORD_STACK_HIGH_ADDRESS

    /* Setup end address if it is available. If it is not stored in the TCB, the end address was configured
     * earlier in prvPortStartFirstTask(). */

    /* Bits 0 and 1 of pxCurrentTCB->pxEndOfStack are always 0 since the stack must be aligned on an 8 byte
     * boundary and a multiple of 8 bytes. Or with 3 to ensure they are set. */
  #define pxEndOfStack    pxDummy8
    R_MPU_SPMON->SP[1].EA = (uint32_t) p_current_task->pxEndOfStack | 3U;
  #undef pxEndOfStack
 #endif

    /* Set the new PSP. */
    __set_PSP(psp);

    /* Enable PSP monitoring after configuring the start and end addresses and the PSP for the new thread. */
    R_MPU_SPMON->SP[1].CTL = 1;
}

#endif

/***********************************************************************************************************************
 * Subroutine to restore a task.
 *
 * @note This function does not return to the calling function. It returns to the task being restored. Branch to this
 * function without link (B instruction, not BL).
 **********************************************************************************************************************/
void rm_freertos_port_restore_task_stackless (void)
{
    /* This is a naked/stackless function. Do not pass arguments to the inline assembly when the GCC compiler is
     * used. */

    __asm volatile (
#if defined(__GNUC__)
        ".syntax unified                         \n"
#endif

        /* Restore the thread context and PSP.  */

        /* The first item in pxCurrentTCB is the task top of stack. */
        /* R0 = &pxCurrentTCB */
        /* R1 = pxCurrentTCB */
        /* R2 = pxCurrentTCB[0] = pxCurrentTCB->pxStack */
        "LDR     R0, =pxCurrentTCB               \n"
        "LDR     R1, [R0]                        \n"
        "LDR     R2, [R1]                        \n"

#if __FPU_USED

        /* Restore R4-R11 and LR from the process stack. */
        "LDMIA   R2!, {R4-R11, LR}               \n"

        /* Check to see if the thread being restored is using the FPU. If so, restore S16-S31. */
        "TST     LR, #0x10                       \n"
        "BNE     _skip_vfp_restore               \n"
        "VLDMIA  R2!, {S16-S31}                  \n"
        "_skip_vfp_restore:                      \n"

        /* Set the new thread's stack top in PSP unless the stack monitor is used. If the stack monitor is used, save
         * the new PSP in R0 and it will be set in rm_freertos_port_stack_monitor_cfg(). */
 #if RM_FREERTOS_PORT_CFG_HW_STACK_MONITOR_ENABLE
        "MOV     R0, R2                          \n"
 #else
        "MSR     PSP, R2                         \n"
 #endif
#else                                  /* __FPU_USED */

        /* Move R2 up past where R4-R7 are stored to where R8-R11 are stored and pop them to R4-R7, then move them to
         * R8-R11. */
        "ADDS    R2, R2, #16                     \n"
        "LDMIA   R2!, {R4-R7}                    \n"
        "MOV     R8, R4                          \n"
        "MOV     R9, R5                          \n"
        "MOV     R10, R6                         \n"
        "MOV     R11, R7                         \n"

        /* Set the new thread's stack top in PSP unless the stack monitor is used. If the stack monitor is used, save
         * the new PSP in R0 and it will be set in rm_freertos_port_stack_monitor_cfg(). */
 #if RM_FREERTOS_PORT_CFG_HW_STACK_MONITOR_ENABLE
        "MOV     R0, R2                          \n"
 #else
        "MSR     PSP, R2                         \n"
 #endif

        /* Move R2 back to where R4-R7 are stored and restore R4-R7. */
        "SUBS    R2, R2, #32                     \n"
        "LDMIA   R2!, {R4-R7}                    \n"
#endif                                 /* __FPU_USED */

#if RM_FREERTOS_PORT_CFG_HW_STACK_MONITOR_ENABLE

        /* If the hardware stack monitor is used, configure the process stack monitor and update PSP while the
         * process stack monitor is disabled. The new PSP value is in R0 from earlier calculations, and R0 contains
         * the first argument to a function. */

        /* Save and restore LR if the MCU has an FPU since LR is clobbered by the BL instruction and we need LR
         * to return to the thread. */
 #if __FPU_USED
        "PUSH    {LR}                            \n"
 #endif
        "BL      rm_freertos_port_stack_monitor_cfg \n"
 #if __FPU_USED
        "POP     {LR}                            \n"
 #endif
#endif

#if __FPU_USED

        /** Return to the thread using the process stack and saved FPU setting. */
        "BX     LR                               \n"
#else

        /* Set (EXC_RETURN) to 0xFFFFFFFD. Return to Thread mode, exception return uses non-floating-point state
         * from the PSP and execution uses PSP after return. */

        /* Using the LDR = syntax may use another register (R0-R3) as a scratch register.  This is alright because we
         * are done with all other registers. */
        "LDR    R1, =0xFFFFFFFD                  \n"
        "BX     R1                               \n"
#endif
        );
}

#if configUSE_TICKLESS_IDLE

/***********************************************************************************************************************
 * Subroutine to reset the SysTick. It first sets the SysTick to the tick fraction remaining, then restores the load
 * value to the tick frequency. It also increments the system time to account for ticks elapsed during tickless idle.
 *
 * @param[in]  tick_fraction_remaining  Number of counts remaining in the current tick.
 * @param[in]  completed_ticks          Number of ticks completed during tickless idle.
 **********************************************************************************************************************/
static void rm_freertos_port_reset_systick (uint32_t tick_fraction_remaining, uint32_t completed_ticks)
{
    /* Reload the SysTick to the fraction remaining of the current tick. Setting VAL to 0 resets the LOAD value without
     * pending a SysTick interrupt. */
    SysTick->LOAD = tick_fraction_remaining;
    SysTick->VAL  = 0U;

    /* Set the reload value back to the default reload value. */
    SysTick->LOAD = ulTimerCountsForOneTick - 1UL;

    /* Increment the system time to account for ticks elapsed during tickless idle. */
    vTaskStepTick(completed_ticks);

    /* Clear variable requesting SysTick reset. */
    g_reset_systick = 0U;
}

#endif

/***********************************************************************************************************************
 * Increments system time.
 **********************************************************************************************************************/
void SysTick_Handler (void)
{
#if configUSE_TICKLESS_IDLE

    /* Reset the SysTick reload value if it was reconfigured for a long sleep in tickless idle. */
    if (g_reset_systick)
    {
        /* Subtract one because we are in the SysTick_Handler already and one will be added in xTaskIncrementTick. */
        uint32_t completed_ticks         = (g_reset_systick / ulTimerCountsForOneTick) - 1U;
        uint32_t tick_fraction_remaining = g_reset_systick - SysTick->VAL;
        rm_freertos_port_reset_systick(tick_fraction_remaining, completed_ticks);
    }
#endif

    /* Increment the RTOS tick. */
    if (xTaskIncrementTick() != pdFALSE)
    {
        /* A context switch is required.  Context switching is performed in
         * the PendSV interrupt.  Pend the PendSV interrupt. */
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    }
}

/***********************************************************************************************************************
 * Setup the stack of a new task so it is ready to be placed under the scheduler control.  The registers have to be
 * placed on the stack in the order that the port expects to find them.
 *
 * @param[in]  pxTopOfStack  Pointer to top of this task's stack
 * @param[in]  pxCode        Task function, stored as initial PC for the task
 * @param[in]  pvParameters  Parameters for task
 **********************************************************************************************************************/
StackType_t * pxPortInitialiseStack (StackType_t * pxTopOfStack, TaskFunction_t pxCode, void * pvParameters)
{
    /* Simulate the stack frame as it would be created by a context switch
     * interrupt. */

    /* Offset added to account for the way the MCU uses the stack on entry/exit
     * of interrupts, and to ensure alignment. */
    pxTopOfStack--;

    *pxTopOfStack = portINITIAL_XPSR;                                /* xPSR */
    pxTopOfStack--;
    *pxTopOfStack = ((StackType_t) pxCode) & portSTART_ADDRESS_MASK; /* PC */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portTASK_RETURN_ADDRESS;           /* LR */

    /* Save code space by skipping register initialization. */
    pxTopOfStack -= 5;                                               /* R12, R3, R2 and R1. */
    *pxTopOfStack = (StackType_t) pvParameters;                      /* R0 */

#if __FPU_USED

    /* A save method is being used that requires each task to maintain its
     * own exec return value. */
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_EXC_RETURN;
#endif

    pxTopOfStack -= 8;                 /* R11, R10, R9, R8, R7, R6, R5 and R4. */

    return pxTopOfStack;
}

#ifndef configTASK_RETURN_ADDRESS

/***********************************************************************************************************************
 * Execution only ends up here if a task exits without deleting itself and configTASK_RETURN_ADDRESS is not defined.
 **********************************************************************************************************************/
static void prvTaskExitError (void)
{
    /* A function that implements a task must not exit or attempt to return to
     * its caller as there is nothing to return to.  If a task wants to exit it
     * should instead call vTaskDelete( NULL ).
     *
     * Artificially force an assert() to be triggered if configASSERT() is
     * defined, then stop here so application writers can catch the error. */
    BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);

    while (1)
    {
        /* This file calls prvTaskExitError() after the scheduler has been
         * started to remove a compiler warning about the function being defined
         * but never called.  ulDummy is used purely to quieten other warnings
         * about code appearing after this function is called - making ulDummy
         * volatile makes the compiler think the function could return and
         * therefore not output an 'unreachable code' warning for code that appears
         * after it. */
    }
}

#endif

/***********************************************************************************************************************
 * Setup the hardware for the scheduler to take control. Configures the SysTick frequency and starts the first task.
 **********************************************************************************************************************/
BaseType_t xPortStartScheduler (void)
{
#if (configASSERT_DEFINED == 1)
 #ifdef __ARM_ARCH_7EM__               // CM4
    {
  #ifdef __ARM_ARCH_7EM__              // CM4
        volatile uint8_t         ulOriginalPriority;
        volatile uint8_t * const pucFirstUserPriorityRegister = &NVIC->IP[0];
  #else
        volatile uint32_t         ulOriginalPriority;
        volatile uint32_t * const pucFirstUserPriorityRegister = &NVIC->IPR[0];
  #endif
        volatile uint32_t ucMaxPriorityValue;

        /* Determine the maximum priority from which ISR safe FreeRTOS API
         * functions can be called.  ISR safe functions are those that end in
         * "FromISR".  FreeRTOS maintains separate thread and ISR API functions to
         * ensure interrupt entry is as fast and simple as possible.
         *
         * Save the interrupt priority value that is about to be clobbered. */
        ulOriginalPriority = *pucFirstUserPriorityRegister;

        /* Determine the number of priority bits available.  First write to all
         * possible bits. */
        *pucFirstUserPriorityRegister = UINT8_MAX;

        /* Read the value back to see how many bits stuck. */
        ucMaxPriorityValue = *pucFirstUserPriorityRegister;

        /* Use the same mask on the maximum system call priority. */
        ucMaxSysCallPriority = configMAX_SYSCALL_INTERRUPT_PRIORITY & ucMaxPriorityValue;

        /* Calculate the maximum acceptable priority group value for the number
         * of bits read back. */
        ulMaxPRIGROUPValue = SCB_AIRCR_PRIGROUP_Msk >> SCB_AIRCR_PRIGROUP_Pos;
        while ((ucMaxPriorityValue & portTOP_BIT_OF_BYTE) == portTOP_BIT_OF_BYTE)
        {
            ulMaxPRIGROUPValue--;
            ucMaxPriorityValue <<= 1U;
        }

  #ifdef __NVIC_PRIO_BITS
        {
            /* Check the CMSIS configuration that defines the number of
             * priority bits matches the number of priority bits actually queried
             * from the hardware. */
            configASSERT((portMAX_PRIGROUP_BITS - ulMaxPRIGROUPValue) == __NVIC_PRIO_BITS);
        }
  #endif

  #ifdef configPRIO_BITS
        {
            /* Check the FreeRTOS configuration that defines the number of
             * priority bits matches the number of priority bits actually queried
             * from the hardware. */
            configASSERT((portMAX_PRIGROUP_BITS - ulMaxPRIGROUPValue) == configPRIO_BITS);
        }
  #endif

        /* Shift the priority group value back to its position within the AIRCR
         * register. */
        ulMaxPRIGROUPValue <<= portPRIGROUP_SHIFT;
        ulMaxPRIGROUPValue  &= portPRIORITY_GROUP_MASK;

        /* Restore the clobbered interrupt priority register to its original
         * value. */
        *pucFirstUserPriorityRegister = ulOriginalPriority;
    }
 #endif
#endif                                 /* conifgASSERT_DEFINED */

    /* Make PendSV the lowest priority interrupt. */
    NVIC_SetPriority(PendSV_IRQn, UINT8_MAX);

    /* Start the timer that generates the tick ISR.  Interrupts are disabled
     * here already. */
    vPortSetupTimerInterrupt();

    /* VFP is enabled in SystemInit(), so no need to enable it here. */

#if __FPU_USED

    /* Lazy save always. */
    FPU->FPCCR |= FPU_FPCCR_ASPEN_Msk | FPU_FPCCR_LSPEN_Msk;
#endif

    /* Start the first task. */
    prvPortStartFirstTask();

    /* Should never get here as the tasks will now be executing!  Call the task
     * exit error function to prevent compiler warnings about a static function
     * not being called in the case that the application writer overrides this
     * functionality by defining configTASK_RETURN_ADDRESS.  Call
     * vTaskSwitchContext() so link time optimisation does not remove the
     * symbol. */
    vTaskSwitchContext();              // TODO keep

    /* Should not get here! */
    return 0;
}

/***********************************************************************************************************************
 * Execution should never get here.
 **********************************************************************************************************************/
void vPortEndScheduler (void)
{
    /* Not implemented in ports where there is nothing to return to.
     * Artificially force an assert. */
    BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}

/***********************************************************************************************************************
 * Enter a critical section.
 **********************************************************************************************************************/
void vPortEnterCritical (void)
{
    uint32_t old_mask_level = portSET_INTERRUPT_MASK();
    uxCriticalNesting++;

    /* This is not the interrupt safe version of the enter critical function so
     * assert() if it is being called from an interrupt context.  Only API
     * functions that end in "FromISR" can be used in an interrupt.  Only assert if
     * the critical nesting count is 1 to protect against recursive calls if the
     * assert function also uses a critical section. */
    if (uxCriticalNesting == 1)
    {
        g_mask_level_before_disable = old_mask_level;
        configASSERT((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) == 0);
    }
}

/***********************************************************************************************************************
 * Exit a critical section.
 **********************************************************************************************************************/
void vPortExitCritical (void)
{
    configASSERT(uxCriticalNesting);
    uxCriticalNesting--;
    if (uxCriticalNesting == 0)
    {
        portCLEAR_INTERRUPT_MASK(g_mask_level_before_disable);
    }
}

/*-----------------------------------------------------------*/

#if configUSE_IDLE_HOOK || configUSE_TICKLESS_IDLE

/***********************************************************************************************************************
 * Saves the LPM state, then enters sleep mode. After waking, reenables interrupts briefly to allow any pending
 * interrupts to run.
 *
 * @pre Disable interrupts an suspend all tasks before calling this function.
 *
 * @param[in] xExpectedIdleTime Expected idle time in ticks
 **********************************************************************************************************************/
void rm_freertos_port_sleep_preserving_lpm (uint32_t xExpectedIdleTime)
{
    uint32_t saved_sbycr = 0U;

    /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
     * set its parameter to 0 to indicate that its implementation contains
     * its own wait for interrupt or wait for event instruction, and so wfi
     * should not be executed again. The original expected idle
     * time variable must remain unmodified, so this is done in a subroutine. */
    configPRE_SLEEP_PROCESSING(xExpectedIdleTime);
    if (xExpectedIdleTime > 0)
    {
        /* Save LPM Mode */
        saved_sbycr = R_SYSTEM->SBYCR;

        /** Check if the LPM peripheral is set to go to Software Standby mode with WFI instruction.
         *  If so, change the LPM peripheral to go to Sleep mode. */
        if (R_SYSTEM_SBYCR_SSBY_Msk & saved_sbycr)
        {
            /* Save register protect value */
            uint32_t saved_prcr = R_SYSTEM->PRCR;

            /* Unlock LPM peripheral registers */
            R_SYSTEM->PRCR = RM_FREERTOS_PORT_UNLOCK_LPM_REGISTER_ACCESS;

            /* Clear to set to sleep low power mode (not standby or deep standby) */
            R_SYSTEM->SBYCR = 0U;

            /* Restore register lock */
            R_SYSTEM->PRCR = (uint16_t) (RM_FREERTOS_PORT_LOCK_LPM_REGISTER_ACCESS | saved_prcr);
        }

        /**
         * DSB should be last instruction executed before WFI
         * infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHICBGB.html
         */
        __DSB();

        /* If there is a pending interrupt (wake up condition for WFI is true), the MCU does not enter low power mode:
         * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/BABHHGEB.html
         * Note that interrupt will bring the CPU out of the low power mode.  After exiting from low power mode,
         * interrupt will be re-enabled. */
        __WFI();

        /* Instruction Synchronization Barrier. */
        __ISB();

        /* Re-enable interrupts to allow the interrupt that brought the MCU
         * out of sleep mode to execute immediately. This will not cause a
         * context switch because all tasks are currently suspended. */
        __enable_irq();
        __ISB();

        /* Disable interrupts again to restore the LPM state. */
        __disable_irq();
    }

    configPOST_SLEEP_PROCESSING(xExpectedIdleTime);

    /** Check if the LPM peripheral was supposed to go to Software Standby mode with WFI instruction.
     *  If yes, restore the LPM peripheral setting. */
    if (R_SYSTEM_SBYCR_SSBY_Msk & saved_sbycr)
    {
        /* Save register protect value */
        uint32_t saved_prcr = R_SYSTEM->PRCR;

        /* Unlock LPM peripheral registers */
        R_SYSTEM->PRCR = RM_FREERTOS_PORT_UNLOCK_LPM_REGISTER_ACCESS;

        /* Restore LPM Mode */
        R_SYSTEM->SBYCR = (uint16_t) saved_sbycr;

        /* Restore register lock */
        R_SYSTEM->PRCR = (uint16_t) (RM_FREERTOS_PORT_LOCK_LPM_REGISTER_ACCESS | saved_prcr);
    }
}

#endif

#if configUSE_IDLE_HOOK

/***********************************************************************************************************************
 * Suspends tasks and sleeps, waking for each interrupt.
 *
 * @note This is a weak function. It can be overridden by an application specific implementation if desired.
 **********************************************************************************************************************/
__attribute__((weak)) void vApplicationIdleHook (void)
{
    /* Enter a critical section but don't use the taskENTER_CRITICAL() method as that will mask interrupts that should
     * exit sleep mode. This must be done before suspending tasks because a pending interrupt will prevent sleep from
     * WFI, but a task ready to run will not. If a task becomes ready to run before disabling interrupts, a context
     * switch will happen. */
    __disable_irq();

    /* Don't allow a context switch during sleep processing to ensure the LPM state is restored
     * before switching from idle to the next ready task. This is done in the idle task
     * before vPortSuppressTicksAndSleep when configUSE_TICKLESS_IDLE is used. */
    vTaskSuspendAll();

    /* Save current LPM state, then sleep. */
    rm_freertos_port_sleep_preserving_lpm(1);

    /* Exit with interrupts enabled. */
    __enable_irq();

    /* Allow context switches again. No need to yield here since the idle task yields when it loops around. */
    (void) xTaskResumeAll();
}

#endif

#if (configUSE_TICKLESS_IDLE == 1)

/***********************************************************************************************************************
 * Configures the SysTick for the maximum possible sleep given the expected idle time, then sleeps. Restores Systick
 * settings after waking.
 *
 * @note Tasks are already suspended when this function is called.
 *
 * @note This is a weak function. It can be overridden by an application specific implementation if desired.
 **********************************************************************************************************************/
__attribute__((weak)) void vPortSuppressTicksAndSleep (TickType_t xExpectedIdleTime)
{
    uint32_t ulReloadValue;
    uint32_t ulCompleteTickPeriods;
    uint32_t ulCompletedSysTickDecrements;

    /* Make sure the SysTick reload value does not overflow the counter. */
    if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks)
    {
        xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
    }

    /* Calculate the reload value required to wait xExpectedIdleTime
     * tick periods.  -1 is used because this code will execute part way
     * through one of the tick periods. */
    ulReloadValue = ulTimerCountsForOneTick * (xExpectedIdleTime - 1UL);

    /* Enter a critical section but don't use the taskENTER_CRITICAL()
     * method as that will mask interrupts that should exit sleep mode. */
    __disable_irq();

    /* Set the new reload value. */
    ulReloadValue += SysTick->VAL;
    SysTick->LOAD  = ulReloadValue;

    /* Clear the SysTick count flag and set the count value back to
     * zero. */
    SysTick->VAL = 0UL;

    /* Reset the SysTick to the default in the next SysTick interrupt. */
    g_reset_systick = ulReloadValue;

    /* If an interrupt was processed while tasks were suspended, another task may be ready to run. If so, don't
     * sleep. */
    if (eTaskConfirmSleepModeStatus() != eAbortSleep)
    {
        /* This function sleeps if no interrupt is pending, and enables
         * interrupts briefly after waking to allow any pending interrupt
         * to be serviced. */
        rm_freertos_port_sleep_preserving_lpm(xExpectedIdleTime);
    }

    /* If the SysTick has not yet counted to zero, an interrupt other than
     * the SysTick must have brought the system out of sleep mode). */
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0)
    {
        /* Something other than the tick interrupt ended the sleep.
         * Work out how long the sleep lasted rounded to complete tick
         * periods. */
        ulCompletedSysTickDecrements = ulReloadValue - SysTick->VAL;

        /* How many complete tick periods passed while the processor
         * was waiting? */
        ulCompleteTickPeriods = ulCompletedSysTickDecrements / ulTimerCountsForOneTick;

        /* The reload value is set to whatever fraction of a single tick
         * period remains. */
        uint32_t tick_fraction_remaining = ulCompletedSysTickDecrements % ulTimerCountsForOneTick;

        rm_freertos_port_reset_systick(tick_fraction_remaining, ulCompleteTickPeriods);
    }

    /* Exit with interrupts enabled. */
    __enable_irq();
}

#endif                                 /* #if configUSE_TICKLESS_IDLE */

/***********************************************************************************************************************
 * Setup the systick timer to generate the tick interrupts at the required frequency.
 **********************************************************************************************************************/
__attribute__((weak)) void vPortSetupTimerInterrupt (void)
{
    /* Calculate the constants required to configure the tick interrupt. */
#if (configUSE_TICKLESS_IDLE == 1)
    {
        ulTimerCountsForOneTick         = (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ);
        xMaximumPossibleSuppressedTicks = portMAX_24_BIT_NUMBER / ulTimerCountsForOneTick;
    }
#endif                                 /* configUSE_TICKLESS_IDLE */

    /* Configure SysTick to interrupt at the requested rate. */
    SysTick_Config(SystemCoreClock / configTICK_RATE_HZ);
}

#if (configASSERT_DEFINED == 1)

void vPortValidateInterruptPriority (void)
{
    IRQn_Type ulCurrentInterrupt;
    uint32_t  ulCurrentPriority;

    /* Obtain the number of the currently executing interrupt. */
    ulCurrentInterrupt = R_FSP_CurrentIrqGet();

    /* Is the interrupt number a user defined interrupt? */
    if (ulCurrentInterrupt >= portFIRST_USER_INTERRUPT_NUMBER)
    {
        /* Look up the interrupt's priority. */
        ulCurrentPriority = NVIC_GetPriority(ulCurrentInterrupt);

        /* The following assertion will fail if a service routine (ISR) for
         * an interrupt that has been assigned a priority above
         * configMAX_SYSCALL_INTERRUPT_PRIORITY calls an ISR safe FreeRTOS API
         * function.  ISR safe FreeRTOS API functions must *only* be called
         * from interrupts that have been assigned a priority at or below
         * configMAX_SYSCALL_INTERRUPT_PRIORITY.
         *
         * Numerically low interrupt priority numbers represent logically high
         * interrupt priorities, therefore the priority of the interrupt must
         * be set to a value equal to or numerically *higher* than
         * configMAX_SYSCALL_INTERRUPT_PRIORITY.
         *
         * Interrupts that  use the FreeRTOS API must not be left at their
         * default priority of  zero as that is the highest possible priority,
         * which is guaranteed to be above configMAX_SYSCALL_INTERRUPT_PRIORITY,
         * and  therefore also guaranteed to be invalid.
         *
         * FreeRTOS maintains separate thread and ISR API functions to ensure
         * interrupt entry is as fast and simple as possible.
         *
         * The following links provide detailed information:
         * http://www.freertos.org/RTOS-Cortex-M3-M4.html
         * http://www.freertos.org/FAQHelp.html */
        configASSERT(ulCurrentPriority >= ucMaxSysCallPriority);
    }

 #ifdef __ARM_ARCH_7EM__

    /* Priority grouping:  The interrupt controller (NVIC) allows the bits
     * that define each interrupt's priority to be split between bits that
     * define the interrupt's pre-emption priority bits and bits that define
     * the interrupt's sub-priority.  For simplicity all bits must be defined
     * to be pre-emption priority bits.  The following assertion will fail if
     * this is not the case (if some bits represent a sub-priority).
     *
     * If the application only uses CMSIS libraries for interrupt
     * configuration then the correct setting can be achieved on all Cortex-M
     * devices by calling NVIC_SetPriorityGrouping( 0 ); before starting the
     * scheduler.  Note however that some vendor specific peripheral libraries
     * assume a non-zero priority group setting, in which cases using a value
     * of zero will result in unpredictable behaviour. */
    configASSERT(NVIC_GetPriorityGrouping() <= ulMaxPRIGROUPValue);
 #endif
}

#endif                                 /* configASSERT_DEFINED */
