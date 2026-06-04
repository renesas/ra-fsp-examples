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
 * Description  : This file implements the callback functions called from the Modbus TCP Server and functions 
 *                for each Function code.
 *********************************************************************************************************************/

/*============================================================================*/
/* I N C L U D E                                                              */
/*============================================================================*/
#include "hal_data.h"
#include "r_modbus_tcp_server_core.h"

/*============================================================================*/
/* E X T E R N                                                                */
/*============================================================================*/
extern uint32_t g_coil_end_addr;
extern uint32_t g_discrete_input_end_addr;
extern uint32_t g_holding_reg_end_addr;
extern uint32_t g_input_reg_end_addr;

extern modbus_tcp_server_user_function_error_t (*MB_Coils_Read[])(uint8_t *data);
extern modbus_tcp_server_user_function_error_t (*MB_Coils_Write[])(uint8_t data);
extern modbus_tcp_server_user_function_error_t (*MB_Discretes_Input[])(uint8_t *data);
extern modbus_tcp_server_user_function_error_t (*MB_HoldingRegs_Read[])(uint16_t *data);
extern modbus_tcp_server_user_function_error_t (*MB_HoldingRegs_Write[])(uint16_t data);
extern modbus_tcp_server_user_function_error_t (*MB_Input_Regs[])(uint16_t *data);

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
static void mb_func_exp_addr (modbus_tcp_server_user_function_error_t * res);
static void mb_func_exp_data (modbus_tcp_server_user_function_error_t * res);
static void mb_func_exp_execution (modbus_tcp_server_user_function_error_t * res,
        modbus_tcp_server_user_function_error_t type);

/*============================================================================*/
/* P R O G R A M                                                              */
/*============================================================================*/
static void func_code01 (modbus_tcp_server_read_coils_t * const p_fc_args);
static void func_code02 (modbus_tcp_server_read_inputs_t * const p_fc_args);
static void func_code03 (modbus_tcp_server_read_holding_registers_t * const p_fc_args);
static void func_code04 (modbus_tcp_server_read_input_registers_t * const p_fc_args);
static void func_code05 (modbus_tcp_server_write_single_coil_t * const p_fc_args);
static void func_code06 (modbus_tcp_server_write_single_register_t * const p_fc_args);
static void func_code15 (modbus_tcp_server_write_multiple_coils_t * const p_fc_args);
static void func_code16 (modbus_tcp_server_write_multiple_registers_t * const p_fc_args);
static void func_code23 (modbus_tcp_server_read_write_multiple_registers_t * const p_fc_args);


/**********************************************************************************************************************
 * Function Name: function_code_callback
 * Description  : Callback functions called from the Modbus TCP Server
 * Argument     : modbus_tcp_server_callback_args_t * const p_args
 * Return Value : None
 *********************************************************************************************************************/
