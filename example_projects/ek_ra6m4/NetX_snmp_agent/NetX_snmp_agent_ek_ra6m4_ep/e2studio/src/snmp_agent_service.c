/***********************************************************************************************************************
 * File Name    : snmp_agent_service.c
 * Description  : Contains entry function of SNMP Agent.
 ***********************************************************************************************************************/
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
#include "snmp_agent_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup NetX_snmp_agent_ep
 * @{
 **********************************************************************************************************************/

/* DS for SNMP agent services */
#if  SNMP_V3
/* Authentication enabled */
NX_SNMP_SECURITY_KEY v3_authentication_key;
/* Privacy enabled */
NX_SNMP_SECURITY_KEY v3_privacy_key;
#endif /* END NX_SNMP_DISABLE_V3 */

/* Private function */
static UINT  get_supportFunc(VOID *source_ptr, NX_SNMP_OBJECT_DATA *object_data);
static UINT  set_supportFunc(VOID *destination_ptr, NX_SNMP_OBJECT_DATA *object_data);
static void terminate_nullchar(char * dest_str, UCHAR * input_str);

ULONG oid_var = 1234U;
UCHAR   kit_name[STRING_LEN] = "Renesas";

NXD_ADDRESS g_snmp_manager_module =
{
 .nxd_ip_address.v4 = IP_ADDRESS(132,158,124,56),
 .nxd_ip_version = NX_IP_VERSION_V4,
};

/*MIB entry list */
MIB_ENTRY   mib_entries[] = {

    {(UCHAR *) "1.3.6.1.4.1.51000.1.4.0", &oid_var, get_supportFunc, set_supportFunc},
    {NX_NULL, NX_NULL, NX_NULL, NX_NULL}

};

/*******************************************************************************************************************//**
 * @brief     system source get operation
 * @param[in]     source_ptr
 * @param[in]     object_data
 * @retval    Any Other Error code apart from NX_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
static UINT  get_supportFunc(VOID *source_ptr, NX_SNMP_OBJECT_DATA *object_data)
{
   return nx_snmp_object_integer_get(source_ptr, object_data);
}

/*******************************************************************************************************************//**
 * @brief    system source set operation
 * @param[in]     destination_ptr
 * @param[in]     object_data
 * @retval    Any Other Error code apart from NX_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
static UINT  set_supportFunc(VOID *destination_ptr, NX_SNMP_OBJECT_DATA *object_data)
{
    UINT    status    = NX_SUCCESS;

    /* Validate the input */
    FSP_PARAMETER_NOT_USED(object_data);
    status = nx_snmp_object_integer_set(destination_ptr, object_data);
    return status;
}

/*******************************************************************************************************************//**
 * @brief     Starts snmp agent services.
 * @param[IN] None
 * @retval    Any Other Error code apart from NX_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
UINT snmp_agent_services_init(void)
{
   UINT status = NX_SUCCESS;
   status = nx_snmp_agent_private_string_set (&g_snmp_agent0, (UCHAR *) SNMP_W_COMMUNITY);
   if (NX_SUCCESS != status)
    {
       PRINT_ERR_STR("nx_snmp_agent_private_string_set failed");
       return status;
    }

    status = nx_snmp_agent_public_string_set (&g_snmp_agent0, (UCHAR *) SNMP_UNAME);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_snmp_agent_public_string_set failed");
        return status;
    }

#if  SNMP_V3
    UCHAR context_engine_id[ARRAY_SIZE] =
    { 0x80, 0x00, 0xc7, 0x38, 0x03, 0x00, 0x30, 0x55, 0x23, 0x44, 0x55 };
    UINT context_engine_size = 11;
    nx_snmp_agent_context_engine_set (&g_snmp_agent0, context_engine_id, context_engine_size);

    /* if you want to use SHA key change nx_snmp_agent_md5_key_create to nx_snmp_agent_sha_key_create*/
    /* Create an authentication key.  */
    nx_snmp_agent_md5_key_create (&g_snmp_agent0, (UCHAR *) SNMP_V3_AUTH_KEY, &v3_authentication_key);

    /* Use the authentication key.  */
    nx_snmp_agent_authenticate_key_use (&g_snmp_agent0, &v3_authentication_key);

    /* Create a privacy key.  */
    nx_snmp_agent_md5_key_create (&g_snmp_agent0, (UCHAR *) SNMP_V3_PRIV_KEY, &v3_privacy_key);

    /* Use the privacy key.  */
    nx_snmp_agent_privacy_key_use (&g_snmp_agent0, &v3_privacy_key);

#endif

    /* Start the SNMP instance.  */
    status = nx_snmp_agent_start (&g_snmp_agent0);
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
 * @brief     sends snmp agent trap for v2.
 * @param[IN] None
 * @retval    Any Other Error code apart from NX_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
