/***********************************************************************************************************************
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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
