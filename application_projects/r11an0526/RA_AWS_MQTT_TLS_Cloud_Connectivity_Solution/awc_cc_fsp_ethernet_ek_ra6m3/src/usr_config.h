/***********************************************************************************************************************
 * File Name    : usr_config.h
 * Description  : Contains macros, data structures and functions used  in the Application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef USR_CONFIG_H_
#define USR_CONFIG_H_

#include "common_utils.h"
#define USR_LOG_LVL          (LOG_ERROR)     /* User Options are:  LOG_ERROR, LOG_WARN, LOG_INFO, , LOG_DEBUG */
#define USR_LOG_TERMINAL     (RTT_TERMINAL)  /* User Options are:  RTT_TERMINAL */
#define USR_MQTT_DATA_FORMAT (JSON)          /* JSON, UTF8  */

#define LOGGING_TASK_STACK_SIZE         (1 * 1024)
#define LOGGING_TASK_STACK_PRIORITY     (6)
#define LOGGING_TASK_QUEUE_SIZE         (1 * 1024)

#define DEBUG 1
#define USR_IOT_THING_NAME              "RA6M3"

/***********************************************************************************************************************
 * AWS MQTT End point  detail for the application. These needs to be updated based on the endpoint from your device
 * creation using your aws end point.
 ***********************************************************************************************************************/

#define USR_MQTT_ENDPOINT "aoh5lvd4o23ku-ats.iot.us-east-1.amazonaws.com"


#define USR_MQTT_BROKER_PORT            8883

#define IO_USERNAME                     ""
#define IO_KEY                          ""

/***********************************************************************************************************************
 * AWS Server Certificate obtained as part of the device creation as documented in the Application Note
 * Users are required to modify this as required for the device. 
 ***********************************************************************************************************************/
#if 0
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
#endif
/***********************************************************************************************************************
 * AWS Client Certificate obtained as part of the device creation as documented in the Application Note
 * Users are required to modify this as required for the device. 
 ***********************************************************************************************************************/
#define CLIENT_CERTIFICATE                                          \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWjCCAkKgAwIBAgIVAPjMET0VInWpPlrUSfSfsPyUjvL3MA0GCSqGSIb3DQEB\n"\
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n"\
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMTA4MzEyMzI2\n"\
"MjhaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n"\
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCz1E3rwZ9aIkjn08Jo\n"\
"v/fqWpBS2Ux9MtDpPRsj/pJWUS89x6Bt8Dhzbjr62Fyfu2ZZ7vriSM3/xEqMyyYU\n"\
"L1LNvYFSbGn8ofCRZwl4DUumRcDr07zURjK5FNR963D+1NqXzEf6jR7EejCVk7zn\n"\
"HPanQCz3SZDZXn/K/CGNdnAYEIn4JAPsAospf2mtnm2nCsP+riSkYeoK89RP+w8S\n"\
"O8hQFua2saXuEoueFIPNuZlW0rFdVNNU1KEwPLeaR588h0i+DoHfNE2d6qiUAEHC\n"\
"q5U3A7h+/U+T0AXbKcB0BBzNHwp/KX+0T8ieF7odjg/3l+8pKCHdYA0+Mj98B0Gi\n"\
"5aRFAgMBAAGjYDBeMB8GA1UdIwQYMBaAFDoIiQW5lG6fjNjD7rBa9/J9NP1hMB0G\n"\
"A1UdDgQWBBSAUvrb2lV1P/vIw+SF25WZOLbWVTAMBgNVHRMBAf8EAjAAMA4GA1Ud\n"\
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAA6mNNo50CkOrEy7NceVB+7h8\n"\
"sYFuWCB01PcdaLxNtpFfZxtAu2XIHz0gXqLloM+QPI00yeEDKW7Mkkmh4LrW6hw4\n"\
"w+PlFqE+ynuCiosbGyhJ2qnwJuib6X66uoJcyqFHbShcv/bnuGrbXk4hxWPJG7yT\n"\
"H/6IVYAFXwT7bXh/oTjW84dz0KHYrWGcnN6/sLMuz1gLNWEwT2CliyekD5eXvBki\n"\
"WxkqT9KlFj73rE53Wyi7lsDrpGstPThbTkO1x0k5jVejVr7uQ9+gjGorkb1m4uQY\n"\
"JkwjNZgUbbc1VybrEps/UPYTBWSDJHaVASnS6LjhdRz40CEHpwqCNcfv9sSagw==\n"\
"-----END CERTIFICATE-----";

