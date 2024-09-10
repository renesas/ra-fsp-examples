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
/* lss */
#define LSS_NOTIF_DIS_CCCD      (0x0000)
#define LSS_WAIT_EN_CCCD        (0x0001)
#define LSS_WAIT_DIS_CCCD       (0x0002)
#define LSS_WAIT_PUSH_SW        (0x0004)
#define LSS_WAIT_WR_BLINK       (0x0008)


#endif /* TASK_FUNCTION_H_ */
