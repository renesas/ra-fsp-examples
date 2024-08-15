/***********************************************************************************************************************
 * File Name    : filex_dir_operation.h
 * Description  : Contains macros, data structures and functions used in FileX thread.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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
