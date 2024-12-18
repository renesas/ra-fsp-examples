/* Algorithm for heart rate, SpO2 and respiratory rate (RR)
 * See Renesas app note on OB1203 algorithm for details.
 * Compatible with 16 bit processor (minimal float operations) with 32 bit fixed point math capability (e.g. RL78)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "KALMAN/kalman.h"
#include "SPO2/SPO2.h"
#include <math.h>
#ifdef USE_SG
#include "SAVGOL/SAVGOL.h"
#endif

#define LOW_AC_PERSIST (10)

//extern int16_t IRdat; //debug
//extern int16_t Rdat; //debug

//struct spo2 sp; // declare one instance of the spo2 struct

//void spo2_config(void)
//{
//	sp.instance_num = 0;
//}

void spo2_reset(struct spo2 *_sp)
{
    _sp->data_ptr = 0;
    _sp->period2use1f = 0;
    _sp->b2b1f = 0;
    _sp->systole_found = 0;
    _sp->period2use1f = 0;
    _sp->systole = 0;
    _sp->hr_zc = 0;
    _sp->est_hr_period_from_zc1f = 0;
#ifdef USE_SG
    _sp->sg_interval = SG_INT;
    init_savgol(RED, _sp->sg_interval);
    init_savgol(IR, _sp->sg_interval);
    _sp->persist_up = 0;
    _sp->persist_down = 0;
    _sp->num_samples = 0;
#endif
    const uint8_t temp_arr1[] =
        {12, 16, 20, 24, 28};
    const uint8_t temp_arr2[] =
        {19, 25, 32, 38, 45}; // e.g. 1.6 x rr_filter_lens
    _sp->rr_display_num2avg = RR_DISPLAY_INITIAL_NUM2AVG;
    for (int n = 0; n < NUM_RR_FILTERS; n++)
    {
        _sp->rr_filter_lens[n] = temp_arr1[n];
        _sp->rr_filter_spans[n] = temp_arr2[n];
    }
    _sp->do_rr_alg = 0;
    _sp->char_array_iir_initialized = 0;
    _sp->breathing_rate1f = 0;
    // these variables are for counting number of samples collected between runs of the algorithm.
    _sp->sample_count = 0;
    _sp->alg_start_sample_count = 0;
    _sp->prev_alg_start_sample_count = 0;
    _sp->prev_sample_count = 0;
    _sp->prev_missed_samples = 0;
    _sp->read_sample_count = 0;
    _sp->missed_sample_count = 0;
    _sp->hr_data_buffer_ind = 0;
    _sp->current_filter = 0;
    _sp->filter_tracker = 0;
    _sp->rr_current_monitor1f = 0;
    _sp->ok_to_use_pc = 1;
    reset_kalman(zc_filt);
    reset_kalman(hr_filt);
    reset_kalman(spo2_filt);
    reset_kalman(rr_filt);
    reset_kalman(b2b_filt);
    kalman_filters[hr_filt].max_kalman_length = 4;   // start out with a short average on HR and SpO2
    kalman_filters[spo2_filt].max_kalman_length = 4; // start out with a short average on HR and SpO2
//  uart2_busy_flag = 0;
    kalman_filters[b2b_filt].max_outlier_cnt = 1; // temporarily make this smaller in case of a bad initial grab
    _sp->breathing_rate1f = 0;
    _sp->avg_PI = get_avg_PI(0, &_sp->pi_rms_val); // reset PI average
    _sp->rr_buffer_full = 0;
    _sp->alg_count = 0;
    hpf_window_peak_detect_init(_sp); // initialize the variables for window peak detect method
#ifdef UPDATE_DISPLAY
#ifdef SHOW_PPG_WAVE
    _sp->wave_ptr = 0;
#endif
#endif
#ifdef USE_RR_DISPLAY_AVG
    _sp->rr_display_avg_cnt = 0;
    _sp->rr_display_avg_buffer_ind = 0;
#endif
//  resetFIFO(_ob);
#ifdef USE_PVI
    _sp->pvi = 0;
    _sp->pvi_avg = 0;
#endif

    _sp->display_spo2 = 0;
    _sp->perfusion_index[RED] = 0;
    _sp->perfusion_index[IR] = 0;
    _sp->display_hr = 0;
    _sp->br = 0;
    _sp->low_ac_amplitude_cnt = 0;
    _sp->no_disp_spo2_cnt = 0;
    _sp->spo2_err = spo2_err_none;

}

/************************************************************************************
 * Name:       SPO2_init
 * Function:   function for initializing and resetting algorithm parameters
 * 			  call after initializing kalman filters
 * Parameters: none
 * Return:     none
 ************************************************************************************/
void spo2_init(struct spo2 *_sp)
{ /*class initializer*/

    init_kalman(kalman_filters); //must run before SPO2_init()

    spo2_reset(_sp);
}

void hr_and_spo2_calculate(struct spo2 *_sp, uint16_t * p_hr, uint16_t * p_spo2, float * p_pi, spo2_target_counts_t target_counts)
{
    do_algorithm_part1(_sp);
    do_algorithm_part2(_sp, target_counts);
    _sp->avg_PI = get_avg_PI(_sp->perfusion_index[IR], &_sp->pi_rms_val);

    /* Set data */
    *p_hr   = _sp->display_hr;
//    *p_spo2 = _sp->display_spo2;
    *p_spo2 = round_dec_spo2(_sp->display_spo2);
    *p_pi   = _sp->avg_PI;
}

void rr_calculate(struct spo2 *_sp, uint16_t * p_rr)
{
    uint16_t temp_br;

    do_algorithm_part3(_sp);
    do_algorithm_part4(_sp);
    temp_br = (uint16_t)(_sp->breathing_rate1f >> FIXED_BITS);
    _sp->br = temp_br; // no averaging

    /* Set data */
    *p_rr = _sp->br;
}

/************************************************************************************
 * Name:       do_algorithm_part1
 * Function:   first part of SPO2,HR and RR algorithm. Algorithm run time may be longer than the
 *             time between OB1203 data ready interrupts, so algorithm is split into parts to allow getting
 *             data from OB1203 in the middle of the algorithm.
 *             Copies data from circular buffers to linear arrays for filtering.
 *             Performs the FIR filtering for the beat to beat detection algorithm.
 * Parameters: none
 * Return:     none
 ************************************************************************************/
void do_algorithm_part1(struct spo2 *_sp)
{ /*This gets the mean levels, copies AC values into arrays and does DC and slope removal
  variance or RMS calculation for SpO2 if there are enough samples.*/
	_sp->alg_start_sample_count = _sp->read_sample_count;
	_sp->read_samples = _sp->read_sample_count - _sp->prev_sample_count;
	_sp->prev_sample_count = _sp->read_sample_count;
	_sp->missed_samples = _sp->missed_sample_count - _sp->prev_missed_samples;
	// Debug_OLED_display_update((uint16_t) (read_samples), (uint16_t) (missed_samples), 5.0, 0, 0);
	_sp->prev_missed_samples = _sp->missed_sample_count;
	_sp->indx1 = _sp->data_ptr; // capture the current data pointer value
	if (_sp->alg_start_sample_count >= ARRAY_LENGTH + STABILITY_PERIOD)
	{
		//		get_DC(_sp); //calculate DC level
		copy_data(_sp); // copies data to AC1f[n] array (extended precision) and removes DC
	}
}

/************************************************************************************
 * Name:       do_algorithm_part2
 * Function:   second part of SPO2,HR and RR algorithm. Calculates SPO2 from RMS and HR from threshold crossing.
 * Parameters: none
 * Return:     none
 ************************************************************************************/
void do_algorithm_part2(struct spo2 *_sp, spo2_target_counts_t target_counts)
{
	/*This runs the threshold crossing algorithm and does SpO2 calculation and heart rate calculation. The algorithm
	 was split in half to allow a non-RTOS machine to take a break to collect samples
	 from the OB1203 buffer.*/
	uint8_t hr_zc_p; // positive threshold crossings
	uint8_t hr_zc_n; // negative threshold crossings

#ifdef USE_WINDOW
	hpf_window_peak_detect(_sp);
#ifdef USE_WINDOW_RMS
	get_rms_from_sum_abs(_sp);
#endif
	get_dc_from_baseline(_sp, target_counts);
	get_pressure(_sp);
#endif
	if (_sp->alg_start_sample_count >= ARRAY_LENGTH + STABILITY_PERIOD)
	{					  // original algorithm part 2 (RMS-based)
		_sp->alg_count++; // increment the algorithm run count. Like a 1 second tick. Keeps track of peak and valley positions.
		high_pass_filter(_sp, (uint16_t)HPF_BITS, &_sp->hpf_array_len);
#ifndef USE_WINDOW_RMS
#ifdef USE_CLASSIC_RMS
		get_rms_classic(sp->hpf_array_len, sp->hpf_array_len);
#else
		get_rms_simple(_sp, _sp->hpf_array_len, _sp->hpf_array_len); // for R ratio of ratios, PI and SpO2 calculation
#endif
#endif
		// get rough and reliable estimate of heart rate using zero crossings relative to a threshold at 1/8 of max signal in the center half of the IR data buffer
		hr_zc_p = (int16_t)count_threshold_crossings_int16(_sp, &_sp->AC1f[IR][0],
														   _sp->hpf_array_len, 1);
		hr_zc_n = (int16_t)count_threshold_crossings_int16(_sp, &_sp->AC1f[IR][0],
														   _sp->hpf_array_len, 0);

		if ((kalman_filters[hr_filt].kalman_avg >> FIXED_BITS) > USE_HIGHER_CROSSING_COUNT_HR_BPM)
		{
			_sp->hr_zc = (hr_zc_p > hr_zc_n) ? hr_zc_p : hr_zc_n; // pick the higher of the two threshold crossing counts (missing beats due to breathing is higher risk)
		}
		else
		{
			_sp->hr_zc = (hr_zc_n < hr_zc_p) ? hr_zc_n : hr_zc_p; // pick the lower of the two threshold crossing counts (overcounting due to dicrotic notch is higher risk)
		}
		if (_sp->hr_zc > 0)
		{
			_sp->est_hr_period_from_zc1f = ((_sp->hpf_array_len << FIXED_BITS) / _sp->hr_zc) * 2;
			if (_sp->est_hr_period_from_zc1f > (int16_t)MAX_HR_PERIOD << FIXED_BITS)
			{
				_sp->est_hr_period_from_zc1f = (int16_t)MAX_HR_PERIOD << FIXED_BITS;
			}
			else if (_sp->est_hr_period_from_zc1f < (int16_t)MIN_HR_PERIOD << FIXED_BITS)
			{
				_sp->est_hr_period_from_zc1f = (int16_t)MIN_HR_PERIOD << FIXED_BITS;
			}
		}
		if (_sp->alg_count == MIN_ALG_RUNS_SWITCH_TO_LONGER_AVG)
		{
			kalman_filters[hr_filt].kalman_ind =
				kalman_filters[hr_filt].kalman_length;
			kalman_filters[spo2_filt].kalman_ind =
				kalman_filters[spo2_filt].kalman_length;
			kalman_filters[hr_filt].max_kalman_length = HR_KALMAN_LENGTH;
			kalman_filters[spo2_filt].max_kalman_length = SPO2_KALMAN_LENGTH;
		}
		static uint8_t nothing = 0;
		/******DEBUGGING****************/
		if (_sp->alg_count > 22)
		{
			nothing++;
			if (nothing == 300)
			{
				do_algorithm_part1(_sp);
			}
		}
		/********************************/
		run_kalman(zc_filt, _sp->est_hr_period_from_zc1f, NO_RATIO);

#ifdef USE_NOISE_CORRECTION // extends useful range of SpO2 at low PI -- MAX_NOISE must be characterized per hardware and supply source.
		// Can cause jumping in ppg_rms1f values when signal gets close to noise because if noise is bigger then the subtraction won't occur
		do_noise_correction(_sp);
#endif
		calc_R(_sp); // calc PI and R ratio of ratios
		calc_spo2(_sp);

		calc_hr(_sp);
#ifdef USE_SG
		set_sg_interval(_sp);
#endif
#ifdef USE_PVI
		get_pvi(_sp->hpf_array_len, _sp->period2use1f);
#endif
		run_kalman(hr_filt, _sp->current_hr1f, USE_RATIO); // this is an average of the heart rate from multiple algorithm runs that produces a medically relevant value.
		// The effective filter length is slightly longer when we apply the ZC and B2B filter, which does some pre-averaging. To first order add the filter lengths in quadrature sqrt(8^2+3^3) = 8.5 sec
		run_kalman(spo2_filt, _sp->current_spo21f, NO_RATIO); // this is a traditional several second flat average of the heart rate that produces a medically relevant value. 9-10 seconds matches the older Nellcor reference meter best. Newer Nellcor and Masimo devices average 5 pulses.
		_sp->display_spo2 = (_K_F[spo2_filt].kalman_avg * (uint16_t)10) >> FIXED_BITS;
		_sp->display_hr = _K_F[hr_filt].kalman_avg >> FIXED_BITS;
		// rounding
		int16_t spare_change = _K_F[hr_filt].kalman_avg & (int16_t)0x0007; // look at lowest 3 bits
		if (spare_change >= 4)
			_sp->display_hr += 1; // round up
	}
	else
	{
		_sp->period2use1f = 0;
		_sp->display_spo2 = 0;
		_sp->display_hr = 0;
	}
}

