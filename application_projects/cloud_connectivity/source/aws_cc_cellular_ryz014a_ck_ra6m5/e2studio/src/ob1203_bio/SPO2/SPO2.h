#ifndef __SPO2_H__
#define __SPO2_H__

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
 #include <stdint.h>
 #include <stdbool.h>
#elif defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL78__)
 #include <stdint.h>
 #include <stdbool.h>
#else
 #include "bsp_api.h"
#endif

/**************ALGORITHM CONFIG OPTIONS***************************/
#define COMPUTE_RR //DEFINE THIS TO RUN ALG 2.5x MORE OFTEN FOR CALCULATING RESPIRATION RATE (not verified with this undefined in Gen 3 alg (Dec 2021)
/****************do not clip spo2 values (useful for calibration)***********/
//#define SPO2_DEBUG //define to not clip Spo2 values at 100--for R curve calibration purposes during hypoxia testing
/*************change algorithm data sample rate (default 100sps)***********/
//#define _50sps //takes data at half rate, doubles averages (reduces computation at similar LED current--unverified in Gen 3 alg (Dec 2021)
/**************use window method for HR*************************/
#define USE_WINDOW //Define this to use the window-based peak detection
/*************signal amplitude estimate from recursive window method (default is from threshold crossing)****/
//#define USE_WINDOW_RMS //otherwise using high pass filtered data estimate of RMS in threshold crossing calc
/*************use peak to peak instead of rms for SpO2 cal (default is RMS)******************/
//#define USE_WINDOW_P2P //accurate when PI is high, otherwise RMS is better
/***********compensate for sensor noise to fix too low SpO2 whe PI is low--requires tuning**********************/
#define USE_NOISE_CORRECTION
//#define SCALE_TX_NOISE // for use in applications where measured noise scales with LED current, otherwise typical (unscaled) noise is used
/***********attempt to compensate for finger pressure variation of SpO2*****************************/
#define USE_PRESSURE_CORRECTION
/***********Savitsky Golay noise filter*********************************/
#define USE_SG //use the savitsky-golay filter to smooth noise without distorting the waveform shape (raw data buffer will contain filtered values)
/***********Optionally use PVI (rms of 3sec buffer) for RR--worse for slow breathing, better for fast (default is peak to peak amplitude)**********/
//#define USE_PVI //declare this to use PVI instead of systole amplitude for RR
/***********Report RR measurements 4 sec sooner (less accurate though, tends to be higher than expected**************/
//#define QUICKSTART_RR //attempts to report RR sooner--initially less accurate, tends to be high
/************Alternate more computationally expensive way to get signal amplitude (default is mean absolute deviation)******/
//#define USE_CLASSIC_RMS //versus simple average absolute deviation from baseline--more computation for classic RMS (n multiplications)

/***************************PPG wave display variables************************/
 #ifndef _50sps
	#define PPG_WAVE_DOWNSAMPLE_RATIO (3)
#else
	#define PPG_WAVE_DOWNSAMPLE_RATIO (2)
#endif
/*********************Abbreviations and definitions*************************/
typedef unsigned long long      uint64_t;
typedef signed long long        int64_t;
#define _K_F kalman_filters
#define FIXED_BITS 3 //for basic fixed precision enhancement. Don't change this without looking carefully at spo2.corr function >>2 instances and potential overflows.
#define IR 0 //channel definitions.
#define RED 1
#ifndef _50sps
	#ifdef COMPUTE_RR //compile option for respiration rate (set in Oximeter.c)
		#define INTERVAL 40
	#else
		#define INTERVAL 100
	#endif
#else
	#ifdef COMPUTE_RR //compile option for respiration rate (set in Oximeter.c)
		#define INTERVAL 20
	#else
		#define INTERVAL 50
	#endif
#endif
/******************Moving average filter length (bandwidth)*************************/
//running low pass filter length
#ifdef _50sps
	#define NUM2AVG 4
	#define AVG_BITS 2
#else
	#define NUM2AVG 8
	#define AVG_BITS 3
