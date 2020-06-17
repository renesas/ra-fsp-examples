/***********************************************************************************************************************
 * File Name    : application_thread_entry.c
 * Description  : Contains data structures and functions used in Cloud Connectivity application
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2020] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

#include "application_thread.h"

#include "iot_logging_task.h"
#include "iot_mqtt.h"
#include "iot_system_init.h"
#include "iot_init.h"
#include "aws_dev_mode_key_provisioning.h"

#include "mqtt_interface.h"
#include "common_utils.h"
#include "usr_app.h"
#include "usr_hal.h"
#include "usr_wifi.h"
#include "usr_config.h"

#define LIBRARY_LOG_LEVEL               IOT_LOG_DEBUG  /* IOT_LOG_DEBUG, IOT_LOG_WARN, IOT_LOG_INFO, IOT_LOG_ERROR, IOT_LOG_NONE */
#define LOGGING_TASK_STACK_SIZE         (1 * 1024)
#define LOGGING_TASK_STACK_PRIORITY     (6)
#define LOGGING_TASK_QUEUE_SIZE         (1 * 1024)

#define IOT_MQTT_NETWORK_ERROR           5

char cBuffer[16] = { RESET_VALUE };

static char SERVER_CERTIFICATE_PEM[] = SERVER_CERTIFICATE;
static char CLIENT_CERTIFICATE_PEM[] = CLIENT_CERTIFICATE;
static char CLIENT_KEY_PEM[]         = CLIENT_KEY;
static uint32_t mqtt_fail_count      = RESET_VALUE;

static mqtt_cfg_t g_mqtt_cfg     = { RESET_VALUE };
static mqtt_rx_payload_t mq_data = { RESET_VALUE };
static double mcu_temp __attribute__ ((used)) = RESET_VALUE;
static mqtt_status_t g_mqtt_status = 
{ .mqtt_lib_init_status = !(FSP_SUCCESS), .mqtt_connect_status = !(FSP_SUCCESS), .status = !(FSP_SUCCESS)};

const IotNetworkServerInfo_t serverInfo =
{ .pHostName = (char *) cBuffer, .port = 8883 };

const IotNetworkCredentials_t afrCredentials =
{ .pAlpnProtos = NULL,
  .maxFragmentLength = 1400,
  .disableSni = true,
  .pRootCa = SERVER_CERTIFICATE_PEM,
  .rootCaSize = sizeof(SERVER_CERTIFICATE_PEM),
  .pClientCert = CLIENT_CERTIFICATE_PEM,
  .clientCertSize = sizeof(CLIENT_CERTIFICATE_PEM),
  .pPrivateKey = CLIENT_KEY_PEM,
  .privateKeySize = sizeof(CLIENT_KEY_PEM), 
};

/****************************************************************************************
 * Application Thread entry function  is the main thread for the Application.
 * Initialization of HAL,xLoggingTaskInit, wifi init, mbed_tls_setup, IOT Lib init are
 * done before the MQTT connect is initiated.
 * Message processing is done in this thread, the Message from the Queue are processed 
 * and actions are taken accordingly. 
 * 
 ***************************************************************************************/

