/***********************************************************************************************************************
 * File Name    : user_app.h
 * Description  : Contains macros, data structures and functions used  in the Application
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
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef USER_APP_H_
#define USER_APP_H_

#include "FreeRTOS_DHCP.h"


/******************************************************************************
 Macro definitions
 ******************************************************************************/

/**
 *  User Configurable Settings
 **/

/** User has to update according to their LAN/WAN test setup for testing of ping IP address **/
#define IOT_DEMO_TEST_PING_IP                       "8.8.8.8"

/** HOST Address **/
#define IOT_DEMO_HOST_ADDRESS                       "io.adafruit.com"

/**
 *  @brief User has to be update according to their credentials viz., {user_name} and {feed_key} in the respective
 *  GET, PUT and POST URLs in the specified formats.
 *  For more information of obtaining user credentials can follow the steps provided in mark down file.
 **/

/** @brief To get the most recent value. Use GET url: https://io.adafruit.com/api/v2/{username}/feeds/{feed_key}/data?limit=1 **/
//#define IOT_DEMO_HTTPS_PRESIGNED_GET_URL   "https://io.adafruit.com/api/v2/mnkraj1/feeds/temp/data?limit=1"
#define IOT_DEMO_HTTPS_PRESIGNED_GET_URL   "https://io.adafruit.com/api/v2/canhtran/feeds/adc/data?limit=1"

/** @brief IOT_DEMO_HTTPS_PRESIGNED_PUT_POST_URL can be used in PUT and POST methods.
 *  PUT method will update data point at requested <id>.
 *  PUT url: https://ioadafruit.com/{user_name}/{feeds}/{feed_key}/data/{id} , where the <id> will append to the url in the process of
 *  PUT request, once it fetch from the GET request.
 *  POST method will send new data to the server.
 *  POST url:https://ioadafruit.com/{user_name}/{feeds}/{feed_key}/data/
 **/
#define IOT_DEMO_HTTPS_PRESIGNED_PUT_POST_URL    "https://io.adafruit.com//api/v2/canhtran/feeds/adc/data/"

/** @brief User has to update their generated active key from the io.adafruit.com server. */
//#define ACTIVE_KEY                             "aio_YUcA19S8MKK2a9p1fi6iIabzdk3J"
#define ACTIVE_KEY                             "aio_YZrR194SVXWrUzvmDJHAEFSXaRDQ"

/** @brief If the privacy of feed data visibility is set to private then 'ENABLE' the PRIVATE_KEY for processing of GET request **/
#define PRIVATE_KEY                        DISABLE


/**
 *  Client certificate to be updated by the user by following the process specified in the mark down file
 **/


 #define CLIENT_CERTIFICATE_PEM                                                  \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDqTCCApGgAwIBAgIUPr6maUI0N7UIq27lJ47oMVPB+tkwDQYJKoZIhvcNAQEL\n" \
