/***********************************************************************************************************************
 * File Name    : wifi_ep.h
 * Description  : Contains macros and functions used in wifi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef WIFI_EP_H_
#define WIFI_EP_H_

#include "http_client_ep.h"

/* Macro for max number Access Point scanning */
#define MAX_WIFI_SCAN_RESULTS               (10U)

/* Macros for Wi-Fi Access Point selections */
#define SCAN_AP_SELECTION                   (1U)
#define ENTER_SSID_SELECTION                (2U)

/* Extern variables - defined in wifi_ep.c */
extern WIFIScanResult_t g_scan_data[MAX_WIFI_SCAN_RESULTS];
extern WIFIIPConfiguration_t g_ip_config;

/* Function declarations */
WIFIReturnCode_t wifi_init(void);
WIFIReturnCode_t wifi_config(void);
WIFIReturnCode_t wifi_scan(void);
void wifi_deinit(void);
void wifi_set_password(const char * password);
void wifi_set_ap_ssid(uint32_t index);
void wifi_set_manual_ssid(const char *ssid, WIFISecurity_t security);
const char * wifi_get_ssid(void);
WIFISecurity_t wifi_get_security(void);

#endif /* WIFI_EP_H_ */
