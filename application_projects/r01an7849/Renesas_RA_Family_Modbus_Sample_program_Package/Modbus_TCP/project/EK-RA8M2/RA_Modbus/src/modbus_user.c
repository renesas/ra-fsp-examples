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
 * File Name    : modbus_user.c
 * Version      : 1.0
 * Description  : This file implements the Read / Write functions and their function tables corresponding 
 *                to each address of the Coils / Discretes input / Holding registers / Input register.
 *********************************************************************************************************************/

/*============================================================================*/
/* I N C L U D E                                                              */
/*============================================================================*/
#include "hal_data.h"

/*============================================================================*/
/* E X T E R N                                                                */
/*============================================================================*/
/* definition in board_leds.c in FSP */
extern bsp_leds_t g_bsp_leds;

/*============================================================================*/
/* D E F I N E                                                                */
/*============================================================================*/
#define MB_MAXIMUM_DATA_VALUE            (10)

#if defined(BOARD_RA6M3_EK)
    #define MB_SW1            (BSP_IO_PORT_00_PIN_09)
    #define MB_SW2            (BSP_IO_PORT_00_PIN_08)
#elif defined(BOARD_RA6M4_EK)
    #define MB_SW1            (BSP_IO_PORT_00_PIN_05)
    #define MB_SW2            (BSP_IO_PORT_00_PIN_06)
#elif defined(BOARD_RA6M5_EK)
    #define MB_SW1            (BSP_IO_PORT_00_PIN_05)
    #define MB_SW2            (BSP_IO_PORT_00_PIN_04)
#elif defined(BOARD_RA8D1_EK)
    #define MB_SW1            (BSP_IO_PORT_00_PIN_09)
    #define MB_SW2            (BSP_IO_PORT_00_PIN_08)
#elif defined(BOARD_RA8M1_EK)
    #define MB_SW1            (BSP_IO_PORT_00_PIN_09)
    #define MB_SW2            (BSP_IO_PORT_00_PIN_08)
#else
    #define MB_SW1            (BSP_IO_PORT_00_PIN_09)
    #define MB_SW2            (BSP_IO_PORT_00_PIN_08)
#endif

/*============================================================================*/
/* S T A T I C   F U N C T I O N   P R O T O T Y P E                          */
/*============================================================================*/
/* function for address error */
static modbus_tcp_server_user_function_error_t mb_reg_exp_addr_p8 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_reg_exp_addr_p16 (uint16_t * data);
static modbus_tcp_server_user_function_error_t mb_reg_exp_addr_16 (uint16_t data);

/************************************************
 READ_COILS(0x01) functions
 */
static modbus_tcp_server_user_function_error_t mb_coil_read_00001 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_coil_read_00002 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_coil_read_00003 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_coil_read_00004 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_coil_read_00005 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_coil_read_00006 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_coil_read_00007 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_coil_read_00008 (uint8_t * data);

/************************************************
 WRITE_SINGLE_COIL(0x05) functions
 */
static modbus_tcp_server_user_function_error_t mb_coil_write_00001 (uint8_t data);
static modbus_tcp_server_user_function_error_t mb_coil_write_00002 (uint8_t data);
static modbus_tcp_server_user_function_error_t mb_coil_write_00003 (uint8_t data);
static modbus_tcp_server_user_function_error_t mb_coil_write_00004 (uint8_t data);
static modbus_tcp_server_user_function_error_t mb_coil_write_00005 (uint8_t data);
static modbus_tcp_server_user_function_error_t mb_coil_write_00006 (uint8_t data);
static modbus_tcp_server_user_function_error_t mb_coil_write_00007 (uint8_t data);
static modbus_tcp_server_user_function_error_t mb_coil_write_00008 (uint8_t data);

/************************************************
 READ DISCRETE INPUTS(0x02) functions
 */
static modbus_tcp_server_user_function_error_t mb_d_read_10001 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_d_read_10002 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_d_read_10003 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_d_read_10004 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_d_read_10005 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_d_read_10006 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_d_read_10007 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_d_read_10008 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_d_read_10009 (uint8_t * data);