void application_thread_entry(void *pvParameters)
{
    fsp_err_t err = FSP_SUCCESS;
    int ierr = FSP_SUCCESS;
    BaseType_t bt_status =     { RESET_VALUE };
    fsp_pack_version_t version =    { RESET_VALUE };
    ProvisioningParams_t params =   { RESET_VALUE };
    char temp_str[MAX_PROPERTY_LEN] =  { RESET_VALUE };
    uint16_t switch_num = RESET_VALUE;

    FSP_PARAMETER_NOT_USED(pvParameters);

    params.pucClientPrivateKey = (uint8_t *) CLIENT_KEY_PEM;
    params.pucClientCertificate = (uint8_t *) CLIENT_CERTIFICATE_PEM;
    params.ulClientPrivateKeyLength = 1 + strlen ((const char *) params.pucClientPrivateKey);
    params.ulClientCertificateLength = 1 + strlen ((const char *) params.pucClientCertificate);

    /* version get API for FLEX pack information */
    R_FSP_VersionGet (&version);

    /* Application Project information printed on the Console */
    IotLog(BANNER_INFO, AP_VERSION, version.major, version.minor, version.patch);
    IotLog(AP_INFO);

    err = usr_hal_init ();
    if (FSP_SUCCESS != err)
    {
        FAILURE_INDICATION
        IotLogError("** HAL Initialization failed **\r\n");
        IotLogErrorTrap(err);
    }
#if 0
    bt_status = xLoggingTaskInitialize(LOGGING_TASK_STACK_SIZE, LOGGING_TASK_STACK_PRIORITY, LOGGING_TASK_QUEUE_SIZE);
    if (pdPASS != bt_status)
    {
        FAILURE_INDICATION
        IotLogError("** xLogging Task Initialize failed **\r\n");
        IotLogErrorTrap(bt_status);
    }
#endif

	err = connect_wifi();
    if (FSP_SUCCESS != err)
    {
        FAILURE_INDICATION
        IotLogError("** Wi-Fi Connectivity Failed **\r\n");
        IotLogErrorTrap(err);
    }

    err = dns_query (USR_MQTT_ENDPOINT, (uint8_t *) &g_network.server_ip);
    if (FSP_SUCCESS == err)
    {
        if (g_network.server_ip != 0)
        {
            /* Convert the IP address to a string. */
            FreeRTOS_inet_ntoa(g_network.server_ip, (char * ) cBuffer);
            IotLog("MQTT Endpoint IP address = %s\r\n",cBuffer);
            NETWORK_CONNECT_INDICATION
        }
        else
        {
            FAILURE_INDICATION
            IotLogError("** DNS Querry Failed for the MQTT Endpoint **\r\n");
            IotLogErrorTrap(err);
        }
    }
    else
    {
        FAILURE_INDICATION
        IotLogError("** dns_query API Failed **\r\n");
        IotLogErrorTrap(err);
    }

    ierr = mbedtls_platform_setup (NULL);
    if (FSP_SUCCESS != ierr)
    {
        FAILURE_INDICATION
        IotLogError("** HW SCE Init failed **\r\n");
        IotLogErrorTrap(ierr);
    }

    err = vAlternateKeyProvisioning (&params);
    if (FSP_SUCCESS != err)
    {
        FAILURE_INDICATION
        IotLogError("** Alternate Key Provisioning failed **\r\n");
        IotLogErrorTrap(err);
    }

    bt_status = SYSTEM_Init ();
    if (pdPASS != bt_status)
    {
        FAILURE_INDICATION
        IotLogError("** Socket Init failed **\r\n");
        IotLogErrorTrap(bt_status);
    }

    err = IotSdk_Init ();
    if (pdPASS != err)
    {
        FAILURE_INDICATION
        IotLogError("** SDK Init failed **\r\n");
        IotLogErrorTrap(err);
    }

    g_mqtt_cfg.aws_mqtt_mode = false;
    g_mqtt_cfg.p_identifier = "RA-mqtt-application";
    g_mqtt_cfg.p_nwk_credn_info = (IotNetworkCredentials_t *) &afrCredentials;
    g_mqtt_cfg.p_nwk_intf = (IotNetworkInterface_t *) IOT_NETWORK_INTERFACE_AFR;
    g_mqtt_cfg.p_nwk_srvr_info = (IotNetworkServerInfo_t *) &serverInfo;
    g_mqtt_cfg.mqtt_connect_status = false;
    g_mqtt_cfg.mqtt_lib_init_status = false;

    if (EXIT_FAILURE == mqtt_init (&g_mqtt_cfg))
    {
        FAILURE_INDICATION
        IotLogError("** MQTT Client returned with Error **\r\n");
        IotLogErrorTrap(EXIT_FAILURE);
    }

    if((true == g_mqtt_cfg.mqtt_connect_status) && (true == g_mqtt_cfg.mqtt_lib_init_status))
    {
		g_network.status = FSP_SUCCESS;
		g_mqtt_status.status = EXIT_SUCCESS;
	    AWS_CONNECT_INDICATION
    }
		
    IotLog("Successful MQTT Connection to the endpoint %s\r\n",USR_MQTT_ENDPOINT);
    IotLog("Device is Ready for Publishing and Subscription of Messages \r\n\r\n");

    while (xQueueReceive (g_topic_queue, &mq_data, portMAX_DELAY))
    {
        switch (mq_data.id)
        {
            case ID_PB:
            {
                IotLog("Push Button	S%d Pressed\r\n",mq_data.value.pb_data.pb_num);
                switch_num = mq_data.value.pb_data.pb_num;

                if(EXIT_SUCCESS == g_mqtt_status.status)
                {
                    g_mqtt_status.status = sendMessage_pb (switch_num);
	                if ((EXIT_SUCCESS != g_mqtt_status.status))
	                {
	                    IotLogError("** Push Button Message Publish failed **\r\n");
	                    mqtt_fail_count++;
	                    mqtt_cleanup_and_reinit ();
	                }
	                else
	                {
	                    AWS_ACTIVITY_INDICATION
	                }
                }
				else
				{
                    IotLog("MQTT Cleanup and reinit in Progress. Messages will be published once Up and Running \r\n");
				}
            }
            break;

            case ID_TEMPERATURE:
            {
                mcu_temp = ((((float) mq_data.value.adc_data.adc_value) * 0.353793f) - 467.39f);
                print_float (temp_str, sizeof(temp_str), mcu_temp);
                IotLog("MCU Temperature	%s \r\n",temp_str);

                if(EXIT_SUCCESS == g_mqtt_status.status)
                {
	                g_mqtt_status.status = sendMessage_ts (temp_str);
	                if ((EXIT_SUCCESS != g_mqtt_status.status))
	                {
	                    IotLogError("** Temperature Sensor Message Publish failed **\r\n");
	                    mqtt_fail_count++;
	                    mqtt_cleanup_and_reinit ();
	                }
	                else
	                {
	                    AWS_ACTIVITY_INDICATION
	                }
                }
				else
				{
                    IotLog("MQTT Cleanup and reinit in Progress. Messages will be published once Up and Running \r\n");
				}
            }
            break;

            case ID_LED_ACT:
            {
                IotLog("Topic Received from Cloud %s \r\n",mq_data.value.led_data.led_act_topic_msg);
                if (FSP_SUCCESS != parse_led_actuation_message (&mq_data))
                {
                    IotLogError("** LED Topic Actuation returned  Error **\r\n");
                    mqtt_fail_count++;
                }
                else
                {
                    AWS_ACTIVITY_INDICATION
                }
            }
            break;

            default:
            {
                IotLogError("** Error in MQTT Messages.. MQTT Client Cleanup started  **\r\n");
                mqtt_fail_count++;
                mqtt_cleanup_and_reinit ();
            }
            break;
        }
    }
}

