/**********************************************************************************************************************
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
 * File Name    : modbus_func.c
 * Version      : 1.0
 * Description  : This file implements the callback functions called from the Modbus Serial Slave and functions 
 *                for each Function code.
 *********************************************************************************************************************/

/*============================================================================*/
/* I N C L U D E                                                              */
/*============================================================================*/
#include "new_thread0.h"
#include "r_modbus_common.h"
#include "r_modbus_typedef.h"
#include "r_modbus_serial_slave_core.h"
#include <stdint.h>

/*============================================================================*/
/* E X T E R N                                                                */
/*============================================================================*/
/* definitions in modbus_user.c */
extern uint32_t g_coil_end_addr;
extern uint8_t  (*MB_Coils_Read[])(uint8_t *data);
extern uint8_t  (*MB_Coils_Write[])(uint8_t data);
extern uint32_t g_discrete_input_end_addr;
extern uint8_t  (*MB_Discretes_Input[])(uint8_t *data);
extern uint32_t g_holding_reg_end_addr;
extern uint8_t  (*MB_HoldingRegs_Read[])(uint16_t *data);
extern uint8_t  (*MB_HoldingRegs_Write[])(uint16_t data);
extern uint32_t g_input_reg_end_addr;
extern uint8_t  (*MB_Input_Regs[])(uint16_t *data);

/*============================================================================*/
/* D E F I N E                                                                */
/*============================================================================*/
#define MODBUS_COIL_START_ADDR           (0)
#define MODBUS_DISCRETE_INPUT_START_ADDR (0)
#define MODBUS_HOLDING_REG_START_ADDR    (0)
#define MODBUS_INPUT_REG_START_ADDR      (0)
#define MODBUS_COIL_END_ADDR             (g_coil_end_addr)
#define MODBUS_DISCRETE_INPUT_END_ADDR   (g_discrete_input_end_addr)
#define MODBUS_HOLDING_REG_END_ADDR      (g_holding_reg_end_addr)
#define MODBUS_INPUT_REG_END_ADDR        (g_input_reg_end_addr)
#define MB_BIT_8                         (8)

/*============================================================================*/
/* V A R I A B L E                                                            */
/*============================================================================*/

/*============================================================================*/
/* S T A T I C   F U N C T I O N   P R O T O T Y P E                          */
/*============================================================================*/
static uint32_t mb_func_exp_addr (uint8_t * u8_res);
static uint32_t mb_func_exp_data (uint8_t * u8_res);
static uint32_t mb_func_exp_execution (uint8_t * u8_res, int type);

/*============================================================================*/
/* P R O G R A M                                                              */
/*============================================================================*/
static uint32_t cb_func_code01 (p_modbus_serial_slave_req_read_coils_t pt_request, 
                                p_modbus_serial_slave_resp_read_coils_t pt_response);
static uint32_t cb_func_code02 (p_modbus_serial_slave_req_read_inputs_t pt_request, 
                                p_modbus_serial_slave_resp_read_inputs_t pt_response);
static uint32_t cb_func_code03 (p_modbus_serial_slave_req_read_holding_reg_t pt_request, 
                                p_modbus_serial_slave_resp_read_holding_reg_t pt_response);
static uint32_t cb_func_code04 (p_modbus_serial_slave_req_read_input_reg_t pt_request, 
                                p_modbus_serial_slave_resp_read_input_reg_t pt_response);
static uint32_t cb_func_code05 (p_modbus_serial_slave_req_write_single_coil_t pt_request, 
                                p_modbus_serial_slave_resp_write_single_coil_t pt_response);
static uint32_t cb_func_code06 (p_modbus_serial_slave_req_write_single_reg_t pt_request, 
                                p_modbus_serial_slave_resp_write_single_reg_t pt_response);
static uint32_t cb_func_code15 (p_modbus_serial_slave_req_write_multiple_coils_t pt_request, 
                                p_modbus_serial_slave_resp_write_multiple_coils_t pt_response);
static uint32_t cb_func_code16 (p_modbus_serial_slave_req_write_multiple_reg_t pt_request, 
                                p_modbus_serial_slave_resp_write_multiple_reg_t pt_response);
