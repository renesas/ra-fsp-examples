/***********************************************************************************************************************
 * File Name    : filex_dir_operation.h
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

#ifndef FILEX_DIR_OPERATION_H_
#define FILEX_DIR_OPERATION_H_

#include "filex.h"

#define DIR_NAME_ONE                    "dir_one"

#define PRINT_VOLUME_INFO(info)         (send_data_to_rtt(RTT_OUTPUT_APP_MEDIA_VOLUME_INFO, sizeof(volume_info_t), &(info)))
#define PRINT_ENTRY_INFO(info)          (send_data_to_rtt(RTT_OUTPUT_APP_ENTRY_INFO, sizeof(entry_info_t), &(info)))
#define PRINT_DIR_PROPERTY(property)    (send_data_to_rtt(RTT_OUTPUT_APP_DIR_PROPERTY, sizeof(dir_property_t), &(property)))

UINT dir_create(void);
UINT dir_get_property(void);
UINT dir_delete(void);

#endif /* FILEX_DIR_OPERATION_H_ */
