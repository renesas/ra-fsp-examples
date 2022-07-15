/***********************************************************************************************************************
 * File Name    : oximeter.c
 * Description  : Contains data stucture and function definationas of OB1203 sensor data calculation
 ***********************************************************************************************************************/
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "ob1203_bio/OB1203/OB1203.h"
#include "ob1203_bio/KALMAN/kalman.h"
#include "ob1203_bio/SPO2/SPO2.h"
#include "oximeter.h"
#include "I2C/i2c.h"
#include "stdio.h"
#include "stddef.h"
#include "hal_data.h"
#include "common_utils.h"
#include "user_choice.h"

/******************************************************
Set configurations in oximstruct.h
*******************************************************/
struct oxim ox;

extern kalman_t kalman_filters[NUM_KALMAN_FILTERS];

unsigned char g_adc_completed = 0;
uint8_t mode = PROX_MODE;
uint8_t no_disp_spo2 = 0;
void (*p_IntB_Event)(void) = NULL;
char comment[30];
char tx_buffer[64];
uint8_t UART_Plot = 1;

volatile uint16_t t_timer = 0;
volatile uint8_t samples_ready = 0;
volatile uint8_t just_woke_up = 0;
volatile unsigned char uart2_busy_flag;
volatile uint8_t uart2_receive_complete_flag;
static volatile uint32_t total_time = 0;
static volatile uint16_t t_time = 0;

static bool_t OB_1203_calibrated = false;
static bool_t OB_1203_sensing = false;

/* value output to display */
uint16_t spo2_val;
uint16_t heart_Rate_Val;
uint16_t breathing_rate;
uint16_t r_p2p;


/*******************************************************************************************************************//**
 * @brief       Initialization of oximeter structure
 * @param[in]   _ox      Oximeter structure
 * @retval
 * @retval
 ***********************************************************************************************************************/
static void oxim_struct_init(struct oxim *_ox)
{
    _ox->sample_log = 0;
    _ox->samples_processed = 0;
    _ox->update_display = 0;
    _ox->update_ppg_wave = 0;
    _ox->count = 0;
    _ox->clear = 0;
    _ox->overflow = 0; //missed sample count is global scope for debugging_ox->`
    _ox->Change_Block = 0;
    _ox->fifo_mode = 0;
    _ox->heart_beat = 0;
    _ox->demo_stop = 0;
    _ox->BIO_THRESHOLD = 0x4000; //Set to MIN_TARGET-2^16. Minimum biosensor ADC count. Below this sensor exits BIO mode and reverts to proximity sensor "wait for finger" mode
    _ox->MAX_LOW_SAMPLES = DEFAULT_MAX_LOW_SAMPLES;
    _ox->delay_adjust = 0;
    _ox->mode = PROX_MODE; //start in prox mode, then switch to HR mode when we detect proximity
    _ox->idle_counter = 0;
    _ox->num_low_samples = 0;
    _ox->sample_cnt = 0;
    _ox->reset_alg = 0;
    _ox->mode = 0;
#ifdef HR_ONLY
	_ox->red_agc = 0;
	_ox->ppg2 = 0;
#else
    _ox->red_agc = 1; //default to spo2 mode
    _ox->ppg2 = 1; //default to spo2 mode
#endif
    _ox->ir_agc = 1;

}

/*******************************************************************************************************************//**
 * @brief       Configure OB1203 registers. This function gives visibility into theOB1203 register settings.
 * @param[in]   _ox      Oximeter structure
 * @param[in]   _ob      OB1203 sensor data structure
 * @retval
 * @retval
 ***********************************************************************************************************************/
