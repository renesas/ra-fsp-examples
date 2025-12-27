/***********************************************************************************************************************
 * File Name    : filex_file_operation.h
 * Description  : Contains macros, data structures and functions used in FileX thread.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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

/* Function declarations */
UINT file_create(void);
UINT file_write(void);
UINT file_read(void);
UINT file_delete(void);

#endif /* FILEX_FILE_OPERATION_H_ */
