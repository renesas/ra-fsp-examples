/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : root_ca.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#ifndef ROOT_CA_H_
#define ROOT_CA_H_

#define HTTPS_CURRENCY_HOST_ADDRESS    "api.currencyapi.com"
#define HTTPS_WEATHER_HOST_ADDRESS     "api.weatherapi.com"
#define HTTPS_PORT                     ((uint16_t) 443U)

// CURRENCY_API
#define HTTPS_TRUSTED_ROOT_CA_CURRENCY                                   \
    "-----BEGIN CERTIFICATE-----\n"                                      \
    "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\n" \
    "bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c\n"                             \
    "-----END CERTIFICATE-----\n"                                        \


// WEATHER_API
#define HTTPS_TRUSTED_ROOT_CA_WEATHER                                    \
    "-----BEGIN CERTIFICATE-----\n"                                      \
    "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\n" \
    "-----END CERTIFICATE-----\n"

#define CLIENT_DUMMY_CERTIFICATE                                       \
    "-----BEGIN CERTIFICATE-----"                                      \
    "MIIDwTCCAqmgAwIBAgIUaOzetFVzHl99zN5HMrIssI40KGEwDQYJKoZIhvcNAQEL" \
    "BQAwcDELMAkGA1UEBhMCVUsxDzANBgNVBAgMBkxvbmRvbjETMBEGA1UEBwwKTWFp" \
    "ZGVuaGVhZDEQMA4GA1UECgwHUmVuZXNhczEMMAoGA1UECwwDU0REMRswGQYDVQQD" \
    "DBJ3d3cud2VhdGhlcmFwaS5jb20wHhcNMjMwNDI3MTExOTQyWhcNMjQwNDI2MTEx" \
    "OTQyWjBwMQswCQYDVQQGEwJVSzEPMA0GA1UECAwGTG9uZG9uMRMwEQYDVQQHDApN" \
    "YWlkZW5oZWFkMRAwDgYDVQQKDAdSZW5lc2FzMQwwCgYDVQQLDANTREQxGzAZBgNV" \
    "BAMMEnd3dy53ZWF0aGVyYXBpLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC" \
    "AQoCggEBAKSup1jTO33v9D/jehA4XF6shubBbn4XnfiGMaUpIf9aQAtp6E9ub27W" \
    "CJWwBFhrzBeQdJXGgp5NL9/sPkRHWcg4Ng+kNLjjH7qN0K5XPGVXio15RQwaAkL8" \
    "5HOIj1RPRc/FoRNPSqqhLFb4BjlFqzPFqI41tqCH+fDmhM9f7pd+/QlLksvMX8Gg" \
    "h1YckIMJJhmySlaqtgK9fYGRBg8VooOVpF5J7BbQFC+qxprJ5In1qKU0D2tMLGPw" \
    "vwrYUW9/3uD8h97tuPC5Lr6ESrnooPH6raiHvMGL3ECkPvRWNYznaWcKD/MHyQcA" \
    "K+VFV7cWtm9ZF24/ptoPHHRpnxjVvS8CAwEAAaNTMFEwHQYDVR0OBBYEFJ4zqde8" \
    "urg32N3NQwT5kvsMRmDRMB8GA1UdIwQYMBaAFJ4zqde8urg32N3NQwT5kvsMRmDR" \
    "MA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAKRVQvJNiDqOwrdc" \
    "AVcnfMo1yx1H+366Njm+cgm5kDtFJSVnKwH5GLoyUrELtotw4ztfrGV8+JsDaVan" \
    "mOXPW5hoU7QTDBCAVGQTO96Sj6aMLCoyyHmsDRspK0JIM/+DbhFsVE+JUKgySI5u" \
    "7pYc2CNdNWIa4duephQGY0SeuiNkcCmOEd0KUJvUNCPDx8Ws0KvJKAdseUFOl/Ue" \
    "+nQMfIsnBMya7vEZHSJYQTngtjtiVp3eXFEfnBB0gIPMBDk4noHuJRj9OV/1RZyJ" \
    "sYqZ07X+cYkihJt1HFEfPsZQUXBNxVWEnmploCZeG6DTE/JbMRGrANTa6OyofeIn" \
    "btOXEEY="                                                         \
    "-----END CERTIFICATE-----"

