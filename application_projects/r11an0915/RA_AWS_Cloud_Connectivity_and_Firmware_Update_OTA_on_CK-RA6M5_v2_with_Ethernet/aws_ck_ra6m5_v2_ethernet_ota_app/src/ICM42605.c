/*
 * ICM_42605.c
 *
 *  Created on: 01-May-2023
 *      Author: 31342
 */

#include "ICM42605.h"

float aRes = 0;
float gRes = 0;

//int8_t err = 0;
uint8_t temp = 0;

extern float Ares;
extern float Gres;

int8_t writeByte(uint8_t reg, uint8_t *data_in, uint32_t len)
{
	int8_t err;
	err = RmCom_I2C_w_ICM(reg, data_in, len);
	return err;
}


uint8_t readByte(uint8_t reg, uint8_t *data_out, uint32_t len)
{
	int8_t err;
	err = RmCom_I2C_r_ICM(reg, data_out, len);
	return err;
}


int8_t getChipID(uint8_t *val)
{
	int8_t err;
	err = readByte(ICM42605_WHO_AM_I, val, 1);
	return err;
}


void delay_ms(uint32_t delayms)
{
  delay(delayms); //R_BSP_SoftwareDelay(delay, BSP_DELAY_MILLISECS);
}


float getAres(uint8_t Ascale)
{
	switch (Ascale)
	{
		// Possible accelerometer scales (and their register bit settings) are:
		// 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
		case AFS_2G:
		  aRes = 2.0f / 32768.0f;
		  break;
		case AFS_4G:
		  aRes = 4.0f / 32768.0f;
		  break;
		case AFS_8G:
		  aRes = 8.0f / 32768.0f;
		  break;
		case AFS_16G:
		  aRes = 16.0f / 32768.0f;
		  break;
	}

	return aRes;
}


float getGres(uint8_t Gscale)
{
	switch (Gscale)
	{
		case GFS_15_125DPS:
		  gRes = 15.125f / 32768.0f;
		  break;
		case GFS_31_25DPS:
		  gRes = 31.25f / 32768.0f;
		  break;
		case GFS_62_5DPS:
		  gRes = 62.5f / 32768.0f;
		  break;
		case GFS_125DPS:
		  gRes = 125.0f / 32768.0f;
		  break;
		case GFS_250DPS:
		  gRes = 250.0f / 32768.0f;
		  break;
		case GFS_500DPS:
		  gRes = 500.0f / 32768.0f;
		  break;
		case GFS_1000DPS:
		  gRes = 1000.0f / 32768.0f;
		  break;
		case GFS_2000DPS:
		  gRes = 2000.0f / 32768.0f;
		  break;
	}

	return gRes;
}


int8_t ICM42605_reset()
{
  int8_t err = 0;
  //uint8_t temp = 0;

  //reset device
  err = readByte(ICM42605_DEVICE_CONFIG, &temp, 1);
  if(RIIC_SUCCESS != err)
  {
      return err;
  }

  temp = temp | 0x01;
  err = writeByte(ICM42605_DEVICE_CONFIG, &temp, 1); // Set bit 0 to 1 to reset ICM42605
  if(RIIC_SUCCESS != err)
  {
      return err;
  }

  delay_ms(50); // Wait for all registers to reset

  return err;
}


int8_t ICM42605_status(uint8_t *val)
{
	int8_t err = 0;
	err = readByte(ICM42605_INT_STATUS, val, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}

	return err;
}


int8_t select_bank_0()
{
    int8_t err = 0;
	temp = 0;
	err = readByte(ICM42605_REG_BANK_SEL, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}
	temp = temp & (~(0x07));
	err = writeByte(ICM42605_REG_BANK_SEL, &temp, 1); // select Bank 0
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}

	return err;
}

int8_t select_bank_4()
{
    int8_t err = 0;
	temp = 0;
	err = readByte(ICM42605_REG_BANK_SEL, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}
	temp = (temp | 0x04);
	err = writeByte(ICM42605_REG_BANK_SEL, &temp, 1); // select Bank 4
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}

	return err;
}


