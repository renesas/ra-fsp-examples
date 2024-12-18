/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/**********************************************************************************************************************
 * File Name    : ov3640.c
 * Description  : This file defines the functions to configure and ommunicate with the ov3640.c.
 **********************************************************************************************************************/
/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/

#include <stdarg.h>
#include "hal_data.h"
#include "common_util.h"
#include "jlink_console.h"


/***************************************************************************************************************************
 * Macro definitions
 ***************************************************************************************************************************/
#define RESET_VALUE             		(0x00)
#define CAM_PWDN_PIN            		BSP_IO_PORT_07_PIN_04
#define OV3640_END_OF_ARRAY             (0xFFFF)

typedef enum
{
    POWER_UP      = BSP_IO_LEVEL_LOW,
    POWER_DOWN    = BSP_IO_LEVEL_HIGH,
} camera_power_t;

/***************************************************************************************************************************
 * Typedef definitions
 ***************************************************************************************************************************/

typedef struct ov3640_sensor_reg {
    uint16_t reg;
    uint8_t val;
} sensor_reg_t;

/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/


/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * Private global variables and functions
 ***************************************************************************************************************************/
/* define link to your i2c driver HERE */
static iic_master_instance_ctrl_t * p_api_ctrl = &g_i2c_master1_ctrl;
static volatile i2c_master_event_t i2c_event = I2C_MASTER_EVENT_ABORTED;
static fsp_err_t validate_i2c_event(void);
static face_det_err_t camera_write_array(sensor_reg_t *array);
static face_det_err_t wrSensorReg16_8(int regID, int regDat);
static fsp_err_t i2c_cam_cb_wait(void);

/* set up the OV3640 register */
static sensor_reg_t g_ov3640_qvga[] =
{

	{0x300e, 0x32}, {0x3010, 0x20}, {0x3011, 0x00}, {0x3010, 0x20},
	{0x3015, 0x12}, {0x3016, 0x82}, {0x3018, 0x38}, {0x3019, 0x30},
	{0x301a, 0x61}, {0x304c, 0x85}, {0x304d, 0x45}, {0x307d, 0x00},
	{0x3082, 0x20}, {0x308d, 0x04}, {0x30a7, 0x5e}, {0x309c, 0x1a},
	{0x30a2, 0xe4}, {0x30aa, 0x42}, {0x30b0, 0xff}, {0x30b1, 0xff},
	{0x30b2, 0x10}, {0x30d7, 0x10}, {0x30d9, 0x0d}, {0x30db, 0x08},
	{0x30b8, 0x20}, {0x30b9, 0x17}, {0x30ba, 0x04}, {0x30bb, 0x08},
	{0x3300, 0x13}, {0x3301, 0xde}, {0x3302, 0xef}, {0x3304, 0xfc},
	{0x330c, 0x18}, {0x330d, 0x18}, {0x330e, 0x56}, {0x330f, 0x5c},
	{0x330b, 0x1c}, {0x3310, 0xd0}, {0x3311, 0xbd}, {0x3312, 0x26},
	{0x3314, 0x42}, {0x3313, 0x2b}, {0x3315, 0x42}, {0x3362, 0x01},
	{0x3363, 0x48}, {0x3364, 0xf4}, {0x336a, 0x52}, {0x3370, 0x46},
	{0x3376, 0x38}, {0x3400, 0x01}, {0x3403, 0x42}, {0x3404, 0x11},
	{0x3307, 0x11}, {0x3507, 0x06}, {0x350a, 0x4f}, {0x3100, 0x02},
	{0x3088, 0x01}, {0x3089, 0x48}, {0x308a, 0x00}, {0x308b, 0xf0},
	{OV3640_END_OF_ARRAY, 0xff}     //  End of file marker (0xFFFF)
};


