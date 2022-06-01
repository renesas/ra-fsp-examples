/***********************************************************************************************************************
 * File Name    : glcdc_ep.h
 * Description  : Contains declarations of data structures used in hal_entry.c.
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

#ifndef GLCDC_EP_H_
#define GLCDC_EP_H_

#define EP_INFO  "\r\nThe Example Project demonstrates the basic functionalities of GLCDC device driver \r\n"\
                  "on Renesas RA MCUs based on Renesas FSP. On successful initialization of GLCDC module, the \r\n"\
                  "EP will be able to read the display resolution and frame buffer set in the GLCDC module and\r\n"\
                  "draw color bands to cover the entire screen of external LCD connected to RA MCU. \r\n"\
                  "Additionally, API Failure messages will be displayed on RTT Viewer.\r\n\n "\

/* RGB 16-bit color format*/
#ifdef DISPLAY_IN_FORMAT_16BITS_RGB565_0
    #define BYTES_PER_PIXEL  2       //16 bit format
    #define RED         0xF800
    #define GREEN       0x07E0
    #define BLUE        0x001F
    #define BLACK       0x0000
    #define WHITE       0xFFFF
    #define YELLOW      0xFFE0
    #define MAGENTA     0xF81F
    #define CYAN        0x07FF
#endif

#define X1_CO_ORDINATE      0U
#define Y1_CO_ORDINATE      0U
#define COLOR_BAND_COUNT    8U
#define INC_DEC_VALUE       1


#endif /* GLCDC_EP_H_ */
