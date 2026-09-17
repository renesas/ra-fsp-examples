/***********************************************************************************************************************
 * File Name    : tas_example.c
 * Description  : Standalone TAS example orchestration and UART interaction. Calls only tas_ep_* functions,
 *                no direct FSP API calls.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "tas_example.h"
#include "tas_ep.h"
#include "console/uart_console.h"

/* Private functions declarations */
static void           tas_example_handle_error(fsp_err_t err, const char * p_msg);
static void           tas_example_print_banner(void);
static void           tas_example_print_schedule(const tas_info_t * p_info);
static tas_cycle_start_config_t tas_example_select_cycle_start(void);
static bool           tas_example_read_line(char * p_input, uint32_t input_size, uint32_t * p_input_length);
static bool           tas_example_read_main_menu_request(tas_use_case_t * p_use_case,
                                                          bool * p_cycle_start_requested);
static bool           tas_example_read_fixed_delay_seconds(uint32_t * p_seconds);
static bool           tas_example_cycle_start_is_same(const tas_cycle_start_config_t * p_left,
                                                       const tas_cycle_start_config_t * p_right);
static const char * tas_example_use_case_description(tas_use_case_t use_case);

/***********************************************************************************************************************
 *  Function Name: tas_example_handle_error
 *  Description  : Prints the failing call's message and error code, then traps execution.
 *  Arguments    : err           The FSP error code to check.
 *                 p_msg         Message identifying the failing call, printed on error.
 *  Return Value : None.
 **********************************************************************************************************************/
static void tas_example_handle_error(fsp_err_t err, const char * p_msg)
{
    if (FSP_SUCCESS != err)
    {
        printf("ERROR: %s (0x%x)\r\n", p_msg, (unsigned int) err);
        if (FSP_ERR_UNSUPPORTED == err)
        {
            printf("TAS is not enabled in the RA Configurator "
                   "(LAYER3_SWITCH_CFG_TAS_ENABLE == 0)\r\n");
        }
        __asm("BKPT #0\n");
    }
}
/***********************************************************************************************************************
* End of function tas_example_handle_error.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_example_print_banner
 *  Description  : Prints the boot banner and EP info directly from their macros.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
static void tas_example_print_banner(void)
{
    printf(BANNER_INFO, EP_VERSION,
           (unsigned int) FSP_VERSION_MAJOR,
           (unsigned int) FSP_VERSION_MINOR,
           (unsigned int) FSP_VERSION_PATCH);
    printf(EP_INFO);
    printf("\r\n");
}
/***********************************************************************************************************************
* End of function tas_example_print_banner.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_example_print_schedule
 *  Description  : Prints the programmed cycle time/start, per-port/per-queue gate entry counts, and MAC table
 *                 entry count.
 *  Arguments    : p_info        Schedule info as filled in by tas_configure_schedule()/tas_configure_mac_table().
 *  Return Value : None.
 **********************************************************************************************************************/
static void tas_example_print_schedule(const tas_info_t * p_info)
{
    printf("\r\n");
    printf("cycle_time=%u ns  timer=%u\r\n", p_info->cycle_time_ns, p_info->timer_number);
    if (TAS_CYCLE_START_MODE_FIXED == p_info->cycle_start_mode)
    {
        printf("Cycle start: fixed delay=%u s\r\n", p_info->fixed_delay_seconds);
    }
    else
    {
        printf("Cycle start: dynamic\r\n");
    }
    printf("  timer now            = %llu ns\r\n", (unsigned long long) p_info->timer_now_ns);
    printf("  starts after         = %llu ns\r\n", (unsigned long long) p_info->cycle_start_lead_ns);
    printf("  gate cycle starts at = %llu ns\r\n", (unsigned long long) p_info->cycle_start_ns);

    printf("\r\n");
    for (uint8_t i = 0U; i < p_info->port_count; i++)
    {
        uint8_t port = p_info->ports[i];
        printf("Port %u TAS enabled\r\n", port);
        for (uint8_t q = 0U; q < BSP_FEATURE_ESWM_ETHA_IPV_QUEUE_NUM; q++)
        {
            printf("  Q%u: %u entries\r\n", q, p_info->entries_per_gate[port][q]);
        }
    }

    printf("\r\n");
    printf("MAC table entries: %u\r\n", p_info->mac_entry_count);
}
/***********************************************************************************************************************
* End of function tas_example_print_schedule.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_example_select_cycle_start
 *  Description  : Selects the runtime cycle-start mode and, for fixed mode, its delay from the next timer epoch.
 *  Arguments    : None.
 *  Return Value : Validated runtime cycle-start configuration.
 **********************************************************************************************************************/
