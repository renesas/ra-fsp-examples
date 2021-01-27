/***********************************************************************************************************************
 * File Name    : usr_config.h
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#ifndef USR_CONFIG_H_
#define USR_CONFIG_H_

#define USR_LOG_LVL          (LOG_ERROR)     /* User Options are:  LOG_ERROR, LOG_WARN, LOG_INFO, , LOG_DEBUG */
#define USR_LOG_TERMINAL     (RTT_TERMINAL)  /* User Options are:  RTT_TERMINAL, ITM_TERMINAL */
#define USR_MQTT_DATA_FORMAT (JSON)          /* JSON, UTF8  */

#define DEBUG 1

/***********************************************************************************************************************
 * Wi-FI Credentials for the Module to Connect to the AP/Router. User needs to Modify these according to the Wi-Fi     *
 * Settings available to you for the Wi-Fi connection.                                                                 *
 ***********************************************************************************************************************/

#define WIFI_SSID "Renesas"
#define WIFI_PW   "@Renesas123"
#define SECURITY  "WPA2"                /* User Options are: "Open", "WEP", "WPA", "WPA2" */

/***********************************************************************************************************************
 * AWS MQTT End point  detail for the application. These needs to be updated based on the endpoint from your device
 * creation using your aws end point.
 ***********************************************************************************************************************/

#define USR_MQTT_ENDPOINT "aoh5lvd4o23ku-ats.iot.us-east-1.amazonaws.com"

/***********************************************************************************************************************
 * AWS Server Certificate obtained as part of the device creation as documented in the Application Note
 * Users are required to modify this as required for the device. 
 ***********************************************************************************************************************/

#define SERVER_CERTIFICATE                                          \
"-----BEGIN CERTIFICATE-----\n"                                     \
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"\
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"\
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"\
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"\
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"\
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"\
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"\
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"\
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"\
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"\
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"\
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"\
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"\
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"\
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"\
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"\
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"\
"rqXRfboQnoZsG4q5WTP468SQvvG5\n"                                    \
"-----END CERTIFICATE-----";

/***********************************************************************************************************************
 * AWS Client Certificate obtained as part of the device creation as documented in the Application Note
 * Users are required to modify this as required for the device. 
 ***********************************************************************************************************************/
#define CLIENT_CERTIFICATE                                          \
"-----BEGIN CERTIFICATE-----\n"                                     \
"MIIDWjCCAkKgAwIBAgIVAPlSFZkRauTV7grMyxS1eSx1hLRFMA0GCSqGSIb3DQEB\n"\
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n"\
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDAzMDkyMjAz\n"\
"MzVaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n"\
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDH9dyW4WDmsimx63Dy\n"\
"R+T2BQkEdEQEzXgDxMOeMYmkhF7W8f2pQOEE9L2UOxVXe/3vEJfjruSyBSplR7x1\n"\
"cUxL8P3qWjHVbcIjm1Aa4vYYnpPgChfQTwvOOd0MjeWspbidtrXO2BNJGUeXXDdN\n"\
"J1FwuI8AWkGsQj5BzHvk6qFeU/LW1q5in1zYPYgKad/arzrdeHR12BB4aF2f2rba\n"\
"UrRJCviXNF/1I1iPegKjri5RxKSwi/Xiszxv1kdTBBNrS2IMNVvghRpMWfXHIECF\n"\
"MRMnlV1k2WmtiEoW7RDHoT2eR2om0sk+YCyCepmmzlN4vCJ6EtAznWVcPuzIudVC\n"\
"w20hAgMBAAGjYDBeMB8GA1UdIwQYMBaAFKQiDz2qsyG7AQVeF2/CdZUJCC4KMB0G\n"\
"A1UdDgQWBBTSeHTqi8IFv3+vq5Kt6q3ZAU8cPzAMBgNVHRMBAf8EAjAAMA4GA1Ud\n"\
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAgLT0EAER2gloT0utc2LfqhyY\n"\
"6+/05EPyQUhSO/FtT8uwvbKpbBFlAr+L9EcikpijAf+xTuslOf8rrXI/ZJ/6XmuK\n"\
"a1bRm6fDmM+Qso7DvqSnmYoumsZAdIypnjHIk3Ii0tbTpVVKT3CV1oFurCRRlRa8\n"\
"utbaok/s/4yT73hYzHoRjq4nygxknSK0tPwDCMo0Dj1LGGgVxNR0sPsGm/P3JNmk\n"\
"j3/E+bgRkZFJqr6+6J7yUtmdkF90a1DsncSKDwqGIgGDFKCQioTxigRtxFwyjgtb\n"\
"1tbMehEqophWgWzXE+abMkN8gzez784I5LimyA+GtP8jpWgZprELQTQiIbutVQ==\n"\
"-----END CERTIFICATE-----\n";

