/*
 * ICP10101.c
 *
 *  Created on: Jan 24, 2022
 *      Author: Vivek
 */ 

#include "ICP_10101.h"
#include "RmcI2C.h"
#include "common_utils.h"
#include "sensor_thread.h"

#define ICP_I2C_ID 0x63

#define ICP_CMD_READ_ID   (uint16_t) 0xefc8
#define ICP_CMD_SET_ADDR  0xc595
#define ICP_CMD_READ_OTP  0xc7f7
#define ICP_CMD_MEAS_LP   0x609c
#define ICP_CMD_MEAS_N    0x6825
#define ICP_CMD_MEAS_LN   0x70df
#define ICP_CMD_MEAS_ULN  0x7866

// constants for presure calculation
const float _pcal[3] = { 45000.0, 80000.0, 105000.0 };
const float _lut_lower = 3.5 * 0x100000;    // 1<<20
const float _lut_upper = 11.5 * 0x100000;   // 1<<20
const float _quadr_factor = 1 / 16777216.0;
const float _offst_factor = 2048.0;

#define I2C_TRANSMISSION_IN_PROGRESS        0
#define I2C_TRANSMISSION_COMPLETE           1
#define I2C_TRANSMISSION_ABORTED            2

char Tempf[20];
float Temperature,Pressure,ChPressure;
extern float reference_pressure;

float _scal[4];
uint16_t _raw_t;
uint32_t _raw_p;
float _temperature_C;
float _pressure_Pa;
unsigned long _meas_start;
uint8_t _meas_duration;
bool _data_ready;

volatile uint32_t milliTicks;

/**************************************************************************************
 * @brief  Get ICP_10101 data
 * @param[in] reg
 * @param[in] bytes
 * @param[in]
 * @retval
 **************************************************************************************/

void ICP_10101_get(void){
	if (dataReady()) {
		Temperature = getTemperatureC();
		Pressure =  getPressurePa();
		ChPressure= Pressure-reference_pressure;
		timer_reset();
		measureStart(VERY_ACCURATE);
	}
}


/**************************************************************************************
 * @brief  timer start
 * @param[in]
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/

void timer_start(void){
	g_timer2.p_api->open(g_timer2.p_ctrl, g_timer2.p_cfg);
	g_timer2.p_api->enable(g_timer2.p_ctrl);
	g_timer2.p_api->start(g_timer2.p_ctrl);

}

/**************************************************************************************
 * @brief  begin
 * @param[in]
 * @param[in]
 * @param[in]
 * @retval  True if  sensor is responding, False otherwise
 **************************************************************************************/

uint8_t begin(void){
	timer_start();
	// verify that the sensor is repsonding
	if (!isConnected()) {
		return false;
	}
	// read sensor calibration data
	uint8_t addr_otp_cmd[5] = {
			(ICP_CMD_SET_ADDR >> 8) & 0xff,
			ICP_CMD_SET_ADDR & 0xff,
			0x00, 0x66, 0x9c };
	uint8_t otp_buf[3];
	_sendCommands(addr_otp_cmd, 5);
	for (int i=0; i<4; i++) {
		_sendCommand(ICP_CMD_READ_OTP);
		_readResponse(otp_buf, 3);
		_scal[i] = (float) ((otp_buf[0] << 8) | otp_buf[1]);
	}
	return true;
}

/**************************************************************************************
 * @brief  isConnected
 * @param[in]
 * @param[in]
 * @param[in]
 * @retval  True if  sensor is connected , False otherwise
 **************************************************************************************/

bool isConnected(void){
	uint8_t id_buf[2];
	uint16_t id;
	_sendCommand(ICP_CMD_READ_ID);
	_readResponse(id_buf, 2);
	id = (uint16_t)(((uint16_t)(id_buf[Marco_Zero]) << 8) | id_buf[Marco_One]);
	if ((id & 0x03f) == 0x8)
        {
		return true;
	}
        else
        {
		return false;
	}
}

/**************************************************************************************
 * @brief  measure
 * @param[in]mode enumeration
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/

void measure(mmode mode){
	_delay(measureStart(mode));
	while (!dataReady());
}

/**************************************************************************************
 * @brief  Measure Start
 * @param[in]mode enumeration
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/

uint8_t measureStart(mmode mode){
	uint16_t cmd;
	switch (mode)
        {
	case FAST:
		cmd = ICP_CMD_MEAS_LP;
		_meas_duration = 3;
		break;
	case ACCURATE:
		cmd = ICP_CMD_MEAS_LN;
		_meas_duration = 24;
		break;
	case VERY_ACCURATE:
		cmd = ICP_CMD_MEAS_ULN;
		_meas_duration = 95;
		break;
	case NORMAL:
	default:
		cmd = ICP_CMD_MEAS_N;
		_meas_duration = 7;
		break;
	}
	_sendCommand(cmd);
	_data_ready = false;
	_meas_start = GetMilliTick();
	return _meas_duration;
}

/**************************************************************************************
 * @brief   Check if Data Ready
 * @param[in]
 * @param[in]
 * @param[in]
 * @retval  True if Data is Ready ,False Otherwise
 **************************************************************************************/

