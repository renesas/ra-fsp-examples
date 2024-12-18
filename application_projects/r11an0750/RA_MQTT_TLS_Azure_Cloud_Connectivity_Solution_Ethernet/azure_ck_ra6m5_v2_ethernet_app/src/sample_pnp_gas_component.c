/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

#include "nx_api.h"
#include "nx_azure_iot_hub_client.h"
#include "user_choice.h"
#include "RA_ZMOD4XXX_Common.h"
#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_json_writer.h"
#include "sample_config.h"
#include "usr_app.h"

#define SAMPLE_PNP_GAS_COMPONENT_NAME									"Gas"
#define DOUBLE_DECIMAL_PLACE_DIGITS                                     (2)
#define THREE_DECIMAL_PLACE_DIGITS                                      (3)

/* TODO : Add property name */
static const CHAR sample_pnp_iaq_tvoc_property_name[] = "iaq-tvoh";
static const CHAR sample_pnp_iaq_etoh_property_name[] = "iaq-etoh";
static const CHAR sample_pnp_iaq_eco2_property_name[] = "iaq-eco2";

static UCHAR telemetry_buffer[256];
usr_iaq_data_t g_iaq_data_rc;
extern SAMPLE_CLIENT  client;
#define iothub_client client.iothub_client

UINT sample_gas_telemetry_messages_send(void);

UINT sample_gas_telemetry_messages_send(void)
{
    UINT status;
    NX_PACKET *packet_ptr;
    UINT data_length;
    NX_AZURE_IOT_JSON_WRITER json_writer;

    /* Receive ZMOD4410 sensor data from the IAQ queue */
    status = tx_queue_receive (&g_iaq_queue, &g_iaq_data_rc, TX_NO_WAIT);
    if(TX_SUCCESS != status)
    {
        g_iaq_data_rc.gs_eco2 = 0;
        g_iaq_data_rc.gs_etoh = 0;
        g_iaq_data_rc.gs_tvoc = 0;
        APP_DBG_PRINT("Can not get value of ZMOD4410 sensor \r\n");
    }
    else
    {
        /* Do nothing */
    }
    
    /* Create a telemetry message packet.  */
    if ((status = nx_azure_iot_hub_client_telemetry_message_create(&iothub_client, &packet_ptr, NX_WAIT_FOREVER)))
    {
        return(status);
    }

    if ((status = nx_azure_iot_hub_client_telemetry_component_set(packet_ptr, (UCHAR *)SAMPLE_PNP_GAS_COMPONENT_NAME, sizeof(SAMPLE_PNP_GAS_COMPONENT_NAME) - 1, NX_WAIT_FOREVER)))
    {
        return(status);
    }


    /* Build telemetry JSON payload.  */
    if (nx_azure_iot_json_writer_with_buffer_init(&json_writer, (UCHAR *)telemetry_buffer, sizeof(telemetry_buffer)))
    {
        nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
        return(NX_NOT_SUCCESSFUL);
    }

    /* Append ambient light property name and value.  */
    /* TODO : Replace real data from temp.*/
    if (nx_azure_iot_json_writer_append_begin_object(&json_writer) ||
        nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
                                                                   (UCHAR *)sample_pnp_iaq_tvoc_property_name,
                                                                   sizeof(sample_pnp_iaq_tvoc_property_name) - 1,
                                                                   g_iaq_data_rc.gs_tvoc,
																   THREE_DECIMAL_PLACE_DIGITS) ||
		nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
																   (UCHAR *)sample_pnp_iaq_etoh_property_name,
																   sizeof(sample_pnp_iaq_etoh_property_name) - 1,
																   g_iaq_data_rc.gs_etoh,
																   THREE_DECIMAL_PLACE_DIGITS) ||
		nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
																	(UCHAR*) sample_pnp_iaq_eco2_property_name,
																	sizeof(sample_pnp_iaq_eco2_property_name) - 1,
																	g_iaq_data_rc.gs_eco2,
																	THREE_DECIMAL_PLACE_DIGITS)||
        nx_azure_iot_json_writer_append_end_object(&json_writer))
    {
    	//demo_printf("Telemetry message failed to build message\r\n");
        nx_azure_iot_json_writer_deinit(&json_writer);
        nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
        return(NX_NOT_SUCCESSFUL);
    }

    data_length = nx_azure_iot_json_writer_get_bytes_used(&json_writer);
    if ((status = nx_azure_iot_hub_client_telemetry_send(&iothub_client, packet_ptr,
                                                         (UCHAR *)telemetry_buffer, data_length, NX_WAIT_FOREVER)))
    {

        nx_azure_iot_json_writer_deinit(&json_writer);
        nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
        return(status);
    }

    nx_azure_iot_json_writer_deinit(&json_writer);
    nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
    /* TODO : Replace real data from temp.*/
#if ENABLE_SENSORS_MQTT_PRINTS
    demo_printf("\r\nTelemetry message sent: IAQ-TVOC = %f,IAQ-ETOH = %f, IAQ-ECO2 = %f\r\n", zmod4410_Data.zmod4410Data.tvoc, zmod4410_Data.zmod4410Data.etoh,zmod4410_Data.zmod4410Data.ec02);
#endif
    return(NX_AZURE_IOT_SUCCESS);
}