/* Generate exception at 10010 by MB_Reg_Exp_Addr() */

static modbus_tcp_server_user_function_error_t mb_d_read_10011 (uint8_t * data);
static modbus_tcp_server_user_function_error_t mb_d_read_10012 (uint8_t * data);

/************************************************
 READ_INPUT_REGISTERS(0x04) functions
 */
static modbus_tcp_server_user_function_error_t mb_ireg_read_30001 (uint16_t * data);
static modbus_tcp_server_user_function_error_t mb_ireg_read_30002 (uint16_t * data);
static modbus_tcp_server_user_function_error_t mb_ireg_read_30003 (uint16_t * data);

/* Generate exception at 30004 by MB_Reg_Exp_Addr() */
/* Generate exception at 30005 by MB_Reg_Exp_Addr() */
/* Generate exception at 30006 by MB_Reg_Exp_Addr() */
/* Generate exception at 30007 by MB_Reg_Exp_Addr() */

static modbus_tcp_server_user_function_error_t mb_ireg_read_30008 (uint16_t * data);

/************************************************
 READ_HOLDING_REGISTERS(0x03) functions
 */
static modbus_tcp_server_user_function_error_t mb_reg_read_40001 (uint16_t * data);
static modbus_tcp_server_user_function_error_t mb_reg_read_40002 (uint16_t * data);
static modbus_tcp_server_user_function_error_t mb_reg_read_40003 (uint16_t * data);

/* Generate exception at 40004 by MB_Reg_Exp_Addr() */
/* Generate exception at 40005 by MB_Reg_Exp_Addr() */
/* Generate exception at 40006 by MB_Reg_Exp_Addr() */

static modbus_tcp_server_user_function_error_t mb_reg_read_40007 (uint16_t * data);

/************************************************
 WRITE_SINGLE_REGISTER(0x06) functions
 */
static modbus_tcp_server_user_function_error_t mb_reg_write_40001 (uint16_t data);
static modbus_tcp_server_user_function_error_t mb_reg_write_40002 (uint16_t data);
static modbus_tcp_server_user_function_error_t mb_reg_write_40003 (uint16_t data);

/* Generate exception at 40004 by MB_Reg_Exp_Addr() */
/* Generate exception at 40005 by MB_Reg_Exp_Addr() */
/* Generate exception at 40006 by MB_Reg_Exp_Addr()*/

static modbus_tcp_server_user_function_error_t mb_reg_write_40007 (uint16_t data);

/*============================================================================*/
/* V A R I A B L E                                                            */
/*============================================================================*/

/************************************************
 READ_COILS(0x01) functions
 */
modbus_tcp_server_user_function_error_t (*MB_Coils_Read[])(uint8_t *data)=
{
    mb_coil_read_00001, /* 00001 */
    mb_coil_read_00002, /* 00002 */
    mb_coil_read_00003, /* 00003 */
    mb_coil_read_00004, /* 00004 */
    mb_coil_read_00005, /* 00005 */
    mb_coil_read_00006, /* 00006 */
    mb_coil_read_00007, /* 00007 */
    mb_coil_read_00008, /* 00008 */
};

/************************************************
 WRITE_SINGLE_COIL(0x05) functions
 */
modbus_tcp_server_user_function_error_t (*MB_Coils_Write[])(uint8_t data)=
{
    mb_coil_write_00001, /* 00001 */
    mb_coil_write_00002, /* 00002 */
    mb_coil_write_00003, /* 00003 */
    mb_coil_write_00004, /* 00004 */
    mb_coil_write_00005, /* 00005 */
    mb_coil_write_00006, /* 00006 */
    mb_coil_write_00007, /* 00007 */
    mb_coil_write_00008, /* 00008 */
};

/************************************************
 READ DISCRETE INPUTS(0x02) functions
 */
