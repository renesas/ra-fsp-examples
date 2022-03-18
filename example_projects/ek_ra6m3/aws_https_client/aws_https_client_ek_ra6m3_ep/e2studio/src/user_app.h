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
#define IOT_DEMO_HTTPS_PRESIGNED_GET_URL   "https://io.adafruit.com/api/v2/mnkraj1/feeds/temp/data?limit=1"

/** @brief IOT_DEMO_HTTPS_PRESIGNED_PUT_POST_URL can be used in PUT and POST methods.
 *  PUT method will update data point at requested <id>.
 *  PUT url: https://ioadafruit.com/{user_name}/{feeds}/{feed_key}/data/{id} , where the <id> will append to the url in the process of
 *  PUT request, once it fetch from the GET request.
 *  POST method will send new data to the server.
 *  POST url:https://ioadafruit.com/{user_name}/{feeds}/{feed_key}/data/
 **/
#define IOT_DEMO_HTTPS_PRESIGNED_PUT_POST_URL    "https://io.adafruit.com/api/v2/mnkraj1/feeds/temp/data/"

/** @brief User has to update their generated active key from the io.adafruit.com server. */
#define ACTIVE_KEY                             "aio_YUcA19S8MKK2a9p1fi6iIabzdk3J"

/** @brief If the privacy of feed data visibility is set to private then 'ENABLE' the PRIVATE_KEY for processing of GET request **/
#define PRIVATE_KEY                        DISABLE


/**
 *  Client certificate to be updated by the user by following the process specified in the mark down file
 **/


 #define CLIENT_CERTIFICATE_PEM                                                  \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDqTCCApGgAwIBAgIUOJcbD9G13cSpoC53k5hXcE1oxpgwDQYJKoZIhvcNAQEL\n"\
"BQAwZDELMAkGA1UEBhMCSU4xCzAJBgNVBAgMAktBMRIwEAYDVQQHDAlCZW5nYWx1\n"\
"cnUxDDAKBgNVBAoMA3RhdDEMMAoGA1UECwwDb2RjMRgwFgYDVQQDDA9pby5hZGFm\n"\
"cnVpdC5jb20wHhcNMjIwMTI0MDk0MDM0WhcNMjMwMTI0MDk0MDM0WjBkMQswCQYD\n"\
"VQQGEwJJTjELMAkGA1UECAwCS0ExEjAQBgNVBAcMCUJlbmdhbHVydTEMMAoGA1UE\n"\
"CgwDdGF0MQwwCgYDVQQLDANvZGMxGDAWBgNVBAMMD2lvLmFkYWZydWl0LmNvbTCC\n"\
"ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKyZrAbXelL11YwIoO7oemQp\n"\
"FErwlMghzCRY9/OyohAkzVxLXmmzXFpIQ0tMV8PCmI5+S5E614+hxQiFsw5e1v2Q\n"\
"uEr3cjw47MyrA+7bGdEV4hGWimnPj03NDzrOuIQdVgLuzGodMNegsGenqawnqX5T\n"\
"GAGZiAwOkwUsDDYvL6OsfzEAnlNZRK3U/Pal9iJnn1M/Os61O8jRof94tHIJ3ia5\n"\
"uyeqju+yC8TNw0QFW3lqni8U1FZhWRFyfKoOZRoi5oltVHGAgB7A2fbVNQT9GHo6\n"\
"cbdwJhlARHIVt1bsH6n+unHAd1yjuDC3ZeH/Frc7H3WejW2vrZZnp9LXJeYBMJ0C\n"\
"AwEAAaNTMFEwHQYDVR0OBBYEFFMnjasWBaSkHS9Lq7wUPFhpN6myMB8GA1UdIwQY\n"\
"MBaAFFMnjasWBaSkHS9Lq7wUPFhpN6myMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZI\n"\
"hvcNAQELBQADggEBABAVR117gDPGekYW1ZY455sWj3m1mVZopOuOzo/PQ86JMVgk\n"\
"BTsylB4V50jv/4ROn62VA3bKDFE80KUjKOBxWezhU9ghxLnhvyBO8mT9ilrBaspx\n"\
"oVUtNXG/sbcjPCOmVw7YWFtgC+xtLz/By3Yi6EAawkggVi4gONsPnGkptn17FG7p\n"\
"2Gc/QSuqw7Dj72C+R9oVvIoCIH/1dUUMxDc07WY5+Vf+Iw7qlccIjBdK/PE3lEW6\n"\
"wmlCtJ1jxnLb7DqRM0Z2JPxplff7/D06lWMQJA8gqD3+EKoRNyDbsF+nI1o/Q9td\n"\
"TxAsw4gV/Tb/d3DG5I8y0PHbyBN0GbY2URgp92k=\n"\
"-----END CERTIFICATE-----\n"