int8_t ICM42605_init(uint8_t Ascale, uint8_t Gscale, uint8_t AODR, uint8_t GODR)
{
	int8_t err = 0;
	//uint8_t temp = 0;

	//enable gyro and accel in low noise mode
	err = readByte(ICM42605_PWR_MGMT0, &temp, 1); // make sure not to disturb reserved bit values
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}
	temp = (temp | 0x0F);
	err = writeByte(ICM42605_PWR_MGMT0, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}

	//set gyro scale and data rate
	temp = 0;
	err = readByte(ICM42605_GYRO_CONFIG0, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}
	temp = temp & (~(0xEF));
	temp = (temp | GODR | (Gscale << 5));
	err = writeByte(ICM42605_GYRO_CONFIG0, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}

	// set accel scale and data rate
	temp = 0;
	err = readByte(ICM42605_ACCEL_CONFIG0, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}
	temp = temp & (~(0xEF));
	temp = (temp | AODR | (Ascale << 5));
	err = writeByte(ICM42605_ACCEL_CONFIG0, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}

	// set temperature sensor low pass filter to 5Hz, use first order gyro filter
	temp = 0;
	err = readByte(ICM42605_GYRO_CONFIG1, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}
	temp = temp & (~(0xEF));
	temp = (temp | 0xC0);    //(temp | 0xD0);
	err = writeByte(ICM42605_GYRO_CONFIG1, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}

	// set both interrupts active high, push-pull, pulsed
	temp = 0;
	err = readByte(ICM42605_INT_CONFIG, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}
	temp = (temp | 0x18 | 0x03);
	err = writeByte(ICM42605_INT_CONFIG, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}

	// set bit 4 to zero for proper function of INT1 and INT2
	temp = 0;
	err = readByte(ICM42605_INT_CONFIG1, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}
	temp = temp & (~(0x10));
	err = writeByte(ICM42605_INT_CONFIG1, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}

	//route data ready interrupt to INT1
	temp = 0;
	err = readByte(ICM42605_INT_SOURCE0, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}
	temp = (temp | 0x08);
	err = writeByte(ICM42605_INT_SOURCE0, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}

//	temp = 0;
//	err = readByte(ICM42605_INT_SOURCE3, &temp, 1);
//	if(RIIC_SUCCESS != err)
//	{
//	  return err;
//	}
//	temp = (temp | 0x01);
//	err = writeByte(ICM42605_INT_SOURCE3, &temp, 1); // route AGC interrupt to INT2
//	if(RIIC_SUCCESS != err)
//	{
//	  return err;
//	}

	// Select Bank 4
	temp = 0;
	err = readByte(ICM42605_REG_BANK_SEL, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}
	temp = (temp | 0x04);
	err = writeByte(ICM42605_REG_BANK_SEL, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}

	// select unitary mounting matrix
	temp = 0;
	err = readByte(ICM42605_APEX_CONFIG5, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}
	temp = temp & (~(0x07));
	err = writeByte(ICM42605_APEX_CONFIG5, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}

	// select Bank 0
	temp = 0;
	err = readByte(ICM42605_REG_BANK_SEL, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}
	temp = temp & (~(0x07));
	err = writeByte(ICM42605_REG_BANK_SEL, &temp, 1);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}

    return err;
}

int16_t acl_val_ofst[3] = {0};
int16_t gy_val_ofst[3] = {0};
int16_t tempr_ofst = 0;
int32_t sum[7] = {0, 0, 0, 0, 0, 0, 0};

int8_t ICM42605_offsetBias(float *a_ofst, float *g_ofst)
{
int8_t err = 0;
//	int16_t acl_val[3] = {0};
//	int16_t gy_val[3] = {0};
//	int16_t tempr = 0;
//	int32_t sum[7] = {0, 0, 0, 0, 0, 0, 0};

	//Serial.println("Calculate accel and gyro offset biases: keep sensor flat and motionless!");
	//delay_ms(4000);

	for (int ii = 0; ii < 128; ii++)
	{
		err = ICM42605_readData(&acl_val_ofst[0], &gy_val_ofst[0], &tempr_ofst);
		if(RIIC_SUCCESS != err)
		{
		  return err;
		}

		sum[1] += acl_val_ofst[0];
		sum[2] += acl_val_ofst[1];
		sum[3] += acl_val_ofst[2];
		sum[4] += gy_val_ofst[0];
		sum[5] += gy_val_ofst[1];
		sum[6] += gy_val_ofst[2];
		delay_ms(50);
	}

	a_ofst[0] = sum[1] * aRes / 128.0f;
	a_ofst[1] = sum[2] * aRes / 128.0f;
	a_ofst[2] = sum[3] * aRes / 128.0f;

	g_ofst[0] = sum[4] * gRes / 128.0f;
	g_ofst[1] = sum[5] * gRes / 128.0f;
	g_ofst[2] = sum[6] * gRes / 128.0f;

	if (a_ofst[0] > 0.8f)
	{
	  a_ofst[0] -= 1.0f; // Remove gravity from the x-axis accelerometer bias calculation
	}
	if (a_ofst[0] < -0.8f)
	{
	  a_ofst[0] += 1.0f; // Remove gravity from the x-axis accelerometer bias calculation
	}

	if (a_ofst[1] > 0.8f)
	{
	  a_ofst[1] -= 1.0f; // Remove gravity from the y-axis accelerometer bias calculation
	}
	if (a_ofst[1] < -0.8f)
	{
	  a_ofst[1] += 1.0f; // Remove gravity from the y-axis accelerometer bias calculation
	}

	if (a_ofst[2] > 0.8f)
	{
	  a_ofst[2] -= 1.0f; // Remove gravity from the z-axis accelerometer bias calculation
	}
	if (a_ofst[2] < -0.8f)
	{
	  a_ofst[2] += 1.0f; // Remove gravity from the z-axis accelerometer bias calculation
	}


	return err;
}