#define CLIENT_DUMMY_PRIVATE_KEY                                       \
    "-----BEGIN PRIVATE KEY-----"                                      \
    "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCkrqdY0zt97/Q/" \
    "43oQOFxerIbmwW5+F534hjGlKSH/WkALaehPbm9u1giVsARYa8wXkHSVxoKeTS/f" \
    "7D5ER1nIODYPpDS44x+6jdCuVzxlV4qNeUUMGgJC/ORziI9UT0XPxaETT0qqoSxW" \
    "+AY5RaszxaiONbagh/nw5oTPX+6Xfv0JS5LLzF/BoIdWHJCDCSYZskpWqrYCvX2B" \
    "kQYPFaKDlaReSewW0BQvqsaayeSJ9ailNA9rTCxj8L8K2FFvf97g/Ife7bjwuS6+" \
    "hEq56KDx+q2oh7zBi9xApD70VjWM52lnCg/zB8kHACvlRVe3FrZvWRduP6baDxx0" \
    "aZ8Y1b0vAgMBAAECgf86MFr8Zm9LhW0yR1VwWqZfEEk2QCLmdhjQQvN/be15iguR" \
    "o9TQwdY5056N97+915DF5Ck3dQjp5n2PN/s3wNz+FELDIrc7kePhZcBGHMSZCnF1" \
    "rSa+MJ8AIr/N0CqSH8mm9ChUxi+slVG2456dRPsorpL8MmK0CHlN4u5jpevEFed7" \
    "CdIKZijZ7SqqSXflyhjF/rPT53a/WIb1Mc5lg+wKz9tPLK/GmAwzPdMX2dadAxl9" \
    "/BrJhIb5EBPXgrazoGSM4oi8xjDpyF2otOw0XY3joaTuAex4Ck/TXXSlASPW+HVI" \
    "nVdv5U6l9kswVz6p8jzwwH5Ijv0l+tzE/+BV+4UCgYEA5RkW64kT+yEsZRiAQoYe" \
    "L/by4HjMqivw4MjyOE8rXJ+XPQHjS6eCIJHqhhIg9QfMg1OVYRM8s9oZb48C8dkw" \
    "Ao8G+DT5JVwQOWJkEkd+0g4DzkUUsBxEeL4LYWOglPxVNhKXZPd+N1laiQeXcvUT" \
    "0v3IgjwzlXeDFCFMf+CApssCgYEAuAUpxdxG/fk8KHD9sBaFN5qFV4rQ+k5yoCsq" \
    "RNkHlFrfQFHKvMDUQ+8BVet2X1Yp9VGBD3VeYJIjVYltTYOB27JRuw+kjVQiaor8" \
    "bbs94teA1ktlEVOdpnVMs/K04ijGg5NdhMDJmbyDChsxAPvrDH40m53vnJTG8Rnb" \
    "Xv5oUq0CgYEAwLJ9yNef6tdYVCBroYjV01pPp+dznp8fGLNsqVhODLZfq3zvBi97" \
    "EJIrRWHyPqe0qbGgL+rSYCTOyDruKyaXb0+GeTd1aKsrKllL+uQQE/ad1N3eLNNZ" \
    "9IVFZUE7TJWBxt2UYeSaUUvjDudHx/XEGvBDuueQK+Q+aKwpgFYBYEkCgYEAta8b" \
    "VCesy9oERCVywyUo+AIOvqTmuQs07uBPRlREwHuLyhQ3J2724++YkPz+DRikKnoD" \
    "A4Es4gnG6w/ZI9vAHWoMebTzETpNFIYOvkRM0KdWo70ilx3IG+3aAhyR7Gy3unaH" \
    "sacqJChT9GluU3tl0RrV1AL17NGTLb4Ksdpao60CgYEAzfsx3HmgzPPACCg3OglG" \
    "OSNtEeBfQr68QTsOpHhQN+STzqlMMNSN/138Mw33iIWjjWdw3VvKTClLUc/8mMn6" \
    "1O78l3Y7PBKnkLx9B/PaOHE3gto2OcCMRfHdXK3gXcOj9FmK96F7/3x8jEVbPQ5r" \
    "aC699c1qkZiBd99ul3tjH2A="                                         \
    "-----END PRIVATE KEY-----"
#endif                                 /* ROOT_CA_H_ */

// 1796c233af584ce5b35142450231310