#endif
/****************Sample rate-dependent parameters defining HR data buffer size and startup****************************/
#ifndef _50sps
	#define SAMPLE_LENGTH (148) //40.5 BPM at 100sps
	#define MAX_WINDOW_LENGTH 64 //40% of sample length--rounded to nearest power of 2 for fast masking
	#define MIN_WINDOW_LENGTH 12 //40% of sample length at max HR
	#define MIN_PPG_FALLTIME (8)
	#define ARRAY_LENGTH (320) //set this to at least 2*ARRAY_LENGTH. Threshold crossing count duration will be this minus - 63.
	#define MIN_HR_PERIOD (32) //max HR is 6000/MIN_HR_PERIOD, so 32 = 187.5 bpm
	#define MAX_HR_PERIOD (SAMPLE_LENGTH) //min HR is 6000/MAX_HR_PERIOD, so 148 = 41 bpm
	#define STABILITY_PERIOD (200) //add an extra number of samples to collect before analyzing (finger settling motion, signal drift, etc reduce initial accuracy)
	#define SAMPLE_RATE (100) //100 samples per second --also adjust SAMPLE_RATE_PER_MINUTE if you change this
	#define SAMPLE_RATE_PER_MIN 6000 //6000 //samples per minute (set to 60 * sample rate)
#else
	#define SAMPLE_LENGTH (74) //40.5 BPM at 50sps,
	#define MAX_WINDOW_LENGTH 32
	#define MIN_WINDOW_LENGTH 6
	#define MIN_PPG_FALLTIME (3)
	#define ARRAY_LENGTH (164) //must be at least twice sample length --can reduce this a little to save some RAM and compute time. Longer is better for lower heart rates, but compute time increases.
	#ifdef USE_SG
		#define USE_ARRAY_LENGTH (160) //ARRAY_LENGTH-NUM2AVG
	#else
		#define USE_ARRAY_LENGTH ARRAY_LENGTH
	#endif
	#define MIN_HR_PERIOD (15) //max HR is 3000/MIN_HR_PERIOD, so 16 = 187.5 bpm
	#define MAX_HR_PERIOD (SAMPLE_LENGTH) //min HR is 6000/MAX_HR_PERIOD, so 148 = 41 bpm
	#define STABILITY_PERIOD (100) //add an extra number of samples to collect before analyzing (finger settling motion, signal drift, etc reduce initial accuracy)
	#define SAMPLE_RATE (50) //50 samples per second --also adjust SAMPLE_RATE_PER_MINUTE if you change this
	#define SAMPLE_RATE_PER_MIN 3000 //6000 //samples per minute
	#define MIN_50_SPS_PERIOD_2_USE_B2B (23)
#endif

#define MIN_ALG_RUNS_SWITCH_TO_LONGER_AVG (16)
#define MIN_ALG_RUNS (7) //minimum number of times the algorithm has to run before the display will update. Increases startup time, but increases accuracy of initial value.
#define B2B_DELAY (10) //number of algorithm runs before we start using the b2b method
/******************Perfusion Index parameters**********************************/
#ifdef COMPUTE_RR
	#define PI_AVGS 12//12 //number of algorithm iteration runs to average on display
#else
	#define PI_AVGS 5 //number of algorithm runs at interval 100 to average on display
#endif
/********************Parameters for RR calculation**************************************/
#define USE_RR_DISPLAY_AVG
#define RR_DISPLAY_NUM2AVG 30 //(max 60 = 24 secs) Avg's samples on top of the previous Kalman average. Duration is DISPLAY_NUM2AVG * INTERVAL/SAMPLE_RATE
#define RR_DISPLAY_INITIAL_NUM2AVG 11 //initial 4.4 sec average
#define RR_DISPLAY_INITIAL_RUNS 24 //running a 4.4 sec average for 8 secs allows us to dump the first 3.6 seconds of data before transitioning to a longer average
#define NUM_RR_FILTERS 5
#define MAX_FILTER_LENGTH (8) //number of samples to average initially
#define MAX_BREATH_FILTER_LENGTH 28 //This sets the minimum detectable breathing rate 60/(interval/sample_rate*MBFL)
#define MAX_BREATH_FILTER_SPAN 45//1.6xMAX_BREATH_FILTER_LENGTH
#define MAX_BREATH_ARRAY_LENGTH  73 //max filter len 28 +max filter span 45 (2.6*max filter len)
#define START_BREATH_ARRAY_LENGTH 62
#define ALLOW_LONGEST_RR_FILTER_THRESHOLD (15<<FIXED_BITS) //allow longest RR filters if current breaths are coming once every N algorithm runs. e.g 17 alg runs = 17*0.4 = ~6.8 secs or ~9 RPM
#define RR_MAX_PERIOD_1F 300//longest allowed RR period in algorithm runs (4 RPM = period of ~37) in extended precision
#define RR_MIN_PERIOD_1F  20//shortest allowed RR period in algorithm runs (40 RPM) in extended precision
#define FILTER_DAMPING 12 //increasing this will increase the time to find the correct signal but it should also increase the stability
/*******Parameters for the high pass filter, threshold crossing method*******************/
#define BAD_HR_THRESHOLD 6 //this is 4x the fraction of error relative to zero crossings that is allowable to consider
#ifndef _50sps
	#define HPF_BITS (6) //sets the high pass filter length, i.e. baseline_len = 1<<HPF_BITS, so 6 corresponds to a filter length of 64