uint8_t rawData[14] = {0};
//int16_t accel_val[3] = {0};
//int16_t gyro_val[3] = {0};
//int16_t tempr = 0;

int8_t ICM42605_readData(int16_t *accel_val, int16_t *gyro_val, int16_t *tempr)
{
	int8_t err = 0;
	//uint8_t rawData[14];  // x/y/z accel register data stored here

	// Read the 14 raw data registers into data array
	err = readByte(ICM42605_TEMP_DATA1, &rawData[0], 14);
	if(RIIC_SUCCESS != err)
	{
	  return err;
	}

	// Turn the MSB and LSB into a signed 16-bit value
	//temperature value
	*tempr = ((int16_t)rawData[0] << 8) | rawData[1] ;

	//accel x value
	accel_val[0] = ((int16_t)rawData[2] << 8) | rawData[3] ;
	//accel y value
	accel_val[1] = ((int16_t)rawData[4] << 8) | rawData[5] ;
	//accel z value
	accel_val[2] = ((int16_t)rawData[6] << 8) | rawData[7] ;

	//gyro x value
	gyro_val[0] = ((int16_t)rawData[8] << 8) | rawData[9] ;
	//gyro y value
	gyro_val[1] = ((int16_t)rawData[10] << 8) | rawData[11] ;
	//gyro z value
	gyro_val[2] = ((int16_t)rawData[12] << 8) | rawData[13] ;

	return err;
}



int8_t init_snsr_typical_config()
{
    int8_t err = 0;
    uint8_t Ascal = AFS_16G;
    uint8_t Gscal = GFS_2000DPS;
    uint8_t Aodr = AODR_1000Hz;      //AODR_50Hz;
    uint8_t Godr = GODR_1000Hz;

    // get sensor resolutions
    Ares = getAres(Ascal);
    Gres = getGres(Gscal);

    //----------  Initialize sensor in a typical configuration  -----------------//
    //select bank 0
    err = select_bank_0();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //set accel scale and data rate
    temp = 0;
    err = readByte(ICM42605_ACCEL_CONFIG0, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0xEF));
    temp = (temp | Aodr | (Ascal << 5));
    err = writeByte(ICM42605_ACCEL_CONFIG0, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    // set gyro scale and data rate
    temp = 0;
    err = readByte(ICM42605_GYRO_CONFIG0, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0xEF));
    temp = (temp | Godr | (Gscal << 5));
    err = writeByte(ICM42605_GYRO_CONFIG0, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //accel low noise, gyro low noise mode
    temp = 0;
    err = readByte(ICM42605_PWR_MGMT0, &temp, 1); // make sure not to disturb reserved bit values
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = (temp | 0x0F);
    err = writeByte(ICM42605_PWR_MGMT0, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //(for gyro)  // for xyz
    // set temperature sensor low pass filter to 5Hz, use first order gyro filter
    temp = 0;
    err = readByte(ICM42605_GYRO_CONFIG1, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0xEF));
    temp = (temp | 0xC0);    //(temp | 0xD0);
    err = writeByte(ICM42605_GYRO_CONFIG1, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    // set both interrupts active high, push-pull, pulsed
    temp = 0;
    err = readByte(ICM42605_INT_CONFIG, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = (temp | 0x18 | 0x03);
    err = writeByte(ICM42605_INT_CONFIG, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    // set bit 4 to zero for proper function of INT1 and INT2
    temp = 0;
    err = readByte(ICM42605_INT_CONFIG1, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0x10));
    err = writeByte(ICM42605_INT_CONFIG1, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //for xyz
    //route data ready interrupt to INT1
    temp = 0;
    err = readByte(ICM42605_INT_SOURCE0, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = (temp | 0x08);
    err = writeByte(ICM42605_INT_SOURCE0, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

//  //ACCEL_LP_CLK_SEL  (need to check)
//  temp = 0;
//  err = readByte(ICM42605_INTF_CONFIG1, &temp, 1);
//  if(RIIC_SUCCESS != err)
//  {
//    return err;
//  }
//  temp = (temp & (~(1<<3)));
//  err = writeByte(ICM42605_INTF_CONFIG1, &temp, 1);
//  if(RIIC_SUCCESS != err)
//  {
//    return err;
//  }

    //DMP ODR 50 Hz and disable power save mode
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG0, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    temp = temp & (~(0x83));
    temp = temp | 0x2;
    err = writeByte(ICM42605_APEX_CONFIG0, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    delay_ms(1);

    err = select_bank_4();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //for xyz
    //select unitary mounting matrix
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG5, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0x07));
    err = writeByte(ICM42605_APEX_CONFIG5, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    err = select_bank_0();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    return err;
}



int8_t pedomtr_config()
{
    int8_t err = 0;
    //select bank 4
    err = select_bank_4();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //LOW_ENERGY_AMP_TH_SEL
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG1, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0xF0));
    temp = temp | (0xA << 4);
    err = writeByte(ICM42605_APEX_CONFIG1, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //PED_AMP_TH_SEL
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG2, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0xF0));
    temp = temp | (0x8 << 4);
    err = writeByte(ICM42605_APEX_CONFIG2, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //PED_STEP_CNT_TH_SEL (2 step)    (default is 5)
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG2, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0x0F));
    temp = temp | 0x2;
    err = writeByte(ICM42605_APEX_CONFIG2, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //PED_HI_EN_TH_SEL (01b)
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG3, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0x03));
    temp = temp | 0x1;
    err = writeByte(ICM42605_APEX_CONFIG3, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //PED_SB_TIMER_TH_SEL (4 samples)
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG3, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0x1C));
    temp = temp | (4<<2);
    err = writeByte(ICM42605_APEX_CONFIG3, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //PED_STEP_DET_TH_SEL  (2 step)    (default is 2)
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG3, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0xE0));
    temp = temp | (2<<5);
    err = writeByte(ICM42605_APEX_CONFIG3, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //SENSITIVITY_MODE (0: Low power mode at accelerometer ODR 25Hz; High performance mode at accelerometer ODR ≥ 50Hz)
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG9, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0x01));
    err = writeByte(ICM42605_APEX_CONFIG9, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //select bank 0
    err = select_bank_0();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    return err;
}