void defaultConfig(struct oxim *_ox, struct ob1203 *_ob) //populate the default settings here
{

#ifndef LEAN_OB1203
	temp_en = TEMP_OFF; 	//temperature sensor settings (hidden registers)
	ls_en = LS_OFF;
	//PS and PPG settings
	ps_sai_en = PS_SAI_OFF;
	ps_pulses = PS_PULSES(3);
	ps_pwidth = PS_PWIDTH(1);
	ps_rate = PS_RATE(5); //5 = 100ms
	ps_avg_en = PS_AVG_OFF;
	ps_can_ana = PS_CAN_ANA_0;
	ps_digital_can = 0;
	ps_hys_level = 0;
#endif
    if (MEAS_PS)
    {
        _ob->ps_current = 0x100;
    }
    else
    {
        _ob->ps_current = 0x000;
    }
    //    ps_current = 0;
    _ob->ps_thres_hi = PS_THRESH_HI;
    _ob->ps_thres_lo = 0x00;

    //interrupts
#ifndef LEAN_OB1203
	_ob->ls_int_sel = LS_INT_SEL_W;
	_ob->ls_var_mode = LS_THRES_INT_MODE;
	_ob->ls_int_en = LS_INT_OFF;
	_ob->ls_persist = LS_PERSIST(2);
	_ob->ps_persist = PS_PERSIST(2);
	_ob->ps_logic_mode = PS_INT_READ_CLEARS;
	_ob->ppg_pwidth = PPG_PWIDTH(3);				//PPG_PWIDTH(3);
#endif
    _ob->ppg_ps_en = PPG_PS_ON;
    _ob->ps_int_en = PS_INT_OFF; //turn it on later after display boot.
    //BIO SETTINGS
    //int
    _ob->afull_int_en = AFULL_INT_OFF;
    _ob->ppg_int_en = PPG_INT_ON;
    //PPG
    _ob->ir_current = 0x2FF; //max 1023. 3FF was 1AF
    if (_ox->ppg2)
    {
        _ob->r_current = 0x1AF; //max 511. 1FF was 1AF
    }
    else
    {
        _ob->r_current = 0;
    }
    _ob->ppg_ps_gain = PPG_PS_GAIN_1;
#ifndef LEAN_OB1203
	_ob->ppg_pow_save = PPG_POW_SAVE_OFF;
	_ob->led_flip = LED_FLIP_OFF;
	_ob->ch1_can_ana = PPG_CH1_CAN(0);
	_ob->ch2_can_ana = PPG_CH2_CAN(0);
#endif
#ifndef _50sps
#ifdef BEST_SNR
	_ob->ppg_period = PPG_PERIOD(1); //1600 SPS
	_ob->ppg_avg = PPG_AVG(4);	//16 averages to 100 SPS output data rate for IR and R
	#endif
#ifdef MID_POWER
	_ob->ppg_period = PPG_PERIOD(3); //800 SPS
	_ob->ppg_avg = PPG_AVG(3);//8 averages to 100 SPS output data rate for IR and R
	#endif

#ifdef LOW_POWER
    _ob->ppg_period = PPG_PERIOD(4); //400 SPS
    _ob->ppg_avg = PPG_AVG(2);	//4 averages to 100 SPS output data rate for IR and R
#endif

#ifdef LOW_POWER_2
	_ob->ppg_period = PPG_PERIOD(5); //200 SPS
	_ob->ppg_avg = PPG_AVG(1);	//2 averages to 100 SPS output data rate for IR and R
	#endif

#ifdef LOW_POWER_3
	_ob->ppg_period = PPG_PERIOD(6); //100 SPS
	_ob->ppg_avg = PPG_AVG(0);	//1 averages to 100 SPS output data rate for IR and R
	#endif

#else //_50sps
	#ifdef BEST_SNR
	_ob->ppg_period = PPG_PERIOD(1); //1600 SPS
	_ob->ppg_avg = PPG_AVG(5);	//32 averages to 50 SPS output data rate for IR and R
	#endif

	#ifdef MID_POWER
	_ob->ppg_period = PPG_PERIOD(3); //800 SPS
	_ob->ppg_avg = PPG_AVG(4);	//16 averages to 50 SPS output data rate for IR and R
	#endif

	#ifdef LOW_POWER
	_ob->ppg_period = PPG_PERIOD(4); //400 SPS
	_ob->ppg_avg = PPG_AVG(3);	//8 averages to 50 SPS output data rate for IR and R
	#endif

	#ifdef LOW_POWER_2
	_ob->ppg_period = PPG_PERIOD(5); //200 SPS
	_ob->ppg_avg = PPG_AVG(2);	//4 averages to 50 SPS output data rate for IR and R
	#endif

	#ifdef LOW_POWER_3
	_ob->ppg_period = PPG_PERIOD(6); //100 SPS
	_ob->ppg_avg = PPG_AVG(1);	//2 averages to 50 SPS output data rate for IR and R
	#endif

	#ifdef LOW_POWER_4
	_ob->ppg_period = PPG_PERIOD(7); //50 SPS
	_ob->ppg_avg = PPG_AVG(0);	//0 averages to 50 SPS output data rate for IR and R
	#endif

#endif

#ifndef LEAN_OB1203
	_ob->ppg_freq = PPG_FREQ_PRODUCTION;			//sets the data collection rate to multiples of 50Hz.
	_ob->bio_trim = 3;							//max 3 --this dims the ADC sensitivity, but reduces noise
	_ob->led_trim = 0x00;						//can use to overwrite trim setting and max out the current
	_ob->ppg_LED_settling = PPG_LED_SETTLING(2); //hidden register for adjusting LED setting time (not a factor for noise)
	_ob->ppg_ALC_track = PPG_ALC_TRACK(2);		//hidden register for adjusting ALC track and hold time (not a factor for noise)
	_ob->diff = DIFF_ON;							//hidden register for turning off subtraction of residual ambient light after ALC
	_ob->alc = ALC_ON;							//hidden register for turning off ambient light cancellation track and hold circuit
	_ob->sig_out = SIGNAL_OUT;					//hidden register for selecting ambient sample or LED sample if DIFF is off
	_ob->fifo_rollover_en = FIFO_ROLL_ON;
	_ob->fifo_afull_advance_warning = AFULL_ADVANCE_WARNING(0x0C); //balance early warning versus large sample count
#endif
    //run initialization according to user compile settings
    uint8_t reg_data[2];
    I2C_r (OB1203_ADDR, REG_DIG_LED1_TRIM, reg_data, 2);
    _ob->led1_orig_trim = reg_data[0];
    _ob->led2_orig_trim = reg_data[1];

    if (_ox->mode == BIO_MODE)
    {
        _ox->ppg2 ? init_spo2 (_ob) : init_hr (_ob);
    }
    else
    {
        _ob->ppg_int_en = PPG_INT_OFF;
        init_ps (_ob);
    }
}