static tas_cycle_start_config_t tas_example_select_cycle_start(void)
{
    tas_cycle_start_config_t cycle_start = {0};
    char                     input[8] = {0};
    uint32_t                 input_length;

    while (1)
    {
        printf(CYCLE_START_MENU);
        while (!tas_example_read_line(input, sizeof(input), &input_length))
        {
            tas_wait_for_input();
        }

        if ((1U == input_length) && ('1' == input[0]))
        {
            cycle_start.mode = TAS_CYCLE_START_MODE_FIXED;
            printf("\r\n\r\nFixed delay from timer start (%u..%u seconds): ",
                   TAS_CYCLE_START_FIXED_MIN_S, TAS_CYCLE_START_FIXED_MAX_S);
            while (!tas_example_read_fixed_delay_seconds(&cycle_start.fixed_delay_seconds))
            {
                tas_wait_for_input();
            }
            return cycle_start;
        }
        if ((1U == input_length) && ('2' == input[0]))
        {
            cycle_start.mode = TAS_CYCLE_START_MODE_DYNAMIC;
            return cycle_start;
        }

        printf("\r\n\r\nInvalid cycle-start mode. Enter exactly 1 or 2.\r\n");
    }
}
/***********************************************************************************************************************
* End of function tas_example_select_cycle_start.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_example_read_line
 *  Description  : Reads one completed UART line without waiting for more input.
 *  Arguments    : p_input        Destination buffer for the received line.
 *                 input_size     Destination buffer size in bytes.
 *                 p_input_length Receives the line length when a line is available.
 *  Return Value : true           A completed line was read.
 *                 false          No completed line is available or an argument is invalid.
 **********************************************************************************************************************/
static bool tas_example_read_line(char * p_input, uint32_t input_size, uint32_t * p_input_length)
{
    if ((NULL == p_input) || (0U == input_size) || (NULL == p_input_length) ||
        (0U == uart_console_has_line()))
    {
        return false;
    }

    *p_input_length = uart_console_read(p_input, input_size);
    return true;
}
/***********************************************************************************************************************
* End of function tas_example_read_line.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_example_read_main_menu_request
 *  Description  : Consumes a Main-menu request for a use-case or cycle-start configuration change.
 *  Arguments    : p_use_case              Receives a requested use case for a 1 or 2 command.
 *                 p_cycle_start_requested Receives true for a 3 command.
 *  Return Value : true                    A valid request was received.
 *                 false                   No complete line is available or the received line is invalid.
 **********************************************************************************************************************/
static bool tas_example_read_main_menu_request(tas_use_case_t * p_use_case, bool * p_cycle_start_requested)
{
    char     input[8] = {0};
    uint32_t input_length;

    if ((NULL == p_use_case) || (NULL == p_cycle_start_requested) ||
        !tas_example_read_line(input, sizeof(input), &input_length))
    {
        return false;
    }

    *p_cycle_start_requested = false;
    if ((1U == input_length) && ('1' == input[0]))
    {
        *p_use_case = TAS_USE_CASE_PORT_TO_PORT;
        return true;
    }
    if ((1U == input_length) && ('2' == input[0]))
    {
        *p_use_case = TAS_USE_CASE_CPU_GENERATED;
        return true;
    }
    if ((1U == input_length) && ('3' == input[0]))
    {
        *p_cycle_start_requested = true;
        return true;
    }

    printf("\r\n\r\nInvalid selection. Enter 1, 2, or 3.\r\n");
    printf(MAIN_MENU);
    return false;
}
/***********************************************************************************************************************
* End of function tas_example_read_main_menu_request.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_example_read_fixed_delay_seconds
 *  Description  : Consumes and validates a fixed cycle-start delay in whole seconds.
 *  Arguments    : p_seconds     Receives a delay from TAS_CYCLE_START_FIXED_MIN_S through MAX_S.
 *  Return Value : true          A valid delay was received.
 *                 false         No complete line is available or the received line is invalid.
 **********************************************************************************************************************/