modbus_tcp_server_user_function_error_t (*MB_Discretes_Input[])(uint8_t *data)=
{
    mb_d_read_10001, /* 10001 */
    mb_d_read_10002, /* 10002 */
    mb_d_read_10003, /* 10003 */
    mb_d_read_10004, /* 10004 */
    mb_d_read_10005, /* 10005 */
    mb_d_read_10006, /* 10006 */
    mb_d_read_10007, /* 10007 */
    mb_d_read_10008, /* 10008 */
    mb_d_read_10009, /* 10009 */
    mb_reg_exp_addr_p8, /* 10010 */
    mb_d_read_10011, /* 10011 */
    mb_d_read_10012, /* 10012 */
};

/************************************************
 READ_INPUT_REGISTERS(0x04) functions
 */
modbus_tcp_server_user_function_error_t (*MB_Input_Regs[])(uint16_t *data)=
{
    mb_ireg_read_30001, /* 30001 */
    mb_ireg_read_30002, /* 30002 */
    mb_ireg_read_30003, /* 30003 */
    mb_reg_exp_addr_p16, /* 30004 */
    mb_reg_exp_addr_p16, /* 30005 */
    mb_reg_exp_addr_p16, /* 30006 */
    mb_reg_exp_addr_p16, /* 30007 */
    mb_ireg_read_30008, /* 30008 */
};

/************************************************
 READ_HOLDING_REGISTERS(0x03) functions
 */
modbus_tcp_server_user_function_error_t (*MB_HoldingRegs_Read[])(uint16_t *data)=
{
    mb_reg_read_40001, /* 40001 */
    mb_reg_read_40002, /* 40002 */
    mb_reg_read_40003, /* 40003 */
    mb_reg_exp_addr_p16, /* 40004 */
    mb_reg_exp_addr_p16, /* 40005 */
    mb_reg_exp_addr_p16, /* 40006 */
    mb_reg_read_40007, /* 40007 */
};

/************************************************
 WRITE_SINGLE_REGISTER(0x06) functions
 */
modbus_tcp_server_user_function_error_t (*MB_HoldingRegs_Write[])(uint16_t data)=
{
    mb_reg_write_40001, /* 40001 */
    mb_reg_write_40002, /* 40002 */
    mb_reg_write_40003, /* 40003 */
    mb_reg_exp_addr_16, /* 40004 */
    mb_reg_exp_addr_16, /* 40005 */
    mb_reg_exp_addr_16, /* 40006 */
    mb_reg_write_40007 /* 40007 */
};
/* Modbus coil address 00001 to 09999*/
uint8_t g_coils_area[] =
{ 0, 0, 0, 0, 0, 0, 0, 0 };
/* Modbus discrete input address 10001 to 19999*/
uint8_t g_discrete_input_area[] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
/* Modbus input register address 30001 to 39999*/
uint16_t g_input_reg_area[] =
{ 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x00008 };
/* Modbus holding register address 40001 to 49999*/
uint16_t g_holding_reg_area[] =
{ 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };

/* Number of coils */
uint32_t g_coil_end_addr           = (sizeof(MB_Coils_Read)) / (sizeof(MB_Coils_Read[0]));
/* Number of discrete inputs */
uint32_t g_discrete_input_end_addr = (sizeof(MB_Discretes_Input)) / (sizeof(MB_Discretes_Input[0]));
/* Number of input registers */
uint32_t g_input_reg_end_addr      = (sizeof(MB_Input_Regs)) / (sizeof(MB_Input_Regs[0]));
/* Number of holding registers */
uint32_t g_holding_reg_end_addr    = (sizeof(MB_HoldingRegs_Read)) / (sizeof(MB_HoldingRegs_Read[0]));

/*============================================================================*/
/* P R O G R A M                                                              */
/*============================================================================*/

