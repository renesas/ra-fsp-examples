#include <stdio.h>
#include <stddef.h>
#include "ob1203_bio/OB1203/OB1203.h"
#include "I2C/i2c.h"
#include "oximeter.h"

uint8_t i2c_addr;

struct ob1203 ob;

void OB1203_init(uint8_t addr)
{
    i2c_addr = addr;
}

void reset(void)
{

    uint8_t reg_data[1];
    reg_data[0] = SW_RESET;
    I2C_w (OB1203_ADDR, REG_MAIN_CTRL_0, reg_data, 1); //send POR code
    delayMicroseconds (10000); //10ms delay while chip reboots
}

uint16_t get_status(void)
{
	uint8_t get_status_readData[2];

	I2C_r(OB1203_ADDR, REG_STATUS_0, get_status_readData, 2);

	return (get_status_readData[0] << 8 | get_status_readData[1]);
}

#ifndef LEAN_OB1203
uint8_t dataIsNew(void) 
{
	int status_reg_vals;
	status_reg_vals = get_status();
	return ((status_reg_vals & 0x0100) >> 8) || (status_reg_vals & 0x0091);
}

uint8_t lsIsNew(void) 
{
	int status_reg_vals;
	status_reg_vals = get_status();
	return ((status_reg_vals & 0x0100) >> 8);
}
uint8_t psIsNew(void)
{
    int status_reg_vals;
    status_reg_vals = get_status();
    return (status_reg_vals & 0x0001);
}


uint8_t tempIsNew(void)
{
    int status_reg_vals;
    status_reg_vals = get_status();
    return (status_reg_vals & 0x0080);
}


uint8_t bioIsNew(void)
{
    int status_reg_vals;
    status_reg_vals = get_status();
    return (status_reg_vals & 0x0010);
}


void setOscTrim(void)
{
    uint8_t writeData[1];
    writeData[0] = osc_trim;
    I2C_w(OB1203_ADDR,REG_OSC_TRIM,writeData,1);
}
#endif

void setMainConfig(struct ob1203 *_ob)
{
	uint8_t writeData[2];
#ifdef LEAN_OB1203
	writeData[0] = LS_SAI_ON | ALS_MODE | LS_OFF; //MAIN_CTRL_0
	writeData[1] = TEMP_OFF | PS_SAI_OFF | _ob->ppg_ps_mode | _ob->ppg_ps_en; //MAIN_CTRL_1
#else
	writeData[0] = ls_sai | ls_mode | ls_en; //MAIN_CTRL_0
	writeData[1] = temp_en | ps_sai_en | ppg_ps_mode | ppg_ps_en; //MAIN_CTRL_1
#endif
	I2C_w(OB1203_ADDR, REG_MAIN_CTRL_0, writeData, 2);
}

void setIntConfig(struct ob1203 *_ob)
{
	uint8_t writeData[3];
#ifdef LEAN_OB1203
	writeData[0] = LS_INT_SEL_W | LS_THRES_INT_MODE | LS_INT_OFF;
	writeData[1] = _ob->afull_int_en | _ob->ppg_int_en | PS_INT_READ_CLEARS | _ob->ps_int_en;
	writeData[2] = LS_PERSIST(2) | PS_PERSIST(2);
#else
	writeData[0] = _ob->ls_int_sel | _ob->ls_var_mode | _ob->ls_int_en;
	writeData[1] = _ob->afull_int_en | _ob->ppg_int_en | _ob->ps_logic_mode | _ob->ps_int_en;
	writeData[2] = _ob->ls_persist | _ob->ps_persist;
#endif
	I2C_w(OB1203_ADDR, REG_INT_CFG_0, writeData, 3); //default
}
#ifndef LEAN_OB1203
void setLSthresh(struct ob1203 *_ob)
{
    uint8_t writeData[6];
    writeData[0] = (uint8_t) (_ob->ls_thres_hi & 0x000000FF);
    writeData[1] = (uint8_t) ((_ob->ls_thres_hi & 0x0000FF00)>>8);
    writeData[2] = (uint8_t) ((_ob->ls_thres_hi & 0x00FF0000)>>16);
    writeData[3] = (uint8_t) (_ob->ls_thres_lo & 0x000000FF);
    writeData[4] = (uint8_t) ((_ob->ls_thres_lo & 0x0000FF00)>>8);
    writeData[5] = (uint8_t) ((_ob->ls_thres_lo & 0x00FF0000)>>16);
    I2C_w(OB1203_ADDR,REG_LS_THRES_HI,writeData,6); //default
}
#endif

