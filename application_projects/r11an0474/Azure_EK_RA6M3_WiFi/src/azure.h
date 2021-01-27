/***********************************************************************************************************************
 * File Name    : azure.h
 * Description  : Azure Common Header file
 **********************************************************************************************************************/
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

#ifndef AZURE_H_
#define AZURE_H_

#include <azure/core/az_result.h>

#define WIDTH_16            (16)
#define WIDTH_32            (32)
#define WIDTH_64            (64)
#define WIDTH_128           (128)
#define WIDTH_256           (256)

#define AZURE_DPS_DEFAULT_ENDPOINT          "global.azure-devices-provisioning.net"
#define AZURE_MQTT_SERVER_PORT              (8883)

#define PUBLISH_RETRY_LIMIT     (10)
#define PUBLISH_RETRY_MS        (1000)

#define PROP_TEMP                           "temperature"
#define HOSTNAME                            "ra_ekra6m3_01"
#define AZURE_IOT_CENTRAL_CLIENT_SIGNATURE  "RA-EKA6M3/0.1"
#define MANUFACTURER_STRING                 "Renesas Electronics America"

#define AZURE_SAS_EXPIRY                    (1607023759)  // 7:35 PM 3/9/2021

#define AZURE_RETURN_IF_FAILED(exp)        \
  do                                            \
  {                                             \
    az_result const _iot_sample_result = (exp); \
    if (az_result_failed(_iot_sample_result))   \
    {                                           \
      return _iot_sample_result;                \
    }                                           \
  } while (0)

#define ROOTCA_X509_CERT_PEM_FILE       \
        "-----BEGIN CERTIFICATE-----\n"\
        "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"\
        "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"\
        "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"\
        "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"\
        "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"\
        "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"\
        "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"\
        "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"\
        "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"\
        "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"\
        "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"\
        "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"\
        "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"\
        "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"\
        "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"\
        "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"\
        "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"\
        "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"\
        "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"\
        "-----END CERTIFICATE-----\n"

#define DEVICE_X509_TRUST_PEM_FILE          \
       "-----BEGIN CERTIFICATE-----\n"     \
"MIIDZDCCAkygAwIBAgIQKnjVLK6jRbpOrzGkLlhPtTANBgkqhkiG9w0BAQsFADAo\n"    \
"MSYwJAYDVQQDDB1BenVyZSBJb1QgQ0EgVGVzdE9ubHkgUm9vdCBDQTAeFw0yMDA5\n"    \
"MTYwMzEwMzZaFw0yMDEwMTYwMzIwMzZaMBYxFDASBgNVBAMMCzJmeXJ3N3RtYXlj\n"    \
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA6EVY8ce/3w/4MnDIikDj\n"    \
"fT347Y9IkAoRwvRaFduftVdLjvCgE9V9aLHqBS/331bynR7hPjd6bfKLz9HuZbxU\n"    \
"SLRJTrqD8BbXYpLKgMZEXSqYHBh3B03I0lGjiYeE/eluxfYMldqCdJYfv2P5/M6M\n"    \
"6NZdm7BZHEKdRdGqHdZUJwqp5reAfemQNHjuatJ9x2u+pIDW5DFRV+a8bXIw66ah\n"    \
"Y+uF3xThaFcHLOn4kR4t6gEMpv2k3hhAi/ek9zx5ajo5k/s30SkbzTw09uqIyOQx\n"    \
"+wGs4xS05Ekf5mt40q8bSqb4jnUW4BxXFghb0VJAiGszoFyTfY71Gizmco9mOaiC\n"    \
"oQIDAQABo4GbMIGYMA4GA1UdDwEB/wQEAwIFoDAWBgNVHREEDzANggsyZnlydzd0\n"    \
"bWF5YzAdBgNVHSUEFjAUBggrBgEFBQcDAgYIKwYBBQUHAwEwDwYDVR0TAQH/BAUw\n"    \
"AwIBADAfBgNVHSMEGDAWgBTuX+mzDPQnC7dka5+Qas5vE4OrhTAdBgNVHQ4EFgQU\n"    \
"TBUBu9e8pCWiEfSi8KZkkGPzoHYwDQYJKoZIhvcNAQELBQADggEBAInL4M/qWKg9\n"    \
"uI7IiCrq6O5lCF3f4RwEAUge2BthlO1ro4OQldvK2C66rRK9s1FPxaAkJpr3u/+O\n"    \
"WnhYzL/Ru+qiyiT9sfUni0j5uJTYySFSZgi4pI685tMswdNADgU5BzDaSCEjJoBM\n"    \
"5LOqT0IDt0RH1AxVbEpt5KPoL7ov0tST/I5IQsvkyPamBQbN5TUt4uSOMBKDI2BK\n"    \
"EnOrjTMawdM3iAu5hawVmmf77mQkjFkk+99f9AMB78bK8kVC8w0IcNBhoCL6+5iy\n"    \
"kJcMnBI/Mesqj+bfxpOsf3vC6KYAbPZAAD3Kfna4yEGw8R0Z/u8chb1B76uV3jJU\n"    \
"cM/anYw3JhA=\n"    \
"-----END CERTIFICATE-----\n"


