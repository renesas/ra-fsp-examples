/***********************************************************************************************************************
 * File Name    : gt911.h
 * Description  : Contains data structures and functions used in mipi_dsi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef GT911_H_
#define GT911_H_

#include "r_mipi_dsi_api.h"

#define BUFFER_LENGTH                                (10)
#define GTP_READ_COOR_ADDR                           (0x814E)
#define GT911_BUFFER_STATUS_READY                    (0x80)
#define GTP_ADDR_LENGTH                              (2)

typedef struct st_coord
{
    uint16_t x;
    uint16_t y;
}coord_t;

typedef __PACKED_STRUCT st_gt911_point_payload
{
    uint8_t track_id;
    uint16_t x;
    uint16_t y;
    uint16_t point_size;
    uint8_t reserved;
}gt911_point_payload_t;

fsp_err_t gt911_get_status(uint8_t* status, coord_t * points, uint32_t num_points);

#endif /* GT911_H_ */