int8_t tilt_config()
{
    int8_t err = 0;
    //select bank 4
    err = select_bank_4();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //TILT_WAIT_TIME_SEL (4s)
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG4, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0xC0));
    temp = temp | (2<<6);
    err = writeByte(ICM42605_APEX_CONFIG4, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    delay_ms(1);

    //select bank 0
    err = select_bank_0();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    return err;
}


int8_t config_INT1()
{
    int8_t err = 0;
    //select bank 4
    err = select_bank_4();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //tilt,step,tap, wak/slp detect interrupt routed to INT1
    temp = 0;
    err = readByte(ICM42605_INT_SOURCE6, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = (temp | (1<<3) | (1<<5) | (1<<0) | (1<<2) | (1<<1));
    err = writeByte(ICM42605_INT_SOURCE6, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    delay_ms(50);

    //select bank 0
    err = select_bank_0();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    return err;
}


int8_t enable_apex_features()
{
    int8_t err = 0;
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG0, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //enable tilt,step,tap and wak/slp detection
    temp = (temp | (1<<4) | (1<<5) | (1<<6) | (1<<3));
    err = writeByte(ICM42605_APEX_CONFIG0, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    return err;
}


int8_t tap_config()
{
    int8_t err = 0;
    //ACCEL_UI_FILT_ORD (2)
    temp = 0;
    err = readByte(ICM42605_ACCEL_CONFIG1, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0x18));
    temp = (temp | (2<<3));
    err = writeByte(ICM42605_ACCEL_CONFIG1, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //ACCEL_UI_FILT_BW (0)
    temp = 0;
    err = readByte(ICM42605_GYRO_ACCEL_CONFIG0, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0xF0));
    err = writeByte(ICM42605_GYRO_ACCEL_CONFIG0, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    delay_ms(1);

    //select bank 4
    err = select_bank_4();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //TAP_TMAX=2, TAP_TMIN=3 and TAP_TAVG=3
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG8, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0x7F));
    temp = temp | 0x5B;
    err = writeByte(ICM42605_APEX_CONFIG8, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //TAP_MIN_JERK_THR = 17 and TAP_MAX_PEAK_TOL = 2
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG7, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0xFF));
    temp = temp | (0x11<<2) | 0x2;
    err = writeByte(ICM42605_APEX_CONFIG7, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    delay_ms(1);

    //select bank 0
    err = select_bank_0();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    return err;
}


int8_t raise_wak_slp_config()
{
    int8_t err = 0;
    //select bank 4
    err = select_bank_4();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //SLEEP_TIME_OUT
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG4, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0x38));
    temp = temp | (0x7<<3);
    err = writeByte(ICM42605_APEX_CONFIG4, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    delay_ms(1);

    //MOUNTING_MATRIX   (unitary mounting matrix)
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG5, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0x7));
    err = writeByte(ICM42605_APEX_CONFIG5, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    delay_ms(1);

    //SLEEP_GESTURE_DELAY
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG6, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0x7));
    temp = temp | 0x1;
    err = writeByte(ICM42605_APEX_CONFIG6, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    delay_ms(1);

    //select bank 0
    err = select_bank_0();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    return err;
}


