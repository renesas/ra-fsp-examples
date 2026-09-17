/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "hal_data.h"
#include "common_utils.h"
#include "flash.h"
#include "can.h"

extern bsp_leds_t g_bsp_leds;

/* Define the units to be used with the software delay function */
const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;

/* Set the blink frequency (must be <= bsp_delay_units / 2) */
const uint32_t freq_in_hz = 1;

/* Calculate the delay in terms of bsp_delay_units */
const uint32_t delay = bsp_delay_units / (freq_in_hz * 2);
unsigned long Address = SECONDARY_IMAGE_START_ADDRESS;
uint8_t packet[128] = {RESET_VALUE};
uint8_t frame_count = 0;
uint32_t pack_count = 0;
/* Holds level to set for pins */
bsp_io_level_t pin_level = BSP_IO_LEVEL_LOW;

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

typedef enum
{
    DISABLE = 0,
    RE_ENABLE
} enable_disable_t;


#define CAN_MSG_MATCH(_msg_) (0 == memcmp((uint8_t *)&g_can_rx_frame.data[0], (uint8_t *)g_can_msg_payload[(_msg_)], CAN_FRAME_TRANSMIT_DATA_BYTES))

void interrupt_control(enable_disable_t enable_disable);
/*******************************************************************************************************************//**
 * This function is called to enable WDT counter in debug mode.
 * @brief    Enables WDT counter to count in debug mode.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void enable_wdt_count_in_debug_mode(void)
{
    /* As per hardware manual's DBGREG module,
     * section 2.6.4.2:- Clear this bit to enable WDT Reset/NMI in debug mode */
    R_DEBUG->DBGSTOPCR_b.DBGSTOP_WDT = RESET_VALUE;
}

/*******************************************************************************************************************//**
 * @brief  Blinky example application
 *
 * Blinks all leds at a rate of 1 second using the software delay function provided by the BSP.
 *
 **********************************************************************************************************************/
void hal_entry (void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO);
    APP_PRINT("\r\nSecondary Image is running with Overwrite update mode\r\n");
    APP_PRINT("\r\nThe Red, Blue and Green LEDs are blinking\r\n");

    /* Initializes the module. */
    err = R_GPT_Open(&g_timer1_ctrl, &g_timer1_cfg);
    assert(FSP_SUCCESS == err);
    err = R_GPT_Open( &g_can_timer_ctrl, &g_can_timer_cfg);
    assert(FSP_SUCCESS == err);
    /* Start the timer. */
    (void) R_GPT_Start(&g_timer1_ctrl);

    /* Open Flash_LP module */
    err = R_FLASH_LP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    assert(FSP_SUCCESS == err);

    /*Open can driver*/
    err = can_open();
    assert(FSP_SUCCESS == err);

    err = R_WDT_Open (&g_wdt_ctrl, &g_wdt_cfg);
    assert(FSP_SUCCESS == err);

    /* Enable WDT to count and generate NMI or Reset when the debugger (JLink) is connected */
    enable_wdt_count_in_debug_mode();

    APP_PRINT("\r\nTo start transfer firmware, please choose option 2 on device 1\r\n");

    while (true)
    {

        /* Check if receive flag is set */
        if (true == b_can_rx)
        {
            /*Firmware data frames (ID = CAN_DESTINATION_MAILBOX_2)*/
            if (g_can_rx_frame.id == CAN_DESTINATION_MAILBOX_2)
            {
                /* Reset flag bit */
                b_can_rx = false;
                memcpy(&packet[frame_count*8U], &g_can_rx_frame.data, CAN_FRAME_TRANSMIT_DATA_BYTES );
                if (frame_count == 15U)
                {
                    frame_count = 0U;
                    APP_PRINT("\r\n CAN completely received package %d \r\n", pack_count);
                    (void) R_WDT_Refresh(&g_wdt_ctrl);

                    interrupt_control(DISABLE);
                    err = R_FLASH_LP_Write(&g_flash0_ctrl, (uint32_t)&packet[0], Address, 128);
                    interrupt_control(RE_ENABLE);

                    if(FSP_SUCCESS == err)
                    {
                        memset(packet, RESET_VALUE, 128);
                        Address += 128;

                        /* ACK for each 128B block */
                        (void) can_send_msg(CAN_MSG_SUCCESS, CAN_DESTINATION_MAILBOX_1);
                    }
                    else
                    {
                        APP_ERR_PRINT("ERROR: Cannot write package %d \r\n", pack_count);
                        (void) can_send_msg(CAN_MSG_ERROR, CAN_DESTINATION_MAILBOX_1);

                        APP_PRINT("\r\n Resetting the system\r\n");
                        NVIC_SystemReset();
                    }
                    pack_count ++;
                }
                else
                {
                    frame_count ++;
                }
            }
            else
            {
                /* DONE : reset system */
                if (CAN_MSG_MATCH(CAN_MSG_DONE))
                {
                    b_can_rx = false;
                    APP_PRINT("\r\n Received CAN DONE message\r\n");
                    APP_PRINT("\r\n Resetting the system\r\n");

                    NVIC_SystemReset();
                }

                /* START: erase secondary slot before receiving */
                else if (CAN_MSG_MATCH(CAN_MSG_START))
                {
                    b_can_rx = false;
                    /* Stop GPT module */
                    err = R_GPT_Stop(&g_timer1_ctrl);
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT("\r\nError: GPT Stop \r\n");
                        APP_ERR_TRAP(err);
                    }

                    APP_PRINT("\r\n Received CAN START message\r\n");
                    APP_PRINT("\r\n Erase flash before receiving firmware\r\n");
                    APP_PRINT("Blank checking the secondary slot...\r\n");

                    interrupt_control(DISABLE);
                    err = code_flash_lp_operations();
                    interrupt_control(RE_ENABLE);
                    if (FSP_SUCCESS == err)
                    {

                        /* ACK to sender: ready */
                        (void) can_send_msg(CAN_MSG_SUCCESS, CAN_DESTINATION_MAILBOX_1);

                    }
                    else
                    {
                        APP_ERR_PRINT("ERROR: Erasing the secondary slot\r\n");
                        (void) can_send_msg(CAN_MSG_ERROR, CAN_DESTINATION_MAILBOX_1);
                    }

                }
                else if (CAN_MSG_MATCH(CAN_MSG_FLASH))
                {
                    b_can_rx = false;

                    /* Stop GPT module */
                    err = R_GPT_Stop(&g_timer1_ctrl);
                    assert(FSP_SUCCESS == err);

                    err = display_image_slot_info((uint32_t) PRIMARY_IMAGE_START_ADDRESS);
                    if (err == FSP_SUCCESS)
                    {
                        err = display_image_slot_info((uint32_t) SECONDARY_IMAGE_START_ADDRESS);
                    }
                    /* Stop GPT module */
                    err = R_GPT_Start(&g_timer1_ctrl);
                    assert(FSP_SUCCESS == err);
                }
            }
            /* Reset the RX frame data */
            memset((uint8_t*)&g_can_rx_frame.data[0], RESET_VALUE, CAN_FRAME_TRANSMIT_DATA_BYTES);
        }

    }
}

