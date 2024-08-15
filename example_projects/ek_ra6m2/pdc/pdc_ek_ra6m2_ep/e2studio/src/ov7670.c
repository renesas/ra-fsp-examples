/***********************************************************************************************************************
 * File Name    : ov7670.c
 * Description  : Contains declarations of data structures used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "ov7670.h"
#include "ov7670_registers.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup pdc_ep
 * @{
 **********************************************************************************************************************/

/* Global variable */
/* Capture I2C event */
static volatile i2c_master_event_t g_master_event = (i2c_master_event_t)RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief       Write number of registers to the camera.
 * @param[IN]   regs          Pointer to the first element of an array of registers
 * @param[IN]   count         Number of registers to write.
 * @retval      FSP_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t ov7670_reg_write (uint8_t * const regs, uint8_t count)
{
    fsp_err_t err                 = FSP_SUCCESS;
    uint16_t write_time_out       = UINT16_MAX;

    /* resetting callback event */
    g_master_event = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C write operation */
    err = R_IIC_MASTER_Write(&g_i2c_ctrl, regs, count, false);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** IIC_MASTER_Write API FAILED ** \r\n");
        return err;
    }

    /*Wait until master write transmission event occur*/
    while(I2C_MASTER_EVENT_TX_COMPLETE != g_master_event)
    {
        /*check for aborted event*/
        if(I2C_MASTER_EVENT_ABORTED == g_master_event)
        {
            APP_ERR_PRINT ("** Error EVENT_ABORTED received during Master Write operation **\r\n");
            return FSP_ERR_TRANSFER_ABORTED;
        }
        else
        {
            /*start checking for time out to avoid infinite loop*/
            --write_time_out;

            /*check for time elapse*/
            if (RESET_VALUE == write_time_out)
            {
                /* we have reached to a scenario where i2c event not occurred*/
                APP_ERR_PRINT (" ** No event received during Master Write and Slave read operation **\r\r");
                /*no event received*/
                return FSP_ERR_TIMEOUT;
            }
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Reads the register from the camera.
 * @param[IN]   reg           Register address to write on.
 * @param[OUT]  value         Register value.
 * @retval      FSP_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t ov7670_reg_read (uint8_t reg, uint8_t * p_value)
{
    fsp_err_t err;
    uint16_t read_time_out       = UINT16_MAX;
    /* resetting callback event */
    g_master_event = (i2c_master_event_t)RESET_VALUE;

    err = ov7670_reg_write(&reg, REG_ONE);
    if (FSP_SUCCESS == err)
    {
        /* Perform I2C read operation */
        err = R_IIC_MASTER_Read(&g_i2c_ctrl, p_value, REG_ONE, false);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n ** R_IIC_MASTER_Read API FAILED ** \r\n");
            return err;
        }
        /* Wait until Master read complete event occur */
        while(I2C_MASTER_EVENT_RX_COMPLETE != g_master_event)
        {
           /* check for aborted event*/
            if(I2C_MASTER_EVENT_ABORTED == g_master_event)
            {
                APP_ERR_PRINT ("** EVENT_ABORTED received during Master read operation **\r\n");
                /*I2C transaction failure*/
                return FSP_ERR_TRANSFER_ABORTED;
            }
            else
            {
                /*start checking for time out to avoid infinite loop*/
                --read_time_out;
                /*check for time elapse*/
                if (RESET_VALUE == read_time_out)
                {
                    /*we have reached to a scenario where i2c event not occurred*/
                    APP_ERR_PRINT (" ** No event received during Master read and Slave write operation **\r\r");
                    /*no event received*/
                    return FSP_ERR_TIMEOUT;
                }
            }
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Set register values to the camera registers
 * @param[IN]   reg          Register address to write on.
 * @param[IN]   value         Register value.
 * @retval      FSP_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t ov7670_reg_set (uint8_t reg, uint8_t value)
{
    uint8_t   regs[REG_TWO] = {RESET_VALUE};
    fsp_err_t err           =  FSP_SUCCESS;

    regs[REG_ZERO] = reg;
    regs[REG_ONE]  = value;
    err     = ov7670_reg_write(regs, REG_TWO);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** ov7670_reg_write failed ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Write to the registers of the camera.
 * @param[IN]   p_registers   Array of structures containing a register address and value.
 * @retval      FSP_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t ov7670_write_array (struct ov7670_register_value const * p_registers)
{
    fsp_err_t err = FSP_SUCCESS;
    while (p_registers->reg_address != END_OF_TABLE)
    {
        err = ov7670_reg_set(p_registers->reg_address, p_registers->reg_value);
        if (FSP_SUCCESS != err)
        {
            break;
        }
        p_registers++;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Resets the camera
 * @param[IN]   None
 * @retval      FSP_SUCCESS    Upon successful reset operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t ov7670_reset (void)
{
    fsp_err_t err  = FSP_SUCCESS;

    /* De-assert Camera Power Down */
    R_IOPORT_PinWrite(g_ioport.p_ctrl,CAMERA_PWDWN, BSP_IO_LEVEL_LOW);

    /* Reset the camera */
    R_IOPORT_PinWrite(g_ioport.p_ctrl,CAMERA_RESET, BSP_IO_LEVEL_LOW);

    /* Settling time after HW reset as per hardware datasheet of ov7670*/
    R_BSP_SoftwareDelay(REG_SETTLING_TIME, BSP_DELAY_UNITS_MILLISECONDS);

    R_IOPORT_PinWrite(g_ioport.p_ctrl,CAMERA_RESET, BSP_IO_LEVEL_HIGH);

    /* Settling time after HW reset as per hardware datasheet of ov7670*/
    R_BSP_SoftwareDelay(REG_SETTLING_TIME, BSP_DELAY_UNITS_MILLISECONDS);

    /* Initialize the OV7670 Camera */
    err = R_IIC_MASTER_Open(&g_i2c_ctrl, &g_i2c_cfg);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** IIC_Master Open API FAILED ** \r\n");
        return err;
    }

    /* Reset via I2C command */
    err = ov7670_reg_set(COM7, COM7_RESET);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** Setting camera register failed. ** \r\n");
        ov7670_close();
        return err;
    }
    /* Settling time after HW reset as per hardware datasheet of ov7670*/
    R_BSP_SoftwareDelay(REG_SETTLING_TIME, BSP_DELAY_UNITS_MILLISECONDS);
    return err;
}
/*******************************************************************************************************************//**
 * @brief       Sets camera in RGB 5:6:5 encoding mode.
 * @param[IN]   None
 * @retval      FSP_SUCCESS    Upon successful setting rgb register
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t ov7670_rgb565 (void)
{
    /* RGB565 Register setting */
    static struct ov7670_register_value ov7670_fmt_rgb565[] = {
        {
            COM7, COM7_RGB
        },                      /* RGB mode */
        {
            RGB444, 0
        },                      /* No RGB444 */
        {
            COM15, COM15_RGB565
        },                      /* RGB565 */
        {
            END_OF_TABLE, END_OF_TABLE
        },
    };
    /* END - RGB565 Register setting */

    return ov7670_write_array(ov7670_fmt_rgb565);
}

/*******************************************************************************************************************//**
 * @brief       Initialize the camera.
 * @param[IN]   None
 * @retval      FSP_SUCCESS    Upon successful initialization.
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t ov7670_init (void)
{
    /* Default register values */
    static struct ov7670_register_value ov7670_default_regs[] = {
        {
            GAIN, 0x00
        },
        {
            BLUE, 0x80
        },
        {
            RED, 0x80
        },
        {
            VREF, 0x00
        },
        {
            COM1, 0x00
        },
        {
            BAVE, 0x00
        },
        {
            GBAVE, 0x00
        },
        {
            AECHH, 0x00
        },
        {
            RAVE, 0x00
        },
        {
            COM2, 0x01
        },
        {
            COM3, 0x00
        },
        {
            COM4, 0x00
        },
        {
            COM5, 0x01
        },
        {
            COM6, 0x43
        },
        {
            AECH, 0x40
        },
        {
            CLKRC, 0x80
        },
        {
            COM7, 0x00
        },
        {
            COM8, 0x8F
        },
        {
            COM9, 0x4A
        },
        {
            COM10, 0x00
        },
        {
            HSTART, 0x11
        },
        {
            HSTOP, 0x61
        },
        {
            VSTRT, 0x03
        },
        {
            VSTOP, 0x7B
        },
        {
            PSHFT, 0x00
        },
        {
            MVFP, 0x01
        },
        {
            LAEC, 0x00
        },
        {
            ADCCTR0, 0x04
        },
        {
            ADCCTR1, 0x02
        },
        {
            ADCCTR2, 0x01
        },
        {
            ADCCTR3, 0x00
        },
        {
            AEW, 0x75
        },
        {
            AEB, 0x63
        },
        {
            VPT, 0xD4
        },
        {
            BBIAS, 0x80
        },
        {
            GBBIAS, 0x80
        },
        {
            EXHCH, 0x00
        },
        {
            EXHCL, 0x00
        },
        {
            RBIAS, 0x80
        },
        {
            ADVFL, 0x00
        },
        {
            ADVFH, 0x00
        },
        {
            YAVE, 0x00
        },
        {
            HSYST, 0x08
        },
        {
            HSYEN, 0x30
        },
        {
            HREF, 0x80
        },
        {
            CHLF, 0x08
        },
        {
            ARBLM, 0x11
        },
        {
            ADC, 0x3F
        },
        {
            ACOM, 0x01
        },
        {
            OFON, 0x00
        },
        {
            TSLB, 0x0D
        },
        {
            COM11, 0x00
        },
        {
            COM12, 0x68
        },
        {
            COM13, 0x88
        },
        {
            COM14, 0x00
        },
        {
            EDGE, 0x00
        },
        {
            COM15, 0xC0
        },
        {
            COM16, 0x08
        },
        {
            COM17, 0x00
        },
        {
            AWBC1, 0x14
        },
        {
            AWBC2, 0xF0
        },
        {
            AWBC3, 0x45
        },
        {
            AWBC4, 0x61
        },
        {
            AWBC5, 0x51
        },
        {
            AWBC6, 0x79
        },
        {
            REG4B, 0x00
        },
        {
            DNSTH, 0x00
        },
        {
            MTX1, 0x40
        },
        {
            MTX2, 0x34
        },
        {
            MTX3, 0x0C
        },
        {
            MTX4, 0x17
        },
        {
            MTX5, 0x29
        },
        {
            MTX6, 0x40
        },
        {
            BRIGHT, 0x00
        },
        {
            CONTRAS, 0x40
        },
        {
            CONTRASCENTER, 0x80
        },
        {
            MTXS, 0x1E
        },
        {
            LCC1, 0x00
        },
        {
            LCC2, 0x00
        },
        {
            LCC3, 0x50
        },
        {
            LCC4, 0x30
        },
        {
            LCC5, 0x00
        },
        {
            MANU, 0x80
        },
        {
            MANV, 0x80
        },
        {
            GFIX, 0x00
        },
        {
            GGAIN, 0x00
        },
        {
            DBLV, 0x0A
        },
        {
            AWBCTR3, 0x02
        },
        {
            AWBCTR2, 0x55
        },
        {
            AWBCTR1, 0xC0
        },
        {
            AWBCTR0, 0x9A
        },
        {
            SCALING_XSC, 0x3A
        },
        {
            SCALING_YSC, 0x35
        },
        {
            SCALING_DCWCTR, 0x11
        },
        {
            SCALING_PCLK_DIV, 0x00
        },
        {
            REG74, 0x00
        },
        {
            REG75, 0x0F
        },
        {
            REG76, 0x01
        },
        {
            REG77, 0x10
        },
        {
            SLOP, 0x24
        },
        {
            GAM1, 0x04
        },
        {
            GAM2, 0x07
        },
        {
            GAM3, 0x10
        },
        {
            GAM4, 0x28
        },
        {
            GAM5, 0x36
        },
        {
            GAM6, 0x44
        },
        {
            GAM7, 0x52
        },
        {
            GAM8, 0x60
        },
        {
            GAM9, 0x6C
        },
        {
            GAM10, 0x78
        },
        {
            GAM11, 0x8C
        },
        {
            GAM12, 0x9E
        },
        {
            GAM13, 0xBB
        },
        {
            GAM14, 0xD2
        },
        {
            GAM15, 0xE5
        },
        {
            RGB444, 0x00
        },
        {
            DM_LNL, 0x00
        },
        {
            DM_LNH, 0x00
        },
        {
            LCC6, 0x50
        },
        {
            LCC7, 0x50
        },
        {
            BD50ST, 0x99
        },
        {
            BD60ST, 0x7F
        },
        {
            HAECC1, 0xC0
        },
        {
            HAECC2, 0x90
        },
        {
            SCALING_PCLK_DELAY, 0x02
        },
        {
            NT_CTRL, 0x00
        },
        {
            BD50MAX, 0x0F
        },
        {
            HAECC3, 0xF0
        },
        {
            HAECC4, 0xC1
        },
        {
            HAECC5, 0xF0
        },
        {
            HAECC6, 0xC1
        },
        {
            HAECC7, 0x14
        },
        {
            BD60MAX, 0x0F
        },
        {
            STR_OPT, 0x00
        },
        {
            STR_R, 0x80
        },
        {
            STR_G, 0x80
        },
        {
            STR_B, 0x80
        },
        {
            ABLC1, 0x00
        },
        {
            THL_ST, 0x80
        },
        {
            THL_DLT, 0x04
        },
        {
            AD_CHB, 0x00
        },
        {
            AD_CHR, 0x00
        },
        {
            AD_CHGB, 0x00
        },
        {
            AD_CHGR, 0x00
        },
        {
            SATCTR, 0xC0
        },
        {
            END_OF_TABLE, END_OF_TABLE
        },
    };
    /* END - Default register values */

    return ov7670_write_array(ov7670_default_regs);
}

/*******************************************************************************************************************//**
 * @brief       Setup ov7670 camera register.
 * @param[IN]   None
 * @retval      FSP_SUCCESS    Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t ov7670_write_setup_regs (void)
{
    /* Setup register values */
    static struct ov7670_register_value ov7670_setup_regs[] = {
        {
            CLKRC, 0x0
        },            /* No pre-scale on the input clock */
        {
            COM2, 0x02
        },            /* Drive Strength 3x */
        {
            TSLB,  0x04
        },
        {
            COM7, 0
        },
        {
            HSTART, 0x13
        },
        {
            HSTOP, 0x01
        },
        {
            HREF, 0xb6
        },
        {
            VSTRT, 0x02
        },
        {
            VSTOP, 0x7a
        },
        {
            VREF, 0x0a
        },
        {
            COM3, 0
        },
        {
            COM14, 0
        },
        {
            SCALING_XSC, 0x3a
        },
        {
            SCALING_YSC, 0x35
        },
        {
            SCALING_DCWCTR, 0x11
        },
        {
            SCALING_PCLK_DIV, 0xf0
        },
        {
            SCALING_PCLK_DELAY, 0x02
        },
        {
            COM10, 0x0
        },
        {
            SLOP, 0x20
        },
        {
            GAM1, 0x10
        },
        {
            GAM2, 0x1e
        },
        {
            GAM3, 0x35
        },
        {
            GAM4, 0x5a
        },
        {
            GAM5, 0x69
        },
        {
            GAM6, 0x76
        },
        {
            GAM7, 0x80
        },
        {
            GAM8, 0x88
        },
        {
            GAM9, 0x8f
        },
        {
            GAM10, 0x96
        },
        {
            GAM11, 0xa3
        },
        {
            GAM12, 0xaf
        },
        {
            GAM13, 0xc4
        },
        {
            GAM14, 0xd7
        },
        {
            GAM15, 0xe8
        },
        {
            COM8, COM8_FASTAEC_EN | COM8_AECSTEP_SIZE | COM8_BFILT_EN
        },
        {
            GAIN, 0
        },
        {
            AECH, 0
        },
        {
            COM4, 0x40
        },
        {
            COM9, 0x18
        },
        {
            BD50MAX, 0x05
        },
        {
            BD60MAX, 0x07
        },
        {
            AEW, 0x95
        },
        {
            AEB, 0x33
        },
        {
            VPT, 0xe3
        },
        {
            HAECC1, 0x78
        },
        {
            HAECC2, 0x68
        },
        {
            0xa1, 0x03
        },
        {
            HAECC3, 0xd8
        },
        {
            HAECC4, 0xd8
        },
        {
            HAECC5, 0xf0
        },
        {
            HAECC6, 0x90
        },
        {
            HAECC7, 0x94
        },
        {
            COM8, COM8_FASTAEC_EN | COM8_AECSTEP_SIZE | COM8_BFILT_EN | COM8_AGC_EN | COM8_AEC_EN
        },
        {
            COM5, 0x61
        },
        {
            COM6, 0x4b
        },
        {
            0x16, 0x02
        },
        {
            MVFP, 0x07
        },
        {
            ADCCTR1, 0x02
        },
        {
            ADCCTR2, 0x91
        },
        {
            0x29, 0x07
        },
        {
            CHLF, 0x0b
        },
        {
            0x35, 0x0b
        },
        {
            ADC, 0x1d
        },
        {
            ACOM, 0x71
        },
        {
            OFON, 0x2a
        },
        {
            COM12, 0x78
        },
        {
            0x4d, 0x40
        },
        {
            0x4e, 0x20
        },
        {
            GFIX, 0
        },
        {
            DBLV, 0x4a
        },            /* PLL x4 */
        {
            REG74, 0x10
        },
        {
            0x8d, 0x4f
        },
        {
            0x8e, 0
        },
        {
            0x8f, 0
        },
        {
            0x90, 0
        },
        {
            0x91, 0
        },
        {
            0x96, 0
        },
        {
            0x9a, 0
        },
        {
            0xb0, 0x84
        },
        {
            ABLC1, 0x0c
        },
        {
            0xb2, 0x0e
        },
        {
            THL_ST, 0x82
        },
        {
            0xb8, 0x0a
        },
        {
            AWBC1, 0x0a
        },
        {
            AWBC2, 0xf0
        },
        {
            AWBC3, 0x34
        },
        {
            AWBC4, 0x58
        },
        {
            AWBC5, 0x28
        },
        {
            AWBC6, 0x3a
        },
        {
            0x59, 0x88
        },
        {
            0x5a, 0x88
        },
        {
            0x5b, 0x44
        },
        {
            0x5c, 0x67
        },
        {
            0x5d, 0x49
        },
        {
            0x5e, 0x0e
        },
        {
            AWBCTR3, 0x0a
        },
        {
            AWBCTR2, 0x55
        },
        {
            AWBCTR1, 0x11
        },
        {
            AWBCTR0, 0x9f
        },
        {
            GGAIN, 0x40
        },
        {
            BLUE, 0x40
        },
        {
            RED, 0x60
        },
        {
            COM8, COM8_FASTAEC_EN | COM8_AECSTEP_SIZE | COM8_BFILT_EN | COM8_AGC_EN | COM8_AEC_EN | COM8_AWB_EN
        },
        {
            MTX1, 0x80
        },
        {
            MTX2, 0x80
        },
        {
            MTX3, 0
        },
        {
            MTX4, 0x22
        },
        {
            MTX5, 0x5e
        },
        {
            MTX6, 0x80
        },
        {
            MTXS, 0x9e
        },
        {
            COM16, COM16_AWBGAIN_EN
        },
        {
            EDGE, 0
        },
        {
            REG75, 0x05
        },
        {
            REG76, 0xe1
        },
        {
            DNSTH, 0
        },
        {
            REG77, 0x01
        },
        {
            COM13, 0xc3
        },
        {
            REG4B, 0x09
        },
        {
            SATCTR, 0x60
        },
        {
            COM16, 0x38
        },
        {
            CONTRAS, 0x40
        },
        {
            ARBLM, 0x11
        },
        {
            COM11, COM11_EXP_TIMING | COM11_D56_AUTO
        },
        {
            NT_CTRL, 0x88
        },
        {
            0x96, 0
        },
        {
            0x97, 0x30
        },
        {
            0x98, 0x20
        },
        {
            0x99, 0x30
        },
        {
            0x9a, 0x84
        },
        {
            0x9b, 0x29
        },
        {
            0x9c, 0x03
        },
        {
            0x9d, 0x4c
        },
        {
            0x9e, 0x3f
        },
        {
            0x78, 0x04
        },
        {
            0x79, 0x01
        },
        {
            0xc8, 0xf0
        },
        {
            0x79, 0x0f
        },
        {
            0xc8, 0x00
        },
        {
            0x79, 0x10
        },
        {
            0xc8, 0x7e
        },
        {
            0x79, 0x0a
        },
        {
            0xc8, 0x80
        },
        {
            0x79, 0x0b
        },
        {
            0xc8, 0x01
        },
        {
            0x79, 0x0c
        },
        {
            0xc8, 0x0f
        },
        {
            0x79, 0x0d
        },
        {
            0xc8, 0x20
        },
        {
            0x79, 0x09
        },
        {
            0xc8, 0x80
        },
        {
            0x79, 0x02
        },
        {
            0xc8, 0xc0
        },
        {
            0x79, 0x03
        },
        {
            0xc8, 0x40
        },
        {
            0x79, 0x05
        },
        {
            0xc8, 0x30
        },
        {
            0x79, 0x26
        },
        {
            END_OF_TABLE, END_OF_TABLE
        },
    };
    /* END - Setup register values */

    return ov7670_write_array(ov7670_setup_regs);
}

/*******************************************************************************************************************//**
 * @brief       Setups the camera
 * @param[IN]   None
 * @retval      FSP_SUCCESS    Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t ov7670_setup (void)
{
    fsp_err_t err;

    /* Reset OV7670 Camera */
    err = ov7670_reset();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** ov7670_reset FAILED ** \r\n");
        return err;
    }

    /* Initialize OV7670 Camera */
    err = ov7670_init();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** ov7670_init FAILED ** \r\n");
        ov7670_close();
        return err;
    }

    /* Setup OV7670 Camera registers*/
    err = ov7670_write_setup_regs();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** ov7670_write_setup_regs FAILED ** \r\n");
        ov7670_close();
        return err;
    }

    /* Set camera in RGB format */
    err = ov7670_rgb565();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** ov7670_rgb565 FAILED ** \r\n");
        ov7670_close();
        return err;
    }

    /* Settling time for OV7670 register change, 300ms max */
    R_BSP_SoftwareDelay(OV7670_SETTLING_TIME, BSP_DELAY_UNITS_MILLISECONDS);
    /* End of Camera Setup */

    return err;
}
/*******************************************************************************************************************//**
 * @brief       closes the open i2c_master interface.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void ov7670_close (void)
{
    fsp_err_t  err = FSP_SUCCESS;

    err = R_IIC_MASTER_Close(&g_i2c_ctrl);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** IIC_Master_Close API FAILED ** \r\n");
    }
}

/*******************************************************************************************************************//**
 *  @brief      iic_master callback function
 *  @param[in]  p_args
 *  @retval None
 **********************************************************************************************************************/
void i2c_master_callback(i2c_master_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        g_master_event = p_args->event;
    }
}
/*******************************************************************************************************************//**
 * @} (end addtogroup pdc_ep)
 **********************************************************************************************************************/