static uint32_t cb_func_code23 (p_modbus_serial_slave_req_read_write_multiple_reg_t pt_request, 
                                p_modbus_serial_slave_resp_read_write_multiple_reg_t pt_response);

/**********************************************************************************************************************
 * Function Name: function_code_callback
 * Description  : Callback functions called from the Modbus Serial Slave
 * Argument     : uint8_t u8_function_code
 *                void * pv_request_data
 *                void * pv_response_data
 * Return Value : None
 *********************************************************************************************************************/
void function_code_callback(uint8_t u8_function_code, void * pv_request_data, void * pv_response_data)
{
    modbus_serial_slave_req_read_coils_t               * p_req_read_coils              = NULL;
    modbus_serial_slave_req_read_inputs_t              * p_req_read_inputs             = NULL;
    modbus_serial_slave_req_read_holding_reg_t         * p_req_read_holding_reg        = NULL;
    modbus_serial_slave_req_read_input_reg_t           * p_req_read_input_reg          = NULL;
    modbus_serial_slave_req_write_single_coil_t        * p_req_write_single_coil       = NULL;
    modbus_serial_slave_req_write_single_reg_t         * p_req_write_single_reg        = NULL;
    modbus_serial_slave_req_write_multiple_coils_t     * p_req_write_multiple_coils    = NULL;
    modbus_serial_slave_req_write_multiple_reg_t       * p_req_write_multiple_reg      = NULL;
    modbus_serial_slave_req_read_write_multiple_reg_t  * p_req_read_write_multiple_reg = NULL;

    modbus_serial_slave_resp_read_coils_t              * p_resp_read_coils              = NULL;
    modbus_serial_slave_resp_read_inputs_t             * p_resp_read_inputs             = NULL;
    modbus_serial_slave_resp_read_holding_reg_t        * p_resp_read_holding_reg        = NULL;
    modbus_serial_slave_resp_read_input_reg_t          * p_resp_read_input_reg          = NULL;
    modbus_serial_slave_resp_write_single_coil_t       * p_resp_write_single_coil       = NULL;
    modbus_serial_slave_resp_write_single_reg_t        * p_resp_write_single_reg        = NULL;
    modbus_serial_slave_resp_write_multiple_coils_t    * p_resp_write_multiple_coils    = NULL;
    modbus_serial_slave_resp_write_multiple_reg_t      * p_resp_write_multiple_reg      = NULL;
    modbus_serial_slave_resp_read_write_multiple_reg_t * p_resp_read_write_multiple_reg = NULL;

    switch (u8_function_code)
    {
        case MODBUS_SERIAL_FUNC_CODE_READ_COIL:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_req_read_coils = (modbus_serial_slave_req_read_coils_t *) pv_request_data;

            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_resp_read_coils = (modbus_serial_slave_resp_read_coils_t *) pv_response_data;
            cb_func_code01(p_req_read_coils, p_resp_read_coils);
            break;
        }
        case MODBUS_SERIAL_FUNC_CODE_READ_INPUT:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_req_read_inputs = (modbus_serial_slave_req_read_inputs_t *) pv_request_data;

            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_resp_read_inputs = (modbus_serial_slave_resp_read_inputs_t *) pv_response_data;
            cb_func_code02(p_req_read_inputs, p_resp_read_inputs);
            break;
        }
        case MODBUS_SERIAL_FUNC_CODE_READ_HOLD_REG:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_req_read_holding_reg = (modbus_serial_slave_req_read_holding_reg_t *) pv_request_data;

            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_resp_read_holding_reg = (modbus_serial_slave_resp_read_holding_reg_t *) pv_response_data;
            cb_func_code03(p_req_read_holding_reg, p_resp_read_holding_reg);
            break;
        }
        case MODBUS_SERIAL_FUNC_CODE_READ_INPUT_REG:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_req_read_input_reg = (modbus_serial_slave_req_read_input_reg_t *) pv_request_data;

            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_resp_read_input_reg = (modbus_serial_slave_resp_read_input_reg_t *) pv_response_data;
            cb_func_code04(p_req_read_input_reg, p_resp_read_input_reg);
            break;
        }
        case MODBUS_SERIAL_FUNC_CODE_WRITE_SINGLE_COIL:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_req_write_single_coil = (modbus_serial_slave_req_write_single_coil_t *) pv_request_data;

            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_resp_write_single_coil = (modbus_serial_slave_resp_write_single_coil_t *) pv_response_data;
            cb_func_code05(p_req_write_single_coil, p_resp_write_single_coil);
            break;
        }
        case MODBUS_SERIAL_FUNC_CODE_WRITE_SINGLE_REG:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_req_write_single_reg = (modbus_serial_slave_req_write_single_reg_t *) pv_request_data;

            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_resp_write_single_reg = (modbus_serial_slave_resp_write_single_reg_t *) pv_response_data;
            cb_func_code06(p_req_write_single_reg, p_resp_write_single_reg);
            break;
        }
        case MODBUS_SERIAL_FUNC_CODE_WRITE_MULTIPLE_COIL:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_req_write_multiple_coils = (modbus_serial_slave_req_write_multiple_coils_t *) pv_request_data;

            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_resp_write_multiple_coils = (modbus_serial_slave_resp_write_multiple_coils_t *) pv_response_data;
            cb_func_code15(p_req_write_multiple_coils, p_resp_write_multiple_coils);
            break;
        }
        case MODBUS_SERIAL_FUNC_CODE_WRITE_MULTIPLE_REG:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_req_write_multiple_reg = (modbus_serial_slave_req_write_multiple_reg_t *) pv_request_data;

            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_resp_write_multiple_reg = (modbus_serial_slave_resp_write_multiple_reg_t *) pv_response_data;
            cb_func_code16(p_req_write_multiple_reg, p_resp_write_multiple_reg);
            break;
        }
        case MODBUS_SERIAL_FUNC_CODE_READ_WRITE_MULTIPLE_REG:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_req_read_write_multiple_reg = (modbus_serial_slave_req_read_write_multiple_reg_t *) pv_request_data;

            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_resp_read_write_multiple_reg = (modbus_serial_slave_resp_read_write_multiple_reg_t *) pv_response_data;
            cb_func_code23(p_req_read_write_multiple_reg, p_resp_read_write_multiple_reg);
            break;
        }
        default:
        {
            break;
        }
    }
}
/**********************************************************************************************************************
 End of function function_code_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cb_func_code01
 * Description  : Call back function pointer for MODBUS function code 1(Read Coils) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a read coils(function code 1) processing function to this function 
 *                pointer and stack invokes user registered function when a request received from client side.
 *                If this function pointer is set to NULL means read coils support is not implemented or supported
 *                by application/device.
 * Argument     : pt_request   structure pointer from stack to user with read coils request information
 *                pt_response  structure pointer to stack from user with read coils response data
 * Return Value : 0 on success of read coils operation
 *                1 on failure of read coils operation
 *********************************************************************************************************************/