/*********************************************************************************************************************
 * @brief	cleans up the mqtt by deinitializing and reiniting for cleaner connectivity.
 *
 *  This is called by the application when error in sending or receiving the MQTT messages are encountered at the 
 *  Application level.
 *********************************************************************************************************************/
void mqtt_cleanup_and_reinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    g_mqtt_status.status = EXIT_FAILURE;

    IotLog("MQTT Client Cleaning up and preparing for the reinit \r\n");

    if(FSP_SUCCESS == dns_query (USR_MQTT_ENDPOINT, (uint8_t *) &g_network.server_ip))
    {
        if (RESET_VALUE != g_network.server_ip )
        {
            /* Convert the IP address to a string. */
            FreeRTOS_inet_ntoa(g_network.server_ip, (char * ) cBuffer);
            IotLog("After DNS Requery MQTT Endpoint IP address = %s\r\n",cBuffer);
            NETWORK_CONNECT_INDICATION
        }
		
	    if (EXIT_FAILURE == mqtt_deinit (&g_mqtt_cfg))
	    {
	        FAILURE_INDICATION
	        IotLogError("** MQTT Client returned with Error after deinit**\r\n");
	        IotLogErrorTrap(EXIT_FAILURE);
			/* Add code to start the initialization or wait for Watchdog RESET */
			/* For now it is just trap */
	    }

	    if (EXIT_FAILURE == mqtt_init (&g_mqtt_cfg))
	    {
	        FAILURE_INDICATION
	        IotLogError("** MQTT Client returned with Error after reinit**\r\n");
	        IotLogErrorTrap(EXIT_FAILURE);
    	    /* Add code to start the initialization or wait for Watchdog RESET */
	        /* For now it is just trap */
	    }
    }
	else
	{
		IotLogError("** DNS Requery Failed**\r\n");
        IotLog("** Resetting Wi-Fi and MQTT Reinit Started**\r\n");
        if(FSP_SUCCESS == wifi_deinit())
        {
			err = connect_wifi();
			if (FSP_SUCCESS != err)
			{
				FAILURE_INDICATION
				IotLogError("** Wi-Fi Connectivity Failed **\r\n");
				IotLogErrorTrap(err);
			}

			err = dns_query (USR_MQTT_ENDPOINT, (uint8_t *) &g_network.server_ip);
			if(FSP_SUCCESS != err)
			{
			    IotLogError("** NETWORK Connectivity failed. Reboot the Board**\r\n");
                IotLogErrorTrap(EXIT_FAILURE);
			}
			else
			{
				if (g_network.server_ip != 0)
				{
					/* Convert the IP address to a string. */
					FreeRTOS_inet_ntoa(g_network.server_ip, (char * ) cBuffer);
					IotLog("DNS Requery MQTT Endpoint IP address = %s\r\n",cBuffer);
					NETWORK_CONNECT_INDICATION
				}
			}
			
		    if (EXIT_FAILURE == mqtt_deinit (&g_mqtt_cfg))
		    {
		        FAILURE_INDICATION
		        IotLogError("** MQTT Client returned with Error after deinit**\r\n");
		        IotLogErrorTrap(EXIT_FAILURE);
		    }

		    if (EXIT_FAILURE == mqtt_init (&g_mqtt_cfg))
		    {
		        FAILURE_INDICATION
		        IotLogError("** MQTT Client returned with Error after reinit**\r\n");
		        IotLogErrorTrap(EXIT_FAILURE);
		    }
        }
		else
		{
            IotLog("** Wi-Fi RESET and MQTT REINIT Un-Successful**\r\n");
			/* Add code to start the initialization or wait for Watchdog RESET */
			/* For now it is just trap */
		    IotLogErrorTrap(EXIT_FAILURE);
		}
        IotLog("** Wi-Fi RESET and MQTT REINIT Successful**\r\n");
	}

	if((FSP_SUCCESS == err) && (true == g_mqtt_cfg.mqtt_connect_status) && (true == g_mqtt_cfg.mqtt_lib_init_status))
	{
	    mqtt_fail_count = RESET_VALUE;
	    g_network.status = FSP_SUCCESS;
	    g_wifi.status = FSP_SUCCESS;
	    FAILURE_RECOVERED_INDICATION
	    g_mqtt_status.status = EXIT_SUCCESS;
	    g_mqtt_status.mqtt_connect_status = true;		
	    g_mqtt_status.mqtt_lib_init_status = true;
		
	    IotLog("MQTT Client Cleaning up and reinit done. Ready for Publish and Subscribe \r\n");
	}
}