#else
	#define HPF_BITS (5) //sets the high pass filter length, i.e. baseline_len = 1<<HPF_BITS, so 5 corresponds to a filter length of 32
#endif
#define HPF_THRESHOLD_DEN (5) //HPF threshold denominator
#define HPF_THRESHOLD_NUM (2) //HPF threshold numerator
#define MAX_HR_TO_USE_DEBOUNCING 140 //above this HR rate is so high, slope is such that there is little risk in a double crossing. Under-counting is a higher risk.
#define USE_HIGHER_CROSSING_COUNT_HR_BPM 140 //at 140 BPM switch to taking the larger of the two threshold crossing counts as risk of missing a beat due to breathing is higher than risk of double counting from a dicrotic notch
/***************************** Parameters for window peak detection method************/
#ifdef COMPUTE_RR
	#define MAX_BEATS_PER_INTERVAL 2
#else
	#define MAX_BEATS_PER_INTERVAL 4
#endif
#define INIT_WINDOW_WIDTH 40 // 3/5 of typical HR period
#define NUM_INTERVALS_FOR_RMS 6 //For USE_WINDOW_RMS this times interval is the number of samples averaged in the RMS estimate of SpO2
#define WINDOW_WIDTH_NUM 3
#define WINDOW_WIDTH_DEN 5 //ration of num/dem determines window width relative to current HR (e.g. 3/5 = 60%)
/****************************** Parameters for PVI calculation***********************/
#define PVI_FILTER_NUM 2
#define PVI_FILTER_DEN 3 //e.g. 2/3 for num = 2 and den = 3
/************************* Parameters for SG filter **********************************/
#ifdef _50sps
	#define SHORT_SG 2 //must be a power of 2
	#define LONG_SG 4 //must be a power of 2
	#define SHORT_PER1F_THRESH_DOWN 200 //120 BPM
	#define SHORT_PER1F_THRESH_UP 222 //110 BPM
#else
	#define SHORT_SG 4 //must be a power of 2
	#define LONG_SG 8 //must be a power of 2
	#define SHORT_PER1F_THRESH_DOWN 400 //120 BPM
	#define SHORT_PER1F_THRESH_UP 444 //110 BPM
#endif
#define PERSIST 5 //how many intervals the HR must go above or below threshold before switching SG filter length
//***********************Finger pressure correction parameters*******************************
#define MIN_PI_FOR_PRESSURE_CORRECTION 0.1 //no finger correction below this PI
#define PRESSURE_FACTOR_THRESHOLD 70 //pressure factor below this has no effect
#define PRESSURE_SLOPE_DENOM 9 //larger means less correction
#define PRESSURE_FACTOR_MAX 140 //pressure factor above this adds no correction
#define HR_FOR_PRESSURE_CORRECTION_UPPER_THRESH 120 //pressure factor relies on baseline period < HR period, so PF loses accuracy at higher HR
#define HR_FOR_PRESSURE_CORRECTION_LOWER_THRESH 110 //for hysteresis TODO: scale baseline period in window met
//********************** Parameters for SpO2 calibration ***************************************
// R - curve: SpO2 = Rsquare * R * R + Rlinear * R + Rconstant
#define Rsquare (0)
#define Rlinear (-48)
#define Rconstant (119.5)