int8_t set_wom_thrshld()
{
    int8_t err = 0;
    //select bank 4
    err = select_bank_4();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //----------- config ---------------//
    //WOM_X_TH
    temp = 0;
    err = readByte(ICM42605_ACCEL_WOM_X_THR, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = 98;
    err = writeByte(ICM42605_ACCEL_WOM_X_THR, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //WOM_Y_TH
    temp = 0;
    err = readByte(ICM42605_ACCEL_WOM_Y_THR, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = 98;
    err = writeByte(ICM42605_ACCEL_WOM_Y_THR, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //WOM_Z_TH
    temp = 0;
    err = readByte(ICM42605_ACCEL_WOM_Z_THR, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = 98;
    err = writeByte(ICM42605_ACCEL_WOM_Z_THR, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    //----------- config ---------------//

    //select bank 0
    err = select_bank_0();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    return err;
}


int8_t wom_config()
{
    int8_t err = 0;
    //select bank 0
    err = select_bank_0();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //WOM interrupt routed to INT1 for all the 3 axes
    temp = 0;
    err = readByte(ICM42605_INT_SOURCE1, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0x7));
    temp = (temp | 0x7);
    err = writeByte(ICM42605_INT_SOURCE1, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    delay_ms(50);

    //Turn on WOM feature by setting WOM_INT_MODE to 0, WOM_MODE to 1, SMD_MODE to 1
    temp = 0;
    err = readByte(ICM42605_SMD_CONFIG, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0xF));
    temp = (temp | 0x5);
    err = writeByte(ICM42605_SMD_CONFIG, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    return err;
}


int8_t smd_config()
{
    int8_t err = 0;
    //select bank 0
    err = select_bank_0();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //SMD interrupt routed to INT1
    temp = 0;
    err = readByte(ICM42605_INT_SOURCE1, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp | (1<<3);
    err = writeByte(ICM42605_INT_SOURCE1, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    delay_ms(50);

    //Turn on SMD feature by setting WOM_INT_MODE to 0, WOM_MODE to 1, SMD_MODE to 2
    temp = 0;
    err = readByte(ICM42605_SMD_CONFIG, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp & (~(0xF));
    temp = (temp | 0x6);
    err = writeByte(ICM42605_SMD_CONFIG, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    return err;
}


int8_t ICM42605_apex_featur_init()
{
    int8_t err = 0;
    //----------  Initialize sensor in a typical configuration  -----------------//
    err = init_snsr_typical_config();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

#if(TAP_CHECK == 1)
    //tap configuration
    err = tap_config();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
#endif

    //set wom thresholds
    err = set_wom_thrshld();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

#if(WOM == 1)
    //WOM configuration and WOM interrupt routed to INT1
    err = wom_config();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
#else
    //SMD configuration and SMD interrupt routed to INT1
    err = smd_config();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
#endif

    //--------------------  Initialize APEX hardware  ---------------------------//
    //DMP_MEM_RESET_EN
    temp = 0;
    err = readByte(ICM42605_SIGNAL_PATH_RESET, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp | (1<<5);
    err = writeByte(ICM42605_SIGNAL_PATH_RESET, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    delay_ms(1);

    //tilt configuration
    err = tilt_config();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //pedometer configuration
    err = pedomtr_config();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //raise to wake/sleep config
    err = raise_wak_slp_config();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //DMP_INIT_EN
    temp = 0;
    err = readByte(ICM42605_SIGNAL_PATH_RESET, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    temp = temp | (1<<6);
    err = writeByte(ICM42605_SIGNAL_PATH_RESET, &temp, 1);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }
    delay_ms(50);

    //configure interrupt routed to INT1
    err = config_INT1();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    //enable apex features
    err = enable_apex_features();
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    return err;
}





