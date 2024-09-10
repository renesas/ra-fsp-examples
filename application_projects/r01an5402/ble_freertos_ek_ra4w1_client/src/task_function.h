/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#ifndef TASK_FUNCTION_H_
#define TASK_FUNCTION_H_
void bas_task(void * pvParameters);
void lss_task(void * pvParameters);

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* bac */
#define BAC_WAIT_EN_CCCD        (0x0001)
#define BAC_WAIT_DIS_CCCD       (0x0002)
#define BAC_READ_LEVEL          (0x0004)

/* lsc */
#define LSC_NOTIF_DIS_CCCD      (0x0000)
#define LSC_WAIT_EN_CCCD        (0x0001)
#define LSC_WAIT_DIS_CCCD       (0x0002)
#define LSC_WAIT_RECV_NTF       (0x0004)
#define LSC_WAIT_WR_BLINK       (0x0008)
#define LSC_WAIT_RD_BLINK       (0x0010)

#endif /* TASK_FUNCTION_H_ */
