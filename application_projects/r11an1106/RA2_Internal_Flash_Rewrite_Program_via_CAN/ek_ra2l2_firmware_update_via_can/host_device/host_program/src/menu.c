/***********************************************************************************************************************
 * File Name    : menu.c
 * Description  : Contains menu item implementations
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include <menu.h>

void interrupt_control(enable_disable_t enable_disable);
uint8_t menu(void)
{
    unsigned char rByte[16] = {RESET_VALUE};
    volatile fsp_err_t err = FSP_SUCCESS;
    uint8_t menu_input = RESET_VALUE;   /* User input for menu options */

    FSP_PARAMETER_NOT_USED(err);

    while (1)
    {
        APP_PRINT("\r\nPlease select from below menu options:");
        APP_PRINT("\r\n1 - Display image slot info");
        APP_PRINT("\r\n2 - Send firmware to device 2");
        APP_PRINT("\r\n>");

        while ( !APP_CHECK_DATA);
        APP_READ(rByte);
        menu_input = (uint8_t) atoi((char*) rByte);
        switch (menu_input)
        {
            case FIRMWARE_INFO:
                err = display_image_slot_info();
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("ERROR: Display image slot information failed\r\n");
                }
                break;

            case FIRMWARE_TRANSFER:
                
                APP_PRINT("Checking firmware...\r\n");
                interrupt_control(DISABLE);
                err = code_flash_lp_operations();
                interrupt_control(RE_ENABLE);

                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("ERROR: There is no firmware to transfer\r\n");
                    APP_PRINT("Please prepare firmware before choosing option 2\r\n");
                    break;
                }
                APP_PRINT("Connecting to board 2 to clean secondary slot\r\n");

                err = can_send_msg(CAN_MSG_START, CAN_DESTINATION_MAILBOX_1);

                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("ERROR: Cannot connect to device 2\r\n");
                    break;
                }

                err = can_receive();

                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("ERROR: Device 2 not ready\r\n");
                    break;
                }

                APP_PRINT("Start transferring firmware...\r\n");


                err = firmware_transfer();
                switch (err)
                {
                    case FSP_SUCCESS:
                        APP_PRINT("INFO: Firmware transfer completed successfully\r\n");
                        break;

                    case FSP_ERR_TIMEOUT:
                        APP_ERR_PRINT("ERROR: Timeout during firmware transfer via CAN\r\n");
                        break;

                    case FSP_ERR_ASSERTION:
                        APP_ERR_PRINT("ERROR: Assertion failure during firmware transfer via CAN\r\n");
                        break;

                    default:
                        APP_ERR_PRINT("ERROR: Firmware transfer failed (%d)\r\n", err);
                        break;
                }
                break;

            default:
                APP_PRINT("Invalid input. Please enter 1 or 2\r\n");
                break;
        }

    }
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