static bool tas_example_read_fixed_delay_seconds(uint32_t * p_seconds)
{
    char     input[8] = {0};
    uint32_t input_length;
    uint32_t seconds = 0U;

    if ((NULL == p_seconds) || !tas_example_read_line(input, sizeof(input), &input_length))
    {
        return false;
    }

    for (uint32_t index = 0U; index < input_length; index++)
    {
        if ((input[index] < '0') || (input[index] > '9'))
        {
            break;
        }

        seconds = (seconds * 10U) + (uint32_t) (input[index] - '0');
        if (seconds > TAS_CYCLE_START_FIXED_MAX_S)
        {
            break;
        }

        if ((index + 1U) == input_length)
        {
            if (seconds >= TAS_CYCLE_START_FIXED_MIN_S)
            {
                *p_seconds = seconds;
                return true;
            }
        }
    }

    printf("\r\n\r\nInvalid fixed delay. Enter an integer from %u to %u seconds.\r\n",
           TAS_CYCLE_START_FIXED_MIN_S, TAS_CYCLE_START_FIXED_MAX_S);
    printf("The timer starts after confirmation, so this is a delay from its new epoch.\r\n");
    return false;
}
/***********************************************************************************************************************
* End of function tas_example_read_fixed_delay_seconds.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_example_cycle_start_is_same
 *  Description  : Checks whether two cycle-start settings would program the same schedule start.
 *  Arguments    : p_left        First cycle-start settings.
 *                 p_right       Second cycle-start settings.
 *  Return Value : true          Both settings select the same mode and fixed delay, when applicable.
 *                 false         Settings differ or an argument is invalid.
 **********************************************************************************************************************/
static bool tas_example_cycle_start_is_same(const tas_cycle_start_config_t * p_left,
                                            const tas_cycle_start_config_t * p_right)
{
    if ((NULL == p_left) || (NULL == p_right) || (p_left->mode != p_right->mode))
    {
        return false;
    }

    return (TAS_CYCLE_START_MODE_DYNAMIC == p_left->mode) ||
           (p_left->fixed_delay_seconds == p_right->fixed_delay_seconds);
}
/***********************************************************************************************************************
* End of function tas_example_cycle_start_is_same.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_example_use_case_description
 *  Description  : Returns the banner description for a validated runtime selection.
 *  Arguments    : use_case      Validated use case.
 *  Return Value : Pointer to the constant descriptive string.
 **********************************************************************************************************************/
