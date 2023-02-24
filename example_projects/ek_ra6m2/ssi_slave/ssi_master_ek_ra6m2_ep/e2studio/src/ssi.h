/***********************************************************************************************************************
 * File Name    : ssi.h
 * Description  : Contains macros
 ***********************************************************************************************************************/
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
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef SSI_H_
#define SSI_H_

#define NULL_CHAR                   '\0'
#define SELECTION_MIN               (1U)
#define SELECTION_MAX               (2U)
#define VALUE_0                     (0U)
#define VALUE_1                     (1U)
#define VALUE_2                     (2U)
#define WRITE_COUNTS                (10U)
#define READ_COUNTS                 (10U)
#define WAIT_TIME                   (200U)
#define WAIT_TIME1                   (200U)
#define MASTER_WRITE_SLAVE_READ     (1U)
#define MASTER_READ_SLAVE_WRITE     (2U)
#define INDEX_0                     (0U)
#define INDEX_1                     (1U)

#define ENABLE_READING_FROM_DATA_FLASH  (1U)
#endif /* SSI_H_ */