/************************************************************************************
 * Name:       copy_data
 * Function:   copies data from a fifo buffer into an ordered array
 * Parameters: channel: 0=red, 1=IR IR is run second because we use the IR channel data
 *             for RR calculations
 * Return:     none
 ************************************************************************************/
void copy_data(struct spo2 *_sp)
{
	/*copies all data from the raw_data buffers to temporary buffer
	 * Output is AC1f-->extended precision array
	 * this data is oldest first (like viewing captured data on a screen)
	 * */
	_sp->indx = _sp->data_ptr;
	for (int n = 0; n < ARRAY_LENGTH; n++)
	{
		_sp->AC1f[RED][n] = _sp->raw_data[RED][_sp->indx]; // load the AC data into an array.
		_sp->AC1f[IR][n] = _sp->raw_data[IR][_sp->indx];   // load the AC data into an array.
		_sp->indx++;
		if (_sp->indx == ARRAY_LENGTH)
			_sp->indx = 0; // loop back
	}
}
//
///************************************************************************************
// * Name:       get_DC
// * Function:   adds up values in data array to get the mean, in extended precision
// * Parameters: none
// * Return:     none
// ************************************************************************************/
// void get_DC(struct spo2 *_sp)
//{
//	/*calculates the mean DC level being subtracted in mean and residual DC level
//	 for each channel and stores is in res_dc. THe mean is used for SpO2 calculations.
//	 There is a lag in the mean and rms but this should not be significant as the mean
//	 is quite constant and changes in SpO2 are usually due to rms.*/
//	for (uint8_t channel = 0; channel < 2; channel++)
//	{
//		_sp->mean1f[channel] = 0; //get the running mean
//		for (uint16_t n = 0; n < ARRAY_LENGTH; n++)
//		{
//			_sp->mean1f[channel] += _sp->raw_data[channel][n];
//		}
//		_sp->mean1f[channel] = (_sp->mean1f[channel] << FIXED_BITS)
//				/ (int32_t) ARRAY_LENGTH;
//	}
//}

/************************************************************************************
 * Name:       uint_sqrt
 * Function:   32 bit fixed point square root method
 * Parameters: val
 * Return:     square root of val
 ************************************************************************************/
static uint32_t uint_sqrt(uint32_t val)
{
	// integer sqrt function from http://www.azillionmonkeys.com/qed/sqroot.html
	uint32_t temp, g = 0, b = 0x8000, bshft = 15;
	do
	{
		if (val >= (temp = (((g << 1) + b) << bshft--)))
		{
			g += b;
			val -= temp;
		}
	} while (b >>= 1);
	return g;
}

/************************************************************************************
 * Name:       calc_R
 * Function:   Calculate the ratio of ratios R used in SPO2 (R_AC/R_DC) / (IR_AC/IR_DC)
 * Parameters: uint8_t factor -- scale factor for target counts
 * Return:     none
 ************************************************************************************/
void calc_R(struct spo2 *_sp)
{
	/*an error of 0.01 in R is an error of about 0.25% in SpO2. So we want to keep
	 at least 8 bits of precision in R
	 for a huge RMS value like 2056 for IR and 1024 for R.
	 IR: (11 bits+4bits fp=15bits) we  shift 17 bits to 32 bits
	 R: (10 bits+4bits fp=14bits) we shift 17 bits to 31 bits
	 then we divide by the mean ~2e5, (18bits) which leaves 14 bits for IR and 13 for red.
	 We can then shift the red that by 12 bits before we divide by the denominator all 13 bits.
	 Now consider a minimum rms of 32 (5 bits) for R and 64 bits for IR
	 IR: 6bit+4bits FP=10bits. We shift by 17 bits to 27 bits
	 R: 5bit+4bits FP=9bits. We shift by 17 bits to 26 bits
	 then we divide by 18 bits which leaves 8 bits. This is bare bones
	 a 3 fixed point (nibble) shifts*/
#ifndef USE_WINDOW_P2P
	// Default: use mean absolute deviation (~RMS)
	// PI in % peak to peak (2.81 is 2*sqrt(2) and 100 is percent * 1.25 for typical difference between avg abs value and RMS)
	_sp->perfusion_index[RED] = ((float)(((uint32_t)354 * _sp->ppg_rms1f[RED]) >> FIXED_BITS)) / (float)_sp->mean[RED];
	_sp->perfusion_index[IR] = ((float)(((uint32_t)354 * _sp->ppg_rms1f[IR]) >> FIXED_BITS)) / (float)_sp->mean[IR];
	_sp->R = _sp->perfusion_index[RED] / _sp->perfusion_index[IR];
#else // USE_WINDOW_P2P
	if (_sp->beat_amp[RED] > 0 && _sp->beat_amp[IR] > 0)
	{
		_sp->perfusion_index[RED] = ((float)(((int32_t)_sp->beat_amp[RED]) * (int32_t)100)) / (float)_sp->mean[RED];
		_sp->perfusion_index[IR] = ((float)(((int32_t)_sp->beat_amp[IR]) * (int32_t)100)) / (float)_sp->mean[IR];
		R = perfusion_index[RED] / perfusion_index[IR];
	}
#endif

#ifdef USE_PRESSURE_CORRECTION
	int16_t use_pf;
	if (_K_F[hr_filt].kalman_avg >> FIXED_BITS > HR_FOR_PRESSURE_CORRECTION_UPPER_THRESH)
	{
		_sp->ok_to_use_pc = 0;
	}
	else if (_K_F[hr_filt].kalman_avg >> FIXED_BITS < HR_FOR_PRESSURE_CORRECTION_LOWER_THRESH)
	{
		_sp->ok_to_use_pc = 1;
	}

	if (_sp->ok_to_use_pc == 1)
	{
		if (_sp->pressure_factor > (int16_t)PRESSURE_FACTOR_MAX)
		{
			use_pf = (int16_t)PRESSURE_FACTOR_MAX;
		}
		else
		{
			use_pf = _sp->pressure_factor;
		}
		if (_sp->perfusion_index[IR] > (float)MIN_PI_FOR_PRESSURE_CORRECTION)
		{
			if (use_pf >= (int16_t)PRESSURE_FACTOR_THRESHOLD)
			{
				_sp->R -= ((float)((use_pf - (int16_t)PRESSURE_FACTOR_THRESHOLD) / (int16_t)PRESSURE_SLOPE_DENOM)) / (float)100;
			}
		}
	}
#endif
}

/************************************************************************************
 * Name:       calc_spo2
 * Function:   Calculates the SPO2 level using the ratio of ratios or "R" value using the
 * 			  "R" curve calibration values defined in SP02.h. R is calculated in calc_R()
 * Parameters: none
 * Return:     none
 ************************************************************************************/
void calc_spo2(struct spo2 *_sp)
{
	float spo2 = 0;
	float c[3] =
		{Rconstant, Rlinear, Rsquare};
	if (_sp->R > 0)
	{
		spo2 = c[0] + c[1] * _sp->R + c[2] * _sp->R * _sp->R;
		_sp->current_spo21f = (int16_t)(spo2 * (float)(((int16_t)1) << (uint8_t)FIXED_BITS));
#ifndef SPO2_DEBUG // constrain SPO2 values
		_sp->current_spo21f =
			((_sp->current_spo21f >> FIXED_BITS) > 100) ? 100 << FIXED_BITS : _sp->current_spo21f;
		_sp->current_spo21f =
			((_sp->current_spo21f >> FIXED_BITS) < MIN_SPO2) ? MIN_SPO2 << FIXED_BITS : _sp->current_spo21f;
#endif
	}
}

/************************************************************************************
 * Name:       calc_hr
 * Function:   Calculates heart rate based on arbitration of two hr calculations:
 *			  the zero crossings, which indicates the signal periodicity.
 * 			  and a triangle filter estimate of the beat to beat period using systole detection.
 *             While the peak detection method is more accurate and sufficient for signals with good SNR,
 *             the zero crossing method is more robust to weird signals and low SNR.
 *             The input signals for the ZC estimate and b2b estimated are the respective
 *             kalman filtered values. The arbitration between which to use is done in compar_hr().
 *             The output heart rate is yet another kalman average of current_hr1f.
 *             The average is performed by a zero order Kalman (outlier) filter, since sometimes
 *             this arbitration will get tricked and an outlier will need to be thrown out.
 *             Zero values input to the kalman increase the alg_fail_cnt and do not contribute to the average.
 *             High enough alg fail count will reset the algorithm.
 * Parameters: none
 * Return:     none
 ************************************************************************************/