"BQAwZDELMAkGA1UEBhMCSU4xCzAJBgNVBAgMAktBMRIwEAYDVQQHDAlCZW5nYWx1\n" \
"cnUxDDAKBgNVBAoMA3RhdDEMMAoGA1UECwwDb2RjMRgwFgYDVQQDDA9pby5hZGFm\n" \
"cnVpdC5jb20wHhcNMjIwNTE2MDc0NDU4WhcNMjMwNTE2MDc0NDU4WjBkMQswCQYD\n" \
"VQQGEwJJTjELMAkGA1UECAwCS0ExEjAQBgNVBAcMCUJlbmdhbHVydTEMMAoGA1UE\n" \
"CgwDdGF0MQwwCgYDVQQLDANvZGMxGDAWBgNVBAMMD2lvLmFkYWZydWl0LmNvbTCC\n" \
"ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKflPkSRWkLc1WkQ82ki5Mju\n" \
"yh22ShAY7BRsM6sNKj6UzDVazuIWKHxXuhbCnXn8lFysxOqDJaChGEu47iWwgeOr\n" \
"lZf0QCPelrWmjPwYYXBj0OX4b4oSrq1ipeldF/Nr5aLc056UCsPiqPNeC2P29Bow\n" \
"KKDLEoWa9U6iSkejhgScp1lG/AvBg2+ev9adnQtVWl4Qhl2yOxfbnCUYaVF348Uc\n" \
"p0dsO9eMO4HtWEaFrSY3447HTyBFHVIva4DoQ/Cylr21PpCYk5JnZVS5nhSsm/Q5\n" \
"7qbTJmYfnC9IbBPxDJ8MbY/QeSeLL/JE3LpxfcBYMVawInxHd4kWAODkrqgWpzUC\n" \
"AwEAAaNTMFEwHQYDVR0OBBYEFO6CVRrKU5IcEfGvpkAujPwsmUzaMB8GA1UdIwQY\n" \
"MBaAFO6CVRrKU5IcEfGvpkAujPwsmUzaMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZI\n" \
"hvcNAQELBQADggEBAEnVdimRnB6hknVMlbV+adp1J1yP1l73pSzUD6F/ZkEQaPwM\n" \
"JxshaBE3swBE+8HBYfhb5AEOv00pffbigcXaYn1PS/c+pbJThOXw9sBnGfhVW6QP\n" \
"iaVve31TKKKYLT3TIJY0D4bsw5fuqKGTmdWuAeh+bOd/5Ac0a+3axBnJpgRjaNY1\n" \
"/T6/njd+rmVHAx6A5oM+JAbvOIFsOdExYuydh5iBxtpw08X9aSTLIFiC8VEJbIU1\n" \
"5HbwAJGf0OVAFMEs5nTwYszBG35AM7Uay5V8XyMRjWqqgWl92g5gXZh1gOFLLhiJ\n" \
"iRYomN6rI8lGWzukpeO1G0phRIkpup2+DGUo/s8=\n" \
"-----END CERTIFICATE-----\n"


/**
 *  client key to be updated by the user
 **/
#define CLIENT_KEY_PEM                                                          \
"-----BEGIN PRIVATE KEY-----\n" \
"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCn5T5EkVpC3NVp\n" \
"EPNpIuTI7sodtkoQGOwUbDOrDSo+lMw1Ws7iFih8V7oWwp15/JRcrMTqgyWgoRhL\n" \
"uO4lsIHjq5WX9EAj3pa1poz8GGFwY9Dl+G+KEq6tYqXpXRfza+Wi3NOelArD4qjz\n" \
"Xgtj9vQaMCigyxKFmvVOokpHo4YEnKdZRvwLwYNvnr/WnZ0LVVpeEIZdsjsX25wl\n" \
"GGlRd+PFHKdHbDvXjDuB7VhGha0mN+OOx08gRR1SL2uA6EPwspa9tT6QmJOSZ2VU\n" \
"uZ4UrJv0Oe6m0yZmH5wvSGwT8QyfDG2P0Hkniy/yRNy6cX3AWDFWsCJ8R3eJFgDg\n" \
"5K6oFqc1AgMBAAECggEAF6T/s21a7k6mWWETPou09501ZqDM5l50JN4mGqFEgUK4\n" \
"TY1H1Gw1cGl0Yg8vWUYaaMDGDJbCnlABmWxmUXV9nWAFNbymqcaHp/ZYJYqTcl4E\n" \
"RdbKcoH2CqH5Zpd/L39S/DAoN013BKYvj/J/HSlisCt3FUn0Bk/P2y6c0mDzl0N0\n" \
"Ai5YwW6yJeUql310qD8xNy4QDHE0l0pw4wKBP+4BbIxWX4Qd0mADNMv5J/Ma77Y3\n" \
"DNuK6ZE8I+K2ViAlrph49smnfrp3hzFXaQBnZPw9AwgHZvUmPER2nrV4DzHQFjBR\n" \
"5xJENJ/7t64BVj1x3dUtioKKpRx4MeNP4jE5ObsJUwKBgQDdiY8+FJO9yatZLtFS\n" \
"8+G0tzHziIWD7+wjrcI8h9djUDK4Q0WyiRttdwHpvmiwsupEhBDtn3PzEbbrW529\n" \
"Ra5igj5ueVBY2BY/Lw26qbzL7Ki57vN5ow2yyP4uhmLCC19PX4kWTrl3+0L27oh8\n" \
"2DIICLbXAmThuA3ggC1HqUqobwKBgQDCA3ck3NRxPdg0gJqmU2xzSONku8KpW1v1\n" \
"oaOG0v66myuBpxe6rcOR+6/eF7cOJtr9f6ndKozpe3U1IpLF1Siil1cS3xLpV7OA\n" \
"iHLV/vroVWybzcHE04bFfTLLjHpFau7Wq97ytWvfRxsqbN/43oXmTOA64pNmqtZm\n" \
"bYcrceYUmwKBgQDC/Bn5V/8ZIfEQU6alTkYv2rW6VUS+dWLvnDXDNQScQrABgiRM\n" \
"GWTsmbwzUQWv0CllGKAVpSxWmhbXXBGfzz7HMT9l2JyhNb+O6Qk6a77h9XAkBm6B\n" \
"dsPNwh4GMP2nMH84yytLWQrg0WOFFzlGOLsAWzsiTtcf2/FK0UYJNrWhPQKBgCxg\n" \
"Ooo18GmOabtohvbwMegUjd5mi9nMybXfKNYXwKfSAht/qo/KWuczGEDGT9puzLwa\n" \
"zTGHH//vexmSNNwHK2o1D3eDioHQy3/ktc94qM8natYgM+ffMf5N6qZgFT93e/Wi\n" \
"DjaZhHjnfLExgfEchKfugYM2e9yJysxW1Qn83VFtAoGAF+b34wIUyMGtMlzHihUH\n" \
"cZH+5Mgpug1BxjbiWQPA24S1Zy1vr659rRifRHBW98HvyEmnWkbLMyu9qDw6r9XE\n" \
"PNacojJkRE72Ejc8vJYcy3MSzv1apzR/9H4phHv5W5r5a9HP33LaYUUeeYjGKYqa\n" \
"lnUFFXX5Wv8D5gQIPZ1mD+A=\n" \
"-----END PRIVATE KEY-----\n"