#define DEVICE_PRIVATE_KEY_FILE                   \
       "-----BEGIN RSA PRIVATE KEY-----\n"      \
"MIIEowIBAAKCAQEA6EVY8ce/3w/4MnDIikDjfT347Y9IkAoRwvRaFduftVdLjvCg\n"    \
"E9V9aLHqBS/331bynR7hPjd6bfKLz9HuZbxUSLRJTrqD8BbXYpLKgMZEXSqYHBh3\n"    \
"B03I0lGjiYeE/eluxfYMldqCdJYfv2P5/M6M6NZdm7BZHEKdRdGqHdZUJwqp5reA\n"    \
"femQNHjuatJ9x2u+pIDW5DFRV+a8bXIw66ahY+uF3xThaFcHLOn4kR4t6gEMpv2k\n"    \
"3hhAi/ek9zx5ajo5k/s30SkbzTw09uqIyOQx+wGs4xS05Ekf5mt40q8bSqb4jnUW\n"    \
"4BxXFghb0VJAiGszoFyTfY71Gizmco9mOaiCoQIDAQABAoIBAD8DsWHHA3MM7hUh\n"    \
"LAb2G8G6snIg0VrioHb3vAj6EJYuHL2m4ob/AIsK2aaWoPTireeanRk4C+sQRoYX\n"    \
"Oko3kdY3szu8tlbIm56OnInbQGjWSXY13PUKUVVd+LQzJA2O6xKh8kAljAb9Q5e1\n"    \
"E/lWQDxbDAJVEZftKqXqf/CgWSxN3/KVWZPqLZNGVT7vD4lo+OtHslHoM40w+H/5\n"    \
"QfckntABgDttkkPHs5IM4eRykvaxBfWjMD+oSTMN2ksB+Y2qsEPGuin1QkdiXAv4\n"    \
"FhfIQ0uE6UijvrG+TstazKIlROd/u2DrsFGESXalqHjBrnAYujp/MFw4pN94ypiK\n"    \
"+BAYNtECgYEA+2uk/oMc58ASIPExJag/0JcP8AgEJh6f308zfilICJWQpZTbTmGk\n"    \
"kKauelFSCqlYr3FWHG4n9vyr7VbyFH19CTieP+Yc6Y1ioa/cSZnUEWb+7+mDN6X7\n"    \
"DrlZUoSMavl3V+9RFD7kDDkdj6PwEyU7pFP/EsSUIgIA9W4xdOGMVl8CgYEA7IBo\n"    \
"5QVmaMsx+4f3aO6dEPM6OSMYLx64ERxga3QfCzvkWMK9nDi3il5fJNiVmxXAUDaA\n"    \
"dTfHJ1vvx5/iM2Zt2xWEgbpVGvkAZqrN8Yz2gwHlEPr+SqfM77DR/c3Kld74aX71\n"    \
"sBT5AzucxaJwTJO/n5u1LO78xFlXvWUK8VuPxv8CgYEAyuYXNFJpTFnEB5O+Hu7F\n"    \
"+Tc/5G0DtCO4z4ViP+leuZbCMe6MgZfpL9dkLVBdTj9WU9e6L51yYS+OLC6jlIel\n"    \
"FYD8rQKuyIiSVXvP4hHjg9vD16dyfsFTd5F7FU3k6BvFBVEb9KPAdGDPi3E4uKqu\n"    \
"lDI5QlMkXMGTNn4kKXgwQi8CgYB19OncaGAWnloUmVEV3gUqwhVlEBLs1YZ6yktw\n"    \
"7J7bLIkMmDyX4VXxxnXv27JqgmPD3hsd268R+3HRf/YDKvBZ2BaZ2YS/Cjhf/Irx\n"    \
"EhyVZOmKGZWhSKYXWnqx+uUy8N23SGJj62HzcdUUYZtfJkXp5nBrs9sQV3tCdFYL\n"    \
"4V4JowKBgGEHckyvfCx5MO1JJ3jU3ZgbpPMtqrwokHvYx4FNpbgTlczwf8pwmLJY\n"    \
"xVCaUfQRXQEA6BuRCOChPCsGhdenbW/uds7frO6dy+7udyN/zrFIyxvllQzWL/sl\n"    \
"NV4Col2gSwfBIvNN2blMNdGbHoXQ+OtdoQK1+OdJZLM5UTHZ4vC+\n"    \
"-----END RSA PRIVATE KEY-----\n"



#endif /* AZURE_H_ */