/*********************************************************************************************************************
 * @brief	    connect_wifi : This function initializes and calls the WIFI connect.
 * @param[in]   None
 * @retval      FSP_SUCCESS             If both the connectivity checks are success.
 * @retval      Any other error         If one of the connectivity check fails.
 *********************************************************************************************************************/
fsp_err_t connect_wifi(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* WiFi Initialization */
    err = wifi_init ();
    if (FSP_SUCCESS != err)
    {
        FAILURE_INDICATION
        IotLogError("** Wi-Fi Initialization failed **\r\n");
        IotLogErrorTrap(err);
    }

    /* Connecting to WiFi AP/Router */
    err = wifi_connect ();
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        FAILURE_INDICATION
        IotLogError("** Wi-Fi Access Point Connect failed. **\r\n");
        if (FSP_SUCCESS == wifi_deinit ())
        {
            IotLogErrorTrap(err);
        }
        else
        {
            IotLogError("** Wi-Fi deinit failed.**\r\n");
            IotLogErrorTrap(err);
        }
    }
    else
    {
        /* Connecting to user selected WiFi AP */
        IotLog("\r\nConnecting to SSID %s\r\n", g_wifi.ssid);
    }

    /* Status check of WiFi connectivity */
    if (pdTRUE != WIFI_IsConnected())
    {
        FAILURE_INDICATION
        IotLog("Wi-Fi Channel not Open.\r\n");
        IotLogErrorTrap(err);

    }

    IotLog("Wi-Fi connected to SSID %s.\r\n",g_wifi.ssid);

    /* Get and print assigned IP address */
    /* Handle error */
    if (eWiFiSuccess != WIFI_GetIP ((uint8_t *)&g_wifi.device_ip_addr))
    {
        FAILURE_INDICATION
        IotLogError("** IP Address get failed **\r\n");
        err = FSP_ERR_WIFI_FAILED;
		
        if (FSP_SUCCESS != wifi_deinit ())
        {
            IotLogError("** Wi-Fi deinit failed.**\r\n");
        }
        IotLogErrorTrap(err);
    }
	g_wifi.status = FSP_SUCCESS;
    IotLog("\r\nDevice IP address: %d.%d.%d.%d\r\n", g_wifi.device_ip_addr[ZERO], g_wifi.device_ip_addr[ONE],
            g_wifi.device_ip_addr[TWO], g_wifi.device_ip_addr[THREE]);

    return FSP_SUCCESS;
}


