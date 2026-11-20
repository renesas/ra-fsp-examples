/***********************************************************************************************************************
 * File Name    : user_app.h
 * Description  : Contains the user configurable settings of the EP (server, credentials and certificates).
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USER_APP_H_
#define USER_APP_H_

#include <stdint.h>

/* The user has to update according to their LAN/WAN test setup for testing of ping IP address */
#define HTTPS_TEST_PING_IP              "8.8.8.8"

/* Host address */
#define HTTPS_HOST_ADDRESS              "io.adafruit.com"

/* TLS port for HTTPS */
#define HTTPS_PORT                      ((uint16_t) 443U)

/* Number of times to retry the HTTPS connection. A connection is only attempted again if
 * TLS_TRANSPORT_SUCCESS is not returned from TLS_FreeRTOS_Connect(). This indicates an error in the network
 * layer. */
#define HTTPS_CONNECTION_NUM_RETRY      ((uint32_t) 5)

/* Timeout in milliseconds of the socket send and receive operations */
#define SOCKET_SEND_RECV_TIME_OUT_MS    ((uint32_t) 10000)

/* Client certificate to be updated by the user by following the process specified in the markdown file */
#define CLIENT_CERTIFICATE_PEM  		"-----BEGIN CERTIFICATE-----\n"\
		                        		"MIID9zCCAt+gAwIBAgIUEJWi4aQTJ79x7GsCX4imnMyyoOYwDQYJKoZIhvcNAQEL\n"\
										"BQAwgYoxCzAJBgNVBAYTAlZOMQswCQYDVQQIDAJTRzEPMA0GA1UEBwwGU2FpZ29u\n"\
										"MQwwCgYDVQQKDANSVkMxDDAKBgNVBAsMA3NzMjEYMBYGA1UEAwwPaW8uYWRhZnJ1\n"\
										"aXQuY29tMScwJQYJKoZIhvcNAQkBFhhjYW5oLnRyYW4ueGpAcmVuZXNhcy5jb20w\n"\
										"HhcNMjMwMTA5MDk0NjQxWhcNMjQwMTA5MDk0NjQxWjCBijELMAkGA1UEBhMCVk4x\n"\
										"CzAJBgNVBAgMAlNHMQ8wDQYDVQQHDAZTYWlnb24xDDAKBgNVBAoMA1JWQzEMMAoG\n"\
										"A1UECwwDc3MyMRgwFgYDVQQDDA9pby5hZGFmcnVpdC5jb20xJzAlBgkqhkiG9w0B\n"\
										"CQEWGGNhbmgudHJhbi54akByZW5lc2FzLmNvbTCCASIwDQYJKoZIhvcNAQEBBQAD\n"\
										"ggEPADCCAQoCggEBANHjj+OUKVYGDYbd1N2dcHZy5aeMpKzRfED8+MlQsoXJBwP0\n"\
										"6Xl8485Klo19qrFGD+NsiGylSdF5lg87iICxkCShTPyYEB5gN29wVIzgPv8HiKLa\n"\
										"Zm66BF2yTcXRbFj36BplqsTq36hiqoJeTaTXnbjGn3Mf/fdb/UIoy0zgCZzXP9Yo\n"\
										"2DlEacp0L+ORisH+J5I8cP4Q8eD04tHr3rSp5pR9SOAh8tIeWxm6VD1/rK8IFFN8\n"\
										"F9Mv/S+p75hPEwrPlhV5XmR9BSFD2Yakc6fbiECnRtyUBgMpKinK+XC98EI3iWh+\n"\
										"rNJONNSRyUMsKXbIpK+KQB80IE/+j3PEPyS7G0ECAwEAAaNTMFEwHQYDVR0OBBYE\n"\
										"FAeoe9a6wgYcmHfXZU0HzuSmOLq7MB8GA1UdIwQYMBaAFAeoe9a6wgYcmHfXZU0H\n"\
										"zuSmOLq7MA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAFxzzHeK\n"\
										"jp4RXUKmXiVnR2XyXVV5Ev5c05OiKd7PTRynhzpbPAoCbiM902RD83ZBA5EzhvK1\n"\
										"cs6/7E8O65PQJjmFKDpfLV7mG+iRzf5+0vn6U4oDMjSpp2mDVdQi8Si4DHiyOkXZ\n"\
										"w/LlDQ0UwVQDJ5zMAAJw9Yrz6Tltkpi5xMK0fnrW8LMyNpJj40Bic7trWu7TTW+w\n"\
										"ls0g/AR7JxlOvAIFdA/VixcalaonTjISD/rm+sPzNNPwC2gzxTyq+1P9uHVGIe1m\n"\
										"I+GA6PmFG28G6TwL81knzYzQppZEwatE0ripTWZ9t4hdsD7tmnZh2y+sZ+BvjTdV\n"\
										"JxlM3y7ATO95Tp4=\n"\
										"-----END CERTIFICATE-----\n"