static uint32_t cb_func_code01(p_modbus_serial_slave_req_read_coils_t pt_request, 
                                p_modbus_serial_slave_resp_read_coils_t pt_response)
{
    uint8_t u8_data;
    uint8_t u8_num_of_bytes;
    uint8_t u8_res;

    int16_t i;

    pt_response->transaction_id  = pt_request->transaction_id;
    pt_response->protocol_id     = pt_request->protocol_id;
    pt_response->slave_id        = pt_request->slave_id;
    pt_response->exception_code  = 0;

    /* Check, Starting Address */
    if ((pt_request->start_addr < MODBUS_COIL_END_ADDR) &&
        ((pt_request->start_addr + pt_request->num_of_coils) <= MODBUS_COIL_END_ADDR))
    {
        if ((pt_request->num_of_coils % MB_BIT_8) == 0)
        {
            /* Cast int to uint8_t */
            u8_num_of_bytes = (uint8_t) pt_request->num_of_coils / MB_BIT_8;
        }
        else
        {
            /* Cast int to uint8_t */
            u8_num_of_bytes = (uint8_t)(pt_request->num_of_coils / MB_BIT_8) + 1;
        }
        pt_response->num_of_bytes = u8_num_of_bytes;

        /*scan_coil(&u8_data); */
        for (i = 0; i < pt_request->num_of_coils; i++)
        {
            if ((i % MB_BIT_8) == 0)
            {
                pt_response->data[i / MB_BIT_8] = 0;
            }
            u8_res = (*MB_Coils_Read[pt_request->start_addr + i]) (&u8_data);
            if (MODBUS_ERR_OK != u8_res)
            {
                return (mb_func_exp_execution (&pt_response->exception_code, u8_res));
            }
            pt_response->data[i / MB_BIT_8] |= (u8_data << (i % MB_BIT_8));
        }
    }
    else
    {
        return (mb_func_exp_addr (&pt_response->exception_code));
    }
    return (MODBUS_ERR_OK);
}
/**********************************************************************************************************************
 *  End of function cb_func_code01
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cb_func_code02
 * Description  : Call back function pointer for MODBUS function code 2(Read Discrete Inputs) processing
 * 
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a read discrete inputs(function code 2) processing function to 
 *                this function pointer and stack invokes user registered function when a request received from 
 *                client side.
 *                If this function pointer is set to NULL means read discrete inputs support is not implemented or 
 *                supported by application/device.
 * Argument     : pt_request   structure pointer from stack to user with read discrete inputs request information
 *                pt_response  structure pointer to stack from user with read discrete inputs response data
 * Return Value : 0 on success of read discrete inputs operation
 *                1 on failure of read discrete inputs operation
 *********************************************************************************************************************/