void calc_hr(struct spo2 *_sp)
{ /*Calculates heart rate from a beat periodicity (samples per period aka "offset" in fixed precision*/
	_sp->b2b1f = _sp->window_beat_period1f;
	if (_sp->beats_found > 1)
	{
		run_kalman(b2b_filt, _sp->b2b1f, NO_RATIO); // this is a short duration filter designed to snag bad n function inputs and replace them with reasonably close values in time.
	}
	_sp->period2use1f = compare_hr(_sp, _sp->est_hr_period_from_zc1f, _sp->b2b1f);
	//	period2use1f = kalman_filters[zc_filt].kalman_avg;
	//	period2use1f = kalman_filters[b2b_filt].kalman_avg;
	if (_sp->period2use1f > 0)
	{
		_sp->current_hr1f =
			((uint32_t)((uint32_t)SAMPLE_RATE_PER_MIN << FIXED_BITS)
			 << FIXED_BITS) /
			(uint32_t)_sp->period2use1f;
	}
	else
	{
		_sp->current_hr1f = 0;
	}
}

/************************************************************************************
 * Name:       add_sample
 * Function:   call to add new IR and R data to the data buffer. This function removes the target count level
 * 			  which reduces the number of required bits to represent the signal, allowing us to use int16.
 * 			  This provides a huge speedup and RAM savings.
 * 			  It also means that we can't let the signal differ by more than 2^15 counts from the target level.
 * 			  This is done in the AGC loop in OB1203 code.
 * Parameters:
 uint32_t ir_data
 uint32_t r_data
 * Return:     none
 ************************************************************************************/
void add_sample(struct spo2 *_sp, spo2_target_counts_t target_counts,uint32_t ir_data, uint32_t r_data)
{
	/*Called by main to load new samples into the algorithm's buffer.
	 *Removes the large DC offset TARGET_COUNTS to store 18 bit unsigned as int16
	 */
	int32_t temp_ir;
	int32_t temp_r;
	// avoid rollover of 16 bit integer conversion -- constrain values at limits
	temp_ir = ((int32_t)ir_data - (int32_t)(target_counts.ir_led));
	if (temp_ir < -32768)
	{
		temp_ir = -32768;
	}
	else if (temp_ir > 30000)
	{
		temp_ir = 30000;
	}

	temp_r = ((int32_t)r_data - (int32_t)(target_counts.red_led));
	if (temp_r < -32768)
	{
		temp_r = -32768;
	}
	else if (temp_r > 30000)
	{
		temp_r = 30000;
	}
	static int16_t ir_buffer[NUM2AVG];
	static int16_t r_buffer[NUM2AVG];
	static uint8_t buf_ind;
	int16_t prev_val;
	uint8_t reset_sg = 0;
	static int32_t ir_running_sum;
	static int32_t r_running_sum;
	uint8_t mask = (uint8_t)NUM2AVG - 1;
	if (_sp->num_samples == 0)
	{ // reset
		ir_running_sum = 0;
		r_running_sum = 0;
		buf_ind = 0;
		reset_sg = 1;
	}
	if (_sp->num_samples < NUM2AVG)
		_sp->num_samples++;
	if (_sp->num_samples == NUM2AVG)
	{
		prev_val = ir_buffer[buf_ind];
		ir_running_sum -= prev_val;
		prev_val = r_buffer[buf_ind];
		r_running_sum -= prev_val;
	}
	ir_buffer[buf_ind] = temp_ir; // load new samples
	ir_running_sum += ir_buffer[buf_ind];
	r_buffer[buf_ind] = temp_r;
	r_running_sum += r_buffer[buf_ind];
	buf_ind++;		 // increment
	buf_ind &= mask; // loop index back
	if (_sp->num_samples <= NUM2AVG)
	{
		temp_ir = ir_running_sum / _sp->num_samples;
		temp_r = r_running_sum / _sp->num_samples;
	}
	else
	{
		temp_ir = ir_running_sum >> AVG_BITS;
		temp_r = r_running_sum >> AVG_BITS;
	}
#ifdef USE_SG
	iterate_savgol(IR, temp_ir, _sp->sg_interval, reset_sg);
	iterate_savgol(RED, temp_r, _sp->sg_interval, reset_sg);
	temp_ir = sg[IR].fit;
	temp_r = sg[RED].fit;
#endif
	_sp->raw_data[IR][_sp->data_ptr] = temp_ir;
	// IRdat = temp_ir; //for viewing in data chart
	_sp->raw_data[RED][_sp->data_ptr] = temp_r;
	// Rdat = temp_r;
	_sp->data_ptr++;
	_sp->data_ptr = (_sp->data_ptr >= ARRAY_LENGTH) ? 0 : _sp->data_ptr; // index roll over
}

void missed_sample_counts_increment(struct spo2 *_sp)
{
    if (_sp->sample_count < ARRAY_LENGTH)
        _sp->sample_count++; //number of samples in buffer
    _sp->missed_sample_count++;
}

void read_sample_counts_increment(struct spo2 *_sp)
{
    if (_sp->sample_count < ARRAY_LENGTH)
        _sp->sample_count++; //number of samples in buffer
    _sp->read_sample_count++;
}

void counts_of_samples_clear(struct spo2 *_sp)
{
    _sp->sample_count = 0;
}

/************************************************************************************
 * Name:       get_direction
 * Function:   returns a value = 1 if slope is positive, -1 if negative or 0 if flat
 * Parameters:
 * 	data1
 *	data2
 * Return:    none
 ************************************************************************************/
int8_t get_direction(int32_t data1, int32_t data2)
{
	int8_t dir;
	if (data2 > data1)
	{
		dir = 1;
	}
	else if (data2 < data1)
	{
		dir = -1;
	}
	else
	{
		dir = 0;
	}
	return dir;
}

/************************************************************************************
 * Name:       do_algorithm_part3
 * Function:   This function does breathing rate detection by detecting periodic variations in heart rate signals.
 * In particular we track the DC heights of the systole which is a kind of AM modulation detection method that senses a part of both
 * DC offset change and PPG amplitude change with respiration.
 * The method used for determining the respiration rate is counting zero crossings.
 * ZC's are calculated for several different baselines. Part 4 determines the most likely accurate baseline.
 * Parameters: none
 * Return:     none
 ************************************************************************************/
