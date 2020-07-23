/***********************************************************************************************************************
 * File Name    : ov7670.h
 * Description  : Contains data structures and functions used in hal_entry.c.
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

#ifndef OV7670_H_
#define OV7670_H_

#include "common_utils.h"

/* OV7670 Register Value */
struct ov7670_register_value
{
    uint8_t  reg_address;
    uint8_t  reg_value;
};

/* Macro Definition */
#define  REG_ZERO              (0U)
#define  REG_ONE               (1U)
#define  REG_TWO               (2U)
#define  OV7670_SETTLING_TIME  (300U)
#define  REG_SETTLING_TIME     (1U)

#if (!defined (BOARD_RA6M2_EK))
/* Camera RESET pin */
#define CAMERA_RESET BSP_IO_PORT_06_PIN_13
/* Camera PWDWN pin */
#define CAMERA_PWDWN BSP_IO_PORT_06_PIN_14
#else
/* Camera RESET pin */
#define CAMERA_RESET BSP_IO_PORT_08_PIN_00
/* Camera PWDWN pin */
#define CAMERA_PWDWN BSP_IO_PORT_08_PIN_01
#endif

/* functions declarations*/
void  ov7670_close (void);
fsp_err_t ov7670_reset (void);
fsp_err_t ov7670_init (void);
fsp_err_t ov7670_rgb565 (void);
fsp_err_t ov7670_setup (void);
fsp_err_t ov7670_write_setup_regs (void);
fsp_err_t ov7670_reg_write (uint8_t * const p_regs, uint8_t count);
fsp_err_t ov7670_reg_read (uint8_t reg, uint8_t * p_value);
fsp_err_t ov7670_reg_set (uint8_t reg, uint8_t value);
fsp_err_t ov7670_write_array (struct ov7670_register_value const * p_registers);

#endif /* OV7670_H_ */
