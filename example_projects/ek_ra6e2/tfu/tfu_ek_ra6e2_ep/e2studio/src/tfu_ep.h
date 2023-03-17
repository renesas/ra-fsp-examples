/***********************************************************************************************************************
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

#ifndef TFU_EP_H_
#define TFU_EP_H_

#ifndef M_TWOPI
    #define M_TWOPI     (2.0f *3.14159265358979323846f)
#endif

#define EP_INFO       "\r\nThis example project demonstrates operation of the Trigonometric Function Unit (TFU)"\
                      "\r\nusing the in-line functions offered by the Board Support Package. The user will set the "\
                      "\r\ndesired resolution, in steps per cycle around the unit circle, through JLink RTT Viewer."\
                      "\r\nThe program will run sine, cosine, arctangent and hypotenuse functions for each step "\
                      "\r\nand execution will halt after one full cycle around the unit circle is calculated."\
                      "r\nThe outputs of each operation will be printed to JLink RTT Viewer in CSV format."\

#define RES_MIN_VAL     (4)
#define RES_MAX_VAL     (128)

#ifndef R_TFU_TRGSTS
    #define R_TFU_TRGSTS    (*(volatile uint32_t *) 0x40021008)
#endif

#define ERRF_MASK       (0x02)

#endif /*TFU_EP_H_ */