/**
 *  client key to be updated by the user
 **/

#define CLIENT_KEY_PEM                                                          \
"-----BEGIN PRIVATE KEY-----\n" \
"MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCsmawG13pS9dWM\n" \
"CKDu6HpkKRRK8JTIIcwkWPfzsqIQJM1cS15ps1xaSENLTFfDwpiOfkuROtePocUI\n" \
"hbMOXtb9kLhK93I8OOzMqwPu2xnRFeIRloppz49NzQ86zriEHVYC7sxqHTDXoLBn\n" \
"p6msJ6l+UxgBmYgMDpMFLAw2Ly+jrH8xAJ5TWUSt1Pz2pfYiZ59TPzrOtTvI0aH/\n" \
"eLRyCd4mubsnqo7vsgvEzcNEBVt5ap4vFNRWYVkRcnyqDmUaIuaJbVRxgIAewNn2\n" \
"1TUE/Rh6OnG3cCYZQERyFbdW7B+p/rpxwHdco7gwt2Xh/xa3Ox91no1tr62WZ6fS\n" \
"1yXmATCdAgMBAAECggEAHYOyr8HRlFWe5HEGSgw9f5qpsm3k1zs2TCHdlpHdliqg\n" \
"5k8qECOgX3zbawkdOeexDC7LD8TS3ygPyBcKySKNG3OPnBwnr+YhEheNVt8nEUKv\n" \
"c4f9V74rOGfgGpOGaGsWze4VTPD8bOb4u2Tnhf2LRYOCh6WAJuWoHNqNJQfZGuDo\n" \
"r7ZYfrHadW7UrHG9Za/1YuOKzEYeYHuArwyiqV9TL9InqvRA/4Eh3BhXKMa3rHIA\n" \
"GE2y4tvNkgW3Rwml4JwYlbS/FVcUhiLUtxdvZevaaCSQhX1rfuY0bpcToDAHM6az\n" \
"GtG9sfxAEjVNy+K9OW/h+Gs2cjoOjaulqUdEm250IQKBgQDcWFhQRMZgLE6z9VU0\n" \
"Hv+thExCnFNO4CMb6pl0pzrPYkuWEOS3My7WBxOIu4/Yn5bsVIKvQ9qBjBJC+ZzM\n" \
"5e0ZucU9ATnIw5dbiXLEI9VXjeXoZTE9PL9FGsMnpRc3VQFVeEBFif0g/uWhcI58\n" \
"ewr9VRHy0hggEKegyUasdyZZfQKBgQDIh4dJKWyHg/TbKYWTS2frh6sAvpMAP82s\n" \
"MFV8xb5dELsR+Oa0OeWX/yhhGfZ1GeYMaM9Tkw/kb+zUtcMhVSI/VFuNziW42mW0\n" \
"6Rbi/zMzunVEaEMrlF2bACP8BIJ/M+LlXv9VtdiztD8baMOnG+nhcHIKSOMyrjrN\n" \
"lQ8da8rdoQKBgBsgNR8MmG9WhQnadfbIqikuVEUPAaKALVJOez3FKJRHQY7j3YRA\n" \
"JM/sZxc6H+dHHklRzhTNgaTZHZZpLEh9Ncco+4MbF4YDripyrVJAJw90RI1rh7hG\n" \
"NjXHqabDyCvjpxpih2cIKcVaG+LX/HKOoze0tVTzUE0CP+CnZq/sqyfxAoGAOaDJ\n" \
"PjtLVz6S1ZD/NC9NdCCPAi3CbSXo430YdD0X0gMG38lZ851X3/CSGCWv8EM7G4Hb\n" \
"Yzc2L/4CX7bElwWyS+TgUsZVjOQmqnD6BM42Vr1VCg20kAeZRvwpWzmojwnIQQal\n" \
"9mnGFw7LvALFh3a8Je98tnvv3xa3C3G04E2kBUECgYBcOyq1rDlVr2htaMqYzr2W\n" \
"Hld1nADi2XBF6xZ/DwDRDOnkxdKJuq/wZztaypfzZ1l4d0qYjsmemrbgrGCYok26\n" \
"naTFWbKl5F5zC+VAWEL2bZYozS789/+z9W4QM6VZvTD5v7flWxtfsFQzwcJJxEYu\n" \
"tMhqddSy521sKRKa7BTRyA==\n" \
"-----END PRIVATE KEY-----\n"