static uint32_t cb_func_code02(p_modbus_serial_slave_req_read_inputs_t pt_request, 
                                p_modbus_serial_slave_resp_read_inputs_t pt_response)
{
    uint8_t u8_data;
    uint8_t u8_num_of_bytes;
    uint8_t u8_res;

    int16_t i;

    pt_response->transaction_id  = pt_request->transaction_id;
    pt_response->protocol_id     = pt_request->protocol_id;
    pt_response->slave_id        = pt_request->slave_id;
    pt_response->exception_code  = 0;

    /* Check, Starting Address */
    if ((pt_request->start_addr < MODBUS_DISCRETE_INPUT_END_ADDR) &&
        ((pt_request->start_addr + pt_request->num_of_inputs) <= MODBUS_DISCRETE_INPUT_END_ADDR))
    {
        if ((pt_request->num_of_inputs % MB_BIT_8) == 0)
        {
            /* Cast int to uint8_t */
            u8_num_of_bytes = (uint8_t)pt_request->num_of_inputs / MB_BIT_8;
        }
        else
        {
            /* Cast int to uint8_t */
            u8_num_of_bytes = (uint8_t)(pt_request->num_of_inputs / MB_BIT_8) + 1;
        }
        pt_response->num_of_bytes = u8_num_of_bytes;

        /*  scan_input_status(&u8_data); */
        for (i = 0; i < pt_request->num_of_inputs; i++)
        {
            if ((i % MB_BIT_8) == 0)
            {
                pt_response->data[i / MB_BIT_8] = 0;
            }
            u8_res = (*MB_Discretes_Input[pt_request->start_addr + i]) (&u8_data);
            if (MODBUS_ERR_OK != u8_res)
            {
                return (mb_func_exp_execution (&pt_response->exception_code, u8_res));
            }
            pt_response->data[i / MB_BIT_8] |= (u8_data << (i % MB_BIT_8));
        }
    }
    else
    {
        return (mb_func_exp_addr (&pt_response->exception_code));
    }
    pt_response->num_of_bytes = u8_num_of_bytes;
    return (MODBUS_ERR_OK);
}
/**********************************************************************************************************************
 *  End of function cb_func_code02
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cb_func_code03
 * Description  : Call back function pointer for MODBUS function code 3(Read Holding Registers) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a read holding registers(function code 3) processing function
 *                to this function pointer and stack invokes user registered function when a request received from 
 *                client side.
 *                If this function pointer is set to NULL means read holding registers support is not implemented or 
 *                supported by application/device.
 * Argument     : pt_request      structure pointer from stack to user with read holding registers request information
 *                pt_response     structure pointer to stack from user with read holding registers response data
 * Return Value : 0 on success of read holding registers operation
 *                1 on failure of read holding registers operation
 *********************************************************************************************************************/
