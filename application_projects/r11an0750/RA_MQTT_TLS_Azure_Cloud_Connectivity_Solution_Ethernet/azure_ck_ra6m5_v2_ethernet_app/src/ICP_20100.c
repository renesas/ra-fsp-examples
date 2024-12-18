/*
 * ICM_20100.c
 *
 *  Created on: Jan 20, 2022
 *      Author: Vivek
 */

#include <ICP_20100.h>
#include "RmcI2C.h"
#include "math.h"


#include "stdint.h"
#include "stdio.h"
//#include "r_comms_i2c_rx_config.h"
//#include "r_smc_entry.h"
//
//#include "r_comms_i2c_if.h"
//#include "r_comms_i2c_rx_config.h"

//#if COMMS_I2C_CFG_DRIVER_I2C
//#include "r_riic_rx_if.h"
//#endif
//#if COMMS_I2C_CFG_DRIVER_SCI_I2C
//#include "r_sci_iic_rx_if.h"
//#endif

#include "string.h"

int8_t pressure20_write_register(uint8_t reg, uint8_t *data_in, uint32_t len)
{
    int8_t err;

    err = RmCom_I2C_w(reg, data_in, len);

    return err;
}


int8_t pressure20_read_register(uint8_t reg, uint8_t *data_out, uint32_t len)
{
    int8_t err;

    err = RmCom_I2C_r(reg, data_out, len);

    return err;
}


int8_t pressure20_read_register_data(uint8_t reg, uint8_t *data_out, uint32_t len)
{
    int8_t err;

    err = RmCom_I2C_r(reg, data_out, len);

    return err;
}


int8_t pressure20_check_communication()
{
    volatile int8_t err = -1;
    volatile uint8_t device_id;

    //while(err == -1)
    {
        if(0 == pressure20_read_register(PRESSURE20_REG_DEVICE_ID, &device_id, 1))
        {
            if ( PRESSURE20_DEVICE_ID == device_id )
            {
                err = FSP_SUCCESS;
            }
        }

        R_BSP_SoftwareDelay(1, 1000);
    }

    //err = FSP_SUCCESS;

    return err;
}


int8_t pressure20_write_mode_select(uint8_t mode_select )
{
    int8_t err;
    uint8_t dev_status;
    do
    {
        R_BSP_SoftwareDelay(1, 1000);
        err = pressure20_read_register(PRESSURE20_REG_DEVICE_STATUS, &dev_status, 1);
    } while ( PRESSURE20_MODE_SYNC_STATUS != (dev_status & PRESSURE20_MODE_SYNC_STATUS ));

    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    err = pressure20_write_register(PRESSURE20_REG_MODE_SELECT, &mode_select, 1);
    return err;
}


int8_t pressure20_clear_interrupts()
{
    volatile uint8_t int_status;
    volatile int8_t err;

    err = pressure20_read_register(PRESSURE20_REG_INTERRUPT_STATUS, &int_status, 1);
    //printf("before int_status = %d \n", int_status);

    if(FSP_SUCCESS == err)
    {
        err = pressure20_write_register(PRESSURE20_REG_INTERRUPT_STATUS, &int_status, 1);
//      err = pressure20_read_register(PRESSURE20_REG_INTERRUPT_STATUS, &int_status, 1);
//      printf("after int_status = %d \n", int_status);
    }

    return err;
}


int8_t pressure20_read_otp_data (uint8_t otp_address, uint8_t *data_out )
{
    int8_t err;
    uint8_t otp_status;
    uint8_t data = 0;

    err = pressure20_write_register(PRESSURE20_REG_OTP_ADDRESS_REG, &otp_address, 1);
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    data = PRESSURE20_OTP_COMMAND_READ_ACTION;
    err = pressure20_write_register(PRESSURE20_REG_OTP_COMMAND_REG, &data, 1);
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    // Wait for the OTP read to finish
    do
    {
        R_BSP_SoftwareDelay(1, 1000);
        err = pressure20_read_register(PRESSURE20_REG_OTP_STATUS, &otp_status, 1);
    } while ( PRESSURE20_BUSY == otp_status );

    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    // Read the data from register
    err = pressure20_read_register(PRESSURE20_REG_OTP_RDATA, data_out, 1);

    return err;
}


