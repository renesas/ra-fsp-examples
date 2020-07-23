/***********************************************************************************************************************
 * File Name    : jpeg_codec_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef JPEG_CODEC_EP_H_
#define JPEG_CODEC_EP_H_

#define RGB565_G_MASK    0x07E0
#define RGB565_B_MASK    0x001F
#define C_0              128
#define X_RESOLUTION             (240U)
#define Y_RESOLUTION             (320U)
#define HOR_STRIDE               (240U)
#define YCBCR_BYTES_PER_PIXEL    (2U)
#define LINES                    (8U)
#define IMAGE_SIZE               (X_RESOLUTION * Y_RESOLUTION * YCBCR_BYTES_PER_PIXEL)
#define ENCODE_BUFFER_SIZE       (30720U)
#define INPUT_MODE_DELAY         (5U)
#define SHIFT_BY_1               (1)
#define SHIFT_BY_2               (2)
#define SHIFT_BY_3               (3)
#define SHIFT_BY_4               (4)
#define SHIFT_BY_5               (5)
#define SHIFT_BY_6               (6)
#define SHIFT_BY_7               (7)
#define SHIFT_BY_8               (8)

#define EP_INFO                "\r\nThis Example Project demonstrates the basic usage of JPEG Codec driver."\
                               "\r\nThe project converts raw image of RGB565 format to YCbCr format and encodes it."\
                               "\r\nThe encoded image is then decoded into RGB565 format."\
                               "\r\nEncoded and decoded images can be viewed using Image viewer option in e2studio.\r\n"

#endif /* JPEG_CODEC_EP_H_ */