/*******************************************************************************************************************//**
 * @brief       Runs after a low signal has been detected and resets relevant parameters in main
 * @param[in]   _ox      Oximeter structure
 * @retval
 * @retval
 ***********************************************************************************************************************/
void reset_oximeter_params(struct oxim *_ox)
{
    _ox->samples_processed = 0;
    _ox->sample_cnt = 0;
    _ox->do_alg = 0;
    _ox->update_display = 0;
    _ox->update_ppg_wave = 0;
    _ox->sample_log = 0;
    _ox->heart_beat = 0;
}

/*******************************************************************************************************************//**
 * @brief       Runs after a low signal has been detected and resets relevant parameters in main
 * @param[in]   _ox      Oximeter structure
 * @retval
 * @retval
 ***********************************************************************************************************************/
void check_for_alg_reset(struct oxim *_ox, struct ob1203 *_ob, struct spo2 *_sp)
{
    if (_ox->reset_alg == 1)
    {
        SPO2_init (_ox, _ob, _sp);
        reset_oximeter_params (_ox);
        _sp->avg_PI = get_avg_PI (0, &_sp->pi_rms_val);
        _ox->reset_alg = 0;
    }
}

/*******************************************************************************************************************//**
 * @brief      Get the status of sensor sensing data
 * @param[in]   p_data      OB1203 sensor status data structure
 * @retval
 * @retval
 ***********************************************************************************************************************/