int8_t pressure20_read_data(float *pressure, float *temperature)
{
    uint8_t data_buf[ 6 ] = { 0 };
    int8_t err;
    int32_t raw_data;

    err = pressure20_read_register_data(PRESSURE20_REG_PRESS_DATA_0, data_buf, 6 );
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    raw_data = ( ( int32_t ) ( ( ( int32_t ) ( data_buf[ 2 ] & 0x0F ) << 16 ) |
                             ( ( uint16_t ) data_buf[ 1 ] << 8 ) | data_buf[ 0 ] ) << 12 ) >> 12;
    // P = ( POUT / 2^17 ) * 400mBar + 700mBar
    *pressure = ( ( float ) raw_data / PRESSURE20_PRESSURE_RES_RAW ) * PRESSURE20_PRESSURE_RES_MBAR +
                PRESSURE20_PRESSURE_OFFSET_MBAR;

    raw_data = ( ( int32_t ) ( ( ( int32_t ) ( data_buf[ 5 ] & 0x0F ) << 16 ) |
                               ( ( uint16_t ) data_buf[ 4 ] << 8 ) | data_buf[ 3 ] ) << 12 ) >> 12;
    // T = ( TOUT / 2^18 ) * 65C + 25C
    *temperature = ( ( float ) raw_data / PRESSURE20_TEMPERATURE_RES_RAW ) * PRESSURE20_TEMPERATURE_RES_C +
                   PRESSURE20_TEMPERATURE_OFFSET_C;
    return err;
}

int8_t dummy_read()
{
    uint8_t data = 0;
    int8_t err = FSP_SUCCESS;

    err = pressure20_read_register(DUMMY_READ_ADDR, &data, 1);
    if(FSP_SUCCESS != err)
    {
       err = ERROR;
    }

    return err;
}