#define MIN_SPO2 (60) //when DEBUG_SPO2 is not defined SPO2 below this is clipped to this value (not verifiable below this)
/********************** Parameters for CHECK_PI*******************************************/
#define MIN_RMS (25) //8x the minimum allowable RMS (avg dev) for IR channel --> for comparison with (ppg_rms1f[IR]>>3)
#define LOW_PI_LEVEL (0.15) ///below this we use a more stringent check on PI rms
#define NORMAL_PI_MAX_RMS (0.3) //RMS fraction of PI allowable with decent PI
#define LOW_PI_MAX_RMS (0.15) //RMS fraction of PI allowable with crappy PI
#define MIN_PI (0.04)
/********************** Parameters for noise correction***********************************/
#define MAX_NOISE 40 //Tx noise at max IR current (0x3F), for use in Tx-noise limited applications. Define SCALE_TX_NOISE
//#define TYP_NOISE 10 //for BEST_SNR mode, EK1 demo
//#define TYP_NOISE 12 //for MID_POWER, EK1 demo
//#define TYP_NOISE 14 //for LOW_POWER, EK1 demo
#define TYP_NOISE 15 //for BEST_SNR mode, EK2 demo
//#define TYP_NOISE 20 //for MID_POWER, EK2 demo
//#define TYP_NOISE 25 //for LOW_POWER, EK2 demo
/********************* PPG wave display settings*******************************/
#define NUM_PPG_WAVE_PTS 	(128)
#define PPG_WAVE_RES		(4)
/**************************End of algorithm parameters section***************************/

#ifdef COMPUTE_RR
    #define MAX_DO_NOT_DISP_SPO2 (25) //i.e. 10 seconds at interval 0.4
#else
    #define MAX_DO_NOT_DISP_SPO2 (10)
#endif

typedef enum spo2_err{spo2_err_none, spo2_err_lo_pi, spo2_err_lo_pi_hi_var, spo2_err_hi_var, spo2_err_alg_reset} spo2_err_t;

typedef struct st_spo2_target_counts
{
    uint32_t ir_led;
    uint32_t red_led;
} spo2_target_counts_t;