/*********************************************************************************************************************
 *  Write to the camera registers from an array until register value is 0xFF.
 *  @param[IN]   sensor_reg_t *array: pointer to the address.
 *  @retval      None
***********************************************************************************************************************/
static face_det_err_t camera_write_array(sensor_reg_t *array)
{
	face_det_err_t face_det_status = FACE_DET_APP_SUCCESS;

    while (0xFFFF != array->reg)
    {
    	face_det_status = wrSensorReg16_8(array->reg, array->val);
    	if (face_det_status != FACE_DET_APP_SUCCESS)
    	{
    		handle_error(FACE_DET_APP_WRITE_SENSOR_ARRAY);
    		return face_det_status;
    	}
        array++;
     }
    return face_det_status;

}
/*********************************************************************************************************************
 *  Provide the clock to the camera and configure the camera to output QVGA RGB565 image
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/

face_det_err_t camera_init(void)
{
	face_det_err_t face_det_status = FACE_DET_APP_SUCCESS;
	fsp_err_t status = FSP_SUCCESS;

    status = R_GPT_Open(&g_cam_clk_ctrl, &g_cam_clk_cfg);
    if (FSP_SUCCESS != status)
    {
    	 handle_error(FACE_DET_APP_GPT_OPEN);
    	 return FACE_DET_APP_GPT_OPEN;
    }
    R_GPT_Start(&g_cam_clk_ctrl);

    // Hardware reset the OV3640
    R_BSP_PinAccessEnable();
    R_IOPORT_PinWrite(&g_ioport_ctrl, CAM_PWDN_PIN, (bsp_io_level_t)POWER_DOWN);
    R_BSP_SoftwareDelay(40, BSP_DELAY_UNITS_MILLISECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, CAM_PWDN_PIN, (bsp_io_level_t)POWER_UP);
    R_BSP_PinAccessDisable();

    status = R_IIC_MASTER_Open(&g_i2c_master1_ctrl, &g_i2c_master1_cfg);
	if (FSP_SUCCESS != status)
	{
		handle_error(FACE_DET_APP_IIC_MASTER_OPEN);
		return FACE_DET_APP_IIC_MASTER_OPEN;
	}

	status = R_CEU_Open(&g_ceu_ctrl, &g_ceu_cfg);
	if (FSP_SUCCESS != status)
	{
		handle_error(FACE_DET_APP_CEU_OPEN);
		return FACE_DET_APP_CEU_OPEN;
	}


	face_det_status = camera_write_array((sensor_reg_t *)&g_ov3640_qvga);

   return (face_det_status);
}

/*********************************************************************************************************************
 *  i2c master callback function
 *  @param[IN]   i2c_master_callback_args_t * p_args: contains the callback context.
 *  @retval      None
***********************************************************************************************************************/
void g_i2c_master1_cb(i2c_master_callback_args_t * p_args)
{
    i2c_event = p_args->event;
}

/*********************************************************************************************************************
 *  i2c callback wait function
 *  @param[IN]   null
 *  @retval      error message
***********************************************************************************************************************/
static fsp_err_t i2c_cam_cb_wait(void)
{
    fsp_err_t ret = FSP_SUCCESS;
    fsp_err_t err = validate_i2c_event();

    /* handle error */
    if(FSP_ERR_TRANSFER_ABORTED == err)
    {
        ret = FSP_ERR_TRANSFER_ABORTED;
    }

    return ret;
}

/*********************************************************************************************************************
 *  validate the i2c event
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
static fsp_err_t validate_i2c_event(void)
{
    uint16_t local_time_out = UINT16_MAX;

    /* resetting call back event capture variable */
    i2c_event = (i2c_master_event_t)RESET_VALUE;

    do
    {
        /* This is to avoid infinite loop */
        --local_time_out;

        if(RESET_VALUE == local_time_out)
        {
            return FSP_ERR_TRANSFER_ABORTED;
        }

    }while(i2c_event == RESET_VALUE);


    if(i2c_event != I2C_MASTER_EVENT_ABORTED)
    {
        /* Make sure this is always Reset before return*/
    	i2c_event = (i2c_master_event_t)RESET_VALUE;
        return FSP_SUCCESS;
    }

    /* Make sure this is always Reset before return*/
    i2c_event = (i2c_master_event_t)RESET_VALUE;
    return FSP_ERR_TRANSFER_ABORTED;
}

/*********************************************************************************************************************
 *  Write to one sensor registerEnable
 *  @param[IN]   int regID: sensor register number
 *  @param[IN]   int regDat: data to write to the register
 *  @retval      None
***********************************************************************************************************************/
static face_det_err_t wrSensorReg16_8(int regID, int regDat)
{
    fsp_err_t err;
    face_det_err_t face_det_status = FACE_DET_APP_SUCCESS;

    uint8_t data[3] = {(uint8_t) (regID >> 8), (uint8_t) regID, (uint8_t) regDat};

    err = R_IIC_MASTER_Write(p_api_ctrl, data, 3, false);
    if (FSP_SUCCESS != err)
    {
    	handle_error(FACE_DET_APP_IIC_MASTER_WRITE);
    	return FACE_DET_APP_IIC_MASTER_OPEN;
    }
	err = i2c_cam_cb_wait();
	if (FSP_SUCCESS != err)
	{
		handle_error(FACE_DET_APP_WRITE_OV3640_REG);
		return FACE_DET_APP_WRITE_OV3640_REG;
	}

    return face_det_status;
}