UINT async_trap_v2_send(void)
{
    UINT status = NX_SUCCESS;
    UINT trap_type = NX_SNMP_TRAP_COLDSTART;
    NX_SNMP_TRAP_OBJECT test_trap_list[2];
    NX_SNMP_OBJECT_DATA trap_data0;
    nx_snmp_object_string_get ((void *) kit_name, &trap_data0);
    test_trap_list[0].nx_snmp_object_string_ptr = (UCHAR *) "1.3.6.1.4.1.51000.1.4.0";
    test_trap_list[0].nx_snmp_object_data = &trap_data0;
    test_trap_list[1].nx_snmp_object_string_ptr = NULL;

    status = nx_snmp_agent_trapv2_send(&g_snmp_agent0, g_snmp_manager_module.nxd_ip_address.v4, (UCHAR *)"public",trap_type ,tx_time_get(), test_trap_list);
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
 * @brief     Username handler.
 * @param[IN] agent_ptr  agent_ptr argument used to identify SNMP agent and username to identify access.
 * @retval    None
 **********************************************************************************************************************/
UINT g_snmp_agent0UsernameHandler(NX_SNMP_AGENT *agent_ptr, UCHAR *username)
{
    if((strstr((CHAR*)username, (CHAR *)"public") || strstr((CHAR*)username, (CHAR *)"private")) ||
            (agent_ptr->nx_snmp_agent_current_version) )
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
 * @brief      This is SNMP get handler function routine
 *
 * @param[in]  agent_ptr argument used to identify SNMP agent.
 * @param[in]  p_oid to identify the requested object. This is in dotted decimal format Ex: "1.3.6.1.2.1"
 * @param[in,out]  object_data to fill the response for the oid
 **********************************************************************************************************************/
UINT g_snmp_agent0GetHandler(NX_SNMP_AGENT *agent_ptr, UCHAR *object_requested, NX_SNMP_OBJECT_DATA *object_data)
{
    UINT    count_val                            = RESET_VALUE;
    UINT    status                               = NX_SUCCESS;;
    NX_PARAMETER_NOT_USED(agent_ptr);
    status                                       =  NX_SNMP_ERROR;
    CHAR    oid_name[NX_SNMP_AGENT_HOSTNAME_MAX] = {NULL_CHAR};
    size_t  oid_data_len                         = RESET_VALUE;

    PRINT_INFO_STR("Get handler request: checking OID");
    while (mib_entries[count_val].object_name)
    {
    	memset(oid_name,NULL_CHAR,sizeof(oid_name));
        status =  nx_snmp_object_compare(object_requested, mib_entries[count_val].object_name);
        if (status == NX_SUCCESS)
        {
            terminate_nullchar(oid_name, mib_entries[count_val].object_name);
            oid_data_len = strlen(oid_name) +ONE;
            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_OID_NAME, oid_data_len, oid_name);
            break;
        }
        count_val++;
    }
    if (NX_SUCCESS != status)
    {
    	PRINT_ERR_STR("Error in processing get handler.");
        return NX_SNMP_ERROR_NOSUCHNAME;
    }

    /* Determine if the entry has a get function.  */
    if (mib_entries[count_val].object_get_callback)
    {
        status = (mib_entries[count_val].object_get_callback) (mib_entries[count_val].object_value_ptr, object_data);
        if (!status)
        {
        	PRINT_INFO_STR("Get request processed");
        }
    }
    else
    {
    	PRINT_ERR_STR("No get function call.");
        status = NX_SNMP_ERROR_NOACCESS;
    }
    /* Return the status.  */
    return (status);
}

/*******************************************************************************************************************//**
 * @brief          This is snmp set handler function
 * @param[in]      agent_ptr argument used to identify SNMP agent.
 * @param[in]      oid to identify the requested object. This is in dotted decimal format Ex: "1.3.6.1.2.1"
 * @param[in,out]  object_data to fill the response for the p_oid
 **********************************************************************************************************************/
UINT g_snmp_agent0SetHandler(NX_SNMP_AGENT *agent_ptr, UCHAR *object_requested, NX_SNMP_OBJECT_DATA *object_data)
{
    UINT    count_val                                = RESET_VALUE;
    UINT    status                                   = NX_SUCCESS;
    NX_PARAMETER_NOT_USED(agent_ptr);
    status                                           =  NX_SNMP_ERROR;
    CHAR    set_oid_name[NX_SNMP_AGENT_HOSTNAME_MAX] = {NULL_CHAR};
    size_t  set_oid_data_len                         = RESET_VALUE;

    PRINT_INFO_STR("Set handler request: checking OID");
    while (mib_entries[count_val].object_name)
    {
    	memset(set_oid_name,NULL_CHAR,sizeof(set_oid_name));
        status =  nx_snmp_object_compare(object_requested, mib_entries[count_val].object_name);
        if (status == NX_SUCCESS)
        {
            terminate_nullchar(set_oid_name, mib_entries[count_val].object_name);
            set_oid_data_len = strlen(set_oid_name) +ONE;
            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_OID_NAME, set_oid_data_len, set_oid_name);
            break;
        }
        count_val++;
    }
    if (status != NX_SUCCESS)
    {
    	PRINT_ERR_STR("Error in processing set handler.");
        return(NX_SNMP_ERROR_NOSUCHNAME);
    }

    /* Determine if the entry has a set function.  */
    if (mib_entries[count_val].object_set_callback)
    {
        status =  (mib_entries[count_val].object_set_callback)(mib_entries[count_val].object_value_ptr, object_data);
        if (!status)
        {
        	PRINT_INFO_STR("Set request processed");
        }
    }
    else
    {
    	PRINT_ERR_STR("No set function call.");
        status =  NX_SNMP_ERROR_NOACCESS;
    }
    return(status);
}

/*******************************************************************************************************************//**
 * @brief      terminate with null char from input string.
 * @param[IN]  input_str  input string.
 * @param[OUT] dest_str   output string.
 * @retval     None
 **********************************************************************************************************************/
static void terminate_nullchar(char * dest_str, UCHAR * input_str)
{
    int8_t index = RESET_VALUE;

    do
    {
        dest_str[index] = input_str[index];
        ++index;
    }while (input_str[index]!=NULL_CHAR);

    dest_str[index] = NULL_CHAR;
}
/*******************************************************************************************************************//**
 * @} (end addtogroup NetX_snmp_agent_ep)
 **********************************************************************************************************************/
