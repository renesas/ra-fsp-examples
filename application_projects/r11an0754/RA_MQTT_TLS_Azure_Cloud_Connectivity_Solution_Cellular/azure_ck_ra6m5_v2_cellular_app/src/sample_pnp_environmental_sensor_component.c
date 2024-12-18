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

#include <math.h>
#include "nx_api.h"
#include "nx_azure_iot_hub_client.h"
#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_json_writer.h"
#include "nx_azure_iot_provisioning_client.h"
#include "RA_HS3001.h"
#include "sample_config.h"
#include "user_choice.h"
#include "usr_app.h"

#define DOUBLE_DECIMAL_PLACE_DIGITS                                     (2)
#define THREE_DECIMAL_PLACE_DIGITS                                     (3)

#define SAMPLE_PNP_ENVIRONMENTAL_COMPONENT_NAME                       "Environmental"

/* TODO : Add property name */
static const CHAR sample_pnp_temperature_name_property_name[] = "temperature";
static const CHAR sample_pnp_humidity_property_name[] = "humidity";

static CHAR telemetry_buffer[256];

extern SAMPLE_CLIENT  client;
#define iothub_client client.iothub_client

usr_hs3001_data_t g_hs3001_data_rc;

UINT sample_environmental_sensor_telemetry_messages_send(void);


UINT sample_environmental_sensor_telemetry_messages_send(void)
{

UINT status;
NX_PACKET *packet_ptr;
UINT data_length;
NX_AZURE_IOT_JSON_WRITER json_writer;

    /* Receive HS3001 sensor data from the HS3001 queue */
    status = tx_queue_receive (&g_hs3001_queue, &g_hs3001_data_rc, TX_NO_WAIT);
    if(TX_SUCCESS != status)
    {
        g_hs3001_data_rc.gs_humidity = 0;
        g_hs3001_data_rc.gs_temperature = 0;
        APP_DBG_PRINT("Can not get value of HS3001 sensor \r\n");
    }
    else
    {
        /* Do nothing */
    }

    /* Create a telemetry message packet.  */
    if ((status = nx_azure_iot_hub_client_telemetry_message_create(&iothub_client, &packet_ptr, NX_WAIT_FOREVER)))
    {
        return (status);
    }

    if ((status = (nx_azure_iot_hub_client_telemetry_component_set(packet_ptr,(UCHAR *)SAMPLE_PNP_ENVIRONMENTAL_COMPONENT_NAME, sizeof(SAMPLE_PNP_ENVIRONMENTAL_COMPONENT_NAME) - 1, NX_WAIT_FOREVER))))
    {
        return(status);
    }

    /* Build telemetry JSON payload.  */
    if (nx_azure_iot_json_writer_with_buffer_init(&json_writer, (UCHAR *)telemetry_buffer, sizeof(telemetry_buffer)))
    {
        nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
        return(NX_NOT_SUCCESSFUL);
    }

    /* Append ambient light property name and value. */
    /* TODO : Replace real data from temp.*/
    if (nx_azure_iot_json_writer_append_begin_object(&json_writer) ||

        nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
                                                                        (UCHAR *)sample_pnp_temperature_name_property_name,
                                                                        sizeof(sample_pnp_temperature_name_property_name) - 1,
                                                                        g_hs3001_data_rc.gs_temperature,
                                                                        THREE_DECIMAL_PLACE_DIGITS) ||
        nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
                                                                    (UCHAR *)sample_pnp_humidity_property_name,
                                                                    sizeof(sample_pnp_humidity_property_name) - 1,
                                                                    g_hs3001_data_rc.gs_humidity,
                                                                    THREE_DECIMAL_PLACE_DIGITS) ||

        nx_azure_iot_json_writer_append_end_object(&json_writer))
        {
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
    /* TODO : Replace real data from temp.*/
#if ENABLE_SENSORS_MQTT_PRINTS
    demo_printf("\r\nTelemetry message sent: temperature = %f, ", g_demo_data.gs_demo_temperature);
    demo_printf("humidity = %f,", g_demo_data.gs_demo_humidity);
#endif
    return(NX_AZURE_IOT_SUCCESS);
}