static uint32_t cb_func_code03(p_modbus_serial_slave_req_read_holding_reg_t pt_request, 
                                p_modbus_serial_slave_resp_read_holding_reg_t pt_response)
{
    uint16_t i;

    uint16_t u16_data;
    uint8_t  u8_res;

    pt_response->transaction_id  = pt_request->transaction_id;
    pt_response->protocol_id     = pt_request->protocol_id;
    pt_response->slave_id        = pt_request->slave_id;
    pt_response->exception_code  = 0;

    /* Check, Starting Address */
    if ((pt_request->start_addr < MODBUS_HOLDING_REG_END_ADDR) &&
        ((pt_request->start_addr + pt_request->num_of_reg) <= MODBUS_HOLDING_REG_END_ADDR))
    {
        /* Cast int to uint8_t */
        pt_response->num_of_bytes = (uint8_t)pt_request->num_of_reg * 2;

        for (i = 0; i < pt_request->num_of_reg; i++)
        {
            u8_res = (*MB_HoldingRegs_Read[pt_request->start_addr + i]) (&u16_data);
            if (MODBUS_ERR_OK != u8_res)
            {
                return (mb_func_exp_execution (&pt_response->exception_code, u8_res));
            }
            pt_response->data[i] = u16_data;
        }
    }
    else
    {
        return (mb_func_exp_addr (&pt_response->exception_code));
    }

    return (MODBUS_ERR_OK);
}
/**********************************************************************************************************************
 *  End of function cb_func_code03
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cb_func_code04
 * Description  : Call back function pointer for MODBUS function code 4(Read Input Registers) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a read input registers(function code 4) processing function 
 *                to this function pointer and stack invokes user registered function when a request received from 
 *                client side.
 *                If this function pointer is set to NULL means read input registers support is not implemented or 
 *                supported by application/device.
 * Argument     : pt_request      structure pointer from stack to user with read input registers request information
 *                pt_response     structure pointer to stack from user with read input registers response data
 * Return Value : 0 on success of read input registers operation
 *                1 on failure of read input registers operation
 *********************************************************************************************************************/
static uint32_t cb_func_code04(p_modbus_serial_slave_req_read_input_reg_t pt_request, 
                                p_modbus_serial_slave_resp_read_input_reg_t pt_response)
{
    uint16_t i;

    uint16_t u16_data;

    uint8_t u8_res;

    pt_response->transaction_id  = pt_request->transaction_id;
    pt_response->protocol_id     = pt_request->protocol_id;
    pt_response->slave_id        = pt_request->slave_id;
    pt_response->exception_code  = 0;

    /* Check, Starting Address */
    if ((pt_request->start_addr < MODBUS_INPUT_REG_END_ADDR) &&
        ((pt_request->start_addr + pt_request->num_of_reg) <= MODBUS_INPUT_REG_END_ADDR))
    {
        /* Cast int to uint8_t */
        pt_response->num_of_bytes = (uint8_t)pt_request->num_of_reg * 2;

        for (i = 0; i < pt_request->num_of_reg; i++)
        {
            u8_res = (*MB_Input_Regs[pt_request->start_addr + i]) (&u16_data);
            if (MODBUS_ERR_OK != u8_res)
            {
                return (mb_func_exp_execution (&pt_response->exception_code, u8_res));
            }
            pt_response->data[i] = u16_data;
        }
    }
    else
    {
        return (mb_func_exp_addr (&pt_response->exception_code));
    }

    return (MODBUS_ERR_OK);
}
/**********************************************************************************************************************
 *  End of function cb_func_code04
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cb_func_code05
 * Description  : Call back function pointer for MODBUS function code 5(Write Single Coil) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a write single coil(function code 5) processing function to 
 *                this function pointer and stack invokes user registered function when a request received from 
 *                client side.
 *                If this function pointer is set to NULL means write single coil support is not implemented or 
 *                supported by application/device.
 * Argument     : pt_request      structure pointer from stack to user with write single coil request information
 *                pt_response     structure pointer to stack from user with write single coil response data
 * Return Value : 0 on success of write single coil operation
 *                1 on failure of write single coil operation
 *********************************************************************************************************************/