void setPSthresh(struct ob1203 *_ob)
{
	uint8_t writeData[4];
	writeData[0] = (uint8_t) (_ob->ps_thres_hi & 0x000FF);
	writeData[1] = (uint8_t) ((_ob->ps_thres_hi & 0xFF00) >> 8);
	writeData[2] = (uint8_t) (_ob->ps_thres_lo & 0x000FF);
	writeData[3] = (uint8_t) ((_ob->ps_thres_lo & 0xFF00) >> 8);
	I2C_w(OB1203_ADDR, REG_PS_THRES_HI, writeData, 4); //default
}

void setPScurrent(struct ob1203 *_ob)
{
	uint8_t writeData[2];
	writeData[0] = (uint8_t) (_ob->ps_current & 0x00FF);
	writeData[1] = (uint8_t) ((_ob->ps_current & 0xFF00) >> 8);
	I2C_w(OB1203_ADDR, REG_PS_LED_CURR, writeData, 2);
}

void setPPGcurrent(struct ob1203 *_ob)
{
	uint8_t writeData[4];
	writeData[0] = (uint8_t) (_ob->ir_current & 0x00FF);
	writeData[1] = (uint8_t) ((_ob->ir_current & 0xFF00) >> 8);
	writeData[2] = (uint8_t) (_ob->r_current & 0x00FF);
	writeData[3] = (uint8_t) ((_ob->r_current & 0xFF00) >> 8);
	I2C_w(OB1203_ADDR, REG_PPG_IRLED_CURR, writeData, 4);
}

void setPPG_PSgain_cfg(struct ob1203 *_ob)
{
	uint8_t writeData[2];
	writeData[0] = _ob->ppg_ps_gain<<4 | PPG_LED_SETTLING(2) | PPG_ALC_TRACK(2);
#ifdef LEAN_OB1203
	writeData[1] = PPG_POW_SAVE_OFF | LED_FLIP_OFF | SIGNAL_OUT | DIFF_ON
			| ALC_ON;
#else
	writeData[1] = _ob->ppg_pow_save | _ob->led_flip | _ob->sig_out | _ob->diff | _ob->alc;
#endif
	I2C_w(OB1203_ADDR, REG_PPG_PS_GAIN, writeData, 2);
}

#ifndef LEAN_OB1203
void setPPGana_can(struct ob1203 *_ob)
{
	uint8_t writeData[1];
	writeData[0] = _ob->ch1_can_ana | _ob->ch2_can_ana;
	I2C_w(OB1203_ADDR, REG_PPG_CAN_ANA, writeData, 1);
}
#endif

void setPPGavg_and_rate(struct ob1203 *_ob)
{
	uint8_t writeData[2];
	writeData[0] = _ob->ppg_avg | 0x0A; //use standard LED max settings
#ifdef LEAN_OB1203
	writeData[1] = PPG_PWIDTH(3) | PPG_FREQ_PRODUCTION<<3 | _ob->ppg_period;

#else
	writeData[1] = _ob->ppg_pwidth | _ob->ppg_freq | _ob->ppg_period;

#endif
	I2C_w(OB1203_ADDR, REG_PPG_AVG, writeData, 2);
}


void setDigTrim(struct ob1203 *_ob)
{
	uint8_t writeData[2];
	writeData[0] = _ob->led1_trim;
	writeData[1] = _ob->led2_trim;
	I2C_w(OB1203_ADDR, REG_DIG_LED1_TRIM, writeData, 2);
}

#ifndef LEAN_OB1203
void setDigitalCan(struct ob1203 *_ob)
{
	uint8_t writeData[2];
	writeData[0] = (uint8_t) (_ob->ps_digital_can & 0x00FF);
	writeData[1] = (uint8_t) ((_ob->ps_digital_can & 0xFF00) >> 8);
	I2C_w(OB1203_ADDR, REG_PS_CAN_DIG, writeData, 2);
}
#endif

