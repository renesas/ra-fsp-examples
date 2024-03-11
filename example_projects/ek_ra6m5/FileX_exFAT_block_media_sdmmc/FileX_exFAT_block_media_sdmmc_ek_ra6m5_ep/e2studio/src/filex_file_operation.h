/***********************************************************************************************************************
 * File Name    : filex_file_operation.h
 * Description  : Contains macros, data structures and functions used in FileX thread.
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef FILEX_FILE_OPERATION_H_
#define FILEX_FILE_OPERATION_H_

#include "filex.h"

/* Macros for file operation */
#define FILE_NAME_ONE               "file_one.txt"
#define TRUNCATE_VALUE              (0UL)
#define SEEK_VALUE                  (0UL)
#define OPERATION_TIME_OUT          (1000U)
#define WRITE_BUFFER_SIZE           (102400U)
#define READ_BUFFER_SIZE            (1024U)
#define WRITE_TIMES                 (42000U)
#define WRITE_ONE_PERCENT           (WRITE_TIMES / 100U)
#define WRITE_LINE_SIZE             (128U)
#define WRITE_LINE_TEXT             "The example project demonstrates the operation of the Azure FileX file system"\
                                    " on block media via the SDHI driver on the RA MCU\r\n"
/* Function declaration */
UINT file_create(void);
UINT file_write(void);
UINT file_read(void);
UINT file_delete(void);

#endif /* FILEX_FILE_OPERATION_H_ */