static uint32_t cb_func_code05(p_modbus_serial_slave_req_write_single_coil_t pt_request, 
                                p_modbus_serial_slave_resp_write_single_coil_t pt_response)
{
    uint8_t u8_data;
    uint8_t u8_res;

    pt_response->transaction_id  = pt_request->transaction_id;
    pt_response->protocol_id     = pt_request->protocol_id;
    pt_response->slave_id        = pt_request->slave_id;
    pt_response->exception_code  = 0;

    /* Check, Register Value. Output Value == 0x0000 OR 0xFF00 */
    if (MODBUS_SERIAL_MAX_WRITE_COIL_VALUE == pt_request->output_value)
    {
        u8_data = 0x01;
    }
    else if (MODBUS_SERIAL_MIN_WRITE_COIL_VALUE == pt_request->output_value)
    {
        u8_data = 0x00;
    }
    else
    {
        return (mb_func_exp_data (&pt_response->exception_code));
    }
    
    /* Check, Register Address */
    if (pt_request->output_addr < MODBUS_COIL_END_ADDR)
    {
        pt_response->output_addr  = pt_request->output_addr;
        pt_response->output_value = pt_request->output_value;

        u8_res = (*MB_Coils_Write[pt_request->output_addr]) (u8_data);
        if (MODBUS_ERR_OK != u8_res)
        {
            return (mb_func_exp_execution (&pt_response->exception_code, u8_res));
        }
    }
    else
    {
        return (mb_func_exp_addr (&pt_response->exception_code));
    }

    return (MODBUS_ERR_OK);
}
/**********************************************************************************************************************
 *  End of function cb_func_code05
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cb_func_code06
 * Description  : Call back function pointer for MODBUS function code 6(Write Single Register) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a write single register(function code 6) processing function to 
 *                this function pointer and stack invokes user registered function when a request received from 
 *                client side.
 *                If this function pointer is set to NULL means write single register support is not implemented or 
 *                supported by application/device.
 * Argument     : pt_request      structure pointer from stack to user with write single register request information
 *                pt_response     structure pointer to stack from user with write single register response data
 * Return Value : 0 on success of write single register operation
 *                1 on failure of write single register operation
 *********************************************************************************************************************/
static uint32_t cb_func_code06(p_modbus_serial_slave_req_write_single_reg_t pt_request, 
                                p_modbus_serial_slave_resp_write_single_reg_t pt_response)
{
    uint8_t u8_res;

    pt_response->transaction_id  = pt_request->transaction_id;
    pt_response->protocol_id     = pt_request->protocol_id;
    pt_response->slave_id        = pt_request->slave_id;
    pt_response->exception_code  = 0;

    /* Check, Register Address */
    if ((pt_request->register_addr < MODBUS_HOLDING_REG_END_ADDR))
    {
        pt_response->register_addr  = pt_request->register_addr;
        pt_response->register_value = pt_request->register_value;

        u8_res = (*MB_HoldingRegs_Write[pt_request->register_addr]) (pt_request->register_value);
        if (MODBUS_ERR_OK != u8_res)
        {
            return (mb_func_exp_execution (&pt_response->exception_code, u8_res));
        }
    }
    else
    {
        return (mb_func_exp_addr (&pt_response->exception_code));
    }

    return (MODBUS_ERR_OK);
}
/**********************************************************************************************************************
 *  End of function cb_func_code06
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cb_func_code15
 * Description  : Call back function pointer for MODBUS function code 15(Write Multiple Coils) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a write multiple coils(function code 15) processing function to 
 *                this function pointer and stack invokes user registered function when a request received from 
 *                client side.
 *                If this function pointer is set to NULL means write multiple coils support is not implemented or 
 *                supported by application/device.
 * Argument     : pt_request      structure pointer from stack to user with write multiple coils request information
 *                pt_response     structure pointer to stack from user with write multiple coils response data
 * Return Value : 0 on success of write multiple coils operation
 *                1 on failure of write multiple coils operation
 *********************************************************************************************************************/
