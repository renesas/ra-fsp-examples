/***********************************************************************************************************************
 * File Name    : oximstruct.h
 * Description  : Containd user defined data types used in application
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
#ifndef __OXIMSTRUCT_H__
#define    __OXIMSTRUCT_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include <stddef.h>
#include "bsp_api.h"

/***********STEP 0: Board Version*************************************************************/

#define RL2_EVK /*define this for the gen 2 board OB1203SD_RL2_EVK*/

/************STEP 1: I/O OPTION******************************************************
 Option 1: (DEFAULT) define UPDATE DISPLAY, do not define PRINT_RAW, do not define DATA_FROM_UART at 230,400 baud
 Option 2: (test canned data): define DATA_FROM_UART, do not define PRINT_RAW, do not define UPDATE_DISPLAY
 Option 3: (print raw data): define PRINT_RAW, do not define DATA_FROM_UART, do not define UPDATE_DISPLAY
 */
/* Option 1: Normal operation. Define UPDATE_DISPLAY. Comment out DATA_FROM_UART and PRINT_RAW
 * Option 2: Print RAW data over UART. Define PRINT_RAW. Comment out DATA_FROM_UART and DISPLAY_UPDATE
 * Option 3: Import canned data over serial. Define DATA_FROM_UART. Comment out PRINT_RAW and UPDATE_DISPLAY
 */

#define UPDATE_DISPLAY //turn this off if PRINT RAW samples are being missed because needs more time
#define TIMEOUT_LIMIT 15 //display timeout in seconds
#define PRINT_RAW //display data on PC with SerialPlot app at 230400 baud
#define PRINT_ALG_RESULTS //prints algorithm results along with raw data
#define PRINT_PPG_AND_CURRENT //print LED current in addition to raw data
/*start debugger then power on FTDI and open SerialPlot or TeraTerm at 230400 baud.
 * Then resume operation and place finger on sensor*/

//#define DATA_FROM_UART
#define PPG_AND_CURRENT //define this if you are passing PPG data and IR and R LED current values
#define HANDSHAKE //this sends a single character after each read data to acknowledge receipt (necessary as some PC com port drivers have inconsistent UART timing)
/*receives data from a python script: write_serial_test_data.py
 * and prints algorithm outputs back to the terminal
 * and saves in a file in the script's directory.
 * (be sure to update COM port number for FTDI board in the python script)
 *more instructions in the python script. COMPUTE_RR will be defined automatically if you use this option*/

/************STEP 2: Compute HR only, Compute SP02 and HR only, OR include respiration rate (RR) (~3-5x increase in computation) */
//#define HR_ONLY //turns off red channel AGC, runs PPG1 mode with IR only (reduced power)

//#define HR_ONLY //turns off red channel AGC, runs PPG1 mode with IR only (reduced power)

/* For respiration rate, in the file SPO2.h include the line #define COMPUTE_RR.
 * Comment out #define COMPUTE_RR to run algorithm part 1 every INTERVAL 100 (1 sec) instead of INTERVAL 40 for ~30% computation time savings.
 * Gen 3 alg note: Newest version of the code are not verified for COMPUTER_RR not defined (just leave it defined)
 */
/*********************************************************************************/

/*************STEP 3: Define power vs. SNR option*********************************/

//#define BEST_SNR //max power, best SNR

//#define MID_POWER //2x lower power, 1.2x lower SNR

#define LOW_POWER //4x lower power, 1.4x lower SNR
//#define LOW_POWER_2 //8x lower LED power
//#define LOW_POWER_3 //16x lower LED power

/****AND OPTIONAL Samples per second option*/
/*In Spo2.h define _50sps to get 50 samples per second instead of 100 sps -- reduces computation, at constant LED current. Accurate up to 160 BPM. Above that not as accurate.*/

/*********************************************************************************/

/*************STEP 4: Select display mode*****************************************/
#define SHOW_PPG_WAVE //Show PPG waveform or comment this out to view a blinky dot for the heart rate
//This increases the MCU on time/power by ~1.5-2x.
/*********************************************************************************/

/*************STEP 5: Select strictness*****************************************/
#define CHECK_PI //Check whether PI values are large enough for reliable data. Comment out to show any value.
/*********************************************************************************/

/*************STEP 6: Enable/disable SPO2 debugging****************************************
 *In Spo2.h define SPO2_DEBUG in order to get SPO2 values that aren't clipped to <100% and >min spo2 %.
 *This is useful for calibration.
 *********************************************************************************/

/*************STEP 7: Enable/disable menu operation [work in progress]*****************/
//#define USE_MENU

/*************STEP 8: Auto power off. For systems which need to turn themselves off after a timeout (not enabled on RL2_EVK EVK)*/
#ifndef RL2_EVK
	#define POWER_OFF_TIMEOUT
#endif

/*************************************************************************************/

/*************STEP 8: Debugging*******************************************************/
//#define DEBUG // uncomment to disable the power off idle timer
//#define ten_x_pi //for higher resolution in PI multiple display value by 10
/*************************************************************************************/

#ifdef DATA_FROM_UART
	#ifndef COMPUTE_RR
		#define COMPUTE_RR //define this automatically--script runs on 40 samples at a time
	#endif

	#ifdef PRINT_RAW
		#undef PRINT_RAW //can't print raw during data from UART
	#endif

	#ifdef PRINT_PPG_AND_CURRENT
		#undef PRINT_PPG_AND_CURRENT
	#endif

	#ifdef UPDATE_DISPLAY
		#undef UPDATE_DISPLAY //can't do this and data from UART at same time
	#endif
#else
#ifdef PRINT_RAW
		#ifdef PPG_AND_CURRENT
			#undef PPG_AND_CURRENT
		#endif
	#endif
#endif

#define LOW_AC_PERSIST (10)
#define DEFAULT_MAX_LOW_SAMPLES (20)
#define PROX_MODE 0 //0 for prox, 1 for bio
#define BIO_MODE 1 //1 for bio mode

typedef struct oxim {
	//variables
	uint8_t sample_log;
	uint16_t status_read;
	uint8_t tx_buffer[64];
	uint16_t num2send;
	volatile unsigned char uart2_busy_flag;
	volatile uint8_t uart2_receive_complete_flag;
	uint32_t latest_ir_data;
	uint32_t latest_r_data;
	int16_t IRdat;
	int16_t Rdat;
	uint8_t do_alg;
	uint8_t samples_processed;
	uint8_t update_display;
	uint8_t update_ppg_wave;
	uint8_t count;
	uint8_t clear;
	uint8_t overflow; //missed sample count is global scope for debugging
	uint8_t writeMain[2];
	uint16_t Change_Block;
	uint8_t fifo_mode;
	float heart_beat;
	uint8_t demo_stop;
	uint8_t display_on;
	uint32_t BIO_THRESHOLD; //Set to MIN_TARGET-2^16. Minimum biosensor ADC count. Below this sensor exits BIO mode and reverts to proximity sensor "wait for finger" mode
	uint16_t MAX_LOW_SAMPLES;
	uint8_t delay_adjust;
	uint8_t ppg2;
	uint8_t red_agc;
	uint8_t ir_agc;
	volatile uint32_t ppgData[16 * 2];

	uint8_t mode; //start in prox mode, then switch to HR mode when we detect proximity
	uint16_t idle_counter;
	volatile uint8_t num_low_samples;

	uint8_t sample_cnt;
	uint8_t reset_alg;
	uint8_t comment[30];
}oxim_t;

#ifdef __cplusplus
}
#endif

#endif