void setFifoConfig(struct ob1203 *_ob)
{
	uint8_t writeData[1];
#ifdef LEAN_OB1203
	writeData[0] = FIFO_ROLL_ON | AFULL_ADVANCE_WARNING(0x0C);
#else
	writeData[0] = _ob->fifo_rollover_en | _ob->fifo_afull_advance_warning;
#endif
	I2C_w(OB1203_ADDR, REG_FIFO_CFG, writeData, 1);
}

void resetFIFO(struct ob1203 *_ob)
{
	uint8_t writeData[3];
#ifdef LEAN_OB1203
	writeData[0] = (TEMP_OFF | PS_SAI_OFF | _ob->ppg_ps_mode | 0);
#else
	writeData[0] = (_ob->temp_en | _ob->ps_sai_en | _ob->ppg_ps_mode | 0);
#endif
	I2C_w(OB1203_ADDR, REG_MAIN_CTRL_1, writeData, 1);
	writeData[0] = 0;
	writeData[1] = 0;
	writeData[2] = 0;
	I2C_w(OB1203_ADDR, REG_FIFO_WR_PTR, writeData, 3); //set write, read pointer and overflow to zero--next sample is newest
	get_status(); //clear any interrupt
#ifdef LEAN_OB1203
	writeData[0] = (TEMP_OFF | PS_SAI_OFF | _ob->ppg_ps_mode | _ob->ppg_ps_en);
#else
	writeData[0] = (_ob->emp_en | _ob->ps_sai_en | _ob->ppg_ps_mode | _ob->ppg_ps_en);
#endif
	I2C_w(OB1203_ADDR, REG_MAIN_CTRL_1, writeData, 1); //MAIN_CTRL_1;
}

#ifndef LEAN_OB1203
void init_rgb(struct ob1203 *_ob)
{
	/*Configures ALS/RGB mode. PS and BIO off.
	 Use: set class variables using header declarations. Then call this function.*/
	uint8_t writeData[2];
	I2C_w(OB1203_ADDR, REG_LS_RES_RATE, writeData, 2);
	writeData[0] = _ob->ls_res | _ob->ls_rate; //LS_RES_RATE
	writeData[1] = _ob->ls_gain; //LS_GAIN
	I2C_w(OB1203_ADDR, REG_LS_RES_RATE, writeData, 2);
	setLSthresh(_ob);
	setIntConfig(_ob);
	ppg_ps_en = PPG_PS_OFF;
	ls_en = LS_ON;
	setMainConfig(_ob);
}
#endif

void init_ps(struct ob1203 *_ob)
{
	reset();
	/*Configures PS mode but not thresholds or interrupts. RGB/ALS and BIO off.
	 Use: set class variables using header declarations. Then call this function.*/
	uint8_t writeData[2];
	//PS settings
	setPScurrent(_ob);
#ifdef LEAN_OB1203
	writeData[0] = PS_CAN_ANA_0 | PS_PULSES(3) | 0x02; //default settings, save RAM
	writeData[1] = PS_PWIDTH(1) | PS_RATE(5); //100ms
#else
	writeData[0] = _ob->ps_can_ana | _ob->ps_pulses | 0x02; //PS_CAN_PULSES -->set hidden max current registers to allow higher LED powers
	writeData[1] = _ob->ps_pwidth | _ob->ps_rate; //PS_PWIDTH_RATE
#endif
	I2C_w(OB1203_ADDR, REG_PS_CAN_PULSES, writeData, 2);

#ifndef LEAN_OB1203
	setDigitalCan(_ob); //PS_CAN_DIG#
#endif
	//set PS moving average and hysteresis

#ifndef LEAN_OB1203 //default settings anyway
	writeData[0] = _ob->ps_avg_en | _ob->ps_hys_level; //PS_MOV_AVG_HYS
	I2C_w(OB1203_ADDR, REG_PS_MOV_AVG_HYS, writeData, 1);
	ls_int_en = LS_INT_OFF;
	ls_en = LS_OFF;
#endif
	//set PS interrupt thresholds
	setPSthresh(_ob);
	setPPG_PSgain_cfg(_ob);
	//config PS
	_ob->ppg_ps_en = 1;
	_ob->ppg_ps_mode = PS_MODE;
	setIntConfig(_ob);
//    setLEDTrim();
	setMainConfig(_ob);
}

