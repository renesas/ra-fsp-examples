/***********************************************************************************************************************
 * File Name    : gt911.h
 * Description  : Contains data structures and functions used in mipi_dsi_ep.c.
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
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