/*******************************************************************************************************************//**
* @brief Timer callback function.
**********************************************************************************************************************/
void operation_callback(timer_callback_args_t *p_args)
{
   FSP_PARAMETER_NOT_USED(p_args);
   /* LED type structure */
   bsp_leds_t leds = g_bsp_leds;

   /* Enable access to the PFS registers. If using r_ioport module then register protection is automatically
    * handled. This code uses BSP IO functions to show how it is used.
    */
   R_BSP_PinAccessEnable();

   /* Update all board LEDs */
   for (uint32_t i = 0; i < leds.led_count; i++)
   {
       /* Get pin to toggle */
       uint32_t pin = leds.p_leds[i];

       /* Write to this pin */
       R_BSP_PinWrite((bsp_io_port_pin_t) pin, pin_level);

   }

   /* Protect PFS registers */
   R_BSP_PinAccessDisable();

   /* Toggle level for next write */
   if (BSP_IO_LEVEL_LOW == pin_level)
   {
       pin_level = BSP_IO_LEVEL_HIGH;
   }
   else
   {
       pin_level = BSP_IO_LEVEL_LOW;
   }
   /* Delay */
   R_BSP_SoftwareDelay(delay, bsp_delay_units);
}

void interrupt_control(enable_disable_t enable_disable)
{
    static uint32_t control_reg_value;
    static uint32_t old_primask;

    if (DISABLE == enable_disable)
    {
        /** Store the interrupt state */
        old_primask = __get_PRIMASK();

        /* Disable other threads whilst flash erasing */
        /* Disable the SysTick timer */
        control_reg_value = SysTick->CTRL;
        SysTick->CTRL = 0;
        NVIC_DisableIRQ( SysTick_IRQn ); /* Disable the SysTick timer IRQ */
        NVIC_ClearPendingIRQ( SysTick_IRQn ); /* Clear any pending SysTick timer IRQ */

        __disable_irq(); /* Disable interrupts */
    }
    else
    {
        NVIC_EnableIRQ( SysTick_IRQn ); /* Enable the SysTick timer IRQ */
        SysTick->CTRL = control_reg_value; /* Restart the SysTick timer */

        /** Restore the interrupt state */
        __set_PRIMASK( old_primask ); /* Enable interrupts */
    }
}
