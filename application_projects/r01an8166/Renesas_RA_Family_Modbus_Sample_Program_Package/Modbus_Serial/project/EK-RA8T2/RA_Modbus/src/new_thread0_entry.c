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
 * Copyright (C) 2025 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : new_thread0_entry.c
 * Version      : 1.0
 * Description  : This file implements new thread entry function.
 *********************************************************************************************************************/
#include "new_thread0.h"
#include "r_modbus_serial_slave.h"

/**********************************************************************************************************************
 * Function Name: new_thread0_entry
 * Description  : New Thread entry function
 * Argument     : void *pvParameters
 * Return Value : None
 *********************************************************************************************************************/
void new_thread0_entry(void *pvParameters)
{
    modbus_err_t err;

    FSP_PARAMETER_NOT_USED(pvParameters);

    /* Modbus SERIAL Initialization: This init initializes the Modbus protocol stack  */
    err = R_MODBUS_SERIAL_SLAVE_Open(&g_modbus_serial_slave0_ctrl, &g_modbus_serial_slave0_cfg);
    if (MODBUS_ERR_OK != err)
    {
        /* Error Handling */
    }

    /* TODO: add your own code here */
    while (1)
    {
        vTaskDelay (1);
    }
}