static uint32_t cb_func_code15(p_modbus_serial_slave_req_write_multiple_coils_t pt_request, 
                                p_modbus_serial_slave_resp_write_multiple_coils_t pt_response)
{
    uint8_t u8_res;

    uint16_t i;

    pt_response->transaction_id  = pt_request->transaction_id;
    pt_response->protocol_id     = pt_request->protocol_id;
    pt_response->slave_id        = pt_request->slave_id;
    pt_response->exception_code  = 0;

    /* Check, Starting Address */
    if ((pt_request->start_addr < MODBUS_COIL_END_ADDR) &&
        ((pt_request->start_addr + pt_request->num_of_outputs) <= MODBUS_COIL_END_ADDR))
    {
        pt_response->start_addr     = pt_request->start_addr;
        pt_response->num_of_outputs = pt_request->num_of_outputs;

        for (i = 0; i < pt_request->num_of_outputs; i++)
        {
            u8_res = (*MB_Coils_Write[pt_request->start_addr + i]) (
                    /* Cast int to uint8_t */
                    (uint8_t)(pt_request->data[(i / MB_BIT_8)] & (1 << (i % MB_BIT_8))) >> (i % MB_BIT_8));
            if (MODBUS_ERR_OK != u8_res)
            {
                return (mb_func_exp_execution (&pt_response->exception_code, u8_res));
            }
        }

    }
    else
    {
        return (mb_func_exp_addr (&pt_response->exception_code));
    }

    return (MODBUS_ERR_OK);
}
/**********************************************************************************************************************
 *  End of function cb_func_code15
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cb_func_code16
 * Description  : Call back function pointer for MODBUS function code 16(Write Multiple Registers) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a write multiple registers(function code 16) processing function to 
 *                this function pointer and stack invokes user registered function when a request received from 
 *                client side.
 *                If this function pointer is set to NULL means write multiple registers support is not implemented or 
 *                supported by application/device.
 * Argument     : pt_request      structure pointer from stack to user with write multiple registers request information
 *                pt_response     structure pointer to stack from user with write multiple registers response data
 * Return Value : 0 on success of write multiple registers operation
 *                1 on failure of write multiple registers operation
 *********************************************************************************************************************/
static uint32_t cb_func_code16(p_modbus_serial_slave_req_write_multiple_reg_t pt_request, 
                                p_modbus_serial_slave_resp_write_multiple_reg_t pt_response)
{
    uint8_t u8_res;

    uint16_t i;

    pt_response->transaction_id  = pt_request->transaction_id;
    pt_response->protocol_id     = pt_request->protocol_id;
    pt_response->slave_id        = pt_request->slave_id;
    pt_response->exception_code  = 0;

    /* Check, Starting Address */
    if ((pt_request->start_addr < MODBUS_HOLDING_REG_END_ADDR) &&
        ((pt_request->start_addr + pt_request->num_of_reg) <= MODBUS_HOLDING_REG_END_ADDR))
    {
        pt_response->start_addr = pt_request->start_addr;
        pt_response->num_of_reg = pt_request->num_of_reg;

        for (i = 0; i < pt_request->num_of_reg; i++)
        {
            u8_res = (*MB_HoldingRegs_Write[pt_request->start_addr + i]) (pt_request->data[i]);
            if (MODBUS_ERR_OK != u8_res)
            {
                return (mb_func_exp_execution (&pt_response->exception_code, u8_res));
            }
        }
    }
    else
    {
        return (mb_func_exp_addr (&pt_response->exception_code));
    }

    return (MODBUS_ERR_OK);
}
/**********************************************************************************************************************
 *  End of function cb_func_code16
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cb_func_code23
 * Description  : Call back function pointer for MODBUS function code 23(Read/Write Multiple Registers) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a read/write multiple registers(function code 23) processing 
 *                function to this function pointer and stack invokes user registered function when a request 
 *                received from client side.
 *                If this function pointer is set to NULL means read/write multiple registers support is not 
 *                implemented or supported by application/device.
 * Argument     : pt_request      structure pointer from stack to user with read/write multiple registers request 
 *                                information
 *                pt_response     structure pointer to stack from user with read/write multiple registers response data
 * Return Value : 0 on success of read/write multiple registers operation
 *                1 on failure of read/write multiple registers operation
 *********************************************************************************************************************/