/**
 *  @brief Trusted ROOT certificate can be update by following the process specified in the mark down file
 **/

#define IOT_DEMO_HTTPS_TRUSTED_ROOT_CA                                \
"-----BEGIN CERTIFICATE-----\n"                                       \
"MIIEizCCA3OgAwIBAgIQBUb+GCP34ZQdo5/OFMRhczANBgkqhkiG9w0BAQsFADBh\n"  \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"  \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"  \
"QTAeFw0xNzExMDYxMjIzNDVaFw0yNzExMDYxMjIzNDVaMF4xCzAJBgNVBAYTAlVT\n"  \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"  \
"b20xHTAbBgNVBAMTFEdlb1RydXN0IFJTQSBDQSAyMDE4MIIBIjANBgkqhkiG9w0B\n"  \
"AQEFAAOCAQ8AMIIBCgKCAQEAv4rRY03hGOqHXegWPI9/tr6HFzekDPgxP59FVEAh\n"  \
"150Hm8oDI0q9m+2FAmM/n4W57Cjv8oYi2/hNVEHFtEJ/zzMXAQ6CkFLTxzSkwaEB\n"  \
"2jKgQK0fWeQz/KDDlqxobNPomXOMJhB3y7c/OTLo0lko7geG4gk7hfiqafapa59Y\n"  \
"rXLIW4dmrgjgdPstU0Nigz2PhUwRl9we/FAwuIMIMl5cXMThdSBK66XWdS3cLX18\n"  \
"4ND+fHWhTkAChJrZDVouoKzzNYoq6tZaWmyOLKv23v14RyZ5eqoi6qnmcRID0/i6\n"  \
"U9J5nL1krPYbY7tNjzgC+PBXXcWqJVoMXcUw/iBTGWzpwwIDAQABo4IBQDCCATww\n"  \
"HQYDVR0OBBYEFJBY/7CcdahRVHex7fKjQxY4nmzFMB8GA1UdIwQYMBaAFAPeUDVW\n"  \
"0Uy7ZvCj4hsbw5eyPdFVMA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEF\n"  \
"BQcDAQYIKwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADA0BggrBgEFBQcBAQQo\n"  \
"MCYwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBCBgNVHR8E\n"  \
"OzA5MDegNaAzhjFodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRHbG9i\n"  \
"YWxSb290Q0EuY3JsMD0GA1UdIAQ2MDQwMgYEVR0gADAqMCgGCCsGAQUFBwIBFhxo\n"  \
"dHRwczovL3d3dy5kaWdpY2VydC5jb20vQ1BTMA0GCSqGSIb3DQEBCwUAA4IBAQAw\n"  \
"8YdVPYQI/C5earp80s3VLOO+AtpdiXft9OlWwJLwKlUtRfccKj8QW/Pp4b7h6QAl\n"  \
"ufejwQMb455OjpIbCZVS+awY/R8pAYsXCnM09GcSVe4ivMswyoCZP/vPEn/LPRhH\n"  \
"hdgUPk8MlD979RGoUWz7qGAwqJChi28uRds3thx+vRZZIbEyZ62No0tJPzsSGSz8\n"  \
"nQ//jP8BIwrzBAUH5WcBAbmvgWfrKcuv+PyGPqRcc4T55TlzrBnzAzZ3oClo9fTv\n"  \
"O9PuiHMKrC6V6mgi0s2sa/gbXlPCD9Z24XUMxJElwIVTDuKB0Q4YMMlnpN/QChJ4\n"  \
"B0AFsQ+DU0NCO+f78Xf7\n"  \
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

void print_ipconfig(void);
void print_pingResult(void);
void dnsQuerryFunc(char *domain_name);
void updateDhcpResponseToUsr(void);
BaseType_t vSendPing(const char *pcIPAddress);
BaseType_t provision_alt_key(void);
eDHCPCallbackAnswer_t  xApplicationDHCPHook(eDHCPCallbackPhase_t eDHCPPhase, uint32_t lulIPAddress);



#endif /* USER_APP_H_ */