/***********************************************************************************************************************
 * AWS Client Key obtained as part of the device creation as documented in the Application Note
 * Users are required to modify this as required for the device. 
 ***********************************************************************************************************************/
#define CLIENT_KEY                                                  \
"-----BEGIN RSA PRIVATE KEY-----\n"                                 \
"MIIEpQIBAAKCAQEAx/XcluFg5rIpsetw8kfk9gUJBHREBM14A8TDnjGJpIRe1vH9\n"\
"qUDhBPS9lDsVV3v97xCX467ksgUqZUe8dXFMS/D96lox1W3CI5tQGuL2GJ6T4AoX\n"\
"0E8LzjndDI3lrKW4nba1ztgTSRlHl1w3TSdRcLiPAFpBrEI+Qcx75OqhXlPy1tau\n"\
"Yp9c2D2ICmnf2q863Xh0ddgQeGhdn9q22lK0SQr4lzRf9SNYj3oCo64uUcSksIv1\n"\
"4rM8b9ZHUwQTa0tiDDVb4IUaTFn1xyBAhTETJ5VdZNlprYhKFu0Qx6E9nkdqJtLJ\n"\
"PmAsgnqZps5TeLwiehLQM51lXD7syLnVQsNtIQIDAQABAoIBAG1qbmMtPk3eDYyq\n"\
"zWn1xyaLyMxkJlgOam59e6ETcELSn+a0Ed5MfxSsKxRyZBIz6dan2MUgjUmQJAu5\n"\
"fjkxXtNrMjXLVtt3rLS5JtMfgBQmqbn9K9yzdD1LUlatSpD3OZcrRD7D44GrUI5V\n"\
"SnlwW1L32mASAz+UG+b7UowHi3dGNmAICQEL1qIdoQdlbuqZwNGpbEJzdJ4lZ1hG\n"\
"xOINwTuQffgFi2KM9qTkxv0kn52sx5skLhh9ci+Jblp4nTjCW8131Hk27apKNG37\n"\
"2hwH04Zdoh6nujblMR2B/MAWfJYgC1KT87OtXylqBke2Of8UIjZFaA90uXVP6cdU\n"\
"unPQuAECgYEA8APFjXiI6Qo7uPNNxsh+CJcLLZFzYlv2gbgKPmD++zr8UluB4mn0\n"\
"pft2wi18l4RUycuGWicYgpbe9nTjcEZXl35ymzxg3cxnfvFVv+AQo1Wu2YoJkCdU\n"\
"ykSXReo45hgia8qfpTUvH1SQYBroKT3GYdBaELNm8LJM+7Cpg4bSQUECgYEA1Ucq\n"\
"2VWXEngeq2k3KmHRvf9WlWlNm4Dg7RcxYlS+cap3sO4ujO8Zlk2LORS0HxeIlqu+\n"\
"Il0KqaZmAHcqp7BjTOoRAc9QOcdFQrM1kygM6msLmNVPIEVsBB/0BPHQu6k0hi7q\n"\
"zXXxD8IIseC7QahCYIYH8S/eVFNM40k7N5TQU+ECgYEAzI2KWm210VbFW3UQfi/1\n"\
"KgEAp89klRlnV1qwcEVeSmMVFGOkqIBA3w0XQ5pcqw0+7dSlJkRXoURPtaAdDl0E\n"\
"3boIlsC4lY9VTV0FBchCpdGqeQAp8k9y9gfLHOznnUJIY7GGqdhP1tXNTfrgjfV2\n"\
"E3rsp3sealirP35qmgvXOYECgYEAgJP1Gy1kdHgCVWiLH3v1jMvhERZ4+STLdWQk\n"\
"G5nnbnNM0VfmrISBMoHxSypWJOB1MhC8Ac+B3CE7IVC2YH4Qf5AlTKgFP7AI61tL\n"\
"Ezpz/ucmCK+gkl4H7rysykZWhqxNxA+I8vEFsXBS+Zn4im68BaEILLmlGcVWdzfx\n"\
"AWMv7UECgYEA71YTiyOzmnNp2gRKaRVi1aA4GfpJwVVxb5KCnFRuciZkx/pvKzD3\n"\
"ix76GlG7j7aLE+J+JvvS0gwi1Ku1C36aBwBRdfXDRtzqJeClR+Xza5EZ4pL0G3j2\n"\
"Xdv67gDeK6IWjxLCn+aU6Tp762IaBNSBkHNHDUuGiz/XaKpIAH8WtSU=\n"        \
"-----END RSA PRIVATE KEY-----\n";
#endif /* USR_CONFIG_H_ */
