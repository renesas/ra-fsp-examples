/***********************************************************************************************************************
 * File Name    : snmp_agent_ep.h
 * Description  : Contains data structures and functions used in snmp_agent_ep.h.
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
#ifndef SNMP_AGENT_EP_H_
#define SNMP_AGENT_EP_H_

#include "common_utils.h"
#include "snmp_thread.h"

/* To display EP info on the RTT viewer */
#define EP_INFO        "\r\nThis project demonstrates the basic functionality of SNMP Agent\r\n"\
                       "with Azure RTOS running on Renesas RA MCUs based on Renesas FSP.\r\n"\
                       "SNMP Agent is created and initialized, An agent is responsible for responding\r\n"\
                       "to SNMP Manager’s commands and sending event driven traps.\r\n"\
					   "On pressing any key from JlinkRTT Viewer, the Agent sends trapv2 event to the manager.\r\n"\
                       "Status of application will be displayed on JlinkRTT Viewer \r\n"\

#define SNMP_AGENT_MENU          "\r\nPress any key to send trap."\
                                "\r\nUser Input:  "

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), str);
#define PRINT_ERR_STR(str)   app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), str);
#define ERROR_TRAP(err)      app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &err);

/**@brief User configurable (ipv4) address. IP address should be provided from where the SNMP Manager is running
 * NOTE: Current IP address is provided according to our test environment.*/
#define MANAGER_IP_ADDRESS         IP_ADDRESS(192,168,0,32)
#define SNMP_V3                    1
#define SNMP_OVERRIDE_HANDLERS     1
#define ONE                        1
#define LINK_ENABLE_WAIT_TIME     (1000U)
#define STRING_LEN                 10
#define ARRAY_SIZE                 15
#define NX_SNMP_AGENT_HOSTNAME_MAX 25
#define NULL_CHAR                  '\0'
extern NX_SNMP_AGENT g_snmp_agent0;

#define SNMP_UNAME             "public"
#define SNMP_W_COMMUNITY       "private"
#define SNMP_TRAP_COMMUNITY    "public"

#define SNMP_V3_AUTH_KEY        "authpassword"
#define SNMP_V3_PRIV_KEY        "privpassword"


typedef enum snmp_verison
{
	SNMP_TRAPV2=1
}snmp_version_t;

/* MIB data structure.*/
typedef struct MIB_ENTRY_STRUCT
{
    UCHAR       *object_name;
    void        *object_value_ptr;
    UINT        (*object_get_callback)(VOID *source_ptr, NX_SNMP_OBJECT_DATA *object_data);
    UINT        (*object_set_callback)(VOID *destination_ptr, NX_SNMP_OBJECT_DATA *object_data);
} MIB_ENTRY;

/* Private function */
UINT g_snmp_agent0UsernameHandler(NX_SNMP_AGENT *agent_ptr, UCHAR *username);
UINT g_snmp_agent0GetHandler(NX_SNMP_AGENT *agent_ptr, UCHAR *object_requested,
        NX_SNMP_OBJECT_DATA *object_data);
UINT g_snmp_agent0SetHandler(NX_SNMP_AGENT *agent_ptr, UCHAR *object_requested,
        NX_SNMP_OBJECT_DATA *object_data);
UINT snmp_agent_services_init(void);
UINT async_trap_v2_send(void);

#endif /* SNMP_AGENT_EP_H_ */