void do_algorithm_part3(struct spo2 *_sp)
{

	if (_K_F[b2b_filt].kalman_avg > 0)
	{
		// do b2b detection
#ifdef USE_PVI
		if (pvi_avg > 0)
		{
			hr_data_buffer[hr_data_buffer_ind] = pvi_avg;
#else
		if (_sp->systole_found != 0)
		{
			_sp->hr_data_buffer[_sp->hr_data_buffer_ind] = _sp->systole; // IR peak (valley) normed to length
#endif
			int ind = 0;
			for (int n = 0; n < (uint8_t)MAX_BREATH_ARRAY_LENGTH; n++)
			{ // newest sample first, oldest sample last
				ind = _sp->hr_data_buffer_ind - n;
				if (ind < 0)
					ind += (uint8_t)MAX_BREATH_ARRAY_LENGTH;
				_sp->lin_buffer[n] = _sp->hr_data_buffer[ind];
			}
			_sp->hr_data_buffer_ind++;
#ifdef QUICKSTART_RR
			if (_sp->hr_data_buffer_ind >= (uint8_t)START_BREATH_ARRAY_LENGTH)
			{
				_sp->do_rr_alg = 1;
			}
#endif
			if (_sp->hr_data_buffer_ind >= (uint8_t)MAX_BREATH_ARRAY_LENGTH)
			{
				_sp->hr_data_buffer_ind = 0;
				_sp->do_rr_alg = 1;
				_sp->rr_buffer_full = 1;
			}

			if (_sp->do_rr_alg)
			{

#ifdef QUICKSTART_RR
				if (_sp->rr_buffer_full == 0)
				{
					_sp->max_rr_filter = 3;
					if (_sp->hr_data_buffer_ind >= (uint8_t)62)
						_sp->max_rr_filter = 4;
				}
				else
				{
					_sp->max_rr_filter = NUM_RR_FILTERS;
				}
				for (int m = 0; m < _sp->max_rr_filter; m++)
				{
#else
				for (int m = 0; m < NUM_RR_FILTERS; m++)
				{
#endif
					uint8_t filter_len = _sp->rr_filter_lens[m];
					int32_t temp = 0;
					_sp->rr_filter[0] = 0;
					// get the running baseline
					for (int p = 0; p < (int)MAX_BREATH_FILTER_SPAN; p++)
					{
						temp = 0;
						for (int n = 0; n < filter_len; n++)
						{
							temp += _sp->lin_buffer[p + n];
						}
						_sp->rr_filter[p] = temp / (int32_t)filter_len;
					}
					_sp->baseline_sign_changes[m] = count_sign_changes(_sp->rr_filter,
																	   filter_len);
					for (int n = 0; n < MAX_BREATH_FILTER_SPAN; n++)
					{
						_sp->detrended[n] = _sp->lin_buffer[n + filter_len / 2] - _sp->rr_filter[n];
					}
					_sp->zero_crossings[m] = count_zero_crossings(_sp->detrended,
																  (uint8_t)MAX_BREATH_FILTER_SPAN);
				}
			} // end if do_rr_alg
		}	  // end if systole found
	}		  // end if kalman_avg>0
} // end function

/************************************************************************************
 * Name:       do_algorithm_part4
 * Function:   Part 4 determines the most likely accurate baseline for RR AM zero crossings, to determine RR.
 * It uses an efficient IIR approximation of RMS and a merit function (badness) to track which baseline is most likely to be accurate.
 * A damping filter is used to prevent rapid changes in baseline.
 * Parameters: none
 * Return:     none
 ************************************************************************************/
void do_algorithm_part4(struct spo2 *_sp)
{
	// USE A VARIETY OF TRIANGLE FILTERS TO FILTER THE DATA AND GET SEVERAL ESTIMATES OF THE RR PERIOD
	int16_t rr_display_avg = 0;
	int32_t numer;
	int32_t denom;
	static int16_t rr_runs = 0;
	if (_sp->filter_tracker == 0)
	{
		_sp->filter_tracker = 4 * FILTER_DAMPING + 1; // start with filter 4--biasing toward lower values
	}
	if (_sp->do_rr_alg)
	{
		// NOW DECIDE WHICH FILTER IS RECENTLY PERFORMING THE BEST
#ifdef QUICKSTART_RR
		for (int m = 0; m < max_rr_filter; m++)
		{
#else
		for (int m = 0; m < NUM_RR_FILTERS; m++)
		{
#endif
			// adding fixed precision to RMS
			characterize_array_iir(_sp, m,
								   (((int16_t)_sp->zero_crossings[m]) << FIXED_BITS), _sp->iir_avg,
								   _sp->iir_rms, _sp->iir_rms_smooth); // get the RMSE for each filter using a nonlinear IIR estimation (saves RAM and multiplications). This is tuned for an effective RMSE over 10 samples.
			if (_sp->baseline_sign_changes[m] > 0)
			{ // protect against div by 0
			  // adding more fixed precision to keep resolution after division
				numer = ((int32_t)_sp->iir_rms_smooth[m]) << FIXED_BITS;
				denom = _sp->baseline_sign_changes[m];
				_sp->badness[m] = numer / denom;
			}
			else
			{
				_sp->badness[m] = 0; // this will make this filter not used
			}
		}

		//		uint8_t center_filter = NUM_RR_FILTERS/2;
		//		badness[center_filter] /= CENTER_BIAS;
		_sp->max_fom = 3; // default
#ifdef QUICKSTART_RR
		if (_sp->max_rr_filter == NUM_RR_FILTERS)
		{
			_sp->max_filter =
				(_K_F[rr_filt].kalman_avg > ALLOW_LONGEST_RR_FILTER_THRESHOLD ? NUM_RR_FILTERS - 1 : NUM_RR_FILTERS - 2);
		}
		else
		{
			_sp->max_filter =
				(max_rr_filter == NUM_RR_FILTERS - 1) ? NUM_RR_FILTERS - 1 : NUM_RR_FILTERS - 2;
		}
#else
		_sp->max_filter = (_K_F[rr_filt].kalman_avg > ALLOW_LONGEST_RR_FILTER_THRESHOLD ? NUM_RR_FILTERS - 1 : NUM_RR_FILTERS - 2);
#endif
		for (int n = 0; n <= _sp->max_filter; n++)
		{
			if ((_sp->badness[n] > 0) && (_sp->badness[n] < _sp->badness[_sp->max_fom]))
			{
				_sp->max_fom = n;
			}
		}

		// key to stability is damping the switching of filters. Increase FILTER_DAMPING for more stability. Decrease for more response.
		// this is the new slope limiter
		if (_sp->current_filter == 0)
			_sp->current_filter = _sp->filter_tracker / FILTER_DAMPING;
		if (_sp->filter_tracker < (_sp->max_fom * FILTER_DAMPING + (FILTER_DAMPING >> 1)))
		{
			_sp->filter_tracker++; // increment filter
		}
		else if (_sp->filter_tracker > (_sp->max_fom * FILTER_DAMPING + (FILTER_DAMPING >> 1)))
		{
			_sp->filter_tracker--; // decrement filter
		}
		_sp->current_filter = (_sp->filter_tracker / FILTER_DAMPING);

		if (_sp->zero_crossings[_sp->current_filter] != 0)
		{ // skipping zero values
			_sp->rr_current_monitor1f = (((uint32_t)MAX_BREATH_FILTER_SPAN * 2)
										 << FIXED_BITS) /
										_sp->zero_crossings[_sp->current_filter]; // RR period measured in algorithm runs per breath with fixed precision. Includes factor of 2 for 2 crossing per breath
			run_kalman(rr_filt, _sp->rr_current_monitor1f, NO_RATIO);
		}

#ifdef USE_RR_DISPLAY_AVG
		if ((_K_F[rr_filt].kalman_avg >= RR_MIN_PERIOD_1F) && (_K_F[rr_filt].kalman_avg <= RR_MAX_PERIOD_1F))
		{
			if (_sp->rr_display_avg_cnt < _sp->rr_display_num2avg)
			{
				_sp->rr_display_avg_cnt++; // ramp average at the beginning (better than initializing to first value)
				rr_runs = _sp->rr_display_avg_cnt;
			}
			else
			{
				if (_sp->rr_display_num2avg != (uint8_t)RR_DISPLAY_NUM2AVG)
				{
					if (rr_runs == (uint8_t)RR_DISPLAY_INITIAL_RUNS)
					{
						_sp->rr_display_num2avg = (uint8_t)RR_DISPLAY_NUM2AVG;	  // change to longer average
						_sp->rr_display_avg_buffer_ind = _sp->rr_display_avg_cnt; // point to end of array where data is not populated (e.g. last time we had 12 samples, put data in slot 12 (13th position)
						_sp->rr_display_avg_cnt++;								  // increment count of number to average
					}
					else if (rr_runs < RR_DISPLAY_INITIAL_RUNS)
					{
						rr_runs++;
					}
				}
			}
			_sp->rr_display_avg_buffer[_sp->rr_display_avg_buffer_ind] =
				_K_F[rr_filt].kalman_avg;
			_sp->rr_display_avg_buffer_ind++; // increment the buffer index for next time
			if (_sp->rr_display_avg_buffer_ind == _sp->rr_display_num2avg)
				_sp->rr_display_avg_buffer_ind = 0; // loop index
			rr_display_avg = 0;						// initialize
			for (int n = 0; n < _sp->rr_display_avg_cnt; n++)
			{
				rr_display_avg += _sp->rr_display_avg_buffer[n];
			}
			rr_display_avg /= _sp->rr_display_avg_cnt;
		}
#else
		rr_display_avg = _K_F[rr_filt].kalman_avg;
#endif

		if (rr_display_avg != 0)
		{
			// below here isn't updated yet
			_sp->breathing_rate1f = ((uint32_t)60) << FIXED_BITS;
			_sp->breathing_rate1f *= (uint32_t)SAMPLE_RATE;
			_sp->breathing_rate1f /= (uint32_t)INTERVAL;
			_sp->breathing_rate1f = _sp->breathing_rate1f << FIXED_BITS;
			_sp->breathing_rate1f /= (uint32_t)rr_display_avg;
		}
		else
		{ // haven't got a good value yet
			_sp->breathing_rate1f = 0;
		}

	} // end if do_rr_alg
}

/************************************************************************************
 * Name:			characterize_array_iir
 * Function:
 *	This function characterizes arrays by calculating their (approximate) RMS value.
 *	The nonlinear IIR filter used is efficient but must be tuned for any given length of data.
 * Parameters:
 *	uint8_t arr_num
 *	int16_t new_val
 *	int16_t* iir_avg
 *	int16_t *iir_rms
 *	int16_t* iir_rms_smooth
 * Return:     none
 ************************************************************************************/
void characterize_array_iir(struct spo2 *_sp, uint8_t arr_num, int16_t new_val, int16_t *_iir_avg,
							int16_t *_iir_rms, int16_t *_iir_rms_smooth)
{
	// max arr_num is 8 the way I am initializing
	// arr_num is the number of the array to update. arr_num is in the range 0:NUM_RR_FILTERS-1
	// NOTE: THIS FUNCTION IGNORES ZEROS IN THE COMPUTATION OF THE AVERAGE--THESE ARE ALGORITHM FAILURES. FOR GENERAL USAGE REMOVE THE "if (new_val !=0)" condition.

	int16_t kinv = 1;				 // 1 == bitshift of 1 or factor of 0.5. In this case where kinv = 0.5, then 1-kinv = 0.5--convenient to bitshift both numbers by one instead of div by 2
	int16_t rms_smooth_len_bits = 1; // this is number of bitshifts, e.g. 1 = divide by a factor of 2
	int16_t avg_len_bits = 1;		 // ditto

	// initialize rms arrays first value to zero
	if (_sp->char_array_iir_initialized == 0)
	{
		for (int n = 0; n < NUM_RR_FILTERS; n++)
		{
			_iir_rms[n] = 0;
			_iir_rms_smooth[n] = 0;
		}
	}
	if ((_sp->char_array_iir_initialized & (1 << arr_num)) == 0)
	{
		_iir_avg[arr_num] = new_val; // initialize avg array to first value (using bits of char_array_iir_initialized to track which filter nums are char_array_iir_initialized)
		_sp->char_array_iir_initialized |= 1 << arr_num;
	}

	// update IIR filters
	_iir_rms[arr_num] = (int16_t)uint_sqrt(
		((((uint32_t)new_val - _iir_avg[arr_num]) * ((uint32_t)new_val - _iir_avg[arr_num])) >> kinv) + (((uint32_t)_iir_rms_smooth[arr_num] * (uint32_t)_iir_rms_smooth[arr_num]) >> kinv)); // sqrt(k*(new_val-avg)^2 + (1-k)*(smooth)^2)
	_iir_rms_smooth[arr_num] += div2n(
		(_iir_rms[arr_num] - _iir_rms_smooth[arr_num]),
		rms_smooth_len_bits);
	if (new_val != 0)
	{
		_iir_avg[arr_num] += div2n((new_val - _iir_avg[arr_num]), avg_len_bits);
	}
}

/****************************************************************
 * Name:		div2n
 * Function:	16 bit divide by n that is numerically equivalent for negative and positive numbers
 * @param num
 * @param bits
 * @return		num/2
 ****************************************************************/
int16_t div2n(int16_t num, uint8_t bits)
{
	return (num & 0x8000) ? ~((((~num) + 1) >> bits) - 1) : num >> bits;
}

/************************************************************************************
 * Name:			count_sign_changes
 * Function:
 *	This function counts the number of direction changes in a baseline. Hint: more = flatter.
 *	This is part of the RR algorithm secret sauce. It favors higher frequencies which cancels the
 *	tendency of the RMS to favor slow variations. It's a heuristic, but valuable.
 * Parameters:
 *	int32_t* arr
 *	uint8_t len
 * Return:     none
 ************************************************************************************/
uint8_t count_sign_changes(int32_t *arr, uint8_t len)
{
	uint8_t num_sign_changes = 0;
	int dir = 0;
	int new_dir = 0;
	for (int n = 1; n < len; n++)
	{
		if (arr[n] > arr[n - 1])
		{
			new_dir = 1;
		}
		else if (arr[n] < arr[n - 1])
		{
			new_dir = -1;
		}
		if (new_dir != 0)
		{ // going up or down
			if (dir != 0)
			{ // not comparing to flat
				if (new_dir != dir)
				{ // dir change
					num_sign_changes++;
				}
			}
		}
		dir = new_dir;
	}
	return num_sign_changes;
}

/************************************************************************************
 * Name:			count_zero_crossings
 * Function:
 *	This function counts the number of zero crossings for an array. Used in RR calculation.
 * Parameters:
 *	int32_t* arr
 *	uint8_t len
 * Return:     number of zero crossings
 ************************************************************************************/
uint8_t count_zero_crossings(int32_t *arr, uint8_t len)
{
	uint8_t num = 0;
	int prev_sign = 0;
	// use prev_sign to track the previous nonzero sign to avoid double counting if a value is actually zero.
	if (sign(arr[0]) != 0)
	{
		prev_sign = sign(arr[0]);
	}
	for (int n = 1; n < len; n++)
	{
		if ((prev_sign != 0) && (sign(arr[n]) != prev_sign))
		{
			num++;
		}
		if (sign(arr[n]) != 0)
		{
			prev_sign = sign(arr[n]);
		}
	}
	return num;
}

/************************************************************************************
 * Name:			count_threshold_crossings_int16
 * Function:
 *	This function counts the number of zero crossings for an array. Used in HR period course estimate
 * Parameters:
 *	int16_t* arr
 *	uint8_t len
 * Return:     number of zero crossings
 ************************************************************************************/
uint8_t count_threshold_crossings_int16(struct spo2 *_sp, int16_t *arr, uint16_t arr_len,
										uint8_t polarity)
{
	int16_t prev_crossing_pos = 0;		// x position in array of the previous crossing
	int16_t prev_prev_crossing_pos = 0; // x position in array of the crossing prior to the previous
	int16_t elapsed = 0;
	uint8_t do_time_check = 0; // we have a HR estimate, use that to make sure we don't double count peaks
	uint16_t kalman_hr_period;
	_sp->bad_crossing_cnt = 0;
	if (_K_F[hr_filt].kalman_avg > 0)
	{
		if (_K_F[hr_filt].kalman_avg < ((uint16_t)MAX_HR_TO_USE_DEBOUNCING) << (uint8_t)FIXED_BITS)
		{
			do_time_check = 1;
			kalman_hr_period = (uint16_t)(((uint32_t)SAMPLE_RATE_PER_MIN)
										  << FIXED_BITS) /
							   (uint32_t)_K_F[hr_filt].kalman_avg;
		}
	}
	int16_t max_val = arr[0];
	uint8_t num = 0;
	// find the max of the data
	for (uint16_t n = 1; n < arr_len; n++)
	{
		if (polarity > 0)
		{
			if (arr[n] > max_val)
			{
				max_val = arr[n];
			}
		}
		else
		{
			if (arr[n] < max_val)
			{
				max_val = arr[n];
			}
		}
	}
	int16_t threshold = (int32_t)max_val * ((int32_t)HPF_THRESHOLD_NUM) / ((int32_t)HPF_THRESHOLD_DEN); // putting threshold above zero to reduce detection of dicrotic notches
	int16_t test_val = (polarity > 0) ? arr[0] - threshold : arr[0] + threshold;
	int prev_sign = 0;
	// use prev_sign to track the previous nonzero sign to avoid double counting if a value is actually zero.
	// calculate the sign relative to the threshold
	if (sign(test_val) != 0)
	{
		prev_sign = sign(test_val);
	}
	// get the threshold crossings
	for (int16_t n = 1; n < arr_len; n++)
	{
		test_val = arr[n] - threshold;
		if ((prev_sign != 0) && (sign(test_val) != prev_sign)) // crossing detection
		{
			num++;
			if (do_time_check)
			{
				if (prev_crossing_pos == 0) // first crossing
				{
					prev_crossing_pos = n;
				}
				else if (prev_prev_crossing_pos == 0) // second crossing
				{
					prev_prev_crossing_pos = prev_crossing_pos;
					prev_crossing_pos = n;
				}
				else // third or later crossing
				{
					elapsed = n - prev_prev_crossing_pos;
					if (elapsed < (kalman_hr_period >> 1)) // invalid peak--too close (less than half the estimated period)
					{
						num--; // erase this potentially invalid crossing
						_sp->bad_crossing_cnt++;
					}
					else // potentially valid crossing, increment
					{
						prev_prev_crossing_pos = prev_crossing_pos;
						prev_crossing_pos = n;
					}
				}
			}
		}
		if (sign(test_val) != 0)
		{
			prev_sign = sign(test_val);
		}
	}
	return num;
}

/************************************************************************************
 * Name:			sign
 * Function:
 *	This function returns the sign of a number.
 * Parameters:
 *	int32_t val
 * Return:     sign (1 = positive, 0 = zero, -1 = negative
 ************************************************************************************/
int sign(int32_t val)
{
	if (val < 0)
		return -1;
	if (val > 0)
		return 1;
	return 0;
}

/************************************************************************************
 * Name:			get_avg_PI
 * Function:
 *	This function performs an average of recent PI values to reduce display variation
 * Parameters:
 *	float new_POR
 * Return:     average PI
 ************************************************************************************/
float get_avg_PI(float new_PI, float *pi_rms)
{ // simple mean. Resets when passed a zero.
	static float PI_buffer[PI_AVGS];
	static int8_t PI_buffer_ind = 0;
	static uint8_t num_PIs = 0;
	float new_avg = 0;
	int32_t int_avg = 0;
	const int8_t bits_of_prec = 12; // bits of precision
	static int16_t PI_int_buffer[PI_AVGS];
	uint32_t rms = 0;
	int8_t new_ind;
	if (new_PI > 0)
	{			   //>0 valid, do average
		num_PIs++; // increment number to average
		if (num_PIs > PI_AVGS)
			num_PIs = PI_AVGS;			   // truncate number of averages at max value
		PI_buffer[PI_buffer_ind] = new_PI; // load new sample into buffer
		PI_int_buffer[PI_buffer_ind] = (int16_t)ldexp((double)new_PI, bits_of_prec);
		new_ind = PI_buffer_ind; // get index of current latest sample

		PI_buffer_ind++; // increment buffer pointer for next time
		if (PI_buffer_ind == PI_AVGS)
			PI_buffer_ind = 0; // loop back

		for (uint8_t n = 0; n < num_PIs; n++)
		{
			new_avg += PI_buffer[new_ind]; // add each previous PI value
			int_avg += PI_int_buffer[new_ind];
			new_ind--; // decrement pointer
			if (new_ind < 0)
				new_ind += PI_AVGS; // loop back
		}
		new_avg /= num_PIs;
		int_avg /= num_PIs;
		new_ind = PI_buffer_ind - 1;
		if (new_ind < 0)
			new_ind += PI_AVGS; // loop back
		for (uint8_t n = 0; n < num_PIs; n++)
		{
            rms += (uint32_t) ((int32_t) PI_int_buffer[new_ind] - int_avg) * (uint32_t) ((int32_t) PI_int_buffer[new_ind] - int_avg);
			new_ind--;
			if (new_ind < 0)
				new_ind += PI_AVGS;
		}
		rms /= num_PIs;										// divide by number of samples
		rms = uint_sqrt((uint32_t)rms);						// square root
		*pi_rms = (float)ldexp((double)rms, -bits_of_prec); // convert back to float
	}
	else
	{ // reset
		new_avg = new_PI;
		num_PIs = 0;
		PI_buffer_ind = 0;
		*pi_rms = 0;
	}
#ifdef ten_x_pi
	return new_avg * (float)10;
#else
	return new_avg;
#endif
}

/************************************************************************************
 * Name:			compare_hr
 * Function:
 *	Decides whether to use beat-to-beat HR period or threshold crossing HR period
 * Parameters:
 *	uint16_t zc_est threshold crossing estimate of HR period with additional fixed precision
 *	uint16_t b2b_est beat-to-beat estimate of HR period with additional fixed precision
 * Return:     average PI
 ************************************************************************************/
uint16_t compare_hr(struct spo2 *_sp, uint16_t zc_est, uint16_t b2b_est)
{
	uint16_t best_hr;
	int16_t b2b_err1f;
	uint8_t use_zc = 0;
	uint8_t use_b2b = 0;

	// all zero
	if (_K_F[zc_filt].kalman_avg > 0)
		use_zc = 1;
	if (_K_F[b2b_filt].kalman_avg > 0)
		use_b2b = 1;
#ifdef _50sps
	if (_K_F[zc_filt].kalman_avg < ((uint16_t)MIN_50_SPS_PERIOD_2_USE_B2B) << FIXED_BITS)
		use_b2b = 0;
#endif

	if (!(use_b2b | use_zc))
	{ // no choice
		best_hr = 0;
	}
	else if (!use_b2b)
	{ // only one choice
		best_hr = _K_F[zc_filt].kalman_avg;
	}
	else
	{
		_sp->b2b_rms_norm1f = ((_K_F[b2b_filt].data_std << FIXED_BITS) << 2) / _K_F[b2b_filt].kalman_avg;
		b2b_err1f = ((int16_t)b2b_est) - (int16_t)_K_F[zc_filt].kalman_avg;
		b2b_err1f = (b2b_err1f << FIXED_BITS) << 2;
		b2b_err1f /= (int16_t)_K_F[zc_filt].kalman_avg;
		b2b_err1f = abs(b2b_err1f);
		if (b2b_err1f > BAD_HR_THRESHOLD)
		{
			use_b2b = 0;
		}
		if (use_b2b)
		{
			if (kalman_filters[b2b_filt].max_outlier_cnt < max_outlier_cnt_init[b2b_filt])
			{ // increment the max outlier count gradually
				kalman_filters[b2b_filt].max_outlier_cnt++;
			}

			if (_sp->alg_count < (int16_t)B2B_DELAY)
			{
				best_hr = zc_est;
			}
			else
			{
				best_hr = _K_F[b2b_filt].kalman_avg;
			}
		}
		else
		{
			best_hr = _K_F[zc_filt].kalman_avg;
		}
	}
	return best_hr;
}

/************************************************************************************
 * Name:			high_pass_filter
 * Function:
 *	Does a centered baseline removal from AC1f. Last baseline_length-1 points are
 *	not baseline-removed (unmodified)
 * Parameters:
 *	uint16_t baseline_bits - number of bits to use for the baseline
 *	(baseline_length = 1<<baseline_bits)
 *	uint16_t * new_array_len - pointer to variable to update with the useful array
 *	 length of the filtered array
 * Return:     average PI
 ************************************************************************************/
int8_t high_pass_filter(struct spo2 *_sp, uint16_t baseline_bits, uint16_t *new_array_len)
{
	/*will shorten array by baseline_len-1 points by removing a centered average of baseline_len
	 * for example an array of 16 pts with a baseline of 3 will return 14 pts, each an average of the previous, current and next point
	 * (ignore the last two data points in the array as these are unmodified from the original array)
	 */

	// condition inputs
	if ((baseline_bits > 7) || (baseline_bits < 2))
	{
		*new_array_len = (uint16_t)ARRAY_LENGTH;
		return -1;
	}
	uint16_t baseline_len = 1 << baseline_bits;
	if (baseline_len >= (uint16_t)ARRAY_LENGTH)
	{
		*new_array_len = (uint16_t)ARRAY_LENGTH;
		return -1;
	}
	uint16_t first_pt;
	uint16_t mid_pt;
	uint16_t last_pt;
	int32_t baseline;
	int32_t temp1;
	//	int32_t res_sum = 0;

	*new_array_len = (uint16_t)ARRAY_LENGTH + 1 - baseline_len;
	for (uint8_t channel = 0; channel < 2; channel++)
	{
		// get initial baseline sum
		int32_t hpf_baseline_sum = 0;
		for (uint16_t n = 0; n < baseline_len; n++)
		{
			hpf_baseline_sum += _sp->AC1f[channel][n];
		}

		first_pt = 0;
		mid_pt = 1 << (baseline_bits - 1);
		last_pt = baseline_len;
		for (uint16_t n = 0; n < *new_array_len - 1; n++)
		{
			// get next baseline before
			baseline = hpf_baseline_sum >> baseline_bits;
			hpf_baseline_sum -= _sp->AC1f[channel][first_pt];
			hpf_baseline_sum += _sp->AC1f[channel][last_pt];
			temp1 = (((int32_t)_sp->AC1f[channel][mid_pt]) - baseline);
			if (temp1 > 0x00007FFFL)
				temp1 = 0x00007FFFL;
			if (temp1 < -0x00008000L)
				temp1 = -0x00008000L;
			_sp->AC1f[channel][n] = (int16_t)temp1;
			first_pt++;
			mid_pt++;
			last_pt++;
		}
		// get last point
		baseline = hpf_baseline_sum >> baseline_bits;
		_sp->AC1f[channel][*new_array_len - 1] =
			(int16_t)(((int32_t)_sp->AC1f[channel][mid_pt]) - baseline);
	}
	return 0;
}

/************************************************************************
 * Name:		hpf_window_peak_detect_init
 * Function:	initialize variables for hpf_window_peak_detect function
 * Parameters:	none
 * Return:		none
 ***********************************************************************/
void hpf_window_peak_detect_init(struct spo2 *_sp)
{
	for (uint8_t ch = 0; ch < 2; ch++)
	{
		_sp->flat_data[ch] = 0;
		_sp->max_position[ch] = 0;
		_sp->max_value[ch] = 0;
		_sp->min_position[ch] = 0;
		_sp->min_value[ch] = 0;
		_sp->diastole_position[ch] = 0;
		_sp->diastole_value[ch] = 0;
		_sp->systole_position[ch] = 0;
		_sp->systole_value[ch] = 0;
		_sp->prev_systole_position[ch] = 0;
		for (uint8_t n = 0; n < (uint8_t)MAX_BEATS_PER_INTERVAL; n++)
		{
			_sp->beat_amps[ch][n] = 0;
		}
		_sp->beat_amp[ch] = 0;
		_sp->beat_cnt[ch] = 0;
		_sp->max_cnt[ch] = 0;
		_sp->min_cnt[ch] = 0;
		_sp->baseline_sum[ch] = 0;

#ifdef USE_SG
		_sp->baseline_cnt = -(2 * (int8_t)MAX_SG_INT) - 1; // first 15 samples don't count (starting SG filter)
#else
		_sp->baseline_cnt = 0;
#endif
		_sp->mean[ch] = 0;
		_sp->beat_cnt[ch] = 0;
		_sp->max_found[ch] = 0;
		_sp->min_found[ch] = 0;
		for (uint8_t n = 0; n < NUM_INTERVALS_FOR_RMS; n++)
		{
			_sp->sum_abs[ch][n] = 0;
		}
	}
	_sp->data_cnt = 0;
	_sp->num_window_samples = 0;
	_sp->sum_abs_ptr = 0;
	_sp->sum_abs_cnt = 0;
	_sp->window_width = INIT_WINDOW_WIDTH; // checking with 80 BPM as default
	_sp->prev_window_width = 0;
	_sp->beats_found = 0;
	_sp->diastole_trend = 0;
	_sp->systole_trend = 0;
	_sp->pressure_factor = 0;
	_sp->window_beat_period1f = 0;
}

/************************************************************************************
 * Name:		hpf_window_peak_detect
 * Function:
 *	This function performs a moving high pass filter on the raw by running the filter on INTERVAL samples
 *	Stores the baseline removed data in windows and does peak detection
 *	also stores sum of absolute errors for ratio of ratios calculation
 * Parameters:	none
 * Return:		none
 ************************************************************************************/
void hpf_window_peak_detect(struct spo2 *_sp)
{
	uint16_t first_pt;
	uint16_t mid_pt;
	uint16_t last_pt;
	uint8_t baseline_length = ((uint8_t)1) << HPF_BITS;
	int32_t temp1;
	int32_t baseline;
	_sp->prev_window_width = _sp->window_width;
	int32_t sum_abs_error[] = {0, 0};
	_sp->beat_cnt[RED] = 0;
	_sp->beat_cnt[IR] = 0;
	if (_K_F[zc_filt].kalman_avg > 0) // get new window length
	{
		_sp->window_width = (uint8_t)(((_K_F[zc_filt].kalman_avg >> (uint8_t)FIXED_BITS) * (uint16_t)WINDOW_WIDTH_NUM) / (uint16_t)WINDOW_WIDTH_DEN); // 60%
		// constrain window length
		if (_sp->window_width < (uint8_t)MIN_WINDOW_LENGTH)
			_sp->window_width = (uint8_t)MIN_WINDOW_LENGTH;
		if (_sp->window_width > (uint8_t)MAX_WINDOW_LENGTH)
			_sp->window_width = (uint8_t)MAX_WINDOW_LENGTH;
	}

	// iterate through new samples
	last_pt = _sp->data_ptr + (uint16_t)ARRAY_LENGTH - (uint16_t)INTERVAL; // most recent point in raw_data
	if (last_pt >= (uint16_t)ARRAY_LENGTH)
		last_pt -= (uint16_t)ARRAY_LENGTH;
	first_pt = _sp->data_ptr + (uint16_t)ARRAY_LENGTH - (uint16_t)INTERVAL - _sp->baseline_cnt - 1; // oldest point in raw_data
	if (first_pt >= (uint16_t)ARRAY_LENGTH)
		first_pt -= (uint16_t)ARRAY_LENGTH;
	mid_pt = first_pt + (baseline_length >> 1) + 1;
	if (mid_pt >= (uint16_t)ARRAY_LENGTH)
		mid_pt -= ARRAY_LENGTH;
	uint8_t ppg_cnt = 0;
	for (uint8_t n = 0; n < (uint8_t)INTERVAL; n++)
	{
		if (_sp->baseline_cnt < baseline_length)
			_sp->baseline_cnt++;
		if (_sp->baseline_cnt == baseline_length)
		{
			_sp->data_cnt++;
			ppg_cnt++;
			for (uint8_t ch = 0; ch < 2; ch++) // for IR and red channels
			{
				baseline = _sp->baseline_sum[ch] >> HPF_BITS;
				temp1 = (((int32_t)_sp->raw_data[ch][mid_pt]) - baseline);
				_sp->baseline_sum[ch] += _sp->raw_data[ch][last_pt];
				_sp->baseline_sum[ch] -= _sp->raw_data[ch][first_pt];

				// constraint to int16 range
				if (temp1 > 0x00007FFFL)
					temp1 = 0x00007FFFL;
				if (temp1 < -0x00008000L)
					temp1 = -0x00008000L;
				_sp->flat_data[ch] = (int16_t)temp1; // assign constrained value
				sum_abs_error[ch] += (uint32_t)abs(temp1);
			}
#ifdef UPDATE_DISPLAY
#ifdef SHOW_PPG_WAVE
			if (ppg_cnt == PPG_WAVE_DOWNSAMPLE_RATIO)
			{
				ppg_wave_dc_remove(_sp, _sp->flat_data[IR], _sp->wave);
				ppg_cnt = 0;
			}
#endif
#endif
			// increment pointers
			first_pt++;
			if (first_pt == (uint16_t)ARRAY_LENGTH)
				first_pt -= (uint16_t)ARRAY_LENGTH;
			mid_pt++;
			if (mid_pt >= (uint16_t)ARRAY_LENGTH)
				mid_pt -= (uint16_t)ARRAY_LENGTH;
			last_pt++;
			if (last_pt >= (uint16_t)ARRAY_LENGTH)
				last_pt -= (uint16_t)ARRAY_LENGTH;
		}
		else if (_sp->baseline_cnt >= 0)
		{ // build up initial baseline
			for (uint8_t ch = 0; ch < 2; ch++)
			{
				_sp->baseline_sum[ch] += _sp->raw_data[ch][last_pt + n];
			}
		}
		window_beat_check(_sp);						// run window beat detection method
		_sp->prev_window_width = _sp->window_width; // update window
	}

	// calculate the heart rate period and average pulse amplitudes.
	_sp->beat_sum = 0;
	uint8_t skipped = 0;
	for (uint8_t n = 0; n < _sp->beat_cnt[IR]; n++)
	{
		if (_sp->beat_periods[n] == 0)
			skipped++;
		_sp->beat_sum += _sp->beat_periods[n];
	}

	if (_sp->beat_cnt[IR] - skipped > 0 && _sp->beats_found > 1)
	{
		_sp->window_beat_period1f = (_sp->beat_sum << (uint8_t)FIXED_BITS) / (uint16_t)(_sp->beat_cnt[IR] - skipped);
		_sp->systole_found = 1;
		_sp->beat_amp[IR] = 0;
		for (uint8_t n = 0; n < _sp->beat_cnt[IR]; n++)
		{
			_sp->beat_amp[IR] += _sp->beat_amps[IR][n];
		}
		_sp->beat_amp[IR] /= (int16_t)_sp->beat_cnt[IR];
		_sp->systole = _sp->beat_amp[IR];
	}
	if (_sp->beat_cnt[RED] > 0 && _sp->beats_found > 1)
	{
		_sp->beat_amp[RED] = 0;
		for (uint8_t n = 0; n < _sp->beat_cnt[RED]; n++)
		{
			_sp->beat_amp[RED] += _sp->beat_amps[RED][n];
		}
		_sp->beat_amp[RED] /= (int16_t)_sp->beat_cnt[RED];
	}

	if (_sp->baseline_cnt == baseline_length)
	{

		// get the pulse amplitude information for SpO2
		for (uint8_t ch = 0; ch < 2; ch++)
		{
			_sp->sum_abs[ch][_sp->sum_abs_ptr] = sum_abs_error[ch];
		}
		// increment count of available intervals
		if (_sp->sum_abs_cnt < NUM_INTERVALS_FOR_RMS)
			_sp->sum_abs_cnt++;
		// increment circular buffer pointer
		_sp->sum_abs_ptr++;
		if (_sp->sum_abs_ptr == (uint8_t)NUM_INTERVALS_FOR_RMS) // rollover
			_sp->sum_abs_ptr = 0;
	}
}

/******************************************************
 * Name:		get_rms_from_sub_abs
 * Function:	calculate quasi-RMS (sum of absolute error)
 * from several intervals of basline-removed PPG signal
 * for use with window method
 * Parameters:	none
 * Return:		none
 ******************************************************/
void get_rms_from_sum_abs(struct spo2 *_sp)
{
	if (_sp->sum_abs_cnt == (uint8_t)NUM_INTERVALS_FOR_RMS)
	{
		_sp->ppg_rms1f[RED] = 0;
		_sp->ppg_rms1f[IR] = 0;
		for (uint8_t ch = 0; ch < 2; ch++)
		{
			for (uint8_t n = 0; n < (uint8_t)NUM_INTERVALS_FOR_RMS; n++)
			{
				_sp->ppg_rms1f[ch] += _sp->sum_abs[ch][n];
			}
			_sp->ppg_rms1f[ch] = (_sp->ppg_rms1f[ch] << FIXED_BITS) / (uint32_t)((uint16_t)NUM_INTERVALS_FOR_RMS * (uint16_t)INTERVAL);
		}
	}
}

/******************************************************
 * Name:		get_dc_from_baseline
 * Function:	get DC level from basline_sum of this interval
 * For use with window method
 * Parameters:	none
 * Return:		none
 ******************************************************/
void get_dc_from_baseline(struct spo2 *_sp, spo2_target_counts_t target_counts)
{
	if (_sp->baseline_cnt == ((uint16_t)1) << HPF_BITS) // if we have enough points
	{
	    _sp->mean[IR] = (_sp->baseline_sum[IR] >> HPF_BITS) + (uint32_t)target_counts.ir_led;
	    _sp->mean[RED] = (_sp->baseline_sum[RED] >> HPF_BITS) + (uint32_t)target_counts.red_led;
	}
}

/******************************************************************
 * Name:		window_beat_check
 * Function:	recursive method to detect beats
 * checks if a point has been a max or a min for the
 * duration of a window of width 40% of the expected heart rate
 * Since the dicrotic notch is within 40% of the expected pulse width of the peak
 * the dicrotic notch cannot hold the entire window and is not counted as
 * a beat. Relies on an accurate estimate of the expected pulse with, which
 * comes from the kalman filtered value of the threshold crossing method.
 * Parameters:	none
 * Return:		none
 *******************************************************************/
void window_beat_check(struct spo2 *_sp)
{
	static uint8_t check_for_systole[] = {0, 0};
	static int16_t prev_data[2];
	int16_t trial_period = 0;
	int8_t trend_iir_divisor_bits = 3;
	if (_sp->data_cnt > 1)
	{
		// for both channels
		for (uint8_t ch = 0; ch < 2; ch++)
		{
			/********/
			// if we have an existing max
			if (_sp->max_cnt[ch] > 0)
			{
				// check if previous max is out of the window
				if ((_sp->data_cnt - _sp->max_position[ch]) > _sp->window_width) // e.g. max_position = 10, data_cnt = 15, window_width = 4 (data exists up to 14) useful data is a 11, 12, 13, 14, so can't use 10.
				{
					// in the case the new window is smaller
					if (_sp->window_width < _sp->prev_window_width)
					{
						// check if the max is less than the previous window width
						if ((_sp->data_cnt - _sp->max_position[ch] < _sp->prev_window_width)) // e.g. max position = 10, data_cnt = 15, prev_width = 6 --> 15-10 = 5 which is less than 6.
						{
							// In this case if the window width hadn't changed this point would have become a max.
							_sp->max_cnt[ch]++;
						}
						else
						{

							_sp->max_cnt[ch] = 0; // reset and find a new max
						}
					}
					else
					{
						_sp->max_cnt[ch] = 0; // reset and find a new max
					}
				}
			}
			// we need to check for a new max use the first point (anything else would not have been max long enough to be considered a peak)
			//  point must be rising to be a candidate for max
			if (_sp->flat_data[ch] > prev_data[ch])
			{
				if ((_sp->max_cnt[ch] == 0) || (_sp->flat_data[ch] > _sp->max_value[ch]))
				{
					_sp->max_found[ch] = 0;
					_sp->max_position[ch] = _sp->data_cnt;
					_sp->max_value[ch] = _sp->flat_data[ch];
					_sp->max_cnt[ch] = 1;
				}
				else
				{
					_sp->max_cnt[ch]++;
				}
			}
			else
			// previous max is still valid
			// i.e. the max value must bypass this comparison window_width times to qualify as a peak
			{
				_sp->max_cnt[ch]++;
			}
			// check if we found a max
			if (_sp->max_cnt[ch] >= _sp->window_width)
			{
				// if we need a max, log the current max as a peak (e.g. if the window grew and previous max still qualifies as a max we don't need it)
				if (_sp->max_found[ch] == 0)
				{
					_sp->max_found[ch] = 1;
					_sp->diastole_position[ch] = _sp->max_position[ch];
					_sp->diastole_value[ch] = _sp->max_value[ch];
					check_for_systole[ch] = 1;
					if (ch == (uint8_t)IR)
					{
						if (_sp->diastole_trend == 0)
						{
							_sp->diastole_trend = _sp->diastole_value[ch];
						}
						else
						{
							_sp->diastole_trend += (_sp->diastole_value[ch] - _sp->diastole_trend) >> trend_iir_divisor_bits;
						}
					}
				}
			} // end max search

			/******/
			// check for mins
			if (_sp->min_cnt[ch] > 0)
			{
				// check if previous min is out of the window
				if ((_sp->data_cnt - _sp->min_position[ch]) > _sp->window_width) // e.g. min_position = 10, data_cnt = 15, window_width = 4 (data exists up to 14) useful data is a 11, 12, 13, 14, so can't use 10.
				{
					// in the case the new window is smaller
					if (_sp->window_width < _sp->prev_window_width)
					{
						// check if the min is less than the previous window width
						if ((_sp->data_cnt - _sp->min_position[ch] < _sp->prev_window_width)) // e.g. min position = 10, data_cnt = 15, prev_width = 6 --> 15-10 = 5 which is less than 6.
						{
							// In this case if the window width hadn't changed this point would have become a min.
							_sp->min_cnt[ch]++;
						}
						else
						{

							_sp->min_cnt[ch] = 0; // reset and find a new min
						}
					}
					else
					{
						_sp->min_cnt[ch] = 0; // reset and find a new min
					}
				}
			}
			// we need to check for a new min use the first point (anything else would not have been min long enough to be considered a peak_
			//  point must be falling to be a candidate for min
			if (_sp->flat_data[ch] < prev_data[ch])
			{
				if ((_sp->min_cnt[ch] == 0) || (_sp->flat_data[ch] < _sp->min_value[ch]))
				{
					_sp->min_found[ch] = 0;
					_sp->min_position[ch] = _sp->data_cnt;
					_sp->min_value[ch] = _sp->flat_data[ch];
					_sp->min_cnt[ch] = 1;
				}
				else
				{
					_sp->min_cnt[ch]++;
				}
			}
			else
			// previous min is still valid
			{
				_sp->min_cnt[ch]++;
			}
			// check if we found a min
			if (_sp->min_cnt[ch] >= _sp->window_width)
			{
				// if we need a min, log the current max as a valley (e.g. if the window grew and previous min still qualifies as a min we don't need it)
				if (_sp->min_found[ch] == 0)
				{
					_sp->min_found[ch] = 1;
					_sp->prev_systole_position[ch] = _sp->systole_position[ch];
					_sp->systole_position[ch] = _sp->min_position[ch];
					_sp->systole_value[ch] = _sp->min_value[ch];
					// register a beat
					if (check_for_systole[ch] == 1)
					{
						if (ch == IR)
						{
							if (_sp->beats_found < 2)
							{
								_sp->beats_found++;
							}
							if (_sp->beats_found > 1)
							{
								trial_period = (int16_t)(_sp->systole_position[ch] - _sp->prev_systole_position[ch]);
								if (trial_period <= SAMPLE_LENGTH && trial_period >= MIN_HR_PERIOD)
								{
									_sp->beat_periods[_sp->beat_cnt[ch]] = trial_period;
								}
								else
								{
									_sp->beat_periods[_sp->beat_cnt[ch]] = 0;
								}
							}
						}
						_sp->beat_amps[ch][_sp->beat_cnt[ch]] = _sp->diastole_value[ch] - _sp->systole_value[ch];
						_sp->beat_cnt[ch]++;
						if (ch == (uint8_t)IR)
						{
							if (_sp->systole_trend == 0)
							{
								_sp->systole_trend = _sp->systole_value[ch];
							}
							else
							{
								_sp->systole_trend += (_sp->systole_value[ch] - _sp->systole_trend) >> trend_iir_divisor_bits;
							}
						}
						check_for_systole[ch] = 0;
					}
				}
			}									// end min search
			prev_data[ch] = _sp->flat_data[ch]; // update previous data point
		}										// for each channel
	}											// if data
	else if (_sp->data_cnt == 1)
	{
		prev_data[0] = _sp->flat_data[0];
		prev_data[1] = _sp->flat_data[1];
	}
} // end function

#ifdef USE_CLASSIC_RMS
/*********************************************************
 * Name:       get_rms_classic
 * Function:   calculate RMS of PPG signal from
 * baseline-removed data in AC1f for IR and Red
 * Parameters:
 * 	uint16_t array_len - length of array
 * 	uint16_t rms_len - length of array to actually add up
 *	(baseline-removed portion)
 * Return:     none
 * ********************************************************/
void get_rms_classic(struct spo2 *_sp, uint16_t array_len, uint16_t rms_len)
{
	if (rms_len == 0 || rms_len > array_len || array_len == 0 || rms_len == 0)
	{
		return;
	}
	uint16_t start_pt = (array_len - rms_len) >> 1;
	uint16_t end_pt = start_pt + rms_len;

	for (uint8_t channel = 0; channel < 2; channel++)
	{
		// calculate average difference from zero
		uint32_t var1f = 0;
		for (uint16_t n = start_pt; n < end_pt; n++)
		{
			var1f += (uint32_t)(labs((int32_t)AC1f[channel][n]) * labs((int32_t)_sp->AC1f[channel][n])); // functions same as RMS but easier to compute
		}
		_sp->ppg_rms1f[channel] = uint_sqrt((var1f << FIXED_BITS) / (uint32_t)rms_len);
	} // end channel loop
}
#else

/*********************************************************
 * Name:       get_rms_simple
 * Function:   calculate effective RMS of PPG signal from
 * baseline-removed data in AC1f for IR and Red using the
 * absolute difference from zero (simpler than RMS) but
 * analogous
 * Parameters:
 * 	uint16_t array_len - length of array
 * 	uint16_t rms_len - length of array to actually add up
 *	(baseline-removed portion)
 * Return:     none
 * ********************************************************/
void get_rms_simple(struct spo2 *_sp, uint16_t array_len, uint16_t rms_len)
{
	if (rms_len == 0 || rms_len > array_len || array_len == 0 || rms_len == 0)
	{
		return;
	}
	uint16_t start_pt = (array_len - rms_len) >> 1;
	uint16_t end_pt = start_pt + rms_len;

	for (uint8_t channel = 0; channel < 2; channel++)
	{
		// calculate average difference from zero
		uint32_t dif1f = 0;
		for (uint16_t n = start_pt; n < end_pt; n++)
		{
			dif1f += (uint32_t)(labs((int32_t)_sp->AC1f[channel][n])); // functions same as RMS but easier to compute
		}
		_sp->ppg_rms1f[channel] = (dif1f << FIXED_BITS) / (uint32_t)rms_len;
	} // end channel loop
}
#endif

#ifdef USE_PVI
/************************************************************************************
 * Name:			get_pvi
 * Function:
 *	This this function calculates the estimated amplitude of one heart beat.
 * It is similar to get_rms_simple except is uses less of the buffer.
 * So it responds faster. It should be nominally similar to what we get from the triangle filtered data
 * and be a useful input to the RR algorithm
 * PVi stands for pleth variability index.
 * Parameters:
 *	int16_t hpf_len - this is the length of the array that has been high pass filtered
 *	int16_t period_1f - this is the period of the heart beat in samples with fixed precision
 * Return:
 * 	uint32_t pvi - effective pleth variability index for the RR calculation
 ************************************************************************************/
void get_pvi(struct spo2 *_sp, uint16_t hpf_len, uint16_t period_1f)
{
	int16_t last_pt = hpf_len - 1;
	int16_t period = period_1f >> FIXED_BITS;
	int16_t first_pt = (last_pt - period) + 1;
	// get mean
	int32_t mean = 0;
	for (int16_t n = first_pt; n <= last_pt; n++)
	{
		mean += (int32_t)_sp->AC1f[IR][n];
	}
	mean /= (int32_t)period;
	for (uint16_t n = first_pt; n <= last_pt; n++)
	{
		_sp->pvi += labs(((int32_t)_sp->AC1f[IR][n]) - mean); // functions same as RMS but easier to compute
	}
	_sp->pvi /= (int32_t)period;
	// get RMS simple over just one heart beat period (the most recent)
	if (_sp->pvi_avg == 0 && _sp->pvi > 0)
	{
		_sp->pvi_avg = _sp->pvi;
	}
	else
	{
		_sp->pvi_avg += (_sp->pvi - _sp->pvi_avg) * (int32_t)PVI_FILTER_NUM / (int32_t)PVI_FILTER_DEN;
	}
}
#endif

#ifdef USE_SG
/*********************************************************
 * Name:       set_sg_interval
 * Function:   check if the HR is too high and shorten the
 * sg filter interval and vice-versa
 * Parameters: none
 * Return:     none
 * ********************************************************/
void set_sg_interval(struct spo2 *_sp)
{
	if (_sp->sg_interval == 0)
	{
		if (_sp->period2use1f < (int16_t)SHORT_PER1F_THRESH_DOWN)
		{
			_sp->sg_interval = SHORT_SG;
		}
		else
		{
			_sp->sg_interval = LONG_SG;
		}
	}
	else if ((_sp->sg_interval == LONG_SG) && (_sp->period2use1f < (int16_t)SHORT_PER1F_THRESH_DOWN))
	{
		_sp->persist_down++; // heart rate is going up (interval goes down)
		_sp->persist_up = 0;
		if (_sp->persist_down >= PERSIST)
		{
			_sp->sg_interval = SHORT_SG;
			_sp->persist_down = 0;
		}
	}
	else if ((_sp->sg_interval == SHORT_SG) && (_sp->period2use1f > (int16_t)SHORT_PER1F_THRESH_UP))
	{
		_sp->persist_up++;
		_sp->persist_down = 0;
		if (_sp->persist_up >= PERSIST)
		{
			_sp->sg_interval = LONG_SG;
			_sp->persist_up = 0;
		}
	}
}
#endif

/************************************************************************************
 * Name:			get_pressure
 * Function:
 *	Estimates the finger pressure by comparing whether the systoles (valleys) or diastoles (peaks)
 *	are more pronounced. At high pressure top of waveform (diastole) squares off leading to more pronounced
 *	diastoles. Intended to be used to correct SpO2 readings for finger pressure (unverified).
 * Parameters:
 *	none
 * Return:
 * 	none
 ************************************************************************************/
void get_pressure(struct spo2 *_sp)
{
	if (_sp->diastole_trend > 0 && _sp->systole_trend < 0)
	{
		_sp->pressure_factor = ((labs(_sp->systole_trend) << FIXED_BITS) << FIXED_BITS) / _sp->diastole_trend; // 64x ratio of systole to diastole (higher ratio = higher pressure = more + correction to SpO2 needed	}
	}
}

#ifdef USE_NOISE_CORRECTION
/************************************************************************************
 * Name:			do_noise_correction
 * Function:		remove estimated baseline Tx noise from the signal estimates to improve SpO2 at low PI
 * Parameters:		none
 * Return:			none
 **********************************************************************************/
void do_noise_correction(struct spo2 *_sp)
{
	{
		int32_t noise[2];
#ifdef SCALE_TX_NOISE
		noise[RED] = ((((int32_t)MAX_NOISE) << FIXED_BITS) * (int32_t)_ob->r_current / (int32_t)1023);
		noise[IR] = ((((int32_t)MAX_NOISE) << FIXED_BITS) * (int32_t)_ob->ir_current / (int32_t)1023);
#else
		noise[RED] = ((int32_t)TYP_NOISE) << FIXED_BITS;
		noise[IR] = ((int32_t)TYP_NOISE) << FIXED_BITS;
#endif
		for (uint8_t m = 0; m < 2; m++)
		{ // linear ramp down noise correction as signal goes to zero starting at 2x noise
			if (2 * noise[m] > _sp->ppg_rms1f[m])
			{
				noise[m] -= (2 * noise[m] - _sp->ppg_rms1f[m]) / 2;
			}
			if (noise[m] < 0)
				noise[m] = 0;
			_sp->ppg_rms1f[m] = uint_sqrt((uint32_t)(((int32_t)_sp->ppg_rms1f[m] * (int32_t)_sp->ppg_rms1f[m]) - (noise[m] * noise[m])));
		}
	}
}
#endif

/*************************************
 * Name: 		round_dec_spo2
 * Function:	decimate spo2 (divide by 10) including rounding
 * Parameters:	display_spo2 (10x actual SpO2 percentage)
 * Return: 		decimated, rounded dispay_spO2 value for two digit display
 ************************************/
uint16_t round_dec_spo2(uint16_t spo2_val)
{
	uint16_t new_val;
	uint16_t display_spo2_round;
	if (spo2_val > (uint16_t)0)
	{
		new_val = spo2_val / (uint16_t)10;
		display_spo2_round = spo2_val - (new_val * (uint16_t)10);
		if (display_spo2_round >= 5)
			new_val++;
#ifndef SPO2_DEBUG
		if (new_val > 100)
			new_val = 100;
#endif
		return new_val;
	}
	else
	{
		return spo2_val;
	}
}

#ifdef UPDATE_DISPLAY
#ifdef SHOW_PPG_WAVE
/************************************************************************************
 * Name:       ppg_wave_dc_remove
 ************************************************************************************/
void ppg_wave_dc_remove(struct spo2 *_sp, int16_t latest_sample, uint8_t *ppg_wave_out)
{
	uint16_t ppg_wave_scale = 0;
	int16_t ppg_temp_min = 0;
	int16_t ppg_temp_max = 0;

	ppg_temp_min = _sp->ppg_wave[0];
	ppg_temp_max = _sp->ppg_wave[0];
	if (_sp->alg_start_sample_count > 0)
	{
		_sp->ppg_wave[_sp->wave_ptr] = latest_sample;
		int16_t index1 = _sp->wave_ptr;
		for (int n = 1; n < NUM_PPG_WAVE_PTS / 4; n++)
		{ // use less of the buffer to make a shorter duration DC filter
			ppg_temp_min =
				(_sp->ppg_wave[index1] < ppg_temp_min) ? _sp->ppg_wave[index1] : ppg_temp_min;
			ppg_temp_max =
				(_sp->ppg_wave[index1] > ppg_temp_max) ? _sp->ppg_wave[index1] : ppg_temp_max;
			index1--;
			if (index1 < 0)
			{
				index1 += (int16_t)NUM_PPG_WAVE_PTS;
			}
		}
		ppg_wave_scale = abs(ppg_temp_max - ppg_temp_min);
		uint16_t ppg_wave_scale_undivided = ppg_wave_scale;
		ppg_wave_scale = ppg_wave_scale >> PPG_WAVE_RES; // for PPG_WAVE_RES=3, this divides by 8; this will allow for values of -8 up to 7 for the PPG waveform.

		if (_sp->wave_cnt < NUM_PPG_WAVE_PTS)
		{
			_sp->wave_cnt++;
			ppg_wave_out[_sp->wave_ptr] = 0;
		}
		else
		{
			ppg_wave_out[_sp->wave_ptr] = (uint8_t)((ppg_temp_max - _sp->ppg_wave[_sp->wave_ptr]) / ppg_wave_scale); // invert the signal for display
		}
		ppg_wave_scale_undivided = ppg_temp_max - ppg_wave_scale_undivided;
		OLED_bar_update(ppg_wave_scale_undivided, ppg_wave_out[_sp->wave_ptr]);
		_sp->wave_ptr++;
		if (_sp->wave_ptr >= 128)
		{
			_sp->wave_ptr = 0;
		}

		// clear data ahead of the pointer on the display
		if (_sp->wave_ptr + 5 < 128)
		{
			ppg_wave_out[_sp->wave_ptr + 5] = 0;
		}
		else
		{
			ppg_wave_out[_sp->wave_ptr + 5 - 128] = 0;
		}
	}
}
#endif
#endif

//void do_algorithm(struct oxim *_ox, struct ob1203 *_ob, struct spo2 *_sp)
//{
//	uint16_t temp_br = 0;
//	do_algorithm_part1(_sp);
//	_ox->do_alg = 2;
//	do_algorithm_part2(_ox, _ob, _sp);
//	_sp->avg_PI = get_avg_PI(_sp->perfusion_index[IR], &_sp->pi_rms_val);
//#ifdef COMPUTE_RR
//	do_algorithm_part3(_sp);
//	do_algorithm_part4(_sp);
//	temp_br = (uint16_t)(_sp->breathing_rate1f >> FIXED_BITS);
//	_sp->br = temp_br; // no averaging
//#endif
//
//#ifdef CHECK_PI
//	check_pi(_ox, _sp); // adds a comment to the UART print, freezes Spo2 if new data is questionable, resets alg if it gets too bad
//#endif
//}

#define CHECK_PI
#ifdef CHECK_PI
void check_pi(struct spo2 *_sp, uint32_t latest_ir_data, bool * p_valid)
{
	// only check if you have data
	if (_sp->ppg_rms1f[IR] != 0 && _sp->perfusion_index[IR] > 0)
	{
		// check for lively subject: RMS > noise and PI is greater than minimum useful PI and signal isn't too small
		if (((_sp->ppg_rms1f[IR] >> (uint8_t)FIXED_BITS) < (uint32_t)MIN_RMS) || (latest_ir_data < (uint32_t)8000) || (_sp->perfusion_index[IR] < (float)MIN_PI))
		{
			// algorithm has run and data is bad
		    _sp->low_ac_amplitude_cnt++;
			if (_sp->low_ac_amplitude_cnt > (uint8_t)LOW_AC_PERSIST)
			{
				_sp->spo2_err = spo2_err_lo_pi; //low PI
			}
			else
			{
                _sp->spo2_err = spo2_err_none; //normal, okay
                _sp->no_disp_spo2_cnt = 0;
			}
		}
		// check if PI is so low we need a stricter standard for RMS variation (need less motion than we can typically tolerate)
		else if (_sp->perfusion_index[IR] < (float)LOW_PI_LEVEL)
		{
			if (_sp->pi_rms_val > (float)LOW_PI_MAX_RMS * _sp->perfusion_index[IR])
			{
                _sp->no_disp_spo2_cnt++;
                _sp->spo2_err = spo2_err_lo_pi_hi_var; //low PI, high var
			}
			else if ((_sp->alg_count >= MIN_ALG_RUNS) && ( (_sp->ppg_rms1f[IR] >> (uint8_t)FIXED_BITS) > (uint32_t)MIN_RMS))
			{
                _sp->no_disp_spo2_cnt = 0;
                _sp->spo2_err = spo2_err_none; //normal, okay
			}
		}
		// check if PI variance is too large (normal PI case) -- typically motion
		else if (_sp->pi_rms_val > (float)NORMAL_PI_MAX_RMS * _sp->perfusion_index[IR])
		{
            _sp->no_disp_spo2_cnt++;
            _sp->spo2_err = spo2_err_hi_var; //PI variance too large (motion)
		}
		// ok
        else if ((_sp->alg_count >= MIN_ALG_RUNS) && ( (_sp->ppg_rms1f[IR] >> (uint8_t)FIXED_BITS ) > (uint32_t)MIN_RMS))
		{
            _sp->spo2_err = spo2_err_none; //normal, okay
            _sp->no_disp_spo2_cnt = 0;
		}
		// too many errors -- give up and reset alg
		if (_sp->no_disp_spo2_cnt > (uint8_t)MAX_DO_NOT_DISP_SPO2)
		{
            _sp->spo2_err = spo2_err_alg_reset; //too many errors, resetting
		}
	}

	/* Check if algorithm needs to be reset */
	if (spo2_err_alg_reset != _sp->spo2_err)
	{
	    *p_valid = true;
	}
	else
	{
	    *p_valid = false;
	}
}
#endif