/**
 *  @brief Trusted ROOT certificate can be update by following the process specified in the mark down file
 **/
#define IOT_DEMO_HTTPS_TRUSTED_ROOT_CA                               \
"-----BEGIN CERTIFICATE-----\n" \
"MIIEjTCCA3WgAwIBAgIQDQd4KhM/xvmlcpbhMf/ReTANBgkqhkiG9w0BAQsFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n" \
"MjAeFw0xNzExMDIxMjIzMzdaFw0yNzExMDIxMjIzMzdaMGAxCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
"b20xHzAdBgNVBAMTFkdlb1RydXN0IFRMUyBSU0EgQ0EgRzEwggEiMA0GCSqGSIb3\n" \
"DQEBAQUAA4IBDwAwggEKAoIBAQC+F+jsvikKy/65LWEx/TMkCDIuWegh1Ngwvm4Q\n" \
"yISgP7oU5d79eoySG3vOhC3w/3jEMuipoH1fBtp7m0tTpsYbAhch4XA7rfuD6whU\n" \
"gajeErLVxoiWMPkC/DnUvbgi74BJmdBiuGHQSd7LwsuXpTEGG9fYXcbTVN5SATYq\n" \
"DfbexbYxTMwVJWoVb6lrBEgM3gBBqiiAiy800xu1Nq07JdCIQkBsNpFtZbIZhsDS\n" \
"fzlGWP4wEmBQ3O67c+ZXkFr2DcrXBEtHam80Gp2SNhou2U5U7UesDL/xgLK6/0d7\n" \
"6TnEVMSUVJkZ8VeZr+IUIlvoLrtjLbqugb0T3OYXW+CQU0kBAgMBAAGjggFAMIIB\n" \
"PDAdBgNVHQ4EFgQUlE/UXYvkpOKmgP792PkA76O+AlcwHwYDVR0jBBgwFoAUTiJU\n" \
"IBiV5uNu5g/6+rkS7QYXjzkwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQGCCsG\n" \
"AQUFBwMBBggrBgEFBQcDAjASBgNVHRMBAf8ECDAGAQH/AgEAMDQGCCsGAQUFBwEB\n" \
"BCgwJjAkBggrBgEFBQcwAYYYaHR0cDovL29jc3AuZGlnaWNlcnQuY29tMEIGA1Ud\n" \
"HwQ7MDkwN6A1oDOGMWh0dHA6Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEds\n" \
"b2JhbFJvb3RHMi5jcmwwPQYDVR0gBDYwNDAyBgRVHSAAMCowKAYIKwYBBQUHAgEW\n" \
"HGh0dHBzOi8vd3d3LmRpZ2ljZXJ0LmNvbS9DUFMwDQYJKoZIhvcNAQELBQADggEB\n" \
"AIIcBDqC6cWpyGUSXAjjAcYwsK4iiGF7KweG97i1RJz1kwZhRoo6orU1JtBYnjzB\n" \
"c4+/sXmnHJk3mlPyL1xuIAt9sMeC7+vreRIF5wFBC0MCN5sbHwhNN1JzKbifNeP5\n" \
"ozpZdQFmkCo+neBiKR6HqIA+LMTMCMMuv2khGGuPHmtDze4GmEGZtYLyF8EQpa5Y\n" \
"jPuV6k2Cr/N3XxFpT3hRpt/3usU/Zb9wfKPtWpoznZ4/44c1p9rzFcZYrWkj3A+7\n" \
"TNBJE0GmP2fhXhP1D/XVfIW/h0yCJGEiV9Glm/uGOa3DXHlmbAcxSyCRraG+ZBkA\n" \
"7h4SeM6Y8l/7MBRpPCz6l8Y=\n" \
"-----END CERTIFICATE-----\n"


