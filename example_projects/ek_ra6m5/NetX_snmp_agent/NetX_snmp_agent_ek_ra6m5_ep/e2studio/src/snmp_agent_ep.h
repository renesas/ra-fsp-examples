/***********************************************************************************************************************
 * File Name    : snmp_agent_ep.h
 * Description  : Contains data structures and functions used in snmp_agent_ep.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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
#define PRINT_INFO_STR(str)  (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)   (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)      (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

/**@brief User configurable (ipv4) address. IP address should be provided from where the SNMP Manager is running
 * NOTE: Current IP address is provided according to our test environment.*/
#define MANAGER_IP_ADDRESS         (IP_ADDRESS(192,168,0,183))
#define SNMP_V3                    (1)
#define SNMP_OVERRIDE_HANDLERS     (1)
#define ONE                        (1)
#define LINK_ENABLE_WAIT_TIME     (1000U)
#define STRING_LEN                 (10)
#define ARRAY_SIZE                 (15)
#define NX_SNMP_AGENT_HOSTNAME_MAX (25)
#define NULL_CHAR                  ('\0')
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
