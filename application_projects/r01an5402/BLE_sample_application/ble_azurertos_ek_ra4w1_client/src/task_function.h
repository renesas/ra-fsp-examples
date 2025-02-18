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
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

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
