/***********************************************************************************************************************
 * File Name    : can_fd_change_baudrate.h
 * Description  : Contains declarations of data structures and functions used in can_fd_change_baudrate.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CAN_FD_CHANGE_BAUDRATE_H_
#define CAN_FD_CHANGE_BAUDRATE_H_

#include "can_fd_ep.h"

/* Macro for CANFD clock */
#define CANFD_CLOCK                      (40000000U)

/* Macros for nominal baudrate timings limitation */
#define CAN_SYNC_SEG                     (1U)
#define NOMINAL_BAUDRATE_MIN             (20000U)
#define NOMINAL_BAUDRATE_MAX             (MBPS_1_VAL)
#define NOMINAL_TSEG1_MIN                (2U)
#define NOMINAL_TSEG1_MAX                (256U)
#define NOMINAL_TSEG2_MIN                (2U)
#define NOMINAL_TSEG2_MAX                (128U)
#define NOMINAL_SJW_MIN                  (1U)
#define NOMINAL_SJW_MAX                  (128U)
#define NOMINAL_PRESCALER_MIN            (1U)
#define NOMINAL_PRESCALER_MAX            (1024U)

/* Macros for data baudrate timings limitation */
#define DATA_BAUDRATE_MIN                (MBPS_1_VAL)
#if (defined BOARD_RA8M1_EK) || (defined BOARD_RA8M1_EK) || (defined BOARD_RA8T1_MCK) || (defined BOARD_RA8E1_FPB)
#define DATA_BAUDRATE_MAX                (MBPS_8_VAL)
#else
#define DATA_BAUDRATE_MAX                (MBPS_5_VAL)
#endif
#define DATA_TSEG1_MIN                   (2U)
#define DATA_TSEG1_MAX                   (32U)
#define DATA_TSEG2_MIN                   (2U)
#define DATA_TSEG2_MAX                   (16U)
#define DATA_SJW_MIN                     (1U)
#define DATA_SJW_MAX                     (16U)
#define DATA_PRESCALER_MIN               (1U)
#define DATA_PRESCALER_MAX               (256U)

/* Macros for sample point limitation */
#define SAMPLE_POINT_MIN                 (60U)
#define SAMPLE_POINT_MAX                 (99U)

/* Macros for CANFD baudrate default */
#define NOMINAL_BAUDRATE_DEFAULT         (KBPS_250_VAL)
#define DATA_BAUDRATE_DEFAULT            (MBPS_1_VAL)
#define SAMPLE_POINT_DEFAULT             (75U)

/* User defined function */
void canfd_baudrate_set(void);

#endif /* CAN_FD_CHANGE_BAUDRATE_H_ */
