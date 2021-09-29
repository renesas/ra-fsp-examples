/***********************************************************************************************************************
 * File Name    : board_cfg.h
 * Description  : Board specific/configuration data.
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

#include "hal_data.h"

extern bsp_leds_t g_bsp_leds;

#define USB_EP_PACKET_SIZE                        (512U)
#define OFF                                       (0U)
#define ON                                        (1U)


#ifdef BOARD_RA4M1_EK
#define RED                                       (BSP_LED_LED1)

#define TURN_RED_ON                               R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[RED], ON); 

#endif

#define KIT_NAME_MACRO                            "EK-RA4M1"
#define KIT_PARTNUM_MACRO                         "RTK7EKA4M1S00001BU"
#define KIT_LANDING_URL                           "renesas.com/ra/ek-ra4m1"
#define PRODUCT_INFO_URL                          "renesas.com/ra"
#define PRODUCT_SUPPORT_URL                       "renesas.com/ra/forum"
#define RENESAS_SUPPORT_URL                       "renesas.com/support"