void function_code_callback(modbus_tcp_server_callback_args_t * const p_args)
{
    modbus_tcp_server_read_coils_t                    * p_tcp_read_coils              = NULL;
    modbus_tcp_server_read_inputs_t                   * p_tcp_read_inputs             = NULL;
    modbus_tcp_server_read_holding_registers_t        * p_tcp_read_holding_reg        = NULL;
    modbus_tcp_server_read_input_registers_t          * p_tcp_read_input_reg          = NULL;
    modbus_tcp_server_write_single_coil_t             * p_tcp_write_single_coil       = NULL;
    modbus_tcp_server_write_single_register_t         * p_tcp_write_single_reg        = NULL;
    modbus_tcp_server_write_multiple_coils_t          * p_tcp_write_multiple_coils    = NULL;
    modbus_tcp_server_write_multiple_registers_t      * p_tcp_write_multiple_reg      = NULL;
    modbus_tcp_server_read_write_multiple_registers_t * p_tcp_read_write_multiple_reg = NULL;

    switch (p_args->function_code)
    {
        case MODBUS_TCP_SERVER_FUNCTION_CODE_READ_COILS:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_tcp_read_coils = (modbus_tcp_server_read_coils_t *) p_args->request_and_response_data;
            func_code01(p_tcp_read_coils);
            break;
        }
        case MODBUS_TCP_SERVER_FUNCTION_CODE_READ_DISCRETE_INPUTS:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_tcp_read_inputs = (modbus_tcp_server_read_inputs_t *) p_args->request_and_response_data;
            func_code02(p_tcp_read_inputs);
            break;
        }
        case MODBUS_TCP_SERVER_FUNCTION_CODE_READ_HOLDING_REGISTERS:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_tcp_read_holding_reg = (modbus_tcp_server_read_holding_registers_t *) p_args->request_and_response_data;
            func_code03(p_tcp_read_holding_reg);
            break;
        }
        case MODBUS_TCP_SERVER_FUNCTION_CODE_READ_INPUT_REGISTERS:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_tcp_read_input_reg = (modbus_tcp_server_read_input_registers_t *) p_args->request_and_response_data;
            func_code04(p_tcp_read_input_reg);
            break;
        }
        case MODBUS_TCP_SERVER_FUNCTION_CODE_WRITE_SINGLE_COIL:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_tcp_write_single_coil = (modbus_tcp_server_write_single_coil_t *) p_args->request_and_response_data;
            func_code05(p_tcp_write_single_coil);
            break;
        }
        case MODBUS_TCP_SERVER_FUNCTION_CODE_WRITE_SINGLE_REGISTER:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_tcp_write_single_reg = (modbus_tcp_server_write_single_register_t *) p_args->request_and_response_data;
            func_code06(p_tcp_write_single_reg);
            break;
        }
        case MODBUS_TCP_SERVER_FUNCTION_CODE_WRITE_MULTIPLE_COILS:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_tcp_write_multiple_coils = (modbus_tcp_server_write_multiple_coils_t *) p_args->request_and_response_data;
            func_code15(p_tcp_write_multiple_coils);
            break;
        }
        case MODBUS_TCP_SERVER_FUNCTION_CODE_WRITE_MULTIPLE_REGISTERS:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_tcp_write_multiple_reg = (modbus_tcp_server_write_multiple_registers_t *) p_args->request_and_response_data;
            func_code16(p_tcp_write_multiple_reg);
            break;
        }
        case MODBUS_TCP_SERVER_FUNCTION_CODE_READ_WRITE_MULTIPLE_REGISTERS:
        {
            /* Cast the request data to a data structure for Function Code within the appropriate range. */
            p_tcp_read_write_multiple_reg = (modbus_tcp_server_read_write_multiple_registers_t *) p_args->request_and_response_data;
            func_code23(p_tcp_read_write_multiple_reg);
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
 * Function Name: func_code01
 * Description  : Call back function pointer for MODBUS function code 1(Read Coils) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a read coils(function code 1) processing function to this function 
 *                pointer and stack invokes user registered function when a request received from client side.
 *                If this function pointer is set to NULL means read coils support is not implemented or supported
 *                by application/device.
 * Argument     : modbus_tcp_server_read_coils_t * const p_fc_args
 * Return Value : None
 *********************************************************************************************************************/
static void func_code01(modbus_tcp_server_read_coils_t * const p_fc_args)
{
    uint8_t u8_data;
    uint8_t u8_num_of_bytes;
    modbus_tcp_server_user_function_error_t res;

    uint16_t i;

    p_fc_args->exception_code = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS;

    /* Check, Starting Address */
    if ((p_fc_args->request_start_address < MODBUS_COIL_END_ADDR) &&
        ((p_fc_args->request_start_address + p_fc_args->request_num_of_coils) <= MODBUS_COIL_END_ADDR))
    {
        if ((p_fc_args->request_num_of_coils % MB_BIT_8) == 0)
        {
            /* Convert the number of coils (in bits) to the number of response bytes and 
             * cast it within the appropriate range */
            u8_num_of_bytes = (uint8_t) p_fc_args->request_num_of_coils / MB_BIT_8;
        }
        else
        {
            /* Convert the number of coils (in bits) to the number of response bytes and 
             * cast it within the appropriate range */
            u8_num_of_bytes = (uint8_t)((p_fc_args->request_num_of_coils / MB_BIT_8) + 1);
        }
        p_fc_args->response_num_of_bytes = u8_num_of_bytes;

        for (i = 0; i < p_fc_args->request_num_of_coils; i++)
        {
            if ((i % MB_BIT_8) == 0)
            {
                p_fc_args->response_data[i / MB_BIT_8] = 0;
            }
            res = (*MB_Coils_Read[p_fc_args->request_start_address + i]) (&u8_data);
            if (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS != res)
            {
                mb_func_exp_execution (&p_fc_args->exception_code, res);
                return;
            }
            p_fc_args->response_data[i / MB_BIT_8] |= (u8_data << (i % MB_BIT_8));
        }
    }
    else
    {
        mb_func_exp_addr (&p_fc_args->exception_code);
    }
}
/**********************************************************************************************************************
 *  End of function func_code01
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: func_code02
 * Description  : Call back function pointer for MODBUS function code 2(Read Discrete Inputs) processing
 * 
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a read discrete inputs(function code 2) processing function to 
 *                this function pointer and stack invokes user registered function when a request received from 
 *                client side.
 *                If this function pointer is set to NULL means read discrete inputs support is not implemented or 
 *                supported by application/device.
 * Argument     : modbus_tcp_server_read_inputs_t * const p_fc_args
 * Return Value : None
 *********************************************************************************************************************/
static void func_code02(modbus_tcp_server_read_inputs_t * const p_fc_args)
{
    uint8_t u8_data;
    uint8_t u8_num_of_bytes;
    modbus_tcp_server_user_function_error_t res;
    
    uint16_t i;

    p_fc_args->exception_code = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS;

    /* Check, Starting Address */
    if ((p_fc_args->request_start_address < MODBUS_DISCRETE_INPUT_END_ADDR)
            && ((p_fc_args->request_start_address + p_fc_args->request_num_of_inputs)
                <= MODBUS_DISCRETE_INPUT_END_ADDR))
    {
        if ((p_fc_args->request_num_of_inputs % MB_BIT_8) == 0)
        {
            /* Convert the number of Discrete Inputs (in bits) to the number of response bytes and 
             * cast it within the appropriate range */
            u8_num_of_bytes = (uint8_t)(p_fc_args->request_num_of_inputs / MB_BIT_8);
        }
        else
        {
            /* Convert the number of Discrete Inputs (in bits) to the number of response bytes and 
             * cast it within the appropriate range */
            u8_num_of_bytes = (uint8_t)((p_fc_args->request_num_of_inputs / MB_BIT_8) + 1);
        }
        p_fc_args->response_num_of_bytes = u8_num_of_bytes;

        for (i = 0; i < p_fc_args->request_num_of_inputs; i++)
        {
            if ((i % MB_BIT_8) == 0)
            {
                p_fc_args->response_data[i / MB_BIT_8] = 0;
            }
            res = (*MB_Discretes_Input[p_fc_args->request_start_address + i]) (&u8_data);
            if (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS != res)
            {
                mb_func_exp_execution (&p_fc_args->exception_code, res);
                return;
            }
            p_fc_args->response_data[i / MB_BIT_8] |= (u8_data << (i % MB_BIT_8));
        }
    }
    else
    {
        mb_func_exp_addr (&p_fc_args->exception_code);
        return;
    }
    p_fc_args->response_num_of_bytes = u8_num_of_bytes;
}
/**********************************************************************************************************************
 End of function func_code02
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: func_code03
 * Description  : Call back function pointer for MODBUS function code 3(Read Holding Registers) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a read holding registers(function code 3) processing function
 *                to this function pointer and stack invokes user registered function when a request received from 
 *                client side.
 *                If this function pointer is set to NULL means read holding registers support is not implemented or 
 *                supported by application/device.
 * Argument     : modbus_tcp_server_read_holding_registers_t * const p_fc_args
 * Return Value : None
 *********************************************************************************************************************/
static void func_code03(modbus_tcp_server_read_holding_registers_t * const p_fc_args)
{
    uint16_t i;

    uint16_t u16_data;
    modbus_tcp_server_user_function_error_t  res;

    p_fc_args->exception_code = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS;

    /* Check, Starting Address */
    if ((p_fc_args->request_start_address < MODBUS_HOLDING_REG_END_ADDR)
            && ((p_fc_args->request_start_address + p_fc_args->request_num_of_registers)
                    <= MODBUS_HOLDING_REG_END_ADDR))
    {
        /* Convert the register number to a response byte count and cast it to the appropriate range */
        p_fc_args->response_num_of_bytes = (uint8_t)(p_fc_args->request_num_of_registers * 2);

        for (i = 0; i < p_fc_args->request_num_of_registers; i++)
        {
            res = (*MB_HoldingRegs_Read[p_fc_args->request_start_address + i]) (&u16_data);
            if (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS != res)
            {
                mb_func_exp_execution (&p_fc_args->exception_code, res);
                return;
            }
            p_fc_args->response_data[i] = u16_data;
        }
    }
    else
    {
        mb_func_exp_addr (&p_fc_args->exception_code);
    }
}
/**********************************************************************************************************************
 End of function func_code03
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: func_code04
 * Description  : Call back function pointer for MODBUS function code 4(Read Input Registers) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a read input registers(function code 4) processing function 
 *                to this function pointer and stack invokes user registered function when a request received from 
 *                client side.
 *                If this function pointer is set to NULL means read input registers support is not implemented or 
 *                supported by application/device.
 * Argument     : modbus_tcp_server_read_input_registers_t * const p_fc_args
 * Return Value : None
 *********************************************************************************************************************/
static void func_code04(modbus_tcp_server_read_input_registers_t * const p_fc_args)
{
    uint16_t i;
    
    uint16_t u16_data;
    modbus_tcp_server_user_function_error_t  res;

    p_fc_args->exception_code = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS;

    /* Check, Starting Address */
    if ((p_fc_args->request_start_address < MODBUS_INPUT_REG_END_ADDR)
            && ((p_fc_args->request_start_address + p_fc_args->request_num_of_regisgers) <= MODBUS_INPUT_REG_END_ADDR))
    {
        /* Convert the register number to a response byte count and cast it to the appropriate range */
        p_fc_args->response_num_of_bytes = (uint8_t)(p_fc_args->request_num_of_regisgers * 2);

        for (i = 0; i < p_fc_args->request_num_of_regisgers; i++)
        {
            res = (*MB_Input_Regs[p_fc_args->request_start_address + i]) (&u16_data);
            if (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS != res)
            {
                mb_func_exp_execution (&p_fc_args->exception_code, res);
                return;
            }
            p_fc_args->response_data[i] = u16_data;
        }
    }
    else
    {
        mb_func_exp_addr (&p_fc_args->exception_code);
    }
}
/**********************************************************************************************************************
 End of function func_code04
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: func_code05
 * Description  : Call back function pointer for MODBUS function code 5(Write Single Coil) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a write single coil(function code 5) processing function to 
 *                this function pointer and stack invokes user registered function when a request received from 
 *                client side.
 *                If this function pointer is set to NULL means write single coil support is not implemented or 
 *                supported by application/device.
 * Argument     : modbus_tcp_server_write_single_coil_t * const p_fc_args
 * Return Value : None
 *********************************************************************************************************************/
static void func_code05(modbus_tcp_server_write_single_coil_t * const p_fc_args)
{
    uint8_t u8_data;
    modbus_tcp_server_user_function_error_t res;

    p_fc_args->exception_code = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS;

    /* Check, Register Value. Output Value == 0x0000 OR 0xFF00 */
    if (MODBUS_TCP_SERVER_MAXIMUM_WRITE_COIL_VALUE == p_fc_args->request_output_value)
    {
        u8_data = 0x01;
    }
    else if (MODBUS_TCP_SERVER_MINIMUM_WRITE_COIL_VALUE == p_fc_args->request_output_value)
    {
        u8_data = 0x00;
    }
    else
    {
        mb_func_exp_data (&p_fc_args->exception_code);
        return;
    }
    
    /* Check, Register Address */
    if (p_fc_args->request_output_address < MODBUS_COIL_END_ADDR)
    {
        p_fc_args->response_output_address = p_fc_args->request_output_address;
        p_fc_args->response_output_value   = p_fc_args->request_output_value;

        res = (*MB_Coils_Write[p_fc_args->request_output_address]) (u8_data);
        if (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS != res)
        {
            mb_func_exp_execution (&p_fc_args->exception_code, res);
        }
    }
    else
    {
        mb_func_exp_addr (&p_fc_args->exception_code);
    }
}
/**********************************************************************************************************************
 End of function func_code05
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: func_code06
 * Description  : Call back function pointer for MODBUS function code 6(Write Single Register) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a write single register(function code 6) processing function to 
 *                this function pointer and stack invokes user registered function when a request received from 
 *                client side.
 *                If this function pointer is set to NULL means write single register support is not implemented or 
 *                supported by application/device.
 * Argument     : modbus_tcp_server_write_single_register_t * const p_fc_args
 * Return Value : None
 *********************************************************************************************************************/
static void func_code06(modbus_tcp_server_write_single_register_t * const p_fc_args)
{
    modbus_tcp_server_user_function_error_t res;

    p_fc_args->exception_code = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS;

    /* Check, Register Address */
    if ((p_fc_args->request_register_address < MODBUS_HOLDING_REG_END_ADDR))
    {
        p_fc_args->response_register_address = p_fc_args->request_register_address;
        p_fc_args->response_register_value   = p_fc_args->request_register_value;

        res = (*MB_HoldingRegs_Write[p_fc_args->request_register_address]) (p_fc_args->request_register_value);
        if (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS != res)
        {
            mb_func_exp_execution (&p_fc_args->exception_code, res);
        }
    }
    else
    {
        mb_func_exp_addr (&p_fc_args->exception_code);
    }
}
/**********************************************************************************************************************
 End of function func_code06
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: func_code15
 * Description  : Call back function pointer for MODBUS function code 15(Write Multiple Coils) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a write multiple coils(function code 15) processing function to 
 *                this function pointer and stack invokes user registered function when a request received from 
 *                client side.
 *                If this function pointer is set to NULL means write multiple coils support is not implemented or 
 *                supported by application/device.
 * Argument     : modbus_tcp_server_write_multiple_coils_t * const p_fc_args
 * Return Value : None
 *********************************************************************************************************************/
static void func_code15(modbus_tcp_server_write_multiple_coils_t * const p_fc_args)
{
    modbus_tcp_server_user_function_error_t res;
    
    uint16_t i;

    p_fc_args->exception_code = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS;

    /* Check, Starting Address */
    if ((p_fc_args->request_start_address < MODBUS_COIL_END_ADDR)
            && ((p_fc_args->request_start_address + p_fc_args->request_num_of_outputs) <= MODBUS_COIL_END_ADDR))
    {
        p_fc_args->response_start_address  = p_fc_args->request_start_address;
        p_fc_args->response_num_of_outputs = p_fc_args->request_num_of_outputs;

        for (i = 0; i < p_fc_args->request_num_of_outputs; i++)
        {
            /* Extract the write value of the target coil from the output value and 
             * cast it within the appropriate range. */
            res = (*MB_Coils_Write[p_fc_args->request_start_address + i]) (
                    (uint8_t)((p_fc_args->request_data[(i / MB_BIT_8)] & (1 << (i % MB_BIT_8))) >> (i % MB_BIT_8)));
            if (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS != res)
            {
                mb_func_exp_execution (&p_fc_args->exception_code, res);
                return;
            }
        }

    }
    else
    {
        mb_func_exp_addr (&p_fc_args->exception_code);
    }
}
/**********************************************************************************************************************
 End of function func_code15
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: func_code16
 * Description  : Call back function pointer for MODBUS function code 16(Write Multiple Registers) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a write multiple registers(function code 16) processing function to 
 *                this function pointer and stack invokes user registered function when a request received from 
 *                client side.
 *                If this function pointer is set to NULL means write multiple registers support is not implemented or 
 *                supported by application/device.
 * Argument     : modbus_tcp_server_write_multiple_registers_t * const p_fc_args
 * Return Value : None
 *********************************************************************************************************************/
static void func_code16(modbus_tcp_server_write_multiple_registers_t * const p_fc_args)
{
    modbus_tcp_server_user_function_error_t res;

    uint16_t i;

    p_fc_args->exception_code = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS;

    /* Check, Starting Address */
    if ((p_fc_args->request_start_address < MODBUS_HOLDING_REG_END_ADDR)
            && ((p_fc_args->request_start_address + p_fc_args->request_num_of_registers)
                    <= MODBUS_HOLDING_REG_END_ADDR))
    {
        p_fc_args->response_start_address    = p_fc_args->request_start_address;
        p_fc_args->response_num_of_registers = p_fc_args->request_num_of_registers;

        for (i = 0; i < p_fc_args->request_num_of_registers; i++)
        {
            res = (*MB_HoldingRegs_Write[p_fc_args->request_start_address + i]) (p_fc_args->request_data[i]);
            if (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS != res)
            {
                mb_func_exp_execution (&p_fc_args->exception_code, res);
                return;
            }
        }
    }
    else
    {
        mb_func_exp_addr (&p_fc_args->exception_code);
    }
}
/**********************************************************************************************************************
 End of function func_code16
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: func_code23
 * Description  : Call back function pointer for MODBUS function code 23(Read/Write Multiple Registers) processing
 *
 *                This call back function is only applicable in slave mode configuration of stack.
 *                Application developer can assign a read/write multiple registers(function code 23) processing 
 *                function to this function pointer and stack invokes user registered function when a request 
 *                received from client side.
 *                If this function pointer is set to NULL means read/write multiple registers support is not 
 *                implemented or supported by application/device.
 * Argument     : modbus_tcp_server_read_write_multiple_registers_t * const p_fc_args
 * Return Value : None
 *********************************************************************************************************************/
static void func_code23(modbus_tcp_server_read_write_multiple_registers_t * const p_fc_args)
{
    uint16_t i;

    uint16_t u16_data;
    modbus_tcp_server_user_function_error_t res;

    p_fc_args->exception_code = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS;

    if ((!(p_fc_args->request_write_start_address < MODBUS_HOLDING_REG_END_ADDR))
            || (!((p_fc_args->request_write_start_address + p_fc_args->request_num_to_write)
                    <= MODBUS_HOLDING_REG_END_ADDR))
            || (!(p_fc_args->request_read_start_address < MODBUS_HOLDING_REG_END_ADDR))
            || (!((p_fc_args->request_read_start_address + p_fc_args->request_num_to_read)
                    <= MODBUS_HOLDING_REG_END_ADDR)))
    {
        mb_func_exp_addr (&p_fc_args->exception_code);
        return;
    }

    /*  The write operation is performed before the read.  */
    {
        /* Convert the register number to a response byte count and cast it to the appropriate range */
        p_fc_args->response_num_of_bytes = (uint8_t)(p_fc_args->request_num_to_read * 2);

        for (i = 0; i < p_fc_args->request_num_to_write; i++)
        {
            res = (*MB_HoldingRegs_Write[p_fc_args->request_write_start_address + i]) (p_fc_args->request_data[i]);
            if (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS != res)
            {
                mb_func_exp_execution (&p_fc_args->exception_code, res);
                return;
            }
        }
    }
    {
        for (i = 0; i < p_fc_args->request_num_to_read; i++)
        {
            res = (*MB_HoldingRegs_Read[p_fc_args->request_read_start_address + i]) (&u16_data);
            if (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS != res)
            {
                mb_func_exp_execution (&p_fc_args->exception_code, res);
                return;
            }
            p_fc_args->response_read_data[i] = u16_data;
        }
    }
}
/**********************************************************************************************************************
 End of function func_code23
 *********************************************************************************************************************/

/************************************************
 * ILLEGAL ADDRESS
 */
/**********************************************************************************************************************
 * Function Name: mb_func_exp_addr
 * Description  : Sets MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_ADDRESS to the argument pointer
 * Argument     : modbus_tcp_server_user_function_error_t *res
 * Return Value : None
 *********************************************************************************************************************/
static void mb_func_exp_addr(modbus_tcp_server_user_function_error_t *res)
{
    *res = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_ADDRESS;
}
/**********************************************************************************************************************
 End of function mb_func_exp_addr
 *********************************************************************************************************************/

/************************************************
 * ILLEGAL_VALUE
 */
/**********************************************************************************************************************
 * Function Name: mb_func_exp_data
 * Description  : Sets MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_VALUE to the argument pointer
 * Argument     : modbus_tcp_server_user_function_error_t *res
 * Return Value : None
 *********************************************************************************************************************/
static void mb_func_exp_data(modbus_tcp_server_user_function_error_t *res)
{
    *res = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_VALUE;
}
/**********************************************************************************************************************
 End of function mb_func_exp_data
 *********************************************************************************************************************/

/************************************************
 * ILLEGAL_CMD
 */
/**********************************************************************************************************************
 * Function Name: mb_func_exp_execution
 * Description  : Sets MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SERVER_DEVICE_FAILURE to the argument pointer
 * Argument     : modbus_tcp_server_user_function_error_t *res
 *                modbus_tcp_server_user_function_error_t type
 * Return Value : None
 *********************************************************************************************************************/
static void mb_func_exp_execution(modbus_tcp_server_user_function_error_t *res,
                                    modbus_tcp_server_user_function_error_t type)
{
    if (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_VALUE == type) /* write data error */
    {
        *res = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_VALUE;
    }
    else
    {
        *res = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SERVER_DEVICE_FAILURE;
    }
}
/**********************************************************************************************************************
 End of function mb_func_exp_execution
 *********************************************************************************************************************/

/* End of File */