/* ERR_ILLEGAL_DATA_ADDRESS is changed to ERR_SLAVE_DEVICE_FAILURE in MB_Func_Exp_Execution() */
/**********************************************************************************************************************
 * Function Name: mb_reg_exp_addr_p8
 * Description  : Returns MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_ADDRESS
 * Argument     : uint8_t *data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_ADDRESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_reg_exp_addr_p8(uint8_t *data)
{
    (void) data;
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_ADDRESS);
}
/**********************************************************************************************************************
 End of function mb_reg_exp_addr_p8
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_reg_exp_addr_p16
 * Description  : Returns MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_ADDRESS
 * Argument     : uint16_t *data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_ADDRESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_reg_exp_addr_p16(uint16_t *data)
{
    (void) data;
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_ADDRESS);
}
/**********************************************************************************************************************
 End of function mb_reg_exp_addr_p16
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_reg_exp_addr_16
 * Description  : Returns MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_ADDRESS
 * Argument     : uint16_t data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_ADDRESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_reg_exp_addr_16(uint16_t data)
{
    (void) data;
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_ADDRESS);
}
/**********************************************************************************************************************
 End of function mb_reg_exp_addr_16
 *********************************************************************************************************************/

/************************************************
 READ_COILS(0x01) functions
 */
/**********************************************************************************************************************
 * Function Name: mb_coil_read_00001
 * Description  : Read Coil at address 00001
 * Argument     : uint8_t *coil
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_read_00001(uint8_t *coil)
{
    uint8_t index = 0;

    R_BSP_PinAccessEnable ();

    /* Cast the address of the target coil to a BSP enumeration type, 
     * and cast the read value of the target coil to within the appropriate range. */
    g_coils_area[index] = (uint8_t)R_BSP_PinRead ((bsp_io_port_pin_t)g_bsp_leds.p_leds[index]);
    R_BSP_PinAccessDisable ();
    *coil = g_coils_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_read_00001
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_coil_read_00002
 * Description  : Read Coil at address 00002
 * Argument     : uint8_t *coil
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_read_00002(uint8_t *coil)
{
    uint8_t index = 1;

    R_BSP_PinAccessEnable ();

    /* Cast the address of the target coil to a BSP enumeration type, 
     * and cast the read value of the target coil to within the appropriate range. */
    g_coils_area[index] = (uint8_t)R_BSP_PinRead ((bsp_io_port_pin_t)g_bsp_leds.p_leds[index]);
    R_BSP_PinAccessDisable ();
    *coil = g_coils_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_read_00002
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_coil_read_00003
 * Description  : Read Coil at address 00003
 * Argument     : uint8_t *coil
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_read_00003(uint8_t *coil)
{
    uint8_t index = 2;

    R_BSP_PinAccessEnable ();

    /* Cast the address of the target coil to a BSP enumeration type, 
     * and cast the read value of the target coil to within the appropriate range. */
    g_coils_area[index] = (uint8_t)R_BSP_PinRead ((bsp_io_port_pin_t)g_bsp_leds.p_leds[index]);
    R_BSP_PinAccessDisable ();
    *coil = g_coils_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_read_00003
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_coil_read_00004
 * Description  : Read Coil at address 00004
 * Argument     : uint8_t *coil
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_read_00004(uint8_t *coil)
{
    uint8_t index = 3;

    *coil = g_coils_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_read_00004
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_coil_read_00005
 * Description  : Read Coil at address 00005
 * Argument     : uint8_t *coil
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_read_00005(uint8_t *coil)
{
    uint8_t index = 4;

    *coil = g_coils_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_read_00005
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_coil_read_00006
 * Description  : Read Coil at address 00006
 * Argument     : uint8_t *coil
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_read_00006(uint8_t *coil)
{
    uint8_t index = 5;

    *coil = g_coils_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_read_00006
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_coil_read_00007
 * Description  : Read Coil at address 00007
 * Argument     : uint8_t *coil
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_read_00007(uint8_t *coil)
{
    uint8_t index = 6;

    *coil = g_coils_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_read_00007
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_coil_read_00008
 * Description  : Read Coil at address 00008
 * Argument     : uint8_t *coil
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_read_00008(uint8_t *coil)
{
    uint8_t index = 7;

    *coil = g_coils_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_read_00008
 *********************************************************************************************************************/

/************************************************
 WRITE_SINGLE_COIL (0x05) functions
 */
/**********************************************************************************************************************
 * Function Name: mb_coil_write_00001
 * Description  : Write Coil at address 00001
 * Argument     : uint8_t data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_write_00001(uint8_t data)
{
    uint8_t index = 0;

    g_coils_area[index] = data & 0x01;
    R_BSP_PinAccessEnable ();

    /* Cast the target coil address and write value to the BSP enumeration type */
    R_BSP_PinWrite ((bsp_io_port_pin_t)g_bsp_leds.p_leds[index], (bsp_io_level_t)g_coils_area[index]);
    R_BSP_PinAccessDisable ();
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_write_00001
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_coil_write_00002
 * Description  : Write Coil at address 00002
 * Argument     : uint8_t data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_write_00002(uint8_t data)
{
    uint8_t index = 1;

    g_coils_area[index] = data & 0x01;
    R_BSP_PinAccessEnable ();

    /* Cast the target coil address and write value to the BSP enumeration type */
    R_BSP_PinWrite ((bsp_io_port_pin_t)g_bsp_leds.p_leds[index], (bsp_io_level_t)g_coils_area[index]);
    R_BSP_PinAccessDisable ();
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_write_00002
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_coil_write_00003
 * Description  : Write Coil at address 00003
 * Argument     : uint8_t data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_write_00003(uint8_t data)
{
    uint8_t index = 2;

    g_coils_area[index] = data & 0x01;
    R_BSP_PinAccessEnable ();

    /* Cast the target coil address and write value to the BSP enumeration type */
    R_BSP_PinWrite ((bsp_io_port_pin_t)g_bsp_leds.p_leds[index], (bsp_io_level_t)g_coils_area[index]);
    R_BSP_PinAccessDisable ();
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_write_00003
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_coil_write_00004
 * Description  : Write Coil at address 00004
 * Argument     : uint8_t data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_write_00004(uint8_t data)
{
    uint8_t index = 3;

    g_coils_area[index] = data & 0x01;
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_write_00004
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_coil_write_00005
 * Description  : Write Coil at address 00005
 * Argument     : uint8_t data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_write_00005(uint8_t data)
{
    uint8_t index = 4;

    g_coils_area[index] = data & 0x01;
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_write_00005
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_coil_write_00006
 * Description  : Write Coil at address 00006
 * Argument     : uint8_t data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_write_00006(uint8_t data)
{
    uint8_t index = 5;

    g_coils_area[index] = data & 0x01;
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_write_00006
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_coil_write_00007
 * Description  : Write Coil at address 00007
 * Argument     : uint8_t data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_write_00007(uint8_t data)
{
    uint8_t index = 6;

    g_coils_area[index] = data & 0x01;
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_write_00007
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_coil_write_00008
 * Description  : Write Coil at address 00008
 * Argument     : uint8_t data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_coil_write_00008(uint8_t data)
{
    uint8_t index = 7;

    g_coils_area[index] = data & 0x01;
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_coil_write_00008
 *********************************************************************************************************************/

/************************************************
 READ_DISCRETE_INPUTS (0x02) functions
 */
/**********************************************************************************************************************
 * Function Name: mb_d_read_10001
 * Description  : Read Discrete Inputs at address 10001
 * Argument     : uint8_t *state
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_d_read_10001(uint8_t *state)
{
    uint8_t index = 0;

    *state = g_discrete_input_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_d_read_10001
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_d_read_10002
 * Description  : Read Discrete Inputs at address 10002
 * Argument     : uint8_t *state
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_d_read_10002(uint8_t *state)
{
    uint8_t index = 1;

    *state                       = g_discrete_input_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_d_read_10002
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_d_read_10003
 * Description  : Read Discrete Inputs at address 10003
 * Argument     : uint8_t *state
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_d_read_10003(uint8_t *state)
{
    uint8_t index = 2;

    *state = g_discrete_input_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_d_read_10003
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_d_read_10004
 * Description  : Read Discrete Inputs at address 10004
 * Argument     : uint8_t *state
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_d_read_10004(uint8_t *state)
{
    uint8_t index = 3;

    *state = g_discrete_input_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_d_read_10004
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_d_read_10005
 * Description  : Read Discrete Inputs at address 10005
 * Argument     : uint8_t *state
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_d_read_10005(uint8_t *state)
{
    uint8_t index = 4;

    *state = g_discrete_input_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_d_read_10005
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_d_read_10006
 * Description  : Read Discrete Inputs at address 10006
 * Argument     : uint8_t *state
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_d_read_10006(uint8_t *state)
{
    
    uint8_t  index = 5;

#ifndef BOARD_RA8T1_MCK
    uint32_t pin;
    R_BSP_PinAccessEnable ();

    /* Cast the address of the target coil to a BSP enumeration type */
    pin = R_BSP_PinRead ((bsp_io_port_pin_t)MB_SW1);
    R_BSP_PinAccessDisable ();

   /* Cast the read value of the target coil to within the appropriate range. */
    g_discrete_input_area[index] = (uint8_t) pin ^ 0x01;
#endif
    *state                       = g_discrete_input_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_d_read_10006
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_d_read_10007
 * Description  : Read Discrete Inputs at address 10007
 * Argument     : uint8_t *state
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_d_read_10007(uint8_t *state)
{

    uint8_t  index = 6;

#ifndef BOARD_RA8T1_MCK
    uint32_t pin;
    R_BSP_PinAccessEnable ();

    /* Cast the address of the target coil to a BSP enumeration type */
    pin = R_BSP_PinRead ((bsp_io_port_pin_t)MB_SW2);
    R_BSP_PinAccessDisable ();

    /* Cast the read value of the target coil to within the appropriate range. */
    g_discrete_input_area[index] = (uint8_t) pin ^ 0x01;
#endif
    *state                       = g_discrete_input_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_d_read_10007
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_d_read_10008
 * Description  : Read Discrete Inputs at address 10008
 * Argument     : uint8_t *state
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_d_read_10008(uint8_t *state)
{
    uint8_t index = 7;

    *state = g_discrete_input_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_d_read_10008
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_d_read_10009
 * Description  : Read Discrete Inputs at address 10009
 * Argument     : uint8_t *state
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_d_read_10009(uint8_t *state)
{
    uint8_t index = 8;

    *state = g_discrete_input_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_d_read_10009
 *********************************************************************************************************************/

/* Generate exception at 10010 by MB_Reg_Exp_Addr() */

/**********************************************************************************************************************
 * Function Name: mb_d_read_10011
 * Description  : Read Discrete Inputs at address 10011
 * Argument     : uint8_t *state
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_d_read_10011(uint8_t *state)
{
    uint8_t index = 10;

    *state = g_discrete_input_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_d_read_10011
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_d_read_10012
 * Description  : Read Discrete Inputs at address 10012
 * Argument     : uint8_t *state
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_d_read_10012(uint8_t *state)
{
    uint8_t index = 11;

    *state = g_discrete_input_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_d_read_10012
 *********************************************************************************************************************/


/************************************************
 READ_INPUT_REGISTERS(0x04) functions
 */
/**********************************************************************************************************************
 * Function Name: mb_ireg_read_30001
 * Description  : Read Input Register at address 30001
 * Argument     : uint16_t *data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_ireg_read_30001(uint16_t *data)
{
    uint8_t index = 0;

    *data = g_input_reg_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_ireg_read_30001
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_ireg_read_30002
 * Description  : Read Input Register at address 30002
 * Argument     : uint16_t *data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_ireg_read_30002(uint16_t *data)
{
    uint8_t index = 1;

    *data = g_input_reg_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_ireg_read_30002
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_ireg_read_30003
 * Description  : Read Input Register at address 30003
 * Argument     : uint16_t *data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_ireg_read_30003(uint16_t *data)
{
    uint8_t index = 2;

    *data = g_input_reg_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_ireg_read_30003
 *********************************************************************************************************************/

/* Generate exception at 30004 by MB_Reg_Exp_Addr() */
/* Generate exception at 30005 by MB_Reg_Exp_Addr() */
/* Generate exception at 30006 by MB_Reg_Exp_Addr() */
/* Generate exception at 30007 by MB_Reg_Exp_Addr() */

/**********************************************************************************************************************
 * Function Name: mb_ireg_read_30008
 * Description  : Read Input Register at address 30008
 * Argument     : uint16_t *data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_ireg_read_30008(uint16_t *data)
{
    uint8_t index = 7;

    *data = g_input_reg_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_ireg_read_30008
 *********************************************************************************************************************/

/************************************************
 READ_HOLDING_REGISTERS(0x03) functions
 */
/**********************************************************************************************************************
 * Function Name: mb_reg_read_40001
 * Description  : Read Holding Register at address 40001
 * Argument     : uint16_t *data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_reg_read_40001(uint16_t *data)
{
    uint8_t index = 0;

    *data = g_holding_reg_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_reg_read_40001
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_reg_read_40002
 * Description  : Read Holding Register at address 40002
 * Argument     : uint16_t *data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_reg_read_40002(uint16_t *data)
{
    uint8_t index = 1;

    *data = g_holding_reg_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_reg_read_40002
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_reg_read_40003
 * Description  : Read Holding Register at address 40003
 * Argument     : uint16_t *data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_reg_read_40003(uint16_t *data)
{
    uint8_t index = 2;

    *data = g_holding_reg_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_reg_read_40003
 *********************************************************************************************************************/

/* Generate exception at 40004 by MB_Reg_Exp_Addr() */
/* Generate exception at 40005 by MB_Reg_Exp_Addr() */
/* Generate exception at 40006 by MB_Reg_Exp_Addr() */

/**********************************************************************************************************************
 * Function Name: mb_reg_read_40007
 * Description  : Read Holding Register at address 40007
 * Argument     : uint16_t *data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_reg_read_40007(uint16_t *data)
{
    uint8_t index = 6;

    *data = g_holding_reg_area[index];
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_reg_read_40007
 *********************************************************************************************************************/

/************************************************
 WRITE_SINGLE_REGISTER(0x06) functions
 */
/**********************************************************************************************************************
 * Function Name: mb_reg_write_40001
 * Description  : Write Holding Register at address 40001
 * Argument     : uint16_t data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_reg_write_40001(uint16_t data)
{
    uint8_t index = 0;

    g_holding_reg_area[index] = data;
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_reg_write_40001
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_reg_write_40002
 * Description  : Write Holding Register at address 40002
 * Argument     : uint16_t data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_reg_write_40002(uint16_t data)
{
    uint8_t index = 1;

    g_holding_reg_area[index] = data;
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_reg_write_40002
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mb_reg_write_40003
 * Description  : Write Holding Register at address 40003
 * Argument     : uint16_t data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
/* ERR_ILEEGAL_DATA_VALUE is changed to ERR_SLAVE_DEVICE_FAILURE in MB_Func_Exp_Execution() */
static modbus_tcp_server_user_function_error_t mb_reg_write_40003(uint16_t data)
{
    uint8_t index = 2;

    if (data > MB_MAXIMUM_DATA_VALUE)
    {
        return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_VALUE);
    }
    g_holding_reg_area[index] = data;
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_reg_write_40003
 *********************************************************************************************************************/

/* Generate exception at 40004 by MB_Reg_Exp_Addr() */
/* Generate exception at 40005 by MB_Reg_Exp_Addr() */
/* Generate exception at 40006 by MB_Reg_Exp_Addr() */

/**********************************************************************************************************************
 * Function Name: mb_reg_write_40007
 * Description  : Write Holding Register at address 40007
 * Argument     : uint16_t data
 * Return Value : MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS
 *********************************************************************************************************************/
static modbus_tcp_server_user_function_error_t mb_reg_write_40007(uint16_t data)
{
    uint8_t index = 6;

    g_holding_reg_area[index] = data;
    return (MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS);
}
/**********************************************************************************************************************
 End of function mb_reg_write_40007
 *********************************************************************************************************************/

/* End of File */