#ifndef LEAN_OB1203
void init_ps_rgb(struct ob1203 *_ob)
{
	reset();
	uint8_t writeData[2];
	writeData[0] = _ob->ls_res | _ob->ls_rate; //LS_RES_RATE
	writeData[1] = _ob->ls_gain; //LS_GAIN
	I2C_w(OB1203_ADDR, REG_LS_RES_RATE, writeData, 2);
	writeData[0] = _ob->ps_can_ana | _ob->ps_pulses | 0x02; //PS_CAN_PULSES -->set hidden max current registers to allow higher LED powers
	writeData[1] = _ob->ps_pwidth | _ob->ps_rate; //PS_PWIDTH_RATE
	I2C_w(OB1203_ADDR, REG_PS_CAN_PULSES, writeData, 2);
#ifndef LEAN_OB1203
	setDigitalCan(_ob); //PS_CAN_DIG
#endif
	//set PS moving average and hysteresis
	writeData[0] = _ob->ps_avg_en | _ob->ps_hys_level; //PS_MOV_AVG_HYS
	I2C_w(OB1203_ADDR, REG_PS_MOV_AVG_HYS, writeData, 1);
	setIntConfig(_ob);
	setPSthresh(_ob);
	setPScurrent(_ob);
	setLSthresh(_ob);
	_ob->ls_en = LS_ON;
	_ob->ppg_ps_en = PPG_PS_ON;
	_ob->ppg_ps_mode = PS_MODE;
	//setLEDTrim(_ob);
	setMainConfig(_ob);
}
#endif

void init_hr(struct ob1203 *_ob)
{
	reset();
	_ob->ps_int_en = PS_INT_OFF;
#ifndef LEAN_OB1203
	_ob->ls_en = LS_OFF;
#endif
	setIntConfig(_ob);
	uint8_t readData[1];
	I2C_r(OB1203_ADDR, REG_PS_INT_CFG_1, readData, 1);
	setPPG_PSgain_cfg(_ob);
	setPPGcurrent(_ob);
#ifndef LEAN_OB1203
	setPPGana_can(_ob);
#endif
	setPPGavg_and_rate(_ob);
	setFifoConfig(_ob);
	_ob->ppg_ps_mode = HR_MODE;
	setMainConfig(_ob);
}

void init_spo2(struct ob1203 *_ob)
{
	reset();
	_ob->ps_int_en = PS_INT_OFF;
#ifndef LEAN_OB1203
	ls_en = LS_OFF;
#endif
	setIntConfig(_ob);
	uint8_t readData[1];
	I2C_r(OB1203_ADDR, REG_PS_INT_CFG_1, readData, 1);
	setPPG_PSgain_cfg(_ob);
	setPPGcurrent(_ob);
#ifndef LEAN_OB1203
	setPPGana_can(_ob);
#endif
	setPPGavg_and_rate(_ob);
	setFifoConfig(_ob);
	_ob->ppg_ps_mode = SPO2_MODE;
	setMainConfig(_ob);
}

uint32_t bytes2uint32(uint8_t *data, int start_byte)
{
	//coverts a string of 3 bytes with LSB first into unsigned long MSB last
	return ((uint32_t) (data[start_byte + 2] & (0x3U))) << 16
			| ((uint32_t) data[start_byte + 1]) << 8
			| ((uint32_t) data[start_byte]);

}

#ifndef LEAN_OB1203
uint32_t twoandhalfBytes2uint32(uint8_t *data, int start_byte) 
{
	/* coverts a string of 3 bytes with LSB first into unsigned long MSB last */
	uint32_t out = 0;
	out |= (data[start_byte + 2] & 0x0F);
	out <<= 8;
	out |= (data[start_byte + 1] & 0x0F);
	out <<= 8;
	out |= (data[start_byte + 0] & 0x0F);

	return out;
}