/***********************************************************************************************************************
 * AWS Client Key obtained as part of the device creation as documented in the Application Note
 * Users are required to modify this as required for the device. 
 ***********************************************************************************************************************/
#define CLIENT_KEY                                                  \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEpQIBAAKCAQEAs9RN68GfWiJI59PCaL/36lqQUtlMfTLQ6T0bI/6SVlEvPceg\n"\
"bfA4c246+thcn7tmWe764kjN/8RKjMsmFC9Szb2BUmxp/KHwkWcJeA1LpkXA69O8\n"\
"1EYyuRTUfetw/tTal8xH+o0exHowlZO85xz2p0As90mQ2V5/yvwhjXZwGBCJ+CQD\n"\
"7AKLKX9prZ5tpwrD/q4kpGHqCvPUT/sPEjvIUBbmtrGl7hKLnhSDzbmZVtKxXVTT\n"\
"VNShMDy3mkefPIdIvg6B3zRNneqolABBwquVNwO4fv1Pk9AF2ynAdAQczR8Kfyl/\n"\
"tE/Inhe6HY4P95fvKSgh3WANPjI/fAdBouWkRQIDAQABAoIBABFupjMRbPIe2rRm\n"\
"abUfzteP+eeeGvUUBP5nJwdhOSxW6F5zyJB/tekyqVMBwFDXqL9VFEr1Rcria1uk\n"\
"ocdZIwzArxWfXtLXm+tscpWJrGis4Fu8BdUKDQa2BzdBqI17csk+S0f1gCMJuUX2\n"\
"sVwyCZMC/JjoL7tylq7FrDb7YThj0FDdJVGxp1K/GHVLr0lBHlOBcnjdTyFPW6Xm\n"\
"VRHndTzqNC25U71mMsNClhwAl6jFNbKExyGDahqNTHBYT6ACrlpWA3PkJdQVyUPj\n"\
"hEmwFi38zh73mf2UMyZiiie4H4MwYbSQkeZOk0gvhPynrOUGliRbFSqQ8EwyF8Gh\n"\
"3cVleEECgYEA6GWvOjxPwEs6t8BHV9KG0o4rX/riy0hby0bwdmv6lms84DFXBq+e\n"\
"czDZDovcKn0IrTAf2VBRokmanbL53BZQ7KQR33b2H7egQXQNXzkIVC79FBlZQycx\n"\
"8fmFB1fwupHkhjp8LMgBVz/tWvBunkY7jl16Oie9veoZHXQR0tL2wLUCgYEAxhfb\n"\
"fqlp0NswYKy9dCpUVIrVS49vFSUOtV7fXCfdiZAXbsT1/AQl1Y6p7z/nCX3rcW4G\n"\
"IGhscgcYc1/1t4f1OhdQA+MKaQtrMIlzyNITHvAhjjQB5kG8JjuYsXFpSQipVbA3\n"\
"PPd9UtVJy6+nqZhl9jcZZMMMX83bUifOxddB31ECgYEAjAXQaVczehoZu+MnWTvo\n"\
"re6pJcBla7QiUr1gXwm6R20/lZlE5StlXeRds80DD7qcQrVPS0GAnjaY6VGQCFKN\n"\
"2bcVab4kl4GtFiLVd71GTmqtY7HOPY7dIf2EcKNE0TTcKyAL93Nek3JaEcPx8n1A\n"\
"nwulfGq7gTUOgtePcaZYtE0CgYEAv3xaYeLz9b+b0SfJ71vnrt2Rd3wMXf03bZ1q\n"\
"/lNO9GYEMMBWJCoCRKCmpi44cFV3XWuEopkukz9GpGiTRrk3FNNqNoenDYnLMNz9\n"\
"vpdLgnGPjj25xFGwxaiAXq6Jnt6vibRQgS+fxo2Ey09eHDPEBDCY7G0bLhT5OD5g\n"\
"w5/j/jECgYEAgPovSPZqDj/wRukr76xHVZTauyQFojOI4w82AIwLvBU+fBGun9nI\n"\
"VzPyZd6rNhWOajCaqoM+mHzRvMFOOxjbpbEX/9cgmscg92PV9Nb3U5iET1FgiSDq\n"\
"VZjKp3eZfkAEgy4KQKQer7WSuDEmOhwzYRmqaqGycO3kt75QBPEyQyk=\n"\
"-----END RSA PRIVATE KEY-----";
#endif /* USR_CONFIG_H_ */