typedef struct spo2 {
	uint8_t instance_num;
	volatile uint16_t data_ptr;
	int32_t mean1f[2];
	uint16_t indx;
	uint16_t indx1;
	uint32_t first_data;
	float R; //R value used for backup SpO2 calculation using RMS
	uint32_t ppg_rms1f[2]; //rms value in fixed precision
	float perfusion_index[2];
	uint16_t current_spo21f; //most recent SpO2 calculation in percent and fixed precision
	uint16_t current_hr1f; //most recent heart rate value in BPM and fixed precision
	volatile uint16_t sample_count; //keeps track of whether the sample buffer is full or not. If full then alg runs.
	volatile uint32_t read_sample_count; //keeps track of total number of samples collected
	volatile uint32_t missed_sample_count;
	uint32_t prev_sample_count;
	uint32_t prev_missed_samples;
	uint16_t read_samples;
	uint16_t missed_samples;
	uint8_t do_rr_alg;
	int32_t alg_start_sample_count; //records the number most recent sample when the algorithm starts
	int32_t prev_alg_start_sample_count; //remembers the sample count stfart position of the previous alg run
	int16_t b2b1f;
	int16_t trial_time;
	uint16_t display_spo2; //value output to display
	uint16_t display_hr; //value output to display
	uint8_t samples2avg; //length of moving average filter for heart rate calculations
	uint8_t ppg_fir_filter_length;
	uint8_t systole_found;
	int32_t systole; //for tracking AC sinus rhythm
	int32_t hr_data_buffer[MAX_BREATH_ARRAY_LENGTH];
	uint8_t hr_data_buffer_ind;
	uint32_t breathing_rate1f; //breaths per minute with fixed precision
	int16_t iir_avg[NUM_RR_FILTERS];
	int16_t iir_rms[NUM_RR_FILTERS];
	int16_t iir_rms_smooth[NUM_RR_FILTERS];
	uint8_t rr_filter_lens[NUM_RR_FILTERS]; //values are set in the class init function
	uint8_t rr_filter_spans[NUM_RR_FILTERS];
	int16_t alg_count;
	uint8_t max_filter;
	uint8_t rr_display_num2avg;
	float pi_rms_val;
	float avg_PI;
	//declaring these large arrays in global scope to limit stack usage
	int16_t AC1f[2][ARRAY_LENGTH]; //including 4 bits fixed point after subtracting local mean (max AC amplitude (half of P2P signal must be less than 2^(15-4) = 2^11 = 2048)
	int16_t raw_data[2][ARRAY_LENGTH];
	uint16_t period2use1f;
	uint16_t b2b_rms_norm1f;
	uint16_t br;
	int32_t rr_filter[MAX_BREATH_FILTER_SPAN];
	int32_t lin_buffer[MAX_BREATH_ARRAY_LENGTH];
	int32_t detrended[MAX_BREATH_FILTER_SPAN];
	uint8_t baseline_sign_changes[NUM_RR_FILTERS]; //RR sign changes
	uint8_t zero_crossings[NUM_RR_FILTERS]; //RR zero crossings
	int16_t badness[NUM_RR_FILTERS]; //calc'd as rms/sign_changes in baseline (flatness of baseline)
	uint8_t max_fom; //min badness
	uint8_t filter_tracker;
	uint8_t current_filter;
	uint8_t char_array_iir_initialized;

	#ifdef USE_PVI
	int32_t pvi;
	int32_t pvi_avg;
	#endif

	#ifdef USE_SG
	uint8_t sg_interval;
	int16_t y_arr[6 + INTERVAL + NUM2AVG - 1]; //assuming read samples will not exceed more than 6 of the expected value
	uint8_t persist_up;
	uint8_t persist_down;
	#endif
	uint8_t num_samples;
	uint8_t bad_crossing_cnt;

	#ifdef USE_RR_DISPLAY_AVG
	int16_t rr_display_avg_buffer[RR_DISPLAY_NUM2AVG];
	int16_t rr_display_avg_cnt;
	int16_t rr_display_avg_buffer_ind;
	#endif

	uint32_t rr_current_monitor1f; //alg runs per breath
	uint16_t hr_zc;
	uint16_t est_hr_period_from_zc1f;
	uint16_t hpf_array_len;
	uint8_t rr_buffer_full;
	uint8_t max_rr_filter;
	int32_t max_position[2];
	int16_t max_value[2];
	int32_t min_position[2];
	int16_t min_value[2];
	int32_t diastole_position[2];
	int16_t diastole_value[2];
	int32_t systole_position[2];
	int32_t prev_systole_position[2];
	int16_t systole_value[2];
	int16_t beat_amp[2];
	int16_t beat_amps[2][MAX_BEATS_PER_INTERVAL]; //most beats to detect (can't do more than 3 because of max heart rate)
	int16_t beat_amp_sum[2]; //use to calculate average beat amp in interval for high hr
	uint8_t beat_cnt[2];
	uint16_t beats_found;
	uint8_t num_window_samples;
	uint8_t max_cnt[2]; //number of samples in a row for this max
	uint8_t min_cnt[2]; //number of samples in a row for this min
	uint8_t window_width;
	uint16_t beat_periods[MAX_BEATS_PER_INTERVAL]; //keeps track of the recent heart beats
	uint8_t beat_ptr[2];
	uint16_t beat_sum; //sum of beat periods divide by number of beats to get average beat period
	int32_t baseline_sum[2];
	uint32_t data_cnt;
	int8_t baseline_cnt;
	uint16_t alg_start_data_ptr; //raw data pointer at start of algorithm
	int32_t mean[2]; //don't calc perfusion if this is zero
	uint32_t sum_abs[2][NUM_INTERVALS_FOR_RMS];
	uint8_t sum_abs_ptr;
	int16_t flat_data[2];
	uint8_t sum_abs_cnt;
	uint8_t prev_window_width;
	uint8_t max_found[2];
	uint8_t min_found[2];
	int32_t systole_trend;
	int32_t diastole_trend;
	 //0-95% in 20 samples or about 8 seconds
	int16_t pressure_factor;
	uint16_t window_beat_period1f;
	#ifdef USE_PRESSURE_CORRECTION
		uint8_t ok_to_use_pc;
	#endif

#ifdef UPDATE_DISPLAY
#ifdef SHOW_PPG_WAVE
	uint8_t wave[NUM_PPG_WAVE_PTS];
	int16_t ppg_wave[NUM_PPG_WAVE_PTS];
	volatile uint8_t wave_ptr;
	uint8_t wave_cnt;
#endif
#endif


	uint8_t low_ac_amplitude_cnt;
    uint8_t no_disp_spo2_cnt;
    spo2_err_t spo2_err;
}spo2_t;