/* Client key to be updated by the user */
#define CLIENT_KEY_PEM          		"-----BEGIN PRIVATE KEY-----\n"\
		                        		"MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDR44/jlClWBg2G\n"\
										"3dTdnXB2cuWnjKSs0XxA/PjJULKFyQcD9Ol5fOPOSpaNfaqxRg/jbIhspUnReZYP\n"\
										"O4iAsZAkoUz8mBAeYDdvcFSM4D7/B4ii2mZuugRdsk3F0WxY9+gaZarE6t+oYqqC\n"\
										"Xk2k1524xp9zH/33W/1CKMtM4Amc1z/WKNg5RGnKdC/jkYrB/ieSPHD+EPHg9OLR\n"\
										"6960qeaUfUjgIfLSHlsZulQ9f6yvCBRTfBfTL/0vqe+YTxMKz5YVeV5kfQUhQ9mG\n"\
										"pHOn24hAp0bclAYDKSopyvlwvfBCN4lofqzSTjTUkclDLCl2yKSvikAfNCBP/o9z\n"\
										"xD8kuxtBAgMBAAECggEADnGYj1K83XyhJazSX0XcXWpev38APhxUbqx5+nChOG+z\n"\
										"GeSaxI+BpcqgPbBuNLDbmwz2/nMJd27kkrvIEwMlJkArah6UzWJVrkeOqfssmqRM\n"\
										"FNjuv6PxdPk2POzChgTZZiek7tQy/AV+MmaG9VcZNYQFqQiEk6cy8ICaLPol7zMI\n"\
										"8StwpadhF03SfOWv44RH8pdCFQEEpZS0/8R8kulF6WDc/HZOZx0XN0ExSDV+gi3N\n"\
										"MSOpaHVANuwJI0CHhcrM+sObTy0cwpVIvZK5ydWxsVNLhOBf8YOwp9moZkoOh6Fp\n"\
										"oJEYw43GcEWmOyjDQ+iVOMVRBm4Q5Ed6Gv/GUdkIgQKBgQD0LxhditcBHfrln3gc\n"\
										"7kUSGL1HruYCEDHCtt9LADzdC+iavuGJpZklEM7Y8k6Pp4WUQhYIeJl32dpGWgMh\n"\
										"4EbhoxXSLCa1E3bX1k5V9lcuWrcBI7qKZM02WbpidGGQFUWF6lUML1t4ugjUIrE3\n"\
										"gcOZ0eV7ghciA2jhwdfUVQuiSQKBgQDcC6BTkQXU0yJXZOX1FMNx8sl7fKEF60AV\n"\
										"hVaTgw0PvY2e9K0egc6Ho9vl7ZpPkRXHTv6bcAHDpqCLbTOahycHdWu3p+imk981\n"\
										"hXQ8imFMcb70UIaQDzluVTZhLX3RuARyaI06MxhP6UKLDFRvl3u52GQR8GEInEI3\n"\
										"Jbnv4nQxOQKBgQDpa3unL+mHr9j/WAnB3iM4wV1vAMnufvySXa3mTDviMlgQVRfU\n"\
										"NPDIdeDFkht3R3Rx3RqizmYmI6J8Td9gqrwXRYa+emYmAo1AliQcGHgOX7lA5Z6U\n"\
										"XPeuVOfO5eG9khf7g535H+wY4/08jE1L+ke46uSrxBfI8+l2pY5j80hDwQKBgQC1\n"\
										"vSR94JlgXcFPTRZghPWnd07Rnu2fZd1X6PjIfMSc64G8d3lEaRGbFcrY7JU2gi3q\n"\
										"n2PKHYhXf5e5kEe+qUGi92gwF44fhqIkIkUOe3xBFvEP+DIraWeVqOtw95GjVMjX\n"\
										"dWKvXuQFnDJJhbV3NTYLGJ4N8usxoV1PDumzJuWwgQKBgGDvZs1eQ6mkbAoxydRT\n"\
										"6Vp324k/KHmS/yv3uvRiGEYeMePwTo0kszZCpEW0z3r5IiqIwV8x9aUb+SAoBEIC\n"\
										"Z1gUZFeHkT75SLmHhr0xs27OogMCqEvIVQqDju7pvCp0atbcoO+UdaqT7X2MQ2bs\n"\
										"D5OdQJo5LRFBMCBoQJCazsed\n"\
										"-----END PRIVATE KEY-----\n"