bool_t R_OB1203_get_sensing_status(st_sensorsOB_t *p_data)
{
    p_data->ob_spo2 = spo2_val;
    p_data->ob_hr = heart_Rate_Val;
    p_data->ob_rr = breathing_rate;
    p_data->ob_pi = 0.0;

    if ((OB_1203_calibrated == true) & (heart_Rate_Val != 0))
    {
        OB_1203_calibrated = true;
        OB_1203_sensing = false;
    }
    else
    {
        heart_Rate_Val = 0;
    }

    p_data->calibrated = OB_1203_calibrated;
    p_data->sensing = OB_1203_sensing;
    return (true);
}

/*******************************************************************************************************************//**
 * @brief      Reads data from the OB1203 prox register or PPG FIFO data
 * @param[in]   _ox      oximeter sensor data structure
 * @param[in]   _ob      OB1203 sensor status data structure
 * @param[in]   _sp      SPO2 data structure
 * @retval
 * @retval
 ***********************************************************************************************************************/
void get_sensor_data(struct oxim *_ox, struct ob1203 *_ob, struct spo2 *_sp)
{
    uint8_t maxSamples2Read;
    uint8_t total_samples;
    if (_ox->ppg2)
    {
        /*FIFO samples, e.g. 4 samples * 3 bytes = 12 bytes (or 2 SpO2 samples) 16 samples is the entire SpO2 buffer.*/
        maxSamples2Read = 10;
    }
    else
    {
        /*in HR mode we collect twice as many samples and get AFUL interrupts half as often*/
        maxSamples2Read = 20;
    }
    uint8_t fifoBuffer[16 * 6];
    uint8_t sample_info[3];
    uint8_t samples2Read = 0;
    uint8_t fifo_reg_data[3];
    uint8_t do_reset_fifo = 0;
    if (_ob->afull_int_en)
    { /*FIFO mode--find out how many samples in buffer*/
        /*read the samples fifo registers and figure out how many samples are left*/
        getNumFifoSamplesAvailable (_ob, fifo_reg_data, sample_info);
        if (_ox->ppg2)
        {
            /*limit the number of samples to the maximum*/
            samples2Read = (sample_info[1] > maxSamples2Read) ? maxSamples2Read : sample_info[1];
        }
        else
        {
            /*limit the number of samples to the maximum and makes sure it is even number*/
            samples2Read =
                    (uint8_t) ((sample_info[1] << (uint8_t) 1) > (uint8_t) maxSamples2Read) ? (uint8_t) maxSamples2Read :
                                                                                              (uint8_t) (sample_info[1]
                                                                                                      << (uint8_t) 1);
        }
        _ox->overflow = sample_info[2];
    }
    else
    {
        /*read one sample*/
        samples2Read = 1;
        _ox->overflow = 0;
    }
    if (_ox->ppg2)
    {
        total_samples = (uint8_t) (samples2Read << (uint8_t) 1);
    }
    else
    {
        total_samples = samples2Read;
    }
    getFifoSamples (total_samples, fifoBuffer);
    parseFifoSamples (total_samples, fifoBuffer, (uint32_t*) _ox->ppgData);

    _ox->latest_ir_data = _ox->ppgData[0];
    if (_ox->ppg2)
    {

        _ox->latest_r_data = _ox->ppgData[1];
    }
    else
    {
        _ox->latest_r_data = 0;
    }

    if (_ob->ir_in_range && ((_ox->ppg2 && _ob->r_in_range) || (_ox->ppg2 == 0))) /*IR in range, and if spo2 mode then R must be in range too*/
    {
        /*Fill in missing samples from FIFO overflow (doesn't normally run)*/
        /*SPO2 mode*/
        if (_ox->ppg2)
        {
            for (int n = 0; n < (_ox->overflow >> 1); n++)
            {
                {
                    add_sample (_ob, _sp, _ox->ppgData[0], _ox->ppgData[1]); /*duplicate oldest data to deal with missing (overwritten) samples*/
                }
                if (_sp->sample_count < ARRAY_LENGTH)
                    /*number of samples in buffer*/
                    _sp->sample_count++;
                _sp->missed_sample_count++;
            }
        }
        /*HR only mode*/
        else
        {
            for (int n = 0; n < _ox->overflow; n++)
            {
                /*duplicate oldest data to deal with missing (overwritten) samples*/
                add_sample (_ob, _sp, _ox->ppgData[0], 0);
                if (_sp->sample_count < ARRAY_LENGTH)
                    /*number of samples in buffer*/
                    _sp->sample_count++;
                _sp->missed_sample_count++;
            }
        }

        /*Load collected samples (normally runs)*/
        for (int n = 0; n < samples2Read; n++)
        {/*add samples*/
            if (_ox->ppg2)
            {
                add_sample (_ob, _sp, _ox->ppgData[2 * n], _ox->ppgData[2 * n + 1]); /*add data to sample buffer when data is in range*/
            }
            else
            {
                add_sample (_ob, _sp, _ox->ppgData[n], 0); /*add data to sample buffer when data is in range*/
            }
            if (_sp->sample_count < ARRAY_LENGTH)
                _sp->sample_count++; /*number of samples in buffer*/
            _sp->read_sample_count++;
        }
    }
    else
    {
        _sp->sample_count = 0;
    }

    if ((_ox->latest_r_data > 8000) && (_ox->delay_adjust < 10))
        _ox->delay_adjust++;

    if (_ox->delay_adjust == 10 || (_ox->ppg2 == 0))
    { /*red LED can heat up so let it warm up and droop before AGC, and allow regardless if ppg1 mode*/
        if (_ox->ppg2 && _ox->red_agc)
        {/* R AGC case */
            do_agc (_ob, _ox->ppgData[2 * (samples2Read - 1) + 1], 1, _ox->ppg2); /*use the most recent sample in the FIFO*/
        }

        if (_ox->ir_agc)
        {/* IR AGC case*/
            if (_ox->ppg2)
            {
                do_agc (_ob, _ox->ppgData[2 * (samples2Read - 1)], 0, _ox->ppg2); /*use the most recent sample in the FIFO*/
            }
            else
            {
                do_agc (_ob, _ox->ppgData[samples2Read - 1], 0, _ox->ppg2); /*use the most recent sample in the FIFO*/
            }
        }
    }

    if (_ob->update_fifo_mode || _ob->update_current)
    {
        do_reset_fifo = 1;
    }
    if (_ob->update_current)
    {
        if (_ox->fifo_mode)
        {
            _ox->reset_alg = 1;
        }
    }
    if (_ob->target_change)
    {
        if ((_ob->ir_current != IR_START_CURRENT) || ((_ox->ppg2 == 1) && (_ob->r_current != R_START_CURRENT)))
        { /*if this isn't the initial startup*/
            if (_ox->fifo_mode)
            { /*if we are logging data*/
                _ox->reset_alg = 1;
            }
        }
        _ob->target_change = 0;
    }
    if (_ob->update_fifo_mode)
    {
        setIntConfig (_ob);
        _ob->update_fifo_mode = 0;
        if (_ob->ppg_int_en == 0)
        { /*not in fifo mode -- switching to collecting data*/
            _ox->fifo_mode = 1;
        }
        else
        {
            _ox->fifo_mode = 0;
        }
    }
    if (_ob->update_current)
    {
        setPPGcurrent (_ob);
        _ob->update_current = 0;
    }
    if (_ob->ppg_gain_change)
    {
        setPPG_PSgain_cfg (_ob);
        _ob->ppg_gain_change = 0;
    }
    if (do_reset_fifo)
    { //reset the FIFO unless we are going to reset
#ifndef UPDATE_DISPLAY
		resetFIFO(_ob);
#else
        if (!_ox->reset_alg)
        { //no need to reset if we are going to reset the algorithm--it does this already
            resetFIFO (_ob);
        }
#endif
        do_reset_fifo = 0;
    }
    /*switch modes if low signal*/
    uint8_t last_ir_sample_index = (_ox->ppg2 == 1) ? (uint8_t) (2 * (samples2Read - 1)) : (uint8_t) (samples2Read - 1);
    if (_ox->ppgData[last_ir_sample_index] < _ox->BIO_THRESHOLD)
    { // counts low samples when num_low_samples exceeds max low samples mode will switch to prox mode
        _ox->num_low_samples++;
    }
    else
    {
        _ox->num_low_samples = 0;
    }
    // here the prox sensor starts / stops the measurement
    if (_ox->num_low_samples >= _ox->MAX_LOW_SAMPLES)
    {
        _ox->mode = PROX_MODE;
        IRQ_Disable ();
        p_IntB_Event = &proxEvent; //set an interrupt on the falling edge
        IRQ_Enable ();
        switch_mode (_ox, _ob, _sp, _ox->mode);
        just_woke_up = 0;
        OB_1203_calibrated = false;
        OB_1203_sensing = false;
    }
    samples_ready = 0;
} //end get_sensor_data