static const char * tas_example_use_case_description(tas_use_case_t use_case)
{
    return (TAS_USE_CASE_CPU_GENERATED == use_case) ?
           "CPU-generated (Port 2 GWCA injects frames -> Port 1 TAS egress)" :
           "port-to-port (Port 0 ingress -> Port 1 TAS egress)";
}
/***********************************************************************************************************************
* End of function tas_example_use_case_description.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_example_run
 *  Description  : Brings up the console, independently selects a use case and cycle-start settings from the Main menu,
 *                 programs and enables TAS, and accepts a new 1/2/3 request while running. A changed request closes
 *                 and reopens the switch.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void tas_example_run(void)
{
    tas_info_t                   info = {0};
    fsp_err_t                    err;
    tas_use_case_t               use_case = TAS_USE_CASE_PORT_TO_PORT;
    tas_cycle_start_config_t     cycle_start =
    {
        .mode = TAS_CYCLE_START_MODE_FIXED,
        .fixed_delay_seconds = TAS_CYCLE_START_FIXED_DEFAULT_S,
    };
    tas_profile_t const * p_profile;
    bool                         use_case_selected = false;

    /* Bring up UART console */
    err = (fsp_err_t) uart_console_init();
    tas_example_handle_error(err, "UART console open");

    tas_example_print_banner();
    while (!use_case_selected)
    {
        bool cycle_start_requested;

        printf(MAIN_MENU);
        while (!tas_example_read_main_menu_request(&use_case, &cycle_start_requested))
        {
            tas_wait_for_input();
        }

        if (cycle_start_requested)
        {
            cycle_start = tas_example_select_cycle_start();
        }
        else
        {
            use_case_selected = true;
        }
    }

    while (1)
    {
        tas_use_case_t           requested_use_case = use_case;
        tas_cycle_start_config_t requested_cycle_start = cycle_start;
        bool                     reconfigure_requested = false;

        /* Look up profile for selected use case */
        p_profile = tas_config_get_profile(use_case);
        if (NULL == p_profile)
        {
            tas_example_handle_error(FSP_ERR_INVALID_ARGUMENT, "use case profile");
        }
        printf("\r\n\r\nUse case %u: %s\r\n", (unsigned int) use_case,
               tas_example_use_case_description(use_case));

        /* Program and enable TAS for this profile */
        err = tas_init(p_profile, &cycle_start, &info);
        tas_example_handle_error(err, "TAS initialization");
        tas_example_print_schedule(&info);

        printf(MAIN_MENU);

        if (TAS_USE_CASE_CPU_GENERATED == use_case)
        {
            /* Wait for egress port link up */
            err = tas_wait_link();
            tas_example_handle_error(err, "wait link");

            /* Create TX queue for CPU-generated frames */
            err = tas_tx_queue_create();
            tas_example_handle_error(err, "tx queue create");

            while (1)
            {
                bool cycle_start_requested;

                if (tas_example_read_main_menu_request(&requested_use_case, &cycle_start_requested))
                {
                    if (cycle_start_requested)
                    {
                        requested_cycle_start = tas_example_select_cycle_start();
                        if (!tas_example_cycle_start_is_same(&cycle_start, &requested_cycle_start))
                        {
                            reconfigure_requested = true;
                            break;
                        }

                        printf("\r\n\r\nCycle-start settings are already active.\r\n");
                        printf(MAIN_MENU);
                    }
                    else if (use_case != requested_use_case)
                    {
                        reconfigure_requested = true;
                        break;
                    }
                    else
                    {
                        printf("\r\n\r\nUse case %u is already active.\r\n", (unsigned int) use_case);
                        printf(MAIN_MENU);
                    }
                }

                /* Send one batch of frames */
                err = tas_send_frames(p_profile);
                tas_example_handle_error(err, "send frames");

                /* Wait one TAS cycle */
                tas_wait_next_cycle();
            }
        }
        else
        {
            while (1)
            {
                bool cycle_start_requested;

                if (tas_example_read_main_menu_request(&requested_use_case, &cycle_start_requested))
                {
                    if (cycle_start_requested)
                    {
                        requested_cycle_start = tas_example_select_cycle_start();
                        if (!tas_example_cycle_start_is_same(&cycle_start, &requested_cycle_start))
                        {
                            reconfigure_requested = true;
                            break;
                        }

                        printf("\r\n\r\nCycle-start settings are already active.\r\n");
                        printf(MAIN_MENU);
                    }
                    else if (use_case != requested_use_case)
                    {
                        reconfigure_requested = true;
                        break;
                    }
                    else
                    {
                        printf("\r\n\r\nUse case %u is already active.\r\n", (unsigned int) use_case);
                        printf(MAIN_MENU);
                    }
                }

                /* Idle while switch shapes external traffic */
                tas_idle_wait();
            }
        }

        if (reconfigure_requested)
        {
            printf("\r\n\r\nReconfiguring TAS.\r\n");
            /* Close switch before reconfigure */
            err = tas_close();
            tas_example_handle_error(err, "reconfiguration close");
        }

        use_case = requested_use_case;
        cycle_start = requested_cycle_start;
    }
}
/***********************************************************************************************************************
* End of function tas_example_run.
***********************************************************************************************************************/