bool dataReady(void) {
	if (_data_ready)
		return true;
	uint32_t MilliTick;
	MilliTick = GetMilliTick();
	if (MilliTick - _meas_start < _meas_duration)
		return false;

	uint8_t res_buf[9];
	_readResponse(res_buf, 9);
	_raw_t = (uint16_t)(((res_buf[0]) << 8) | res_buf[1]);
	uint32_t L_res_buf3 = res_buf[3];   // expand result bytes to 32bit to fix issues on 8-bit MCUs
	uint32_t L_res_buf4 = res_buf[4];
	uint32_t L_res_buf6 = res_buf[6];
	_raw_p = (L_res_buf3 << 16) | (L_res_buf4 << 8) | L_res_buf6;
	_calculate();
	_data_ready = true;
	return true;
}

/**************************************************************************************
 * @brief   Get Temperature Degree Celsius
 * @param[in]
 * @param[in]
 * @param[in]
 * @retval  Temperature float Value
 **************************************************************************************/

float getTemperatureC(void) {
	return _temperature_C;
}

/**************************************************************************************
 * @brief   Get Temperature Degree  Fahrenheit
 * @param[in]
 * @param[in]
 * @param[in]
 * @retval  Temperature float Value
 **************************************************************************************/

float getTemperatureF(void) {
	return (float)(_temperature_C * 1.8 + 32);
}

/**************************************************************************************
 * @brief   Get Pressure in Pascal
 * @param[in]
 * @param[in]
 * @param[in]
 * @retval  Pressure float Value
 **************************************************************************************/

float getPressurePa(void) {
	return _pressure_Pa;
}

/**************************************************************************************
 * @brief   Calculate Temperature and Pressure
 * @param[in]
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/

void _calculate(void) {
	// calculate temperature
	_temperature_C = -45.f + 175.f / 65536.f * _raw_t;

	// calculate pressure
	float t = (float)(_raw_t - 32768);
	float s1 = _lut_lower + (float)(_scal[0] * t * t) * _quadr_factor;
	float s2 = _offst_factor * _scal[3] + (float)(_scal[1] * t * t) * _quadr_factor;
	float s3 = _lut_upper + (float)(_scal[2] * t * t) * _quadr_factor;
	float c = (s1 * s2 * (_pcal[0] - _pcal[1]) +
			s2 * s3 * (_pcal[1] - _pcal[2]) +
			s3 * s1 * (_pcal[2] - _pcal[0])) /
					(s3 * (_pcal[0] - _pcal[1]) +
							s1 * (_pcal[1] - _pcal[2]) +
							s2 * (_pcal[2] - _pcal[0]));
	float a = (_pcal[0] * s1 - _pcal[1] * s2 - (_pcal[1] - _pcal[0]) * c) / (s1 - s2);
	float b = (_pcal[0] - a) * (s1 + c);
	_pressure_Pa = (float)((a + b / (c + (float)_raw_p)));
}
/**************************************************************************************
 * @brief   Send Command over I2C
 * @param[in]cmd Command
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/
void _sendCommand(uint16_t cmd) {
	uint8_t WriteBuff[2]={0};
	WriteBuff[0] = (uint8_t)((cmd >> 8) & 0xFF);
	WriteBuff[1] = (uint8_t)(cmd & 0xFF);
	ICP_RmCom_I2C_w(WriteBuff, (uint32_t)2);
}

/**************************************************************************************
 * @brief   Send Command over I2C
 * @param[in]cmd_buf pointer to Command
 * @param[in]cmd_len Length
 * @param[in]
 * @retval
 **************************************************************************************/
void _sendCommands(uint8_t *cmd_buf, uint8_t cmd_len){
	ICP_RmCom_I2C_w(cmd_buf, (uint32_t)cmd_len);
}


/**************************************************************************************
 * @brief   Read Response over I2C
 * @param[in]res_buf pointer to Response Buffer
 * @param[in]res_len Length
 * @param[in]
 * @retval
 **************************************************************************************/
void _readResponse(uint8_t *res_buf, uint8_t res_len) {

	ICP_RmCom_I2C_r(res_buf,(uint32_t)res_len);
}

/**************************************************************************************
 * @brief   GetMilliTick
 * @param[in]
 * @param[in]
 * @param[in]
 * @retval  milliTicks milli seconds ticks
 **************************************************************************************/

uint32_t GetMilliTick(void){
	return milliTicks;
}


/**************************************************************************************
 * @brief   TimerCallback
 * @param[in]p_args pointer to structure of callback arguments
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/

void TimerCallback(timer_callback_args_t *p_args){
	if (p_args->event == TIMER_EVENT_CYCLE_END){
		milliTicks++;
	}
}

/**************************************************************************************
 * @brief   Reset Timer
 * @param[in]
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/
void timer_reset(void){
	g_timer2.p_api->stop(g_timer2.p_ctrl);
	g_timer2.p_api->disable(g_timer2.p_ctrl);
	g_timer2.p_api->reset(g_timer2.p_ctrl);
	g_timer2.p_api->close(g_timer2.p_ctrl);
	timer_start();
}