/*******************************************************************************************************************//**
 * @brief       Timer callback
 * @param[in]   p_args      Callback event parameter
 * @retval
 * @retval
 ***********************************************************************************************************************/
void t_callback(timer_callback_args_t *p_args)
{
    /* Called periodically from ISR of timing unit */
    if (p_args->event == TIMER_EVENT_CYCLE_END)
    {
        t_time++;
        total_time++;
        t_timer++;
    }
}

/*******************************************************************************************************************//**
 * @brief       ISR for OB1203 interrupt in proximity mode
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void proxEvent(void)
{
    just_woke_up = true;
}

/*******************************************************************************************************************//**
 * @brief       ISR for OB1203 interrupt in bio mode
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void dataEvent(void)
{
    samples_ready = true;
}

/*******************************************************************************************************************//**
 * @brief       Disable Interrupt for sensor pin
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void IRQ_Disable(void)
{
    fsp_err_t status = FSP_SUCCESS;
    status = R_ICU_ExternalIrqDisable (&g_sensorIRQ_ctrl);
    if (FSP_SUCCESS != status)
    {
        APP_DBG_PRINT("\r\nR_ICU_ExternalIrqDisable failed ** %u \r\n", status);
        APP_ERR_TRAP(status);
    }
}

/*******************************************************************************************************************//**
 * @brief      Enable Interrupt for sensor pin
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void IRQ_Enable(void)
{
    fsp_err_t status = FSP_SUCCESS;
    status = R_ICU_ExternalIrqEnable (&g_sensorIRQ_ctrl);
    if (FSP_SUCCESS != status)
    {
        APP_DBG_PRINT("** R_ICU_ExternalIrqEnable failed ** %u \r\n", status);
        APP_ERR_TRAP(status);
    }
    else
    {
        APP_DBG_PRINT("** R_ICU_ExternalIrqEnable success ** %u \r\n", status);
    }
}

void print_ob_data(struct oxim *_ox, struct spo2 *_sp)
{
    if ((UART_Plot) && (_ox->latest_ir_data > 8000))
    {   //wait until not busy
        memset (tx_buffer, 0, sizeof(_ox->tx_buffer)); //zero the send buffer
        if (_sp->display_spo2 == 0)
        {
            sprintf (tx_buffer, "%ld" ", " "SpO2 HR  RR  PI   R\r\n", total_time);
            sprintf (comment, "\r\n");
        }
        else
        {
            /*if (ir_peak_norm >= MIN_RMS)*/
            sprintf (tx_buffer, "%d" ", " "%d" "," "%u" "," "%.3f", (uint16_t) (round_dec_spo2 (_sp->display_spo2)),
                     _sp->display_hr, _sp->br, _sp->R);
            spo2_val = (uint16_t) (_sp->display_spo2 / 10);
            heart_Rate_Val = _sp->display_hr;
            breathing_rate = _sp->br;
            r_p2p = (uint16_t) _sp->R;

