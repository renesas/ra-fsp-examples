/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#if (USE_VIRTUAL_COM == 1)
#ifndef R_UARTA_CFG_H_
#define R_UARTA_CFG_H_
#ifdef __cplusplus
            extern "C" {
            #endif

#define UARTA_CFG_PARAM_CHECKING_ENABLE (BSP_CFG_PARAM_CHECKING_ENABLE)
#define UARTA_CFG_DTC_SUPPORT_ENABLE (1)
#define UARTA_CFG_RECEIVE_ERROR_INTERRUPT_MODE 1
#ifdef __cplusplus
            }
            #endif
#endif /* R_UARTA_CFG_H_ */
#endif /* USE_VIRTUAL_COM */