int8_t pressure20_default_cfg()
{
    volatile int8_t err;
    volatile uint8_t reg_data;
    volatile uint8_t offset, gain, hfosc;
    uint8_t data = 0;

    // dummy write
    data = DUMMY;
    err = pressure20_write_register(PRESSURE20_REG_DUMMY, &data, 1);
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    R_BSP_SoftwareDelay(10, 1000);   //update

    if (ERROR == pressure20_check_communication() )
    {
        return ERROR;
    }

    // Move to stand by mode
    err = pressure20_write_mode_select(PRESSURE20_POWER_MODE_NORMAL);
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }
    R_BSP_SoftwareDelay(5, 1000);

    // Flush the FIFO to empty it
    data = PRESSURE20_FIFO_FLUSH;
    err = pressure20_write_register(PRESSURE20_REG_FIFO_FILL, &data, 1);
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    // Reset FIFO config
    data = PRESSURE20_FIFO_LEVEL_EMPTY;
    err = pressure20_write_register(PRESSURE20_REG_FIFO_CONFIG, &data, 1);
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    // Mask all interrupts
    data = PRESSURE20_INT_MASK_ALL;
    err = pressure20_write_register(PRESSURE20_REG_INTERRUPT_MASK, &data, 1);
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    // Clear interrupts
    err = pressure20_clear_interrupts();
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    // Read boot up status and avoid re-running boot up sequence if it is already done
    err = pressure20_read_register(PRESSURE20_REG_OTP_STATUS2, &reg_data, 1);
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    if ( PRESSURE20_BOOT_UP_STATUS != ( reg_data & PRESSURE20_BOOT_UP_STATUS ) )
    {
        // Continue with boot up sequence for A1
        // Set to power mode to activate the OTP power domain and get access to the main registers */
        err = pressure20_write_mode_select(PRESSURE20_POWER_MODE_ACTIVE);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }
        R_BSP_SoftwareDelay(5, 1000);

        // Unlock the main registers
        data = PRESSURE20_MASTER_UNLOCK;
        err = pressure20_write_register(PRESSURE20_REG_MASTER_LOCK, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }

        // Enable the OTP and the write switch
        data = (PRESSURE20_OTP_WRITE_SWITCH | PRESSURE20_OTP_ENABLE);
        err = pressure20_write_register(PRESSURE20_REG_OTP_CONFIG1, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }
        R_BSP_SoftwareDelay(10, 1000000);

        // Toggle the OTP reset pin
        data = PRESSURE20_OTP_RESET_SET;
        err = pressure20_write_register(PRESSURE20_REG_OTP_DBG2, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }
        R_BSP_SoftwareDelay(10, 1000000);

        data = PRESSURE20_OTP_RESET_CLEAR;
        err = pressure20_write_register(PRESSURE20_REG_OTP_DBG2, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }
        R_BSP_SoftwareDelay(10, 1000000);

        // Program redundant read
        data = PRESSURE20_OTP_MRA_LSB;
        err = pressure20_write_register(PRESSURE20_REG_OTP_MRA_LSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }

        data = PRESSURE20_OTP_MRA_MSB;
        err = pressure20_write_register(PRESSURE20_REG_OTP_MRA_MSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }

        data = PRESSURE20_OTP_MRB_LSB;
        err = pressure20_write_register(PRESSURE20_REG_OTP_MRB_LSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }

        data = PRESSURE20_OTP_MRB_MSB;
        err = pressure20_write_register(PRESSURE20_REG_OTP_MRB_MSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }

        data = PRESSURE20_OTP_MR_LSB;
        err = pressure20_write_register(PRESSURE20_REG_OTP_MR_LSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }

        data = PRESSURE20_OTP_MR_MSB;
        err = pressure20_write_register(PRESSURE20_REG_OTP_MR_MSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }

        // Read offset, gain and hfosc
        err = pressure20_read_otp_data(PRESSURE20_OTP_ADDRESS_OFFSET, &offset);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }
        err = pressure20_read_otp_data(PRESSURE20_OTP_ADDRESS_GAIN, &gain);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }
        err = pressure20_read_otp_data(PRESSURE20_OTP_ADDRESS_HFOSC, &hfosc);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }

        // Disable OTP and write switch
        data = PRESSURE20_OTP_DISABLE;
        err = pressure20_write_register(PRESSURE20_REG_OTP_CONFIG1, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }

        // Write OTP values to main registers
        data = (offset & PRESSURE20_TRIM1_MSB_PEFE_OFFSET_MASK);
        err = pressure20_write_register(PRESSURE20_REG_TRIM1_MSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }

        err = pressure20_read_register (PRESSURE20_REG_TRIM2_MSB, &reg_data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }

        reg_data = ( reg_data & ( ~PRESSURE20_TRIM2_MSB_PEFE_GAIN_MASK ) ) | ( ( gain << 4 ) & PRESSURE20_TRIM2_MSB_PEFE_GAIN_MASK );
        err = pressure20_write_register (PRESSURE20_REG_TRIM2_MSB, &reg_data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }

        data = (hfosc & PRESSURE20_TRIM2_LSB_HFOSC_MASK);
        err = pressure20_write_register (PRESSURE20_REG_TRIM2_LSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }

        // Lock the main register
        data = PRESSURE20_MASTER_LOCK;
        err = pressure20_write_register (PRESSURE20_REG_MASTER_LOCK, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }

        // Move to stand by mode
        err = pressure20_write_mode_select (PRESSURE20_POWER_MODE_NORMAL );
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }
        R_BSP_SoftwareDelay(5, 1000);

        // Update boot up status to 1
        data = PRESSURE20_BOOT_UP_STATUS;
        err = pressure20_write_register (PRESSURE20_REG_OTP_STATUS2, &data, 1);
        if(FSP_SUCCESS != err)
        {
           return ERROR;
        }
    }

    // Enable continuous measurement and set ODR to 25 Hz
    err = pressure20_write_mode_select (PRESSURE20_MEAS_MODE_CONTINUOUS | PRESSURE20_MEAS_CONFIG_MODE0_ODR_25HZ);
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }
    R_BSP_SoftwareDelay(5, 1000);

    // Flush the FIFO to empty it
    data = PRESSURE20_FIFO_FLUSH;
    err = pressure20_write_register (PRESSURE20_REG_FIFO_FILL, &data, 1);
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    // Wait for the FIFO full indication
    do
    {
        R_BSP_SoftwareDelay(100, 1000);
        err = pressure20_read_register (PRESSURE20_REG_FIFO_FILL, &reg_data, 1);
    } while ( PRESSURE20_FIFO_FULL != ( reg_data & PRESSURE20_FIFO_FULL ) );
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    // Flush the FIFO to empty it, because of the FIR filter settling effect (warm up) on the first FIFO output
    data = PRESSURE20_FIFO_FLUSH;
    err = pressure20_write_register (PRESSURE20_REG_FIFO_FILL, &data, 1);
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    // Set FIFO watermark HIGH level to 1
    data = (PRESSURE20_FIFO_LEVEL_8 << 4);       //PRESSURE20_FIFO_LEVEL_1
    err = pressure20_write_register (PRESSURE20_REG_FIFO_CONFIG, &data, 1);
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    // Unmask FIFO watermark HIGH interrupt
    data = ~(PRESSURE20_INT_MASK_FIFO_WMK_HIGH | PRESSURE20_INT_MASK_FIFO_OVERFLOW);
    err = pressure20_write_register (PRESSURE20_REG_INTERRUPT_MASK, &data, 1);
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    err = pressure20_read_register (PRESSURE20_REG_INTERRUPT_MASK, &reg_data, 1);
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }
    //printf("initial INTERRUPT_MASK = %x \n", reg_data);

    // Clear interrupts
    err = pressure20_clear_interrupts();
    if(FSP_SUCCESS != err)
    {
       return ERROR;
    }

    return err;
}