/**
 *  End of User Configurable Settings
 **/


/* ENABLE, DIABLE MACROs */
#define ENABLE      (1)
#define DISABLE     (0)

/* Ethernet setup macros */
#define USR_PING_COUNT              (50)
#define PING_DELAY                  (10)
#define TASK_DELAY                  (100)
#define BUFF_SIZE                   (16)
#define EP_INFO                 "\r\n This project demonstrates the basic functionality of AWS https client library with Ethernet driver"\
		                        "\r\n on Renesas RA MCUs based on Renesas FSP using FreeRTOS. AWS client is used to connect to https adafruit"\
		                        "\r\n server which is cloud platform. On successful connection, menu is displayed enabling user to send"\
                                "\r\n GET,PUT,POST requests to adafruit server. On POST/PUT request, ADC output is read and uploaded to "\
                                "\r\n adafruit server. On GET request, the last updated data is received from adafruit. JlinkRTTViewer is "\
                                "\r\n used to display the status and responses of the requests made to server.\r\n"

#define PRINT_MENU              "\r\nSelect from the below menu options "\
                                "\r\n 1. GET Request"\
                                "\r\n 2. PUT Request"\
                                "\r\n 3. POST Request\r\n"


typedef struct st_ping_data
{
    uint32_t sent;     // Ping Request
    uint32_t received; // Ping Response
    uint32_t lost;     // Ping failure
} ping_data_t;

typedef enum Userinput
{
    GET = 1,
    PUT = 2,
    POST = 3
}Userinput_t;

#if( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
    /* DHCP has an option for clients to register their hostname.  It doesn't
    have much use, except that a device can be found in a router along with its
    name. If this option is used the callback below must be provided by the
    application writer to return a const string, denoting the device's name. */
    const char *pcApplicationHostnameHook( void );
#endif /* ipconfigDHCP_REGISTER_HOSTNAME */

/*
 * function declarations
 */
void print_ipconfig(void);
void print_pingResult(void);
void dnsQuerryFunc(char *domain_name);
void updateDhcpResponseToUsr(void);
BaseType_t vSendPing(const char *pcIPAddress);
BaseType_t provision_alt_key(void);
eDHCPCallbackAnswer_t  xApplicationDHCPHook(eDHCPCallbackPhase_t eDHCPPhase, uint32_t lulIPAddress);

#endif /* USER_APP_H_ */
