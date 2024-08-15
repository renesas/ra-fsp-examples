/***********************************************************************************************************************
 * File Name    : filex_media_operation.h
 * Description  : Contains macros, data structures and functions used in FileX thread.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FILEX_MEDIA_OPERATION_H_
#define FILEX_MEDIA_OPERATION_H_

#include "filex.h"

#define PRINT_MEDIA_PROPERTY(property)      (send_data_to_rtt(RTT_OUTPUT_APP_MEDIA_PROPERTY, sizeof(media_property_t), &(property)))

/* Function declaration */
UINT media_verify(void);
UINT media_open(void);
UINT media_get_property(void);
UINT media_format(void);
UINT media_close(void);

#endif /* FILEX_MEDIA_OPERATION_H_ */