/* Trusted root certificate can be updated by following the process specified in the markdown file */
#define HTTPS_TRUSTED_ROOT_CA   		"-----BEGIN CERTIFICATE-----\n"\
		                        		"MIIEjTCCA3WgAwIBAgIQDQd4KhM/xvmlcpbhMf/ReTANBgkqhkiG9w0BAQsFADBh\n"\
										"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"\
										"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n"\
										"MjAeFw0xNzExMDIxMjIzMzdaFw0yNzExMDIxMjIzMzdaMGAxCzAJBgNVBAYTAlVT\n"\
										"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"\
										"b20xHzAdBgNVBAMTFkdlb1RydXN0IFRMUyBSU0EgQ0EgRzEwggEiMA0GCSqGSIb3\n"\
										"DQEBAQUAA4IBDwAwggEKAoIBAQC+F+jsvikKy/65LWEx/TMkCDIuWegh1Ngwvm4Q\n"\
										"yISgP7oU5d79eoySG3vOhC3w/3jEMuipoH1fBtp7m0tTpsYbAhch4XA7rfuD6whU\n"\
										"gajeErLVxoiWMPkC/DnUvbgi74BJmdBiuGHQSd7LwsuXpTEGG9fYXcbTVN5SATYq\n"\
										"DfbexbYxTMwVJWoVb6lrBEgM3gBBqiiAiy800xu1Nq07JdCIQkBsNpFtZbIZhsDS\n"\
										"fzlGWP4wEmBQ3O67c+ZXkFr2DcrXBEtHam80Gp2SNhou2U5U7UesDL/xgLK6/0d7\n"\
										"6TnEVMSUVJkZ8VeZr+IUIlvoLrtjLbqugb0T3OYXW+CQU0kBAgMBAAGjggFAMIIB\n"\
										"PDAdBgNVHQ4EFgQUlE/UXYvkpOKmgP792PkA76O+AlcwHwYDVR0jBBgwFoAUTiJU\n"\
										"IBiV5uNu5g/6+rkS7QYXjzkwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQGCCsG\n"\
										"AQUFBwMBBggrBgEFBQcDAjASBgNVHRMBAf8ECDAGAQH/AgEAMDQGCCsGAQUFBwEB\n"\
										"BCgwJjAkBggrBgEFBQcwAYYYaHR0cDovL29jc3AuZGlnaWNlcnQuY29tMEIGA1Ud\n"\
										"HwQ7MDkwN6A1oDOGMWh0dHA6Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEds\n"\
										"b2JhbFJvb3RHMi5jcmwwPQYDVR0gBDYwNDAyBgRVHSAAMCowKAYIKwYBBQUHAgEW\n"\
										"HGh0dHBzOi8vd3d3LmRpZ2ljZXJ0LmNvbS9DUFMwDQYJKoZIhvcNAQELBQADggEB\n"\
										"AIIcBDqC6cWpyGUSXAjjAcYwsK4iiGF7KweG97i1RJz1kwZhRoo6orU1JtBYnjzB\n"\
										"c4+/sXmnHJk3mlPyL1xuIAt9sMeC7+vreRIF5wFBC0MCN5sbHwhNN1JzKbifNeP5\n"\
										"ozpZdQFmkCo+neBiKR6HqIA+LMTMCMMuv2khGGuPHmtDze4GmEGZtYLyF8EQpa5Y\n"\
										"jPuV6k2Cr/N3XxFpT3hRpt/3usU/Zb9wfKPtWpoznZ4/44c1p9rzFcZYrWkj3A+7\n"\
										"TNBJE0GmP2fhXhP1D/XVfIW/h0yCJGEiV9Glm/uGOa3DXHlmbAcxSyCRraG+ZBkA\n"\
										"7h4SeM6Y8l/7MBRpPCz6l8Y=\n"\
										"-----END CERTIFICATE-----\n"

#endif /* USER_APP_H_ */
