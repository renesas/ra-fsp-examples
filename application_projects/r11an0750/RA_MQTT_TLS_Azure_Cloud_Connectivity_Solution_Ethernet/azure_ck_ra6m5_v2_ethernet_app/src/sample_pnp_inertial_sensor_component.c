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
#include "RA_ZMOD4XXX_Common.h"
#include "ICM42605.h"
#include "sample_config.h"
#include "usr_app.h"

#define THREE_DECIMAL_PLACE_DIGITS                                     (6)

#define SAMPLE_PNP_INERTIAL_COMPONENT_NAME                       "Inertial"

/* TODO : Add property name */
static const CHAR sample_pnp_accelerometerx_property_name[] = "accelerometerX";
static const CHAR sample_pnp_accelerometery_property_name[] = "accelerometerY";
static const CHAR sample_pnp_accelerometerz_property_name[] = "accelerometerZ";
static const CHAR sample_pnp_gyroscopex_property_name[] = "gyroscopeX";
static const CHAR sample_pnp_gyroscopey_property_name[] = "gyroscopeY";
static const CHAR sample_pnp_gyroscopez_property_name[] = "gyroscopeZ";
static const CHAR sample_pnp_compassx_property_name[] = "magX";
static const CHAR sample_pnp_compassy_property_name[] = "magY";
static const CHAR sample_pnp_compassz_property_name[] = "magZ";

static UCHAR telemetry_buffer[256];


extern SAMPLE_CLIENT  client;
#define iothub_client client.iothub_client

usr_icm_acc_gyr_data_t g_icm_acc_gyr_data_rc;
icm_mag_data_t g_icm_mag_data_rc;

UINT sample_inertial_sensor_telemetry_messages_send(void);

UINT sample_inertial_sensor_telemetry_messages_send(void)
{
    UINT status;
    NX_PACKET *packet_ptr;
    UINT data_length;
    NX_AZURE_IOT_JSON_WRITER json_writer;

    /* Receive ICM sensor data from the ICM queue */
    status = tx_queue_receive (&g_icm_queue, &g_icm_acc_gyr_data_rc, TX_NO_WAIT);
    if(TX_SUCCESS != status)
    {
        g_icm_acc_gyr_data_rc.acc_data.x = 0;
        g_icm_acc_gyr_data_rc.acc_data.y = 0;
        g_icm_acc_gyr_data_rc.acc_data.z = 0;
        g_icm_acc_gyr_data_rc.gyr_data.x = 0;
        g_icm_acc_gyr_data_rc.gyr_data.y = 0;
        g_icm_acc_gyr_data_rc.gyr_data.z = 0;
        APP_DBG_PRINT("Can not get the value of the Accelerometer and Gyrometer - ICM sensor \r\n");
    }
    else
    {
        /* Do nothing */
    }

    /* No support for ICM magnotometer sensor data */
    g_icm_mag_data_rc.x = 0;
    g_icm_mag_data_rc.y = 0;
    g_icm_mag_data_rc.z = 0;

    /* Create a telemetry message packet.  */
    if ((status = nx_azure_iot_hub_client_telemetry_message_create(&iothub_client, &packet_ptr, NX_WAIT_FOREVER)))
    {
        return (status);
    }

    if ((status = (nx_azure_iot_hub_client_telemetry_component_set(packet_ptr, (UCHAR *)SAMPLE_PNP_INERTIAL_COMPONENT_NAME, sizeof(SAMPLE_PNP_INERTIAL_COMPONENT_NAME) - 1, NX_WAIT_FOREVER))))
    {
        return(status);
    }

    /* Build telemetry JSON payload.  */
    if (nx_azure_iot_json_writer_with_buffer_init(&json_writer, (UCHAR *)telemetry_buffer, sizeof(telemetry_buffer)))
    {
        nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
        return(NX_NOT_SUCCESSFUL);
    }

    /* TODO : Replace real data from temp.*/
    if (nx_azure_iot_json_writer_append_begin_object(&json_writer) ||
        nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
                                                                    (UCHAR *)sample_pnp_accelerometerx_property_name,
                                                                    sizeof(sample_pnp_accelerometerx_property_name) - 1,
                                                                    g_icm_acc_gyr_data_rc.acc_data.x,THREE_DECIMAL_PLACE_DIGITS) ||
        nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
                                                                    (UCHAR *)sample_pnp_accelerometery_property_name,
                                                                    sizeof(sample_pnp_accelerometery_property_name) - 1,
                                                                    g_icm_acc_gyr_data_rc.acc_data.y,THREE_DECIMAL_PLACE_DIGITS) ||
        nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
                                                                    (UCHAR *)sample_pnp_accelerometerz_property_name,
                                                                    sizeof(sample_pnp_accelerometerz_property_name) - 1,
                                                                    g_icm_acc_gyr_data_rc.acc_data.z,THREE_DECIMAL_PLACE_DIGITS) ||
        nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
                                                                    (UCHAR *)sample_pnp_gyroscopex_property_name,
                                                                    sizeof(sample_pnp_gyroscopex_property_name) - 1,
                                                                    g_icm_acc_gyr_data_rc.gyr_data.x,THREE_DECIMAL_PLACE_DIGITS) ||
        nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
                                                                    (UCHAR *)sample_pnp_gyroscopey_property_name,
                                                                    sizeof(sample_pnp_gyroscopey_property_name) - 1,
                                                                    g_icm_acc_gyr_data_rc.gyr_data.y,THREE_DECIMAL_PLACE_DIGITS) ||
        nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
                                                                    (UCHAR *)sample_pnp_gyroscopez_property_name,
                                                                    sizeof(sample_pnp_gyroscopez_property_name) - 1,
                                                                    g_icm_acc_gyr_data_rc.gyr_data.z,THREE_DECIMAL_PLACE_DIGITS) ||
        nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
                                                                    (UCHAR *)sample_pnp_compassx_property_name,
                                                                    sizeof(sample_pnp_compassx_property_name) - 1,
                                                                    g_icm_mag_data_rc.x,THREE_DECIMAL_PLACE_DIGITS) ||
        nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
                                                                    (UCHAR *)sample_pnp_compassy_property_name,
                                                                    sizeof(sample_pnp_compassy_property_name) - 1,
                                                                    g_icm_mag_data_rc.y,THREE_DECIMAL_PLACE_DIGITS) ||
        nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
                                                                    (UCHAR *)sample_pnp_compassz_property_name,
                                                                    sizeof(sample_pnp_compassz_property_name) - 1,
                                                                    g_icm_mag_data_rc.z,THREE_DECIMAL_PLACE_DIGITS) ||

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
    /* TODO : Replace real data from temp. */
#if ENABLE_SENSORS_MQTT_PRINTS
    demo_printf("\r\nTelemetry message sent: accel_x = %f, accel_y = %f, accel_z = %f, ", icm_Data.accel.xaxis, icm_Data.accel.yaxis, icm_Data.accel.zaxis);
    demo_printf("gyro_x = %f, gyro_y = %f, gyro_z = %f ", icm_Data.gyro.xaxis, icm_Data.gyro.yaxis, icm_Data.gyro.zaxis);
    demo_printf("mag_x = %f, mag = %f, mag = %f\r\n", icm_Data.mag.xaxis, icm_Data.mag.yaxis, icm_Data.mag.zaxis);
#endif
    return(NX_AZURE_IOT_SUCCESS);
}