/*********************************************************************************************************************
 * @brief	gets the mqtt failed count
 *
 * This function gets the failed mqqt counter stored in the variable mqtt_fail_count.
 *********************************************************************************************************************/

uint32_t get_mqtt_fail_count(void)
{
    return mqtt_fail_count;
}

/*********************************************************************************************************************
 * convert float to string format and store it in buffer
 *********************************************************************************************************************/
void print_float(char *buffer, size_t buflen, double value)
{
    char sign = RESET_VALUE;
    int integer = (int) value;
    int fraction = (int) ((value - integer) * 100.0);

    if (NULL == buffer)
    {
        return;
    }
    if (value < 0.0)
    {
        sign = '-';
        snprintf (buffer, buflen, "%c%d.%02d", sign, abs (integer), abs (fraction));
    }
    else
    {
        snprintf (buffer, buflen, "%d.%02d", abs (integer), abs (fraction));
    }
}

/*********************************************************************************************************************
 * @brief	parse_message function
 *
 * This function parse the incoming MQTT message and actuates the LED on the Board.
 *********************************************************************************************************************/
int parse_led_actuation_message(mqtt_rx_payload_t *lmq_data)
{
    int status = RESET_VALUE;

    const char msg_red_led_on[] = "{\"state\":{\"desired\":{\"Red_LED\":\"ON\"}}}";
    const char msg_red_led_off[] = "{\"state\":{\"desired\":{\"Red_LED\":\"OFF\"}}}";
    const char msg_blue_led_on[] = "{\"state\":{\"desired\":{\"Blue_LED\":\"ON\"}}}";
    const char msg_blue_led_off[] = "{\"state\":{\"desired\":{\"Blue_LED\":\"OFF\"}}}";
    const char msg_green_led_on[] = "{\"state\":{\"desired\":{\"Green_LED\":\"ON\"}}}";
    const char msg_green_led_off[] = "{\"state\":{\"desired\":{\"Green_LED\":\"OFF\"}}}";

    if (NULL == lmq_data)
    {
        IotLogErrorTrap("\r\n NULL pointer in parse_message \r\n");
        return 1;
    }

    /* Red LED */
    if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_red_led_on))
    {
        led_on_off (LED_RED, LED_ON);
        IotLog("\r\nRed LED ON\r\n");
    }
    else if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_red_led_off))
    {
        led_on_off (LED_RED, LED_OFF);
        IotLog("\r\nRed LED OFF \r\n");
    }

    /* Blue LED */
    if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_blue_led_on))
    {
        led_on_off (LED_BLUE, LED_ON);
        IotLog("\r\nBlue LED ON\r\n");
    }
    else if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_blue_led_off))
    {
        led_on_off (LED_BLUE, LED_OFF);
        IotLog("\r\nBlue LED OFF \r\n");
    }

    /* Green LED */
    if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_green_led_on))
    {
        led_on_off (LED_GREEN, LED_ON);
        IotLog("\r\nGreen LED ON\r\n");
    }
    else if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_green_led_off))
    {
        led_on_off (LED_GREEN, LED_OFF);
        IotLog("\r\nGreen LED OFF \r\n");
    }
    return status;
}