static uint32_t cb_func_code23(p_modbus_serial_slave_req_read_write_multiple_reg_t pt_request, 
                                p_modbus_serial_slave_resp_read_write_multiple_reg_t pt_response)
{
    uint16_t i;

    uint16_t u16_data;

    uint8_t u8_res;

    pt_response->transaction_id  = pt_request->transaction_id;
    pt_response->protocol_id     = pt_request->protocol_id;
    pt_response->slave_id        = pt_request->slave_id;
    pt_response->exception_code  = 0;

    if ((!(pt_request->write_start_addr < MODBUS_HOLDING_REG_END_ADDR)) ||
        (!((pt_request->write_start_addr + pt_request->num_to_write) <= MODBUS_HOLDING_REG_END_ADDR)) ||
        (!(pt_request->read_start_addr < MODBUS_HOLDING_REG_END_ADDR)) ||
        (!((pt_request->read_start_addr + pt_request->num_to_read) <= MODBUS_HOLDING_REG_END_ADDR)))
    {
        return (mb_func_exp_addr (&pt_response->exception_code));
    }
    
    /*  The write operation is performed before the read.  */
    {
        /* Cast int to uint8_t */
        pt_response->num_of_bytes = (uint8_t)pt_request->num_to_read * 2;

        /* u16_base = pt_request->write_start_addr; */
        for (i = 0; i < pt_request->num_to_write; i++)
        {
            /*g_HoldingRegArea[u16_base + i] = pt_request->data[i]; */
            u8_res = (*MB_HoldingRegs_Write[pt_request->write_start_addr + i]) (pt_request->data[i]);
            if (MODBUS_ERR_OK != u8_res)
            {
                return (mb_func_exp_execution (&pt_response->exception_code, u8_res));
            }
        }
    }
    {
        /* u16_base = pt_request->write_start_addr; */
        for (i = 0; i < pt_request->num_to_read; i++)
        {
            /* pt_response->aru16_read_data[i] = g_HoldingRegArea[u16_base + i];*/
            u8_res = (*MB_HoldingRegs_Read[pt_request->read_start_addr + i]) (&u16_data);
            if (MODBUS_ERR_OK != u8_res)
            {
                return (mb_func_exp_execution (&pt_response->exception_code, u8_res));
            }
            pt_response->read_data[i] = u16_data;
        }
    }
    return (MODBUS_ERR_OK);
}
/**********************************************************************************************************************
 *  End of function cb_func_code23
 *********************************************************************************************************************/

/************************************************
 * ILLEGAL ADDRESS
 */
/**********************************************************************************************************************
 * Function Name: mb_func_exp_addr
 * Description  : Sets MODBUS_ERR_ILLEGAL_DATA_ADDRESS to the argument pointer
 * Argument     : uint8_t *res
 * Return Value : MODBUS_ERR_OK
 *********************************************************************************************************************/
static uint32_t mb_func_exp_addr(uint8_t *res)
{
    *res = MODBUS_ERR_ILLEGAL_DATA_ADDRESS;
    return (MODBUS_ERR_OK) /*3byte*/;
}
/**********************************************************************************************************************
 *  End of function mb_func_exp_addr
 *********************************************************************************************************************/

/************************************************
 * ILLEGAL_VALUE
 */
/**********************************************************************************************************************
 * Function Name: mb_func_exp_data
 * Description  : Sets MODBUS_ERR_ILLEGAL_DATA_VALUE to the argument pointer
 * Argument     : uint8_t *res
 * Return Value : MODBUS_ERR_OK
 *********************************************************************************************************************/
static uint32_t mb_func_exp_data(uint8_t *u8_res)
{
    *u8_res = MODBUS_ERR_ILLEGAL_DATA_VALUE;
    return (MODBUS_ERR_OK) /*3*/;
}
/**********************************************************************************************************************
 *  End of function mb_func_exp_data
 *********************************************************************************************************************/

/************************************************
 * ILLEGAL_CMD
 */
/**********************************************************************************************************************
 * Function Name: mb_func_exp_execution
 * Description  : Sets MODBUS_ERR_SLAVE_DEVICE_FAILURE to the argument pointer
 * Argument     : uint8_t *res
 *                int type
 * Return Value : MODBUS_ERR_OK
 *********************************************************************************************************************/
static uint32_t mb_func_exp_execution(uint8_t *u8_res, int type)
{
    if (MODBUS_ERR_ILLEGAL_DATA_VALUE == type) /* write data error */
    {
        *u8_res = MODBUS_ERR_ILLEGAL_DATA_VALUE;
    }
    else
    {
        *u8_res = MODBUS_ERR_SLAVE_DEVICE_FAILURE;
    }
    return (MODBUS_ERR_OK) /*3*/;
}
/**********************************************************************************************************************
 *  End of function mb_func_exp_execution
 *********************************************************************************************************************/

/* End of File */
