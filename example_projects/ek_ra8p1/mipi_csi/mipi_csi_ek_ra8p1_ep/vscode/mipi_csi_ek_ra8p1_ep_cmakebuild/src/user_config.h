/***********************************************************************************************************************
 * File Name    : user_config.h
 * Description  : Contains user-configurable macros for the MIPI CSI example project.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USER_CONFIG_H_
#define USER_CONFIG_H_

/* Macro for display output configuration */
#define DISPLAY_OUTPUT              (1U)

/* Macro for manual AWB configuration */
#define AWB_MANUAL_ENABLE           (0U)

/* The FPS value depends on the camera input image size.
 * Please refer to the camera sensor's datasheet before making any adjustments. */
#define FPS_TARGET                  (60)

#endif /* USER_CONFIG_H_ */