#ifdef _OB1203_SENSOR_ENABLE_
            APP_PRINT("\r\n SPO2 : %d  ", spo2_val);
            APP_PRINT(" Heart rate : %d  ", heart_Rate_Val);
            APP_PRINT(" breath rate : %d  ", breathing_rate);
            APP_PRINT("perfusion_index : %d \r\n ",r_p2p);
#endif
        }

#ifndef CHECK_PI
        sprintf(comment, "\r\n");
#endif

    }
}

/*******************************************************************************************************************//**
 * @brief   Changes OB1203 from low power proximity sensor mode to PPG sensing bio mode
 * @param[in]  _ox          Pointer to oximeter sensor data structure
 * @param[in]  _ob          Pointer to OB1203 status data structure
 * @param[in]  _sp          Pointer to SPO2 data structure
 * @param[in] prox_bio_mode Bio mode
 * @retval
 ***********************************************************************************************************************/
void switch_mode(struct oxim *_ox, struct ob1203 *_ob, struct spo2 *_sp, uint8_t prox_bio_mode)
{
    if (prox_bio_mode == BIO_MODE)
    { //switch to bio sensor mode
        _ob->afull_int_en = AFULL_INT_OFF;
        _ob->ppg_int_en = PPG_INT_ON;
        _ox->fifo_mode = 0;
        SPO2_init (_ox, _ob, _sp); /*completely reset the algorithm and OB1203 Fifo*/
        //reset LED currents to 75% of maximum
        _ob->ir_current = IR_START_CURRENT;
        if (_ox->ppg2 == 0)
        {
            _ob->r_current = 0;
        }
        else
        {
            _ob->r_current = R_START_CURRENT;
        }
        _ob->target_change = 1;
        _ob->ppg_gain_change = 0;
        if (_ox->ppg2)
        {
            init_spo2 (_ob); /*initialize the OB1203 sensor in spo2 mode (includes a reset command)*/
        }
        else
        {
            init_hr (_ob);
        }
        _ob->target_counts[0] = 0; //reset AGC
        _ob->target_counts[1] = 0;
        _ob->led1_trim = 0x00; //overwrite default prox sensor trim--not used for PPG mode
        _ob->led2_trim = 0x00; //overwrite default prox sensor trim--not used for PPG mode
        setDigTrim (_ob);
        reset_oximeter_params (_ox);
        _ox->num_low_samples = 0;
    }
    else
    { //switch to proximity sensor mode
        _ob->afull_int_en = AFULL_INT_OFF;
        _ob->ppg_int_en = PPG_INT_OFF;
        _ob->ps_int_en = PS_INT_ON;
        _ob->ppg_ps_mode = PS_MODE;
        _ob->ppg_ps_en = PPG_PS_OFF;
        setMainConfig (_ob);
        init_ps (_ob);
        _ob->led1_trim = _ob->led1_orig_trim; //overwrite default prox sensor trim--not used for PPG mode
        _ob->led2_trim = _ob->led2_orig_trim; //overwrite default prox sensor trim--not used for PPG mode
        setDigTrim (_ob);
        _sp->br = 0;
        _sp->perfusion_index[RED] = 0;
        _sp->perfusion_index[IR] = 0;
    }
}

