/***********************************************************************************************************************
 * File Name    : snmp_agent_service.c
 * Description  : Contains entry function of SNMP agent.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "snmp_agent_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup NetX_snmp_agent_ep
 * @{
 **********************************************************************************************************************/

/* DS for SNMP agent services */
#if SNMP_V3
/* Authentication enabled */
NX_SNMP_SECURITY_KEY g_v3_authentication_key;
/* Privacy enabled */
NX_SNMP_SECURITY_KEY g_v3_privacy_key;
#endif /* END NX_SNMP_DISABLE_V3 */

/* Private functions */
static UINT get_supportFunc(VOID *source_ptr, NX_SNMP_OBJECT_DATA *object_data);
static UINT set_supportFunc(VOID *destination_ptr, NX_SNMP_OBJECT_DATA *object_data);
static void terminate_nullchar(char * dest_str, UCHAR * input_str);

ULONG g_oid_var = 1234U;
UCHAR g_kit_name[STRING_LEN] = "Renesas";

NXD_ADDRESS g_snmp_manager_module =
{
 .nxd_ip_address.v4 = MANAGER_IP_ADDRESS,
 .nxd_ip_version = NX_IP_VERSION_V4,
};

/* MIB entry list */
MIB_ENTRY g_mib_entries[] =
{
 {(UCHAR *) "1.3.6.1.4.1.51000.1.4.0", &g_oid_var, get_supportFunc, set_supportFunc},
 {NX_NULL, NX_NULL, NX_NULL, NX_NULL}
};

/*******************************************************************************************************************//**
 * @brief       Get the integer value from an SNMP object data source.
 * @param[in]   source_ptr      Pointer to the SNMP object source.
 * @param[in]   object_data     Pointer to the SNMP object data structure.
 * @retval      NX_SUCCESS      Operation completed successfully.
 * @retval      Others          Error code returned by nx_snmp_object_integer_get() on failure.
 **********************************************************************************************************************/
static UINT get_supportFunc(VOID *source_ptr, NX_SNMP_OBJECT_DATA *object_data)
{
   return nx_snmp_object_integer_get(source_ptr, object_data);
}

/*******************************************************************************************************************//**
 * @brief      Sets the integer value to an SNMP object data destination.
 * @param[in]  destination_ptr  Pointer to the SNMP object data destination.
 * @param[in]  object_data      Pointer to the SNMP object data structure.
 * @retval     NX_SUCCESS       Operation completed successfully.
 * @retval     Others           Error code returned by nx_snmp_object_integer_set() on failure.
 **********************************************************************************************************************/
static UINT set_supportFunc(VOID *destination_ptr, NX_SNMP_OBJECT_DATA *object_data)
{
    UINT status = NX_SUCCESS;

    /* Validate the input parameter (Unused in this implementation) */
    FSP_PARAMETER_NOT_USED(object_data);
    status = nx_snmp_object_integer_set(destination_ptr, object_data);
    return status;
}

/*******************************************************************************************************************//**
 * @brief   Initializes and starts the SNMP agent services.
 * @retval  NX_SUCCESS      Operation completed successfully.
 * @retval  Others          Error code returned by the underlying NetX SNMP API on failure.
 **********************************************************************************************************************/
UINT snmp_agent_services_init(void)
{
   UINT status = NX_SUCCESS;

   /* Set the SNMP private community string */
   status = nx_snmp_agent_private_string_set(&g_snmp_agent0, (UCHAR *) SNMP_W_COMMUNITY);
   if (NX_SUCCESS != status)
    {
       PRINT_ERR_STR("nx_snmp_agent_private_string_set failed");
       return status;
    }

   /* Set the SNMP public community string */
    status = nx_snmp_agent_public_string_set(&g_snmp_agent0, (UCHAR *) SNMP_UNAME);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_snmp_agent_public_string_set failed");
        return status;
    }

