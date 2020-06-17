/***********************************************************************************************************************
 * File Name    : hal_entry.h
 * Description  : Entry function.
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef HAL_ENTRY_H_
#define HAL_ENTRY_H_

#include "board_cfg.h"

/*******************************************************************************************************************//**
 * @ingroup hal_entry
 * @{
 **********************************************************************************************************************/


#define KIT_INFO_PRIMARY_TEXT                "\r\n 1. KIT INFORMATION \r\n\r\n a) Kit name: \t\t\t\t"\
                                              KIT_NAME_MACRO\
                                             "\r\n b) Kit ordering part number:\t\t"\
                                              KIT_PARTNUM_MACRO\


#define LINE_CODING_LENGTH          (0x07U)
#define ADCTEMP_AS_C(a)             (((((float)a) * (-0.0209031f)) + 312.671f))
#define ADCTEMP_AS_F(a)             (((((float)a) * (-0.0376256f)) + 594.808f))
#define READ_BUF_SIZE               (8U)


/** @} */
#endif /* HAL_ENTRY_H_ */