/*******************************************************************************************************************//**
 * @brief   Tidying up function including all the init functions for OB1203, filters,rtc.
 * @param[in]  _ox          Pointer to oximeter sensor data structure
 * @param[in]  _sp          Pointer to SPO2 status data structure
 * @param[in]  _ob          Pointer to OB1203 data structure
 * @retval
 ***********************************************************************************************************************/
void main_init(struct oxim *_ox, struct spo2 *_sp, struct ob1203 *_ob)
{
    oxim_struct_init (_ox);
    ob1203_struct_init (_ob);
    OB1203_init (OB1203_ADDR);
    init_kalman (kalman_filters); //must run before SPO2_init()
    SPO2_init (_ox, _ob, _sp); //run after init_kalman()
    defaultConfig (_ox, _ob); //do the OB1203 configuration now

    p_IntB_Event = &proxEvent; //set interrupt to call proxEvent ISR
    IRQ_Enable ();
    _ob->ps_int_en = PS_INT_OFF;
    setIntConfig (_ob);
    _ob->ppg_ps_en = PPG_PS_OFF;
    setMainConfig (_ob);
    _ob->ps_int_en = PS_INT_ON;
    setIntConfig (_ob);
    uint8_t reg_data[20];
    I2C_r (OB1203_ADDR, 0x2B, reg_data, 3);
    _ob->ppg_ps_en = PPG_PS_ON;
    setMainConfig (_ob);
}