uint8_t get_ls_data(uint32_t *data)
{  
    uint8_t byte_data[21];
    I2C_r(OB1203_ADDR,REG_STATUS_0,byte_data,21);

    //byte_data[0] is ps (not populated)
    data[1] = twoandhalfBytes2uint32(byte_data,4); //w
    data[2] = twoandhalfBytes2uint32(byte_data,7); //g
    data[3] = twoandhalfBytes2uint32(byte_data,10); //b
    data[4] = twoandhalfBytes2uint32(byte_data,13); //r
    data[5] = twoandhalfBytes2uint32(byte_data,16); //c
    data[6] =  (uint32_t)((byte_data[20] & 0x0F)<<8) | (uint32_t)byte_data[19] ; //temp data
    return ( (byte_data[0] & LS_NEW_DATA) == 0x01 ? 1 : 0); //return 1 if new data or 0 if old data
}

uint8_t get_ps_data(uint32_t *data)
{  
    uint8_t byte_data[4];
    I2C_r(OB1203_ADDR,REG_STATUS_0,byte_data,4);


    data[0] = ((uint32_t)byte_data[3])<<8 | ((uint32_t)byte_data[2]); //ps data
    return ( (byte_data[1] & PS_NEW_DATA) == 0x01 ? 1 : 0); //return 1 if new data or 0 if old data
}


uint8_t get_ps_ls_data(uint32_t *data)
{
	uint8_t byte_data[21];
	I2C_r(OB1203_ADDR, REG_STATUS_0, byte_data, 21);

	data[0] = ((uint32_t) byte_data[3]) << 8 | ((uint32_t) byte_data[2]); //ps
	data[1] = twoandhalfBytes2uint32(byte_data, 4); //w
	data[2] = twoandhalfBytes2uint32(byte_data, 7); //g
	data[3] = twoandhalfBytes2uint32(byte_data, 10); //b
	data[4] = twoandhalfBytes2uint32(byte_data, 13); //r
	data[5] = twoandhalfBytes2uint32(byte_data, 16); //c
	data[6] = (uint32_t) ((byte_data[20] & 0x0F) << 8)
			| (uint32_t) byte_data[19]; //temp data
	return ((byte_data[0] & LS_NEW_DATA) == 0x01 ? 1 : 0); //return 1 if new data or 0 if old data
}
#endif

void getFifoInfo(struct ob1203 *_ob,uint8_t *fifo_info)
{
	I2C_r(OB1203_ADDR, REG_FIFO_WR_PTR, fifo_info, 3);
	_ob->writePointer = fifo_info[0];
	_ob->readPointer = fifo_info[1];
	_ob->fifoOverflow = fifo_info[2];
}

void getNumFifoSamplesAvailable(struct ob1203 *_ob,uint8_t *fifo_info, uint8_t *sample_info)
{
	/*sample_info [3] = {numSamplesHR, numSamplesSpO2, overflow*/

	getFifoInfo(_ob,fifo_info);
	uint8_t numSamples = _ob->writePointer;
	if (_ob->writePointer <= _ob->readPointer)
	{
		numSamples += 32;
	}
	numSamples -= _ob->readPointer;
	sample_info[0] = numSamples; //num HR samples
	sample_info[1] = (numSamples >> 1); //num SpO2 samples
	sample_info[2] = fifo_info[2];
}

void getFifoSamples(uint8_t numSamples, uint8_t *fifoData)
{
	I2C_r(OB1203_ADDR, REG_FIFO_DATA, fifoData, 3 * numSamples);
}

void parseFifoSamples(uint8_t numSamples, uint8_t *fifoData,
		uint32_t *assembledData)
{
	for (int n = 0; n < numSamples; n++)
	{
		assembledData[n] = bytes2uint32(fifoData, 3 * n);
	}
}

#ifndef LEAN_OB1203
uint8_t get_part_ID(uint8_t *data) 
{
	I2C_r(OB1203_ADDR, REG_PART_ID, data, 1);
	return data[0];
}
#endif

