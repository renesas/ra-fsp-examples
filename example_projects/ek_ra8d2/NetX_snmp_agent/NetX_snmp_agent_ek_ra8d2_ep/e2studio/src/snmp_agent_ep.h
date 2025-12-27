/***********************************************************************************************************************
 * File Name    : snmp_agent_ep.h
 * Description  : Contains data structures and functions used in snmp_agent_ep.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef SNMP_AGENT_EP_H_
#define SNMP_AGENT_EP_H_

#include "common_utils.h"
#include "snmp_thread.h"

/* Example project banner info */
#define EP_INFO   "This project demonstrates the basic functionality of an SNMP agent running on\r\n"\
                  "Azure RTOS with Renesas RA MCUs using the Renesas Flexible Software Package (FSP).\r\n"\
                  "The SNMP agent is configured to respond to SNMP manager commands and to send\r\n"\
                  "event-driven traps. When any key is pressed in the J-Link RTT Viewer, the agent\r\n"\
                  "sends an SNMP trapv2 event to the manager. Application status is displayed in\r\n"\
                  "the J-Link RTT Viewer.\r\n"\


#define SNMP_AGENT_MENU     "\r\nPress any key to send trap."\
		                    "\r\nUser Input:  "

/* Macros to print information, errors, and trap errors */
#define PRINT_INFO_STR(str)  (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)   (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)      (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

/* User-configurable IPv4 address of the SNMP Manager.
 * Note: The current IP address is set according to the test environment.
 *       Update this value to match the IP address of the host running the SNMP Manager. */
#define MANAGER_IP_ADDRESS          (IP_ADDRESS(192, 168, 0, 183))
#define SNMP_V3                     (1)
#define SNMP_OVERRIDE_HANDLERS      (1)
#define ONE                         (1)
#define LINK_ENABLE_WAIT_TIME       (1000U)
#define STRING_LEN                  (10)
#define ARRAY_SIZE                  (15)
#define NX_SNMP_AGENT_HOSTNAME_MAX  (25)
#define NULL_CHAR                   ('\0')
extern NX_SNMP_AGENT g_snmp_agent0;

#define SNMP_UNAME                  "public"
#define SNMP_W_COMMUNITY            "private"
#define SNMP_TRAP_COMMUNITY         "public"

#define SNMP_V3_AUTH_KEY            "authpassword"
#define SNMP_V3_PRIV_KEY            "privpassword"

typedef enum snmp_version
{
    SNMP_TRAPV2 = 1
} snmp_version_t;

/* MIB data structure */
typedef struct MIB_ENTRY_STRUCT
{
    UCHAR * object_name;
    void * object_value_ptr;
    UINT (*object_get_callback)(VOID *source_ptr, NX_SNMP_OBJECT_DATA *object_data);
    UINT (*object_set_callback)(VOID *destination_ptr, NX_SNMP_OBJECT_DATA *object_data);
} MIB_ENTRY;

/* Private functions */
UINT g_snmp_agent0UsernameHandler(NX_SNMP_AGENT *agent_ptr, UCHAR *username);
UINT g_snmp_agent0GetHandler(NX_SNMP_AGENT *agent_ptr, UCHAR *object_requested,
        NX_SNMP_OBJECT_DATA *object_data);
UINT g_snmp_agent0SetHandler(NX_SNMP_AGENT *agent_ptr, UCHAR *object_requested,
        NX_SNMP_OBJECT_DATA *object_data);
UINT snmp_agent_services_init(void);
UINT async_trap_v2_send(void);

#endif /* SNMP_AGENT_EP_H_ */