/*******************************************************************************************************************//**
 * @brief   ob1203 spo2 main function
 * @param[in]  _ox          Pointer to oximeter sensor data structure
 * @param[in]  _sp          Pointer to SPO2 status data structure
 * @param[in]  _ob          Pointer to OB1203 data structure
 * @retval
 ***********************************************************************************************************************/
void ob1203_spo2_main(struct oxim *_ox, struct spo2 *_sp, struct ob1203 *_ob)
{
    //main program loop
    if (_ox->mode == PROX_MODE)
    { /*prox mode case*/

        /* put sleep command here and register wake on interrupt */
        xSemaphoreTake(g_ob1203_semaphore, portMAX_DELAY);

        if (just_woke_up)
        {
            _ox->mode = BIO_MODE;
            IRQ_Disable ();
            p_IntB_Event = &dataEvent; //attach interrupt to data events
            IRQ_Enable ();
            switch_mode (_ox, _ob, _sp, _ox->mode); //starts in PPG fast mode
            just_woke_up = 0;
            OB_1203_calibrated = true;
            OB_1203_sensing = true;
        }
    }
    else /*bio_mode*/
    {
        while (_ox->sample_log < INTERVAL)
        {
            if (_ox->mode == BIO_MODE)
            {
                if (samples_ready)
                { //only read data if available (samples_ready is asserted by ISR and cleared by get_sensor_data)
                    get_sensor_data (_ox, _ob, _sp);
                    _ox->samples_processed = true;
                    _ox->sample_log = (uint8_t) (_ox->sample_log + 10);
                }
            }

            else
            { //not bio mode
                break; //exit loop and go to sleep
            }

            /***********Run algorithm************************/

            if ((_ox->do_alg == 1) && _ox->fifo_mode && (_ox->samples_processed == true))
            {
                do_algorithm (_ox, _ob, _sp); /*changes do_alg to 2 so it doesn't run again until all samples have been collected*/
                print_ob_data(_ox,_sp);
                _ox->samples_processed = (uint8_t) (1 - _ox->ppg2); /*if spo2 mode collect another round of samples*/
            }

            /***********Collect other samples***********************/
            if (_ox->samples_processed == true)
            {
                check_for_alg_reset (_ox, _ob, _sp);
                if (_ox->sample_log == INTERVAL)
                {
                    _ox->do_alg = 1; /*go back and run the algorithm*/
                    _ox->samples_processed = false;
                }
            }

            // Wait for a new sample ready interrupt
            if (_ox->mode == BIO_MODE)
            {
                // Wait for a new sample ready interrupt
                xSemaphoreTake(g_ob1203_semaphore, portMAX_DELAY);
            }
        } //end sample collection loop
        _ox->sample_log = 0;
    } //end bio mode case
}