#if SNMP_V3
    UCHAR context_engine_id[ARRAY_SIZE] =
    { 0x80, 0x00, 0xc7, 0x38, 0x03, 0x00, 0x30, 0x55, 0x23, 0x44, 0x55 };
    UINT context_engine_size = 11;
    nx_snmp_agent_context_engine_set(&g_snmp_agent0, context_engine_id, context_engine_size);

    /* To use an SHA key, replace nx_snmp_agent_md5_key_create with nx_snmp_agent_sha_key_create. */
    /* Create an authentication key. */
    nx_snmp_agent_md5_key_create(&g_snmp_agent0, (UCHAR *) SNMP_V3_AUTH_KEY, &g_v3_authentication_key);

    /* Use the authentication key */
    nx_snmp_agent_authenticate_key_use(&g_snmp_agent0, &g_v3_authentication_key);

    /* Create a privacy key */
    nx_snmp_agent_md5_key_create(&g_snmp_agent0, (UCHAR *) SNMP_V3_PRIV_KEY, &g_v3_privacy_key);

    /* Use the privacy key */
    nx_snmp_agent_privacy_key_use(&g_snmp_agent0, &g_v3_privacy_key);

#endif

    /* Start the SNMP instance */
    status = nx_snmp_agent_start(&g_snmp_agent0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_snmp_agent_start failed");
        return status;
    }
    else
    {
        PRINT_INFO_STR("nx_snmp_agent started successfully");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief   Sends an SNMP trap (version 2) to the manager.
 * @retval  NX_SUCCESS      Operation completed successfully.
 * @retval  Others          Error code returned by nx_snmp_agent_trapv2_send() on failure.
 **********************************************************************************************************************/
UINT async_trap_v2_send(void)
{
    UINT status = NX_SUCCESS;
    UINT trap_type = NX_SNMP_TRAP_COLDSTART;
    NX_SNMP_TRAP_OBJECT test_trap_list[2];
    NX_SNMP_OBJECT_DATA trap_data0;

    /* Get the SNMP object data string from the kit name */
    nx_snmp_object_string_get((void *) g_kit_name, &trap_data0);
    test_trap_list[0].nx_snmp_object_string_ptr = (UCHAR *) "1.3.6.1.4.1.51000.1.4.0";
    test_trap_list[0].nx_snmp_object_data = &trap_data0;
    test_trap_list[1].nx_snmp_object_string_ptr = NULL;

    /* Send an SNMPv2 trap to the manager */
    status = nx_snmp_agent_trapv2_send(&g_snmp_agent0, g_snmp_manager_module.nxd_ip_address.v4, (UCHAR *)"public",
                                       trap_type, tx_time_get(), test_trap_list);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_snmp_agent_trapv2_send failed");
        return status;
    }
    else
    {
        PRINT_INFO_STR("Trap sent to manager successfully");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief      Validates the username for SNMP agent access.
 * @param[in]  agent_ptr        Pointer to the SNMP agent control block.
 * @param[in]  username         Pointer to the username string to be validated.
 * @retval     NX_SUCCESS       Username is valid.
 * @retval     NX_SNMP_ERROR    Username is invalid.
 **********************************************************************************************************************/
UINT g_snmp_agent0UsernameHandler(NX_SNMP_AGENT *agent_ptr, UCHAR *username)
{
    /* Accept access if username contains "public" or "private" */
    if ((strstr((CHAR*)username, (CHAR *)"public") || strstr((CHAR*)username, (CHAR *)"private")) ||
        (agent_ptr->nx_snmp_agent_current_version))
    {
        return NX_SUCCESS;
    }
    else
    {
        PRINT_ERR_STR("Invalid user");
        return NX_SNMP_ERROR;
    }
    return NX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       Handles SNMP GET requests from the SNMP manager.
 * @param[in]   agent_ptr           Pointer to the SNMP agent control block.
 * @param[in]   object_requested    Pointer to the requested OID string (in dotted decimal format, e.g., "1.3.6.1.2.1").
 * @param[in,out] object_data       Pointer to the SNMP object data structure used to return the response.
 * @retval      NX_SUCCESS                   Operation completed successfully.
 * @retval      NX_SNMP_ERROR_NOSUCHNAME     The requested OID was not found.
 * @retval      NX_SNMP_ERROR_NOACCESS       The requested OID has no associated GET function.
 **********************************************************************************************************************/
UINT g_snmp_agent0GetHandler(NX_SNMP_AGENT *agent_ptr, UCHAR *object_requested, NX_SNMP_OBJECT_DATA *object_data)
{
    UINT count_val = RESET_VALUE;
    UINT status    = NX_SNMP_ERROR;
    CHAR oid_name[NX_SNMP_AGENT_HOSTNAME_MAX] = {NULL_CHAR};
    size_t oid_data_len = RESET_VALUE;

    NX_PARAMETER_NOT_USED(agent_ptr);

    PRINT_INFO_STR("GET handler request: checking OID");
    while (g_mib_entries[count_val].object_name)
    {
    	memset(oid_name, NULL_CHAR, sizeof(oid_name));
        status = nx_snmp_object_compare(object_requested, g_mib_entries[count_val].object_name);
        if (NX_SUCCESS == status)
        {
            terminate_nullchar(oid_name, g_mib_entries[count_val].object_name);
            oid_data_len = strlen(oid_name) + ONE;
            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_OID_NAME, oid_data_len, oid_name);
            break;
        }
        count_val++;
    }
    if (NX_SUCCESS != status)
    {
    	PRINT_ERR_STR("Error in processing GET handler.");
        return NX_SNMP_ERROR_NOSUCHNAME;
    }

    /* Determine if the entry has a GET function callback */
    if (g_mib_entries[count_val].object_get_callback)
    {
        status = (g_mib_entries[count_val].object_get_callback) (g_mib_entries[count_val].object_value_ptr,
                                                                 object_data);
        if (NX_SUCCESS == status)
        {
        	PRINT_INFO_STR("GET request processed successfully.");
        }
    }
    else
    {
    	PRINT_ERR_STR("No GET function callback available.");
        status = NX_SNMP_ERROR_NOACCESS;
    }
    /* Return the status */
    return status;
}

/*******************************************************************************************************************//**
 * @brief       Handles SNMP SET requests from the SNMP manager.
 * @param[in]   agent_ptr           Pointer to the SNMP agent control block.
 * @param[in]   object_requested    Pointer to the requested OID string (in dotted decimal format, e.g., "1.3.6.1.2.1").
 * @param[in,out] object_data       Pointer to the SNMP object data structure containing the value to be updated.
 * @retval      NX_SUCCESS                   Operation completed successfully.
 * @retval      NX_SNMP_ERROR_NOSUCHNAME     The requested OID was not found.
 * @retval      NX_SNMP_ERROR_NOACCESS       The requested OID has no associated SET function.
 **********************************************************************************************************************/
UINT g_snmp_agent0SetHandler(NX_SNMP_AGENT *agent_ptr, UCHAR *object_requested, NX_SNMP_OBJECT_DATA *object_data)
{
    UINT count_val = RESET_VALUE;
    UINT status    = NX_SNMP_ERROR;
    CHAR set_oid_name[NX_SNMP_AGENT_HOSTNAME_MAX] = {NULL_CHAR};
    size_t set_oid_data_len = RESET_VALUE;

    NX_PARAMETER_NOT_USED(agent_ptr);

    PRINT_INFO_STR("SET handler request: checking OID");
    while (g_mib_entries[count_val].object_name)
    {
    	memset(set_oid_name, NULL_CHAR, sizeof(set_oid_name));
        status = nx_snmp_object_compare(object_requested, g_mib_entries[count_val].object_name);
        if (NX_SUCCESS == status)
        {
            terminate_nullchar(set_oid_name, g_mib_entries[count_val].object_name);
            set_oid_data_len = strlen(set_oid_name) + ONE;
            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_OID_NAME, set_oid_data_len, set_oid_name);
            break;
        }
        count_val++;
    }
    if (NX_SUCCESS != status)
    {
    	PRINT_ERR_STR("Error in processing SET handler.");
        return NX_SNMP_ERROR_NOSUCHNAME;
    }

    /* Determine if the entry has a SET function callback */
    if (g_mib_entries[count_val].object_set_callback)
    {
        status = (g_mib_entries[count_val].object_set_callback)(g_mib_entries[count_val].object_value_ptr, object_data);
        if (NX_SUCCESS == status)
        {
        	PRINT_INFO_STR("SET request processed successfully.");
        }
    }
    else
    {
        PRINT_ERR_STR("No SET function callback available.");
        status = NX_SNMP_ERROR_NOACCESS;
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief       Copies the input string to the destination buffer and terminates it with a null character.
 * @param[out]  dest_str   Pointer to the destination buffer where the string will be copied.
 * @param[in]   input_str  Pointer to the input string to be copied.
 * @retval      None.
 **********************************************************************************************************************/
static void terminate_nullchar(char * dest_str, UCHAR * input_str)
{
    int8_t index = RESET_VALUE;

    do
    {
        dest_str[index] = input_str[index];
        ++index;
    } while (input_str[index] != NULL_CHAR);

    dest_str[index] = NULL_CHAR;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup NetX_snmp_agent_ep)
 **********************************************************************************************************************/