extern struct spo2 sp; //share instance

//functions
void spo2_init(struct spo2 *_sp);
void add_sample(struct spo2 *_sp, spo2_target_counts_t target_counts,uint32_t ir_data, uint32_t r_data);
void missed_sample_counts_increment(struct spo2 *_sp);
void read_sample_counts_increment(struct spo2 *_sp);
void counts_of_samples_clear(struct spo2 *_sp);
void spo2_reset(struct spo2 *_sp);
void hr_and_spo2_calculate(struct spo2 *_sp, uint16_t * p_hr, uint16_t * p_spo2, float * p_pi, spo2_target_counts_t target_counts);
void rr_calculate(struct spo2 *_sp, uint16_t * p_rr);
void calc_hr(struct spo2*);
void calc_R(struct spo2*);
void calc_spo2(struct spo2*);
void characterize_array_iir(struct spo2*, uint8_t arr_num, int16_t new_val, int16_t* iir_avg, int16_t *iir_rms, int16_t* iir_rms_smooth);
uint16_t compare_hr(struct spo2*, uint16_t zc_est, uint16_t b2b_est);
void copy_data(struct spo2*);
int16_t div2n(int16_t num,uint8_t bits); //numerical divide by 2^n for integers. E.g. -1>>1 = -1. But rs(-1,1) = 0, just like 1>>1 = 0;
uint8_t count_sign_changes(int32_t* arr, uint8_t len);
uint8_t count_zero_crossings(int32_t* arr, uint8_t len);
uint8_t count_threshold_crossings_int16(struct spo2*, int16_t* arr, uint16_t arr_len,uint8_t polarity);
void do_noise_correction(struct spo2 *_sp);
float get_avg_PI(float new_PI, float * pi_rms);
int8_t get_direction(int32_t data1, int32_t data2);
void check_pi(struct spo2 *_sp, uint32_t latest_ir_data, bool * p_valid);
void do_algorithm_part1(struct spo2*);
void do_algorithm_part2(struct spo2 *_sp, spo2_target_counts_t target_counts);
void do_algorithm_part3(struct spo2*);
void do_algorithm_part4(struct spo2*); //in case we need to split RR algorithm into two parts
//void do_algorithm(struct oxim *_ox, struct ob1203 *_ob, struct spo2 *_sp);
void get_dc_from_baseline(struct spo2 *_sp, spo2_target_counts_t target_counts);
void get_pressure(struct spo2*);
void get_pvi(struct spo2*,uint16_t hpf_len, uint16_t period_1f);
void get_rms_from_sum_abs(struct spo2*);
int8_t high_pass_filter(struct spo2*,uint16_t baseline_bits, uint16_t *new_array_len);
void hpf_window_peak_detect(struct spo2 *_sp);
void hpf_window_peak_detect_init(struct spo2*);
void ppg_wave_dc_remove(struct spo2 *_sp, int16_t latest_sample, uint8_t *ppg_wave_out);
uint16_t round_dec_spo2(uint16_t spo2_val);
int sign(int32_t val);
void window_beat_check(struct spo2*);
void spo2_config(void);

#ifdef USE_CLASSIC_RMS
	void get_rms_classic(struct spo2 *_sp,uint16_t array_len, uint16_t rms_len);
#else
	void get_rms_simple(struct spo2 *_sp,uint16_t array_len, uint16_t rms_len);
#endif

#ifdef USE_SG
	void do_moving_avg(uint8_t ch);
	void sg_filt_raw_data(uint8_t ch);
	void set_sg_interval(struct spo2 *_sp);
#endif



#endif //end spo2.h define
