/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : ospi_b_ep.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/
#ifndef OSPI_B_EP_H_
#define OSPI_B_EP_H_

/* Macro for flash device */
#define OSPI_B_DEVICE_ID                    (0x3a86c2)

FSP_CPP_HEADER
fsp_err_t ospi_b_init ();
fsp_err_t ospi_b_set_protocol_to_spi ();
fsp_err_t ospi_b_set_protocol_to_opi ();
FSP_CPP_FOOTER

#endif /* OSPI_B_EP_H_ */