void do_agc(struct ob1203 *_ob,uint32_t data, uint8_t ch, uint8_t ppg2_mode)
{
	uint32_t tol1 = (uint32_t) TOL1;
	const uint32_t tol2 = (uint32_t) TOL2;
	const uint16_t in_range_persist = IN_RANGE_PERSIST;
	const uint16_t max_current[2] =
	{ IR_MAX_CURRENT, R_MAX_CURRENT };
	const uint16_t step = STEP;
	uint32_t step_change; //how much the counts will change with one step of the current
	if(!ppg2_mode) /*pretend red (unused) is in range*/
	{
		_ob->in_range[1] = IN_RANGE_PERSIST;
		_ob->r_in_range = 1;
	}
	/****************INITIALIZE TARGET COUNTS************************************/
	if (_ob->target_counts[ch] == 0)
	{ /*range uninitialized*/
		_ob->target_counts[ch] = TARGET_COUNTS;
		_ob->target_change = 1;
	}

	/*********************Set tolerance*****************************************/
	if (_ob->target_counts[ch] == 0xC000UL)
	{
		tol1 = 0x5800UL; //allow the signal to be +/=22k from from target counts in the lowest range
	}
	else
	{ //set the tolerance to be the step size at the current target counts
		step_change = (uint32_t) STEP * _ob->target_counts[ch]
				/ (uint32_t) (ch ? _ob->r_current : _ob->ir_current);
		tol1 = (uint32_t) TOL1 > step_change ? (uint32_t) TOL1 : step_change;
		if (tol1 > 0x5800UL)
			tol1 = 0x5800UL; //constrain
	}

	/***********************TOO LOW CASE*************************************/

	if (data
			< _ob->target_counts[ch]
					- ((_ob->in_range[ch] > in_range_persist) ? (tol2) : (tol1)))
	{

		if ((data < _ob->target_counts[ch] - tol2))
		{
			_ob->in_range[ch] = 0;
		}

		if ((ch ? _ob->r_current : _ob->ir_current) + step < max_current[ch])
		{
			if (ch)
			{
				_ob->r_current += step;
			}
			else
			{
				_ob->ir_current += step;
			}
			_ob->update_current = 1;
		}
		else
		{ /*Current is close to max.*/
			if (((uint32_t) 9 * _ob->target_counts[ch])
					/ (uint32_t) 10 > (uint32_t)MIN_TARGET)
			{ //decrease target, if possible
				_ob->target_counts[ch] = ((uint32_t) 9 * _ob->target_counts[ch])
						/ (uint32_t) 10;
				_ob->target_change = 1;
			}
#ifdef GAIN_CHANGE_ALLOWED
			else if (ch == 1 && _ob->target_counts[ch] == (uint32_t) MIN_TARGET)
			{ //if red channel is already at minimum target-->increase the ADC gain (last resort)
				if (_ob->ppg_ps_gain < MAX_PPG_GAIN)
				{
					_ob->ppg_ps_gain++;
					_ob->ppg_gain_change = 1;
					_ob->in_range[0] = 0; //both channels out of range if gain was changed
					_ob->in_range[1] = 0;
				}
			}
#endif
			else
			{
				_ob->target_counts[ch] = (uint32_t) MIN_TARGET;
				_ob->target_change = 1;
			}
			_ob->in_range[ch] = 0;
		}
	}
	/*********************************TOO HIGH CASE********************************************/
	else if (data
			> _ob->target_counts[ch]
					+ ((_ob->in_range[ch] > in_range_persist) ? tol2 : tol1))
	{

		if (data > (_ob->target_counts[ch] + tol2))
		{
			_ob->in_range[ch] = 0;
		}
#ifdef GAIN_CHANGE_ALLOWED
		if (ch)
		{ //only red channel can control the ADC gain
		  //check to see if we can decrease the ADC gain. This will keep our LED as high as possible. Some hysteresis is needed as analog gains are not exact.
			if (_ob->ppg_ps_gain > 0)
			{
				if ((_ob->ppg_ps_gain == 1)
						&& (data
								> (uint32_t) 15 * ((uint32_t) MIN_TARGET)
										/ (uint32_t) 8))
				{//greater than 1.875 times minimum target (nominal gain is 1.5x)
					_ob->ppg_ps_gain--;
					_ob->ppg_gain_change = 1;
				}
				else if ((_ob->ppg_ps_gain == 2)
						&& (data
								> (uint32_t) 3 * ((uint32_t) MIN_TARGET)
										/ (uint32_t) 2))
				{ //greater than 1.5 times minimum target counts (nominal gain is 2x or 1.3x above previous gain)
					_ob->ppg_ps_gain--;
					_ob->ppg_gain_change = 1;
				}
				else if ((_ob->ppg_ps_gain == 3)
						&& (data
								> (uint32_t) 5 * ((uint32_t) MIN_TARGET)
										/ (uint32_t) 2))
				{ //greater than 2x min target counts (nominal gain is 4x or 2x above previous gain)
					_ob->ppg_ps_gain--;
					_ob->ppg_gain_change = 1;
				}
			}
			if (_ob->ppg_gain_change == 1)
			{ //if we changed the gains, we are undoubtedly out of range
				_ob->in_range[0] = 0;
				_ob->in_range[1] = 0;
			}
			if (_ob->ppg_gain_change == 0)
			{ //if we didn't mess with the gain
				//check to see if we can increase the target counts
				if (((ch ? _ob->r_current : _ob->ir_current) * (uint16_t) 9 / (uint16_t) 8
						< max_current[ch])
						&& (_ob->target_counts[ch] * (uint32_t) 10 / (uint32_t) 9
								< (uint32_t) MAX_TARGET))
				{ //if current isn't saturated (with nonlinearity buffer and if our target counts are lower than normal
					_ob->target_counts[ch] = 10 * _ob->target_counts[ch] / 9;
					_ob->target_change = 1;
				}
				else
				{ //mess with the current
					if ((ch ?_ob->r_current : _ob->ir_current) > ((uint16_t) 2) * step)
					{
						if (ch)
						{
							_ob->r_current -= step;
						}
						else
						{
							_ob->ir_current -= step;
						}
						_ob->update_current = 1;
					}
				}
			}
		}
		else
		{ //ir channel doesn't affect ADC gain
#endif
			//check to see if we can increase the target counts
			if (((ch ? _ob->r_current : _ob->ir_current) * (uint16_t) 9 / (uint16_t) 8
					< max_current[ch])
					&& (_ob->target_counts[ch] * (uint32_t) 10 / (uint32_t) 9
							< (uint32_t) MAX_TARGET))
			{ //if current isn't saturated (with nonlinearity margin) and if our target counts are lower than normal
				_ob->target_counts[ch] = (uint32_t) 10 * _ob->target_counts[ch]
						/ (uint32_t) 9;
				_ob->target_change = 1;
			}
			else
			{ //mess with the current
				if ((ch ? _ob->r_current : _ob->ir_current) > ((uint16_t) 2) * step)
				{
					if (ch)
					{
						_ob->r_current -= step;
					}
					else
					{
						_ob->ir_current -= step;
					}
					_ob->update_current = 1;
				}
			}
#ifdef GAIN_CHANGE_ALLOWED
		}
#endif
	}
	/**************************************************************************/
	//JUST RIGHT
	else
	{
		if (((data > (_ob->target_counts[ch] - (tol1)))
				&& (data < (_ob->target_counts[ch] + (tol1)))))
		{
			if (_ob->in_range[ch] <= in_range_persist)
			{
				_ob->in_range[ch]++;
			}
		}
	}

	/****************************************************************************/
	if (_ob->in_range[ch] > in_range_persist)
	{
		if (ch)
		{
			_ob->r_in_range = 1;
		}
		else
		{
			_ob->ir_in_range = 1;
		}
	}
	else
	{
		if (ch)
		{
			_ob->r_in_range = 0;
		}
		else
		{
			_ob->ir_in_range = 0;
		}
	}
	if (_ob->prev_in_range && !(_ob->ir_in_range && _ob->r_in_range))
	{
		_ob->prev_in_range = 0;
		_ob->update_fifo_mode = 1;
		_ob->afull_int_en = AFULL_INT_OFF;
		_ob->ppg_int_en = PPG_INT_ON;
	}
	else if (!_ob->prev_in_range && _ob->ir_in_range && _ob->r_in_range)
	{
		_ob->prev_in_range = 1;
		_ob->update_fifo_mode = 1;
		_ob->afull_int_en = AFULL_INT_ON;
		_ob->ppg_int_en = PPG_INT_OFF;
	}
}

void ob1203_struct_init(struct ob1203 *_ob)
{
	_ob->ppg_gain_change = 0;
	_ob->in_range[0] = 0;
	_ob->in_range[1] = 0;
}
